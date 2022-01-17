/*
 * Copyright (c) 2022, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2022, Florida State University. Contributions from
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
        std::string read_block_size_str,
        std::string read_segment_count_str,
        std::string write_block_size_str,
        std::string write_segment_count_str,
        file_access_types read_access_type,
        file_access_types write_access_type,
        std::string num_procs_per_file_read,
        std::string num_procs_per_file_write,
        std::string ranks_per_file_read,
        std::string ranks_per_file_write)
{
    m_io_api = io_api;
    m_directory = directory;
    m_read_input_dirs = read_input_dirs;
    m_write_input_dirs = write_input_dirs;
    m_read_filenames = read_filenames;
    m_write_filenames = write_filenames;
    m_block_size = block_size;
    m_segment_count = segment_count;
    m_read_block_size_str = read_block_size_str;
    m_read_segment_count_str = read_segment_count_str;
    m_write_block_size_str = write_block_size_str;
    m_write_segment_count_str = write_segment_count_str;
    m_read_access_type = read_access_type;
    m_write_access_type = write_access_type;
    m_num_procs_per_file_read = num_procs_per_file_read;
    m_num_procs_per_file_write = num_procs_per_file_write;
    m_ranks_per_file_read = ranks_per_file_read;
    m_ranks_per_file_write = ranks_per_file_write;
}

app_workload::~app_workload()
{
}

void app_workload::emulate(int argc, char** argv)
{
    initialize_MPI(argc, argv);

    // reading the files
    if (m_read_access_type == file_access_types::e_shared_file)
    {
        int input_dir_id = 0;
        for (std::string _filename : m_read_filename_list)
        {
            int read_block_size_id = (input_dir_id < m_read_block_sizes.size()) ? input_dir_id :
                                                                m_read_block_sizes.size() - 1;
            int read_segment_count_id = (input_dir_id < m_read_segment_counts.size()) ?
                                                input_dir_id : m_read_segment_counts.size() - 1;
            int _block_size = (read_block_size_id < 0) ? m_block_size :
                                            m_read_block_sizes[read_block_size_id];
            int _segment_count = (read_segment_count_id < 0) ? m_segment_count :
                                            m_read_segment_counts[read_segment_count_id];
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
                int read_block_size_id = (_fileid < m_read_block_sizes.size()) ? _fileid :
                                                                m_read_block_sizes.size() - 1;
                int read_segment_count_id = (_fileid < m_read_segment_counts.size()) ?
                                                    _fileid : m_read_segment_counts.size() - 1;
                int _block_size = (read_block_size_id < 0) ? m_block_size :
                                                m_read_block_sizes[read_block_size_id];
                int _segment_count = (read_segment_count_id < 0) ? m_segment_count :
                                                m_read_segment_counts[read_segment_count_id];
                std::string _file_path = _directory + "/" + m_read_filename_list[rank%m_world_size];
                m_io_api->read_or_write(_file_path, _block_size, _segment_count);
            }
        }
    }
    else if (m_read_access_type == file_access_types::e_custom)
    {
        // read according to how the file access pattern is specified in m_read_comms
        int input_dir_id = 0;
        for (std::string _filename : m_read_filename_list)
        {
            int read_block_size_id = (input_dir_id < m_read_block_sizes.size()) ? input_dir_id :
                                                                m_read_block_sizes.size() - 1;
            int read_segment_count_id = (input_dir_id < m_read_segment_counts.size()) ?
                                                input_dir_id : m_read_segment_counts.size() - 1;
            int _block_size = (read_block_size_id < 0) ? m_block_size :
                                            m_read_block_sizes[read_block_size_id];
            int _segment_count = (read_segment_count_id < 0) ? m_segment_count :
                                            m_read_segment_counts[read_segment_count_id];
            std::string _directory =
                (m_read_input_dir_list.size() > input_dir_id)
                ? m_read_input_dir_list[input_dir_id] : m_directory;
            std::string _file_path = _directory + "/" + _filename;

            MPI_Comm _mpi_read_comm = MPI_COMM_NULL;
            if (m_read_comms.find(_filename) != m_read_comms.end())
                _mpi_read_comm = m_read_comms[_filename];
            if (MPI_COMM_NULL != _mpi_read_comm)
            {
                bool _read_check = (m_non_strict_data_ranks[_filename].find(m_world_rank) == m_non_strict_data_ranks[_filename].end());
                m_io_api->read_or_write(_file_path, _block_size, _segment_count, false, _read_check);
            }
            input_dir_id++;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // writing the files
    if (m_write_access_type == file_access_types::e_shared_file)
    {
        int input_dir_id = 0;
        for (std::string _filename : m_write_filename_list)
        {
            int write_block_size_id = (input_dir_id < m_write_block_sizes.size()) ? input_dir_id :
                                                                m_write_block_sizes.size() - 1;
            int write_segment_count_id = (input_dir_id < m_write_segment_counts.size()) ?
                                                input_dir_id : m_write_segment_counts.size() - 1;
            int _block_size = (write_block_size_id < 0) ? m_block_size :
                                            m_write_block_sizes[write_block_size_id];
            int _segment_count = (write_segment_count_id < 0) ? m_segment_count :
                                            m_write_segment_counts[write_segment_count_id];
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
                int write_block_size_id = (_fileid < m_write_block_sizes.size()) ? _fileid :
                                                                m_write_block_sizes.size() - 1;
                int write_segment_count_id = (_fileid < m_write_segment_counts.size()) ?
                                                    _fileid : m_write_segment_counts.size() - 1;
                int _block_size = (write_block_size_id < 0) ? m_block_size :
                                                m_write_block_sizes[write_block_size_id];
                int _segment_count = (write_segment_count_id < 0) ? m_segment_count :
                                                m_write_segment_counts[write_segment_count_id];
                std::string _file_path = _directory + "/" + m_write_filename_list[rank%m_world_size];
                m_io_api->read_or_write(_file_path, _block_size, _segment_count, true);
            }
        }
    }
    else if (m_write_access_type == file_access_types::e_custom)
    {
        // write according to how the file access pattern is specified in m_write_comms
        int input_dir_id = 0;
        for (std::string _filename : m_write_filename_list)
        {
            int write_block_size_id = (input_dir_id < m_write_block_sizes.size()) ? input_dir_id :
                                                                m_write_block_sizes.size() - 1;
            int write_segment_count_id = (input_dir_id < m_write_segment_counts.size()) ?
                                                input_dir_id : m_write_segment_counts.size() - 1;
            int _block_size = (write_block_size_id < 0) ? m_block_size :
                                            m_write_block_sizes[write_block_size_id];
            int _segment_count = (write_segment_count_id < 0) ? m_segment_count :
                                            m_write_segment_counts[write_segment_count_id];
            // std::cout << "updating directory" << std::endl;
            // std::cout << "dir list size: " << m_write_input_dir_list.size() <<  ", filename list size: " << m_write_filename_list.size() << std::endl;
            // std::cout << "index of input dir: " << input_dir_id << std::endl;
            std::string _directory =
                (m_write_input_dir_list.size() > input_dir_id)
                ? m_write_input_dir_list[input_dir_id] : m_directory;
            // std::cout << "done updating directory: " << _directory << std::endl;
            std::string _file_path = _directory + "/" + _filename;
            MPI_Comm _mpi_write_comm = MPI_COMM_NULL;
            if (m_write_comms.find(_filename) != m_write_comms.end())
                _mpi_write_comm = m_write_comms[_filename];
            if (MPI_COMM_NULL != _mpi_write_comm)
            {
                // std::cout << _file_path << " " << _block_size << " " << _segment_count << std::endl;
                bool _read_check = (m_non_strict_data_ranks[_filename].find(m_world_rank) == m_non_strict_data_ranks[_filename].end());
                m_io_api->read_or_write(_file_path, _block_size, _segment_count, true, _read_check);
            }
            input_dir_id++;
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
    if (m_read_input_dir_list.size() < m_world_size &&
        m_read_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_read_input_dir_list.size();
        for (int i = m_read_input_dir_list.size(); i < m_world_size; i++)
        {
            m_read_input_dir_list.push_back(m_read_input_dir_list[orig_size-1]);
        }
    }

    std::stringstream _write_input_dirs_stream(m_write_input_dirs);
    while(std::getline(_write_input_dirs_stream, _input_dir, _separator))
    {
        m_write_input_dir_list.push_back(_input_dir);
    }
    if (m_write_input_dir_list.size() < m_world_size &&
        m_write_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_write_input_dir_list.size();
        for (int i = m_write_input_dir_list.size(); i < m_world_size; i++)
        {
            m_write_input_dir_list.push_back(m_write_input_dir_list[orig_size-1]);
        }
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
    if (m_read_filename_list.size() < m_world_size &&
        m_read_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_read_filename_list.size();
        for (int i = m_read_filename_list.size(); i < m_world_size; i++)
        {
            m_read_filename_list.push_back(
                m_read_filename_list[orig_size-1] + "." + std::to_string(i));
        }
    }
    std::stringstream _write_filenames_stream(m_write_filenames);
    while(std::getline(_write_filenames_stream, _filename, _separator))
    {
        m_write_filename_list.push_back(_filename);
    }
    if (m_write_filename_list.size() < m_world_size &&
        m_write_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_write_filename_list.size();
        for (int i = m_write_filename_list.size(); i < m_world_size; i++)
        {
            m_write_filename_list.push_back(
                m_write_filename_list[orig_size-1] + "." + std::to_string(i));
        }
    }
}

void app_workload::prepare_filesize_lists()
{
    char _separator = ':';
    std::string _block_size_str;
    std::string _segment_count_str;

    std::stringstream _read_block_size_str_stream(m_read_block_size_str);
    while(std::getline(_read_block_size_str_stream, _block_size_str, _separator))
    {
        m_read_block_sizes.push_back(std::stoi(_block_size_str));
    }
    if (m_read_block_sizes.size() < m_world_size &&
        m_read_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_read_block_sizes.size();
        for (int i = m_read_block_sizes.size(); i < m_world_size; i++)
        {
            m_read_block_sizes.push_back(m_read_block_sizes[orig_size-1]);
        }
    }
    std::stringstream _read_segment_count_str_stream(m_read_segment_count_str);
    while(std::getline(_read_segment_count_str_stream, _segment_count_str, _separator))
    {
        m_read_segment_counts.push_back(std::stoi(_segment_count_str));
    }
    if (m_read_segment_counts.size() < m_world_size &&
        m_read_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_read_segment_counts.size();
        for (int i = m_read_segment_counts.size(); i < m_world_size; i++)
        {
            m_read_segment_counts.push_back(m_read_segment_counts[orig_size-1]);
        }
    }
    std::stringstream _write_block_size_str_stream(m_write_block_size_str);
    // std::cout << m_write_block_size_str << std::endl;
    while(std::getline(_write_block_size_str_stream, _block_size_str, _separator))
    {
        m_write_block_sizes.push_back(std::stoi(_block_size_str));
    }
    if (m_write_block_sizes.size() < m_world_size &&
        m_write_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_write_block_sizes.size();
        for (int i = m_write_block_sizes.size(); i < m_world_size; i++)
        {
            m_write_block_sizes.push_back(m_write_block_sizes[orig_size-1]);
        }
    }
    std::stringstream _write_segment_count_str_stream(m_write_segment_count_str);
    // std::cout << m_write_segment_count_str << std::endl;
    while(std::getline(_write_segment_count_str_stream, _segment_count_str, _separator))
    {
        m_write_segment_counts.push_back(std::stoi(_segment_count_str));
    }
    if (m_write_segment_counts.size() < m_world_size &&
        m_write_access_type == file_access_types::e_file_per_process)
    {
        int orig_size = m_write_segment_counts.size();
        for (int i = m_write_segment_counts.size(); i < m_world_size; i++)
        {
            m_write_segment_counts.push_back(m_write_segment_counts[orig_size-1]);
        }
    }
}

void app_workload::prepare_file_access_lists()
{
    char _separator = ':', _internal_separator = ',', _dependency_separator = '-';
    std::string _procs_per_file_read;
    std::stringstream _procs_per_file_read_stream(m_num_procs_per_file_read);
    while(std::getline(_procs_per_file_read_stream, _procs_per_file_read, _separator))
    {
        m_num_procs_per_file_read_list.push_back(std::stoi(_procs_per_file_read));
    }
    std::string _procs_per_file_write;
    std::stringstream _procs_per_file_write_stream(m_num_procs_per_file_write);
    while(std::getline(_procs_per_file_write_stream, _procs_per_file_write, _separator))
    {
        m_num_procs_per_file_write_list.push_back(std::stoi(_procs_per_file_write));
    }

    m_task_to_num_read.resize(m_world_size);
    int task_min_file_read = 0;
    int min_file_read = INT32_MAX;
    int read_fileid = 0;
    for (std::string filename : m_read_filename_list)
    {
        for (int count = 0; m_num_procs_per_file_read_list.size() && count < m_num_procs_per_file_read_list[read_fileid]; count++)
        {
            for (int task_id = 0; task_id < m_task_to_num_read.size(); task_id++)
            {
                if (min_file_read > m_task_to_num_read[task_id])
                {
                    min_file_read = m_task_to_num_read[task_id];
                    task_min_file_read = task_id;
                    if (m_task_to_num_read[task_id] == 0) break;
                }
            }
            m_read_data_to_tasks[filename].push_back(task_min_file_read);
            m_task_to_num_read[task_min_file_read]++;
        }
        read_fileid++;
    }

    m_task_to_num_write.resize(m_world_size);
    int task_min_file_write = 0;
    int min_file_write = INT32_MAX;
    int write_fileid = 0;
    for (std::string filename : m_write_filename_list)
    {
        for (int count = 0; m_num_procs_per_file_write_list.size() && count < m_num_procs_per_file_write_list[write_fileid]; count++)
        {
            for (int task_id = 0; task_id < m_task_to_num_write.size(); task_id++)
            {
                if (min_file_write > m_task_to_num_write[task_id])
                {
                    min_file_write = m_task_to_num_write[task_id];
                    task_min_file_write = task_id;
                    if (m_task_to_num_write[task_id] == 0) break;
                }
            }
            m_write_data_to_tasks[filename].push_back(task_min_file_write);
            m_task_to_num_write[task_min_file_write]++;
        }
        write_fileid++;
    }
    if (!m_read_data_to_tasks.size())
    {
        std::string _ranks_per_file_read;
        std::stringstream _ranks_per_file_read_stream(m_ranks_per_file_read);
        int fileid = 0;
        while(std::getline(_ranks_per_file_read_stream, _ranks_per_file_read, _separator))
        {
            std::stringstream _ranks_per_file_read_stream(_ranks_per_file_read);
            std::vector<int> _ranks_list;
            while(std::getline(_ranks_per_file_read_stream, _ranks_per_file_read, _internal_separator))
            {
                std::stringstream _ranks_per_file_read_stream(_ranks_per_file_read);
                int _tok_id = 0;
                int _current_rank = -1;
                while(std::getline(_ranks_per_file_read_stream, _ranks_per_file_read, _dependency_separator))
                {
                    if (_tok_id == 0)
                    {
                        _current_rank = std::stoi(_ranks_per_file_read);
                        _ranks_list.push_back(_current_rank);
                    }
                    else
                    {
                        dependency_types _current_dependency = (dependency_types) std::stoi(_ranks_per_file_read);
                        if(_current_dependency == dependency_types::e_non_strict)
                        {
                            if(m_non_strict_data_ranks.find(m_read_filename_list[fileid]) == m_non_strict_data_ranks.end())
                            {
                                m_non_strict_data_ranks[m_read_filename_list[fileid]] = std::unordered_set<int>();
                            }
                            m_non_strict_data_ranks[m_read_filename_list[fileid]].insert(_current_rank);
                        }
                    }
                    _tok_id++;
                }
            }
            m_read_data_to_tasks[m_read_filename_list[fileid]] = _ranks_list;
            fileid++;
        }
    }
    if (!m_write_data_to_tasks.size())
    {
        std::string _ranks_per_file_write;
        std::stringstream _ranks_per_file_write_stream(m_ranks_per_file_write);
        int fileid = 0;
        while(std::getline(_ranks_per_file_write_stream, _ranks_per_file_write, _separator))
        {
            std::stringstream _ranks_per_file_write_stream(_ranks_per_file_write);
            std::vector<int> _ranks_list;
            while(std::getline(_ranks_per_file_write_stream, _ranks_per_file_write, _internal_separator))
            {
                std::stringstream _ranks_per_file_write_stream(_ranks_per_file_write);
                int _tok_id = 0;
                int _current_rank = -1;
                while(std::getline(_ranks_per_file_write_stream, _ranks_per_file_write, _dependency_separator))
                {
                    if (_tok_id == 0)
                    {
                        _current_rank = std::stoi(_ranks_per_file_write);
                        _ranks_list.push_back(_current_rank);
                    }
                    else
                    {
                        dependency_types _current_dependency = (dependency_types) std::stoi(_ranks_per_file_write);
                        if(_current_dependency == dependency_types::e_non_strict)
                        {
                            if(m_non_strict_data_ranks.find(m_write_filename_list[fileid]) == m_non_strict_data_ranks.end())
                            {
                                m_non_strict_data_ranks[m_write_filename_list[fileid]] = std::unordered_set<int>();
                            }
                            m_non_strict_data_ranks[m_write_filename_list[fileid]].insert(_current_rank);
                        }
                    }
                    _tok_id++;
                }
            }
            m_write_data_to_tasks[m_write_filename_list[fileid]] = _ranks_list;
            fileid++;
        }
    }
}

void app_workload::initialize_MPI(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    g_profiler.m_timer.adjust_time_deviation();
    MPI_Comm_size(MPI_COMM_WORLD, &m_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &m_world_rank);
    MPI_Comm_group(MPI_COMM_WORLD, &m_world_group);

    g_profiler.m_rank = m_world_rank;

    // prepare the configuration space for current run
    prepare_input_dir_list();
    prepare_filename_list();
    prepare_filesize_lists();
    prepare_file_access_lists();

    for (auto _data_to_tasks_item : m_read_data_to_tasks)
    {
        std::string _data_id = _data_to_tasks_item.first;
        std::vector<int> _task_ranks = _data_to_tasks_item.second;
        int _group_size = _task_ranks.size();
        int *_task_ranks_array = new int[_group_size];
        MPI_Group _mpi_group;
        MPI_Comm _mpi_comm;
        int _id = 0;
        // std::cout << _data_id << std::endl;
        for (int _itr : _task_ranks)
        {
            _task_ranks_array[_id] = _itr;
            // std::cout << _itr << ","; 
            _id++;
        }
        // std::cout << std::endl;
        MPI_Group_incl(m_world_group, _group_size, _task_ranks_array, &_mpi_group);
        MPI_Comm_create_group(MPI_COMM_WORLD, _mpi_group, 0, &_mpi_comm);
        delete[] _task_ranks_array;
        m_read_comms[_data_id] = _mpi_comm;
    }

    for (auto _data_to_tasks_item : m_write_data_to_tasks)
    {
        std::string _data_id = _data_to_tasks_item.first;
        std::vector<int> _task_ranks = _data_to_tasks_item.second;
        int _group_size = _task_ranks.size();
        int *_task_ranks_array = new int[_group_size];
        MPI_Group _mpi_group;
        MPI_Comm _mpi_comm;
        int _id = 0;
        // std::cout << _data_id << std::endl;
        for (int _itr : _task_ranks)
        {
            _task_ranks_array[_id] = _itr;
            // std::cout << _itr << ",";
            _id++;
        }
        //std::cout << std::endl;
        MPI_Group_incl(m_world_group, _group_size, _task_ranks_array, &_mpi_group);
        MPI_Comm_create_group(MPI_COMM_WORLD, _mpi_group, 0, &_mpi_comm);
        delete[] _task_ranks_array;
        m_write_comms[_data_id] = _mpi_comm;
    }
}

void app_workload::finalize_MPI()
{
    if (g_profiler.m_enabled)
    {
        g_profiler.write_to_file();
    }
    MPI_Finalize();
}
