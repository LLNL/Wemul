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

#ifndef __CHECKPOINT_RESTART_HPP__
#define __CHECKPOINT_RESTART_HPP__

#include <string>
#include <vector>

#include "dataflow_workload.hpp"
#include "types.hpp"

class checkpoint_restart : public dataflow_workload
{
public:
    checkpoint_restart(std::string directory,
        std::string staging_directory = ".",
        int num_checkpointing_ranks = 1,
        int num_ck_files_per_rank = 1,
        int block_size = 1000000,
        int segment_count = 100,
        double checkpointing_interval = 0.5,
        int stage_out_steps = 10,
        bool enable_staging = false,
        bool use_axl = false,
        float ck_error_rate = 20.0,
        int num_checkpoint_iterations = -1,
        axl_xfer_types axl_xfer_type = axl_xfer_types::e_default);

    virtual ~checkpoint_restart() override;
    virtual void emulate(int argc, char** argv) override;

private:
    std::string m_directory;
    std::string m_staging_directory;
    int m_num_checkpointing_ranks;
    int m_num_ck_files_per_rank;
    int m_block_size;
    int m_segment_count;
    double m_checkpointing_interval;
    int m_stage_out_steps;
    bool m_enable_staging;
    bool m_use_axl;
    float m_ck_error_rate;
    int m_num_checkpoint_iterations;

    int m_axl_id;
    axl_xfer_types m_axl_xfer_type;

    // TODO: move to a utility class
    std::vector<std::string> tokenize_string(std::string in_regex_str, std::string in_string);
    void read_file(std::string filepath);
    int test_axl();

    // debug variable
    int m_world_rank;
};

#endif // __CHECKPOINT_RESTART_HPP__
