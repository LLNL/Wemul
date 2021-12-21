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

#ifndef __APP_WORKLOAD_HPP__
#define __APP_WORKLOAD_HPP__

#include <memory>
#include <mpi.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "base_io_api.hpp"
#include "dataflow_workload.hpp"
#include "types.hpp"


class app_workload : public dataflow_workload
{
public:
    app_workload(
        std::shared_ptr<base_io_api> io_api,
        std::string directory,
        int block_size,
        int segment_count,
        std::string read_input_dirs,
        std::string write_input_dirs,
        std::string read_filenames,
        std::string write_filenames,
        std::string read_block_size_str = "",
        std::string read_segment_count_str = "",
        std::string write_block_size_str = "",
        std::string write_segment_count_str = "",
        file_access_types read_access_type = file_access_types::e_custom,
        file_access_types write_access_type = file_access_types::e_custom,
        std::string num_procs_per_file_read = "",
        std::string num_procs_per_file_write = "",
        std::string ranks_per_file_read = "",
        std::string ranks_per_file_write = "");

    virtual ~app_workload() override;
    virtual void emulate(int argc, char** argv) override;

private:
    std::shared_ptr<base_io_api> m_io_api;
    // TODO: make it directory list for heterogeneous storage
    //       or take list of absolute file paths and extract directory
    //       now all the files are read/written in the same mount point
    std::string m_directory;
    int m_block_size;
    int m_segment_count;
    int m_world_rank;
    int m_world_size;

    // Colon separated input directories
    std::string m_read_input_dirs;
    // Colon separated filenames
    std::string m_read_filenames;
    std::string m_read_block_size_str;
    std::string m_read_segment_count_str;
    std::vector<int> m_read_block_sizes;
    std::vector<int> m_read_segment_counts;
    file_access_types m_read_access_type;
    std::vector<std::string> m_read_input_dir_list;
    std::vector<std::string> m_read_filename_list;

    std::string m_write_input_dirs;
    std::string m_write_filenames;
    std::string m_write_block_size_str;
    std::string m_write_segment_count_str;
    std::vector<int> m_write_block_sizes;
    std::vector<int> m_write_segment_counts;
    file_access_types m_write_access_type;
    std::vector<std::string> m_write_input_dir_list;
    std::vector<std::string> m_write_filename_list;
    std::string m_num_procs_per_file_read;
    std::string m_num_procs_per_file_write;
    std::vector<int> m_num_procs_per_file_read_list;
    std::vector<int> m_num_procs_per_file_write_list;
    std::string m_ranks_per_file_read;
    std::string m_ranks_per_file_write;
    std::unordered_map<std::string, std::vector<int>> m_read_data_to_tasks;
    std::unordered_map<std::string, std::vector<int>> m_write_data_to_tasks;
    std::vector<int> m_task_to_num_read;
    std::vector<int> m_task_to_num_write;
    std::unordered_map<std::string, std::unordered_set<int>> m_non_strict_data_ranks;

    MPI_Group m_world_group;
    std::unordered_map<std::string, MPI_Comm> m_read_comms;
    std::unordered_map<std::string, MPI_Comm> m_write_comms;

    void prepare_input_dir_list();
    void prepare_filename_list();
    void prepare_filesize_lists();
    void prepare_file_access_lists();
    void initialize_MPI(int argc, char** argv);
    void finalize_MPI();
};

#endif // __APP_WORKLOAD_HPP__
