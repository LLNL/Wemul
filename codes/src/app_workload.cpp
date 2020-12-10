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

#include <fstream>
#include <iostream>
#include <sstream>

#include "app_workload.hpp"
#include "profiler.hpp"

extern profiler g_profiler;

app_workload::app_workload(
        std::shared_ptr<base_io_api> io_api,
        std::string directory,
        int block_size,
        int segment_count,
        std::string read_input_dirs,
        std::string write_input_dirs,
        std::string read_filenames,
        std::string write_filenames,
        int read_block_size,
        int read_segment_count,
        int write_block_size,
        int write_segment_count,
        file_access_types read_access_type,
        file_access_types write_access_type)
{
    m_io_api = io_api;
    m_directory = directory;
    m_read_input_dirs = read_input_dirs;
    m_write_input_dirs = write_input_dirs;
    m_read_filenames = read_filenames;
    m_write_filenames = write_filenames;
    m_block_size = block_size;
    m_segment_count = segment_count;
    m_read_block_size = read_block_size;
    m_read_segment_count = read_segment_count;
    m_write_block_size = write_block_size;
    m_write_segment_count = write_segment_count;
    m_read_access_type = read_access_type;
    m_write_access_type = write_access_type;
    prepare_input_dir_list();
    prepare_filename_list();
}

app_workload::~app_workload()
{
}

void app_workload::emulate(int argc, char** argv)
{
    initialize_MPI(argc, argv);

    // reading the files
    int _block_size = (m_read_block_size > 0) ? m_read_block_size : m_block_size;
    int _segment_count = (m_read_segment_count > 0) ? m_read_segment_count : m_segment_count;
    if (m_read_access_type == file_access_types::e_shared_file)
    {
        int input_dir_id = 0;
        for (std::string _filename : m_read_filename_list)
        {
            std::string _directory =
                (m_read_input_dir_list.size() > input_dir_id)
                ? m_read_input_dir_list[input_dir_id] : m_directory;
            std::string _file_path = _directory + "/" + _filename;
            m_io_api->read_or_write(_file_path, _block_size, _segment_count);
            input_dir_id++;
        }
    }
    else if (m_read_access_type == file_access_types::e_file_per_process)
    {
        for (int rank = 0; rank < m_world_size; rank++)
        {
            if (m_world_rank == rank)
            {
                int _fileid = rank%m_world_size;
                std::string _directory =
                    (m_read_input_dir_list.size() > _fileid)
                    ? m_read_input_dir_list[_fileid] : m_directory;
                std::string _file_path = _directory + "/" + m_read_filename_list[rank%m_world_size];
                m_io_api->read_or_write(_file_path, _block_size, _segment_count);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // writing the files
    _block_size = (m_write_block_size > 0) ? m_write_block_size : m_block_size;
    _segment_count = (m_write_segment_count > 0) ? m_write_segment_count : m_segment_count;
    if (m_write_access_type == file_access_types::e_shared_file)
    {
        int input_dir_id = 0;
        for (std::string _filename : m_write_filename_list)
        {
            // std::cout << "updating directory" << std::endl;
            // std::cout << "dir list size: " << m_write_input_dir_list.size() <<  ", filename list size: " << m_write_filename_list.size() << std::endl;
            // std::cout << "index of input dir: " << input_dir_id << std::endl;
            std::string _directory =
                (m_write_input_dir_list.size() > input_dir_id)
                ? m_write_input_dir_list[input_dir_id] : m_directory;
            // std::cout << "done updating directory: " << _directory << std::endl;
            std::string _file_path = _directory + "/" + _filename;
            m_io_api->read_or_write(_file_path, _block_size, _segment_count, true);
            input_dir_id++;
        }
    }
    else if (m_write_access_type == file_access_types::e_file_per_process)
    {
        for (int rank = 0; rank < m_world_size; rank++)
        {
            if (m_world_rank == rank)
            {
                int _fileid = rank%m_world_size;
                std::string _directory =
                    (m_write_input_dir_list.size() > _fileid)
                    ? m_write_input_dir_list[_fileid] : m_directory;
                std::string _file_path = _directory + "/" + m_write_filename_list[rank%m_world_size];
                m_io_api->read_or_write(_file_path, _block_size, _segment_count, true);
            }
        }
    }

    finalize_MPI();
}

void app_workload::prepare_input_dir_list()
{
    // std::cout << "Preparing input dirs list" << std::endl;
    std::stringstream _read_input_dirs_stream(m_read_input_dirs);
    std::string _input_dir;
    char _separator = ':';
    while(std::getline(_read_input_dirs_stream, _input_dir, _separator))
    {
        m_read_input_dir_list.push_back(_input_dir);
    }
    std::stringstream _write_input_dirs_stream(m_write_input_dirs);
    while(std::getline(_write_input_dirs_stream, _input_dir, _separator))
    {
        m_write_input_dir_list.push_back(_input_dir);
    }
    // for (auto dirname : m_write_input_dir_list)
    // {
    //    std::cout << dirname << std::endl;
    // }
    // std::cout << "Done preparing input dir list" << std::endl;
}

void app_workload::prepare_filename_list()
{
    std::stringstream _read_filenames_stream(m_read_filenames);
    std::string _filename;
    char _separator = ':';
    while(std::getline(_read_filenames_stream, _filename, _separator))
    {
        m_read_filename_list.push_back(_filename);
    }
    std::stringstream _write_filenames_stream(m_write_filenames);
    while(std::getline(_write_filenames_stream, _filename, _separator))
    {
        m_write_filename_list.push_back(_filename);
    }
}

void app_workload::initialize_MPI(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    g_profiler.m_timer.adjust_time_deviation();
    MPI_Comm_size(MPI_COMM_WORLD, &m_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &m_world_rank);
}

void app_workload::finalize_MPI()
{
    if (g_profiler.m_enabled)
    {
        g_profiler.write_to_file();
    }
    MPI_Finalize();
}
