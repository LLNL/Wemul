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

#include "base_io_api.hpp"

base_io_api::base_io_api()
{
}

base_io_api::~base_io_api()
{
}

void base_io_api::read_file(std::string filepath)
{
}

void base_io_api::read_file(std::string filepath, int block_size,
    int segment_count)
{
}

void base_io_api::write_file(std::string filepath, int block_size,
    int segment_count, bool is_fsync)
{
}

void base_io_api::read_or_write(std::string filepath, int block_size,
    int segment_count, bool is_write, bool read_check, bool is_fsync)
{
}
