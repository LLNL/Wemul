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

#ifndef __DEEP_LEARNING_HPP__
#define __DEEP_LEARNING_HPP__

#include <memory>
#include <string>
#include <cstring>
#include <vector>

#include "base_io_api.hpp"
#include "dataflow_workload.hpp"

// TODO: Rename it to Distributed DL IO later?
class deep_learning : public dataflow_workload
{
public:
    deep_learning(std::shared_ptr<base_io_api> io_api,
        std::string directory = ".", bool use_ior = false,
        int num_epochs = 1, int comp_time_per_epoch = 2);
    virtual ~deep_learning() override;
    virtual void emulate(int argc, char** argv) override;
    void set_directory(std::string directory);

private:
    void populate_list_of_all_filepaths();

    std::shared_ptr<base_io_api> m_io_api;
    std::string m_directory;
    std::vector<std::string> m_filepath_list;
    std::vector<int> m_file_ids;
    bool m_use_ior;
    int m_num_epochs;
    int m_comp_time_per_epoch;
};

#endif // __DEEP_LEARNING_HPP__
