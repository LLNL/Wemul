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

#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__

#include <string>
#include <vector>

#include "timer.hpp"

class profiler
{
public:
    static int m_rank;
    static bool m_enabled;
    static timer m_timer;
    static std::string m_out_filepath;
    static std::vector<std::pair<double, double>> m_wait_intervals;
    static std::vector<std::pair<double, double>> m_read_intervals;
    static std::vector<std::pair<double, double>> m_write_intervals;
    static unsigned long long m_total_bytes_read;
    static unsigned long long m_total_bytes_written;
    static void write_to_file();
};

#endif // __PROFILER_HPP__
