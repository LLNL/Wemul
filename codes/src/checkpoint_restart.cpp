/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2020, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

#include <chrono>
#include <fstream>
// #include <iostream>
#include <mpi.h>
#include <regex>
#include <sstream>
#include <thread>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>

#include <iostream>

#include "axl.h"
#include "checkpoint_restart.hpp"
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

checkpoint_restart::checkpoint_restart(std::string directory,
        std::string staging_directory,
        int num_checkpointing_ranks,
        int num_ck_files_per_rank,
        int block_size, int segment_count,
        double checkpointing_interval,
        int stage_out_steps,
        bool enable_staging, bool use_axl,
        float ck_error_rate,
        int num_checkpoint_iterations,
        axl_xfer_types axl_xfer_type)
        : dataflow_workload()
{
    m_directory = directory;
    m_staging_directory = staging_directory;
    m_num_checkpointing_ranks = num_checkpointing_ranks;
    m_num_ck_files_per_rank = num_ck_files_per_rank;
    m_block_size = block_size;
    m_segment_count = segment_count;
    m_checkpointing_interval = checkpointing_interval;
    m_stage_out_steps = stage_out_steps;
    m_enable_staging = enable_staging;
    m_use_axl = use_axl;
    m_ck_error_rate = ck_error_rate;
    m_num_checkpoint_iterations = num_checkpoint_iterations;

    m_axl_xfer_type = axl_xfer_type;

    srand(time(NULL));

    if (m_enable_staging && m_use_axl)
    {
        int rc = -1;
        rc = AXL_Init(NULL);
        // printf("AXL INIT RC: %d\n", rc);
        if (rc != AXL_SUCCESS)
        {
            printf("AXL_Init() failed (error %d)\n", rc);
            return;
        }
    }
    // printf("enable staging %d use axl %d\n", m_enable_staging, m_use_axl);
    // if (m_enable_staging && m_use_axl)
    // {
    //     printf("test axl to be called\n");
    //     test_axl();
    // }
    // else
    // {
    //     printf("test axl not called\n");
    // }
}

checkpoint_restart::~checkpoint_restart()
{
    if (m_enable_staging && m_use_axl)
    {
        int rc = AXL_Finalize();
        // printf("AXL FINALIZE RC: %d\n", rc);
        if (rc != AXL_SUCCESS)
        {
            printf("AXL_Finalize() failed (error %d)\n", rc);
        }
    }
}

void checkpoint_restart::emulate(int argc, char** argv)
{
    // Use codes from IOR here
    // One or more (as specified) rank will create files in a common interval and randomly die
    // All ranks will poll until the first process emulates a crash and read the latest file
    // std::cout << "Rank: " << m_world_rank << ":" <<  "Emulating Checkpoint Restart I/O Pattern\n";

    MPI_Init(&argc, &argv);
    g_profiler.m_timer.adjust_time_deviation();

    int _world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &_world_size);

    int _world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &_world_rank);
    m_world_rank = _world_rank;

    int _set_id = 0;
    bool _flushed_to_pfs = false;
    axl_xfer_t axl_xfer_type = (axl_xfer_t) m_axl_xfer_type;

    if (m_enable_staging && m_use_axl)
    {
        m_axl_id = -1;
        m_axl_id = AXL_Create(axl_xfer_type, "transfer_for_stage_out");
        // printf("AXL ID: %d\n", m_axl_id);
        if (m_axl_id == -1)
        {
            printf("AXL_Create() failed (error %d)\n", m_axl_id);
            return;
        }
    }

    // TODO: make it float later
    int _error_rate = (int) m_ck_error_rate;

    while ((rand() % 100 + 1) > _error_rate)
    {
        if (m_num_checkpoint_iterations > -1
            && _set_id > m_num_checkpoint_iterations)
            break;

        if (m_enable_staging && m_use_axl && _flushed_to_pfs)
        {
            m_axl_id = -1;
            m_axl_id = AXL_Create(axl_xfer_type, "transfer_for_stage_out");
            // printf("AXL ID: %d\n", m_axl_id);
            if (m_axl_id == -1)
            {
                printf("AXL_Create() failed (error %d)\n", m_axl_id);
                return;
            }
            
            _flushed_to_pfs = false;
        }

        // emulate some computation
        std::this_thread::sleep_for(std::chrono::milliseconds((long) (m_checkpointing_interval * 1000)));
        for (int _file_id = 0; _file_id < m_num_ck_files_per_rank; _file_id++)
        {
            if (_world_rank < m_num_checkpointing_ranks)
            {
                std::string _file_path;
                unsigned int _current_time = std::chrono::duration_cast<std::chrono::nanoseconds>
                    (std::chrono::system_clock::now().time_since_epoch()).count();

                std::string _current_directory = m_enable_staging ? m_staging_directory : m_directory;

                _file_path = _current_directory + "/ckpt." +
                    std::to_string(_set_id) + "." +
                    std::to_string(_world_rank) + "." +
                    std::to_string(_file_id) + "." +
                    std::to_string(_current_time);

                utils::write_file(_file_path, m_block_size, m_segment_count);
                std::cout << "File written: " << _file_path << std::endl;

                if (m_enable_staging && m_use_axl)
                {
                    int rc = AXL_Add(m_axl_id, _file_path.c_str(), m_directory.c_str());
                    // printf("AXL ADD RC: %d\n", rc);
                    if (rc != AXL_SUCCESS) {
                        // printf("AXL_Add(..., %s, %s) failed (error %d)\n", "testFile", "testFile2", rc);
                        return;
                    }
                }
            }
        }

        _set_id++;

        if (m_enable_staging && m_use_axl && _set_id % m_stage_out_steps == 0)
        {
            // stage out the data in bb to pfs
            int rc = AXL_Dispatch(m_axl_id);
            // printf("AXL DISPATCH RC: %d\n", rc);
            if (rc != AXL_SUCCESS) {
                // printf("AXL_Dispatch() failed (error %d)\n", rc);
                return;
            }

            /* Wait for transfer to complete and finalize axl */
            rc = AXL_Wait(m_axl_id);
            // printf("AXL WAIT RC: %d\n", rc);
            if (rc != AXL_SUCCESS) {
                // printf("AXL_Wait() failed (error %d)\n", rc);
                return;
            }

            rc = AXL_Free(m_axl_id);
            // printf("AXL FREE RC: %d\n", rc);
            if (rc != AXL_SUCCESS) {
                // printf("AXL_Free() failed (error %d)\n", rc);
                return;
            }

            _flushed_to_pfs = true;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    DIR *d;
    struct dirent *dir;
    d = opendir(m_directory.c_str());
    std::string _file_path = "";
    std::string _file_name = "";
    if (d)
    {
        unsigned int _max_mod_time = 0;
        std::string _max_mod_filename = "";
        std::string _max_mod_filepath = "";
        while ((dir = readdir(d)) != NULL)
        {
            //exclude "." and ".."
            if( strcmp( dir->d_name, "." ) == 0 || 
            strcmp( dir->d_name, ".." ) == 0 ) 
            {
                continue;
            }
            std::string _path = m_directory + "/" + std::string(dir->d_name);
            struct stat attr;
            stat(_path.c_str(), &attr);
            unsigned int _time = (unsigned int) attr.st_mtime;
            // std::cout << "Rank: " << m_world_rank << ":" <<  "Finding last modified file path: general time " << _time << std::endl;
            if(_max_mod_time < _time)
            {
                _max_mod_time = _time;
                _max_mod_filename = std::string(dir->d_name);
                _max_mod_filepath = _path;
                // std::cout << "Rank: " << m_world_rank << ":" <<  "Finding last modified file path: max candidate " <<
                //     _max_mod_time << ", " << _max_mod_filepath << ", " <<
                //     _max_mod_filename << "," << std::endl;
            }
        }

        // std::cout << "Rank: " << m_world_rank << ":" <<  "Last modified file path: " << _max_mod_filepath << ", " <<
        // _max_mod_filename << std::endl;

        _file_path = _max_mod_filepath;
        _file_name = _max_mod_filename;
        closedir(d);
    }

    if(m_num_ck_files_per_rank < 2)
    {
        utils::read_file(_file_path, m_block_size, m_segment_count);
        std::cout << "File read: " << _file_path << std::endl;
    }
    else
    {
        std::vector<std::string> _file_path_tokens = tokenize_string("\\.", _file_name);
        std::string _max_set_id = "0";
        std::string _max_world_rank = "0";

        if(_file_path_tokens.size() > 2)
        {
            _max_set_id = _file_path_tokens[1];
            _max_world_rank = _file_path_tokens[2];
        }

        // std::cout << "Rank: " << m_world_rank << ":" <<  "Last modified file set id and rank: " << _max_set_id << ", " <<
        // _max_world_rank << std::endl;

        std::vector<std::string> _file_path_list;
        DIR *_d;
        struct dirent *_dir;
        _d = opendir(m_directory.c_str());
        std::string _file_path = "";
        if (_d)
        {
            while ((_dir = readdir(_d)) != NULL)
            {
                //exclude ".", ".." and "!ckpt.<max_set_id>.<max_world_rank>."
                std::string _regex_str = "ckpt." + _max_set_id + "." + _max_world_rank + ".";
                if( strcmp( _dir->d_name, "." ) == 0 ||
                strcmp( _dir->d_name, ".." ) == 0 ||
                tokenize_string(_regex_str, std::string(_dir->d_name)).size() <= 1)
                {
                    continue;
                }
                std::string _path = m_directory + "/" + std::string(_dir->d_name);
                _file_path_list.push_back(_path);
                // std::cout << "Rank: " << m_world_rank << ":" <<  "File path to read: " << _path << std::endl;
            }
            closedir(_d);
        }
        // std::cout << "Rank: " << m_world_rank << ":" <<  "Size of file path list: " << _file_path_list.size() << std::endl;
        for (auto _file_path_list_element : _file_path_list)
        {
            utils::read_file(_file_path_list_element, m_block_size, m_segment_count);
            std::cout << "File read: " << _file_path << std::endl;
        }
    }

    if (g_profiler.m_enabled)
    {
        g_profiler.write_to_file();
    }
    MPI_Finalize();
}

std::vector<std::string> checkpoint_restart::tokenize_string(std::string in_regex_str,
    std::string in_string)
{
    std::regex _regex(in_regex_str);
    std::sregex_token_iterator _iter(in_string.begin(), in_string.end(), _regex, -1);
    std::sregex_token_iterator _end;
    std::vector<std::string> _string_tokens(_iter, _end);
    return _string_tokens;
}

int checkpoint_restart::test_axl()
{
    printf("checkpoint_restart::test_axl started\n");
    int rc = -1;
    rc = AXL_Init(NULL);
    printf("AXL INIT RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Init() failed (error %d)\n", rc);
        return rc;
    }

    int id = -1;
    id = AXL_Create(AXL_XFER_ASYNC_BBAPI, "test_axl");
    printf("AXL ID: %d\n", id);
    if (id == -1) {
        printf("AXL_Create() failed (error %d)\n", id);
        return id;
    }

    rc = AXL_Add(id, "/p/gpfs1/chowdhur/io_playground/testFile", "/p/gpfs1/chowdhur/io_playground/testFile2");
    printf("AXL ADD RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Add(..., %s, %s) failed (error %d)\n", "testFile", "testFile2", rc);
        return rc;
    }

    rc = AXL_Dispatch(id);
    printf("AXL DISPATCH RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Dispatch() failed (error %d)\n", rc);
        return rc;
    }

    /* Wait for transfer to complete and finalize axl */
    rc = AXL_Wait(id);
    printf("AXL WAIT RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Wait() failed (error %d)\n", rc);
        return rc;
    }

    rc = AXL_Free(id);
    printf("AXL FREE RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Free() failed (error %d)\n", rc);
        return rc;
    }

    rc = AXL_Finalize();
    printf("AXL FINALIZE RC: %d\n", rc);
    if (rc != AXL_SUCCESS) {
        printf("AXL_Finalize() failed (error %d)\n", rc);
        return rc;
    }
    return rc;
}
