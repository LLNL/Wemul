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

#include "profiler.hpp"

// declare global profiler
profiler g_profiler;

bool profiler::m_enabled = false;
timer profiler::m_timer = timer();
std::string profiler::m_out_filepath = "profiler.out";
std::vector<std::pair<double, double>> profiler::m_wait_intervals
    = std::vector<std::pair<double, double>>();
std::vector<std::pair<double, double>> profiler::m_read_intervals
    = std::vector<std::pair<double, double>>();
std::vector<std::pair<double, double>> profiler::m_write_intervals
    = std::vector<std::pair<double, double>>();
unsigned long long profiler::m_total_bytes_read = 0;
unsigned long long profiler::m_total_bytes_written = 0;

void profiler::write_to_file()
{
    std::ofstream _out_file;
    _out_file.open(m_out_filepath, std::ofstream::out | std::ofstream::app);
    if (_out_file.is_open())
    {
        int i = 0;
        for (auto _wait_interval : m_wait_intervals)
        {
            if (i == 0) _out_file << "WAIT:";
            _out_file << _wait_interval.first << "," << _wait_interval.second << ";";
            if (i == m_wait_intervals.size() - 1) _out_file << std::endl;
            i++;
        }
        i = 0;
        for (auto _read_interval : m_read_intervals)
        {
            if (i == 0) _out_file << "READ:";
            _out_file << _read_interval.first << "," << _read_interval.second << ";";
            if (i == m_read_intervals.size() - 1) _out_file << std::endl;
            i++;
        }
        i = 0;
        for (auto _write_interval : m_write_intervals)
        {
            if (i == 0) _out_file << "WRITE:";
            _out_file << _write_interval.first << "," << _write_interval.second << ";";
            if (i == m_write_intervals.size() - 1) _out_file << std::endl;
            i++;
        }
        _out_file << "BYTES:" << g_profiler.m_total_bytes_read << ","
            << g_profiler.m_total_bytes_written << std::endl;
        _out_file.close();
    }
    else
    {
        std::cout << "ERROR: cannot open profile output file";
    }
    
}
