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
    static bool file_ready(std::string filepath, int block_size, int segment_count);
    static void create_groups_from_dag();
};

#endif // __UTILS_HPP__
