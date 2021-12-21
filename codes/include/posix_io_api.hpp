/*
 * Copyright (c) 2021, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2021, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

#ifndef __POSIX_IO_API_HPP__
#define __POSIX_IO_API_HPP__

#include "base_io_api.hpp"

class posix_io_api : public base_io_api
{
public:
    posix_io_api();
    virtual ~posix_io_api() override;

    virtual void read_file(std::string filepath) override;
    virtual void read_file(std::string filepath, int block_size,
        int segment_count) override;
    virtual void write_file(std::string filepath, int block_size,
        int segment_count, bool is_fsync=true) override;
    virtual void read_or_write(std::string filepath, int block_size,
        int segment_count, bool is_write = false, bool read_check=true,
        bool is_fsync=true) override;
};

#endif // __POSIX_IO_API_HPP__
