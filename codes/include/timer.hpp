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

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <mpi.h>

class timer
{
public:
    timer();
    ~timer();
    double get_current_time();
    void adjust_time_deviation();

private:
    double m_delta;
    double m_deviation;
};

#endif // __TIMER_HPP__
