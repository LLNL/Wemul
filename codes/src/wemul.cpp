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

#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#include "dataflow_emulator.hpp"
#include "profiler.hpp"
#include "types.hpp"

extern profiler g_profiler;

int main(int argc, char** argv)
{
    if(argc < 5)
    {
        std::cout << "command: emulator --type data --subtype pc\n";
        std::cout << "emulator types: data, etc.\n";
        std::cout << "emulator subtypes for data: checkpoint restart (cr), producer consumer (pc), deep learning (dl), etc.\n";
        std::cout << "for deep learning emulator: --input_dir <DIRECTORY>\n";
        return EXIT_FAILURE;
    }

    emulator_types _emulator_type = emulator_types::emulator_types_start;
    config_attributes* _config_attribs = new config_attributes();

    for (int i = 1; i < argc; i++)
    {
        std::string _arg(argv[i]);

        if(_arg == "--inter_node")
        {
            _config_attribs->m_inter_node = true;
            continue;
        }
        if(_arg == "--use_ior")
        {
            _config_attribs->m_use_ior = true;
            continue;
        }
        if(_arg == "--use_axl")
        {
            _config_attribs->m_use_axl = true;
            continue;
        }
        if(_arg == "--producer_only")
        {
            _config_attribs->m_producer_only = true;
            continue;
        }
        if(_arg == "--consumer_only")
        {
            _config_attribs->m_consumer_only = true;
            continue;
        }
        if(_arg == "--enable_staging")
        {
            _config_attribs->m_enable_staging = true;
            continue;
        }
        if(_arg == "--single_task_single_data")
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_single_task_single_data;
            continue;
        }
        if(_arg == "--single_task_multi_data")
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_single_task_multi_data;
            continue;
        }
        if(_arg == "--multi_task_single_data")
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_multi_task_single_data;
            continue;
        }
        if(_arg == "--multi_task_multi_data")
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_multi_task_multi_data;
            continue;
        }
        if(_arg == "--dag_read")
        {
            _config_attribs->m_dag_workload_access_type = workload_access_types::e_read;
            continue;
        }
        if(_arg == "--dag_write")
        {
            _config_attribs->m_dag_workload_access_type = workload_access_types::e_write;
            continue;
        }
        if(_arg == "--file_per_process_read")
        {
            _config_attribs->m_read_access_type = file_access_types::e_file_per_process;
            continue;
        }
        if(_arg == "--file_per_process_write")
        {
            _config_attribs->m_write_access_type = file_access_types::e_file_per_process;
            continue;
        }
        if(_arg == "--enable_profiler")
        {
            g_profiler.m_enabled = true;
            continue;
        }

        std::string _arg_val(argv[++i]);
        if (_arg == "--type")
        {
            if (_arg_val == "data")
            {
                _emulator_type = emulator_types::e_dataflow;
            }
        }
        if (_arg == "--subtype")
        {
            if (_arg_val == "cr")
            {
                _config_attribs->m_dataflow_workload_type = dataflow_workload_types::e_checkpoint_restart;
            }
            else if (_arg_val == "pc")
            {
                _config_attribs->m_dataflow_workload_type = dataflow_workload_types::e_producer_consumer;
            }
            else if (_arg_val == "dl")
            {
                _config_attribs->m_dataflow_workload_type = dataflow_workload_types::e_deep_learning;
            }
            else if (_arg_val == "dag")
            {
                _config_attribs->m_dataflow_workload_type = dataflow_workload_types::e_dag_workload;
            }
            else if (_arg_val == "app")
            {
                _config_attribs->m_dataflow_workload_type = dataflow_workload_types::e_app_workload;
            }
        }
        if (_arg == "--io_api")
        {
            std::string _api_name = _arg_val;
            if (_api_name == "posixio")
            {
                _config_attribs->m_io_api_type = io_api_types::e_posixio;
            }
            else if (_api_name == "mpiio")
            {
                _config_attribs->m_io_api_type = io_api_types::e_mpiio;
            }
            else
            {
                std::cout << "ERROR: Unsupported I/O API. Wemul supports 'posixio' and 'mpiio'" << std::endl;
                return EXIT_FAILURE;
            }
        }
        if (_arg == "--input_dir")
        {
            _config_attribs->m_directory = _arg_val;
        }
        if (_arg == "--filename")
        {
            _config_attribs->m_filename = _arg_val;
        }
        if (_arg == "--stage_dir")
        {
            _config_attribs->m_staging_directory = _arg_val;
        }
        if(_arg == "--ranks_per_node")
        {
            _config_attribs->m_num_ranks_per_node = std::stoi(_arg_val);
        }
        if(_arg == "--num_ck_ranks")
        {
            _config_attribs->m_num_checkpointing_ranks = std::stoi(_arg_val);
        }
        if(_arg == "--transfer_size")
        {
            _config_attribs->m_transfer_size = std::stoi(_arg_val);
        }
        if(_arg == "--block_size")
        {
            _config_attribs->m_block_size = std::stoi(_arg_val);
        }
        if(_arg == "--segment_count")
        {
            _config_attribs->m_segment_count = std::stoi(_arg_val);
        }
        if(_arg == "--num_ck_files_per_rank")
        {
            _config_attribs->m_num_ck_files_per_rank = std::stoi(_arg_val);
        }
        if(_arg == "--checkpointing_interval")
        {
            _config_attribs->m_checkpointing_interval = std::stod(_arg_val);
        }
        if(_arg == "--stage_out_steps")
        {
            _config_attribs->m_stage_out_steps = std::stoi(_arg_val);
        }
        if(_arg == "--ck_error_rate")
        {
            _config_attribs->m_ck_error_rate = std::stof(_arg_val);
        }
        if(_arg == "--num_ck_iter")
        {
            _config_attribs->m_num_checkpoint_iterations = std::stof(_arg_val);
        }
        if(_arg == "--axl_xfer_type")
        {
            _config_attribs->m_axl_xfer_type = (axl_xfer_types) std::stoi(_arg_val);
        }
        if(_arg == "--num_epochs")
        {
            _config_attribs->m_num_epochs = std::stoi(_arg_val);
        }
        if(_arg == "--comp_time_per_epoch")
        {
            _config_attribs->m_comp_time_per_epoch = std::stoi(_arg_val);
        }
        if(_arg == "--dag_file")
        {
            _config_attribs->m_dag_filepath = _arg_val;
        }
        if(_arg == "--read_input_dirs")
        {
            _config_attribs->m_read_input_dirs = _arg_val;
        }
        if(_arg == "--write_input_dirs")
        {
            _config_attribs->m_write_input_dirs = _arg_val;
        }
        if(_arg == "--read_filenames")
        {
            _config_attribs->m_read_filenames = _arg_val;
        }
        if(_arg == "--write_filenames")
        {
            _config_attribs->m_write_filenames = _arg_val;
        }
        if(_arg == "--read_block_size")
        {
            _config_attribs->m_read_block_size = std::stoi(_arg_val);
        }
        if(_arg == "--read_segment_count")
        {
            _config_attribs->m_read_segment_count = std::stoi(_arg_val);
        }
        if(_arg == "--write_block_size")
        {
            _config_attribs->m_write_block_size = std::stoi(_arg_val);
        }
        if(_arg == "--write_segment_count")
        {
            _config_attribs->m_write_segment_count = std::stoi(_arg_val);
        }
        if (_arg == "--profile_out_filepath")
        {
            g_profiler.m_enabled = true;
            g_profiler.m_out_filepath = _arg_val;
        }
        if (_arg == "--mpiio_type")
        {
            std::string _type = _arg_val;
            if (_type == "noncollective")
            {
                _config_attribs->m_mpiio_type = mpiio_types::e_non_collective;
            }
            else if (_type == "collective")
            {
                _config_attribs->m_mpiio_type = mpiio_types::e_collective;
            }
        }
    }

    workflow_emulator* _workflow_emulator = nullptr;
    if(_emulator_type == emulator_types::e_dataflow)
    {
        _workflow_emulator = new dataflow_emulator(_config_attribs);
    }

    _workflow_emulator->run(argc, argv);

    return EXIT_SUCCESS;
}
