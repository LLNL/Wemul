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

#include <iostream>

#include "checkpoint_restart.hpp"
#include "dataflow_emulator.hpp"
#include "producer_consumer.hpp"
#include "deep_learning.hpp"
#include "dag_workload.hpp"
#include "app_workload.hpp"
#include "posix_io_api.hpp"
#include "mpi_io_api.hpp"

dataflow_emulator::dataflow_emulator(config_attributes* conf_attrib)
    : workflow_emulator()
{
    m_config_attributes = conf_attrib;
    m_io_api = create_io_api(m_config_attributes->m_io_api_type);
    m_dataflow_workload = create_dataflow_workload(m_config_attributes->m_dataflow_workload_type);
}

dataflow_emulator::~dataflow_emulator()
{
}

void dataflow_emulator::run(int argc, char** argv)
{
    m_dataflow_workload->emulate(argc, argv);
}

std::shared_ptr<base_io_api> dataflow_emulator::create_io_api(io_api_types io_api_type)
{
    switch(io_api_type)
    {
        case io_api_types::e_posixio:
            return std::make_shared<posix_io_api>();
        case io_api_types::e_mpiio:
            return std::make_shared<mpi_io_api>();
        default:
            return std::make_shared<mpi_io_api>();
    };
}

dataflow_workload* dataflow_emulator::create_dataflow_workload(dataflow_workload_types dataflow_workload_type)
{
    switch(dataflow_workload_type)
    {
        case dataflow_workload_types::e_checkpoint_restart:
            return new checkpoint_restart(m_io_api,
                m_config_attributes->m_directory,
                m_config_attributes->m_staging_directory,
                m_config_attributes->m_num_checkpointing_ranks,
                m_config_attributes->m_num_ck_files_per_rank,
                m_config_attributes->m_block_size,
                m_config_attributes->m_segment_count,
                m_config_attributes->m_checkpointing_interval,
                m_config_attributes->m_stage_out_steps,
                m_config_attributes->m_enable_staging,
                m_config_attributes->m_use_axl,
                m_config_attributes->m_ck_error_rate,
                m_config_attributes->m_num_checkpoint_iterations,
                m_config_attributes->m_axl_xfer_type);

        case dataflow_workload_types::e_producer_consumer:
            return new producer_consumer(m_io_api,
                m_config_attributes->m_directory,
                m_config_attributes->m_inter_node,
                m_config_attributes->m_num_ranks_per_node,
                m_config_attributes->m_producer_only,
                m_config_attributes->m_consumer_only,
                m_config_attributes->m_block_size,
                m_config_attributes->m_segment_count);

        case dataflow_workload_types::e_deep_learning:
            return new deep_learning(m_io_api,
                m_config_attributes->m_directory,
                m_config_attributes->m_use_ior,
                m_config_attributes->m_num_epochs,
                m_config_attributes->m_comp_time_per_epoch);

        case dataflow_workload_types::e_dag_workload:
            return new dag_workload(m_io_api,
                m_config_attributes->m_use_ior,
                m_config_attributes->m_directory,
                m_config_attributes->m_filename,
                m_config_attributes->m_dag_workload_type,
                m_config_attributes->m_dag_workload_access_type,
                m_config_attributes->m_dag_filepath,
                m_config_attributes->m_block_size,
                m_config_attributes->m_segment_count);

        case dataflow_workload_types::e_app_workload:
            return new app_workload(m_io_api,
                m_config_attributes->m_directory,
                m_config_attributes->m_block_size,
                m_config_attributes->m_segment_count,
                m_config_attributes->m_read_input_dirs,
                m_config_attributes->m_write_input_dirs,
                m_config_attributes->m_read_filenames,
                m_config_attributes->m_write_filenames,
                m_config_attributes->m_read_block_size_str,
                m_config_attributes->m_read_segment_count_str,
                m_config_attributes->m_write_block_size_str,
                m_config_attributes->m_write_segment_count_str,
                m_config_attributes->m_read_access_type,
                m_config_attributes->m_write_access_type,
                m_config_attributes->m_num_procs_per_file_read,
                m_config_attributes->m_num_procs_per_file_write,
                m_config_attributes->m_ranks_per_file_read,
                m_config_attributes->m_ranks_per_file_write);

        default:
            return nullptr;
    };
}
