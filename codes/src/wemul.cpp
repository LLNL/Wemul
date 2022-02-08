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
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#include "dataflow_emulator.hpp"
#include "profiler.hpp"
#include "types.hpp"

extern profiler g_profiler;

int main(int argc, char** argv)
{
    // if(argc < 5)
    // {
    //     std::cout << "command: wemul --type data --subtype pc\n";
    //     std::cout << "emulation types: data, etc.\n";
    //     std::cout << "emulation subtypes for data: checkpoint restart (cr), producer consumer (pc), deep learning (dl), etc.\n";
    //     std::cout << "for deep learning emulation: --input_dir <DIRECTORY>\n";
    //     return EXIT_FAILURE;
    // }

    config_attributes* _config_attribs = new config_attributes();

    for (int i = 1; i < argc; i++)
    {
        std::string _arg(argv[i]);

        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_inter_node])
        {
            _config_attribs->m_inter_node = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_use_ior])
        {
            _config_attribs->m_use_ior = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_use_axl])
        {
            _config_attribs->m_use_axl = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_producer_only])
        {
            _config_attribs->m_producer_only = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_consumer_only])
        {
            _config_attribs->m_consumer_only = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_enable_staging])
        {
            _config_attribs->m_enable_staging = true;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_single_task_single_data])
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_dwt_single_task_single_data;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_single_task_multi_data])
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_dwt_single_task_multi_data;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_multi_task_single_data])
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_dwt_multi_task_single_data;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_multi_task_multi_data])
        {
            _config_attribs->m_dag_workload_type = dag_workload_types::e_dwt_multi_task_multi_data;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_dag_read])
        {
            _config_attribs->m_dag_workload_access_type = workload_access_types::e_read;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_dag_write])
        {
            _config_attribs->m_dag_workload_access_type = workload_access_types::e_write;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_file_per_process_read])
        {
            _config_attribs->m_read_access_type = file_access_types::e_file_per_process;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_file_per_process_write])
        {
            _config_attribs->m_write_access_type = file_access_types::e_file_per_process;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_shared_file_read])
        {
            _config_attribs->m_read_access_type = file_access_types::e_shared_file;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_shared_file_write])
        {
            _config_attribs->m_write_access_type = file_access_types::e_shared_file;
            continue;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_enable_profiler])
        {
            g_profiler.m_enabled = true;
            continue;
        }

        std::string _arg_val(argv[++i]);
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_config_file])
        {
            _config_attribs->m_config_file = _arg_val;
            _config_attribs->parse_config_file();
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_type])
        {
            if (_arg_val == "data")
            {
                _config_attribs->m_emulation_type = emulation_types::e_dataflow;
            }
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_subtype])
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
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_io_api])
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
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_input_dir])
        {
            _config_attribs->m_directory = _arg_val;
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_filename])
        {
            _config_attribs->m_filename = _arg_val;
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_stage_dir])
        {
            _config_attribs->m_staging_directory = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_ranks_per_node])
        {
            _config_attribs->m_num_ranks_per_node = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_ck_ranks])
        {
            _config_attribs->m_num_checkpointing_ranks = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_transfer_size])
        {
            _config_attribs->m_transfer_size = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_block_size])
        {
            _config_attribs->m_block_size = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_segment_count])
        {
            _config_attribs->m_segment_count = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_ck_files_per_rank])
        {
            _config_attribs->m_num_ck_files_per_rank = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_checkpointing_interval])
        {
            _config_attribs->m_checkpointing_interval = std::stod(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_stage_out_steps])
        {
            _config_attribs->m_stage_out_steps = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_ck_error_rate])
        {
            _config_attribs->m_ck_error_rate = std::stof(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_ck_iter])
        {
            _config_attribs->m_num_checkpoint_iterations = std::stof(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_axl_xfer_type])
        {
            _config_attribs->m_axl_xfer_type = (axl_xfer_types) std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_epochs])
        {
            _config_attribs->m_num_epochs = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_comp_time_per_epoch])
        {
            _config_attribs->m_comp_time_per_epoch = std::stoi(_arg_val);
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_dag_file])
        {
            _config_attribs->m_dag_filepath = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_read_input_dirs])
        {
            _config_attribs->m_read_input_dirs = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_write_input_dirs])
        {
            _config_attribs->m_write_input_dirs = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_read_filenames])
        {
            _config_attribs->m_read_filenames = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_write_filenames])
        {
            _config_attribs->m_write_filenames = _arg_val;
        }
        // --read/write_block_size, --read/write_segment_count can take
        // a list of block sizes, e.g., 1213:1233:2133:5456:...
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_read_block_size])
        {
            _config_attribs->m_read_block_size_str = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_read_segment_count])
        {
            _config_attribs->m_read_segment_count_str = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_write_block_size])
        {
            _config_attribs->m_write_block_size_str = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_write_segment_count])
        {
            _config_attribs->m_write_segment_count_str = _arg_val;
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_profile_out_filepath])
        {
            g_profiler.m_enabled = true;
            g_profiler.m_out_filepath = _arg_val;
        }
        if (_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_mpiio_type])
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
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_read])
        {
            // add the ':' separated string to the config attributes
            _config_attribs->m_num_procs_per_file_read = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_write])
        {
            // add the ':' separated string to the config attributes
            _config_attribs->m_num_procs_per_file_write = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_read])
        {
            // add the ':' then ',' separated string to the config attributes
            _config_attribs->m_ranks_per_file_read = _arg_val;
        }
        if(_arg == "--" + _config_attribs->m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_write])
        {
            // add the ':' then ',' separated string to the config attributes
            _config_attribs->m_ranks_per_file_write = _arg_val;
        }
    }

    workflow_emulator* _workflow_emulator = nullptr;
    if(_config_attribs->m_emulation_type == emulation_types::e_dataflow)
    {
        _workflow_emulator = new dataflow_emulator(_config_attribs);
    }

    _workflow_emulator->run(argc, argv);

    return EXIT_SUCCESS;
}
