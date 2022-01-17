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

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

void utils::recursive_listfiles(const std::string& directory,
    std::vector<std::string>& filepath_list,
    const bool show_hidden_entries)
{
    DIR *input_direntry;
    struct dirent *output_direntry;
    input_direntry = opendir(directory.c_str());
    // std::cout << "input dir: " << directory << std::endl;
    if (input_direntry != NULL)
    {
        while ((output_direntry = readdir(input_direntry)) != NULL)
        {
            // std::cout << "in the loop: " << DT_DIR << std::endl;
            // printf("%p %d \n", output_direntry, output_direntry->d_type);
            if(show_hidden_entries ?
                (output_direntry->d_type == DT_DIR 
                    && std::string(output_direntry->d_name) != ".."
                    && std::string(output_direntry->d_name) != "." )
                : (output_direntry->d_type == DT_DIR
                    && std::string(output_direntry->d_name) != ".."
                    && std::string(output_direntry->d_name) != "."
                    && std::string(output_direntry->d_name).rfind(".", 0) != 0 ))
            {
                // std::string _current_subdir = directory + "/" + output_direntry->d_name;
                // std::cout << "current subdir: " << _current_subdir << std::endl;
                recursive_listfiles(directory + "/" + output_direntry->d_name + "/", filepath_list, show_hidden_entries);
            }
            if(output_direntry->d_type == DT_REG)
            {
                // std::string _current_file = directory + "/" + output_direntry->d_name;
                // std::cout << "current file: " << _current_file << std::endl;
                filepath_list.push_back(directory + "/" + output_direntry->d_name);
            }
        }
    }

    closedir(input_direntry);
}

bool utils::file_ready(std::string filepath, int block_size, int segment_count)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    long _expected_filesize = (long) block_size * segment_count;
    while(true)
    {
        std::ifstream _file_stream(filepath, std::ios::binary | std::ios::ate);
        if (_file_stream.good() && _file_stream.tellg() == _expected_filesize)
        {
            std::cout << "File " << filepath << " is GOOD!" << std::endl;
            std::cout << "File size: " << _file_stream.tellg() << std::endl;
            break;
        }
    }
    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_wait_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
    return true;
}

/*
void utils::create_groups_from_dag()
{
    std::fstream _infile(m_dag_filepath, std::ios::in);
    if(!_infile.is_open())
    {
        std::cout << "File not found!" << std::endl;
        return;
    }
    std::string _line;
    bool _is_read = false;
    bool _is_non_strict = false;
    bool _is_stages = false;
    std::unordered_map<std::string, int> _task_id_to_rank;
    int _current_rank = 0;
    while(std::getline(_infile, _line))
    {
        if (_line == "READ")
        {
            _is_read = true;
            continue;
        }
        else if (_line == "WRITE")
        {
            _is_read = false;
            continue;
        }
        if (_line == "NONSTRICT")
        {
            _is_non_strict = true;
            continue;
        }
        if (_line == "STAGES")
        {
            _is_stages = true;
            continue;
        }
        if (!_is_non_strict && !_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _data_id;
            std::getline(_linestream, _data_id, ':');
            std::string _tasks_line;
            std::getline(_linestream, _tasks_line, ':');
            std::stringstream _tasks_linestream(_tasks_line);
            std::string _task_id;
            std::vector<int> _task_ranks;
            MPI_Group _mpi_group;
            MPI_Comm _mpi_comm;
            int _group_size = 0;
            while(std::getline(_tasks_linestream, _task_id, ','))
            {
                if(_task_id_to_rank.find(_task_id) == _task_id_to_rank.end())
                {
                    _task_id_to_rank[_task_id] = _current_rank;
                    _current_rank++;
                }
                _task_ranks.push_back(_task_id_to_rank[_task_id]);
                _group_size++;
            }
            int *_task_ranks_array = new int[_task_ranks.size()];
            int _id = 0;
            for (int _itr : _task_ranks)
            {
                _task_ranks_array[_id] = _itr;
                _id++;
            }
            MPI_Group_incl(m_world_group, _group_size, _task_ranks_array, &_mpi_group);
            MPI_Comm_create_group(MPI_COMM_WORLD, _mpi_group, 0, &_mpi_comm);
            delete[] _task_ranks_array;
            if(_is_read) m_read_comms[_data_id] = _mpi_comm;
            else m_write_comms[_data_id] = _mpi_comm;
        }
        else if (!_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _data_id;
            std::getline(_linestream, _data_id, ':');
            std::string _task_id;
            std::getline(_linestream, _task_id, ':');
            if(m_non_strict_data_ranks.find(_data_id) == m_non_strict_data_ranks.end())
            {
                m_non_strict_data_ranks[_data_id] = std::unordered_set<int>();
            }
            m_non_strict_data_ranks[_data_id].insert(_task_id_to_rank[_task_id]);
        }
        if (_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _stage_str;
            std::getline(_linestream, _stage_str, ':');
            int _stage = std::stoi(_stage_str);
            std::string _data_units_str;
            std::getline(_linestream, _data_units_str, ':');
            if(m_data_units_per_stage.find(_stage) == m_data_units_per_stage.end())
            {
                m_data_units_per_stage[_stage] = std::unordered_set<std::string>();
            }
            std::stringstream _data_units_stream(_data_units_str);
            std::string _data_id;
            while(std::getline(_data_units_stream, _data_id, ','))
            {
                m_data_units_per_stage[_stage].insert(_data_id);
            }
        }
    }
}
*/

