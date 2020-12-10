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
