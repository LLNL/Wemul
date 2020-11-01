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

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <vector>

class utils
{
public:
    // recursively seek all the file paths in the member directory
    static void recursive_listfiles(const std::string& directory,
        std::vector<std::string>& filepath_list,
        const bool show_hidden_entries = false);

    static void read_file(std::string filepath);
    static void read_file(std::string filepath, int block_size, int segment_count);
    static void write_file(std::string filepath, int block_size, int segment_count, bool fsync=true);
    static bool file_ready(std::string filepath, int block_size, int segment_count);
    static void read_or_write(std::string filepath, int block_size, int segment_count,
        bool write = false, bool read_check=true, bool fsync=true);
};

#endif // __UTILS_HPP__
