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

#include "timer.hpp"

timer::timer()
{
}

timer::~timer()
{
}

double timer::get_current_time()
{
    double _current_time = 0;
    _current_time = MPI_Wtime();
    _current_time -= m_delta;
    return _current_time;
}

void timer::adjust_time_deviation()
{
    double _root_current_time, _current_time, _min_current_time, _max_current_time;
    MPI_Barrier(MPI_COMM_WORLD);
    _current_time = get_current_time();
    MPI_Reduce(&_current_time, &_min_current_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&_current_time, &_max_current_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    _root_current_time = _current_time;
    MPI_Bcast(&_root_current_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    m_delta = _current_time - _root_current_time;
    m_deviation = _max_current_time - _min_current_time;
}
