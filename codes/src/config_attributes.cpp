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
#include <algorithm>
#include <json/json.h>
#include <json/value.h>
#include <fstream>
#include <iostream>

#include "config_attributes.hpp"
#include "profiler.hpp"

extern profiler g_profiler;

config_attributes::config_attributes(
        emulation_types emulation_type,
        std::string config_file,
        dataflow_workload_types dataflow_workload_type,
        io_api_types io_api_type,
        std::string directory,
        std::string filename,
        std::string staging_directory,
        bool inter_node,
        bool use_ior,
        bool use_axl,
        bool producer_only,
        bool consumer_only,
        bool enable_staging,
        int num_ranks_per_node,
        int num_checkpointing_ranks,
        int transfer_size,
        int block_size,
        int segment_count,
        int num_ck_files_per_rank,
        double checkpointing_interval,
        int stage_out_steps,
        int checkpointing_buffer,
        float ck_error_rate,
        int num_checkpoint_iterations,
        axl_xfer_types axl_xfer_type,
        int num_epochs,
        int comp_time_per_epoch,
        dag_workload_types dag_workload_type,
        workload_access_types dag_workload_access_type,
        std::string dag_filepath,
        std::string read_input_dirs,
        std::string write_input_dirs,
        std::string read_filenames,
        std::string write_filenames,
        std::string read_block_size_str,
        std::string read_segment_count_str,
        std::string write_block_size_str,
        std::string write_segment_count_str,
        file_access_types read_access_type,
        file_access_types write_access_type,
        mpiio_types mpiio_type,
        std::string num_procs_per_file_read,
        std::string num_procs_per_file_write,
        std::string ranks_per_file_read,
        std::string ranks_per_file_write)
{
    construct_type_to_str_map();
    m_emulation_type = emulation_type;
    m_config_file = config_file;
    m_dataflow_workload_type = dataflow_workload_type;
    m_io_api_type = io_api_type;
    m_directory = directory;
    m_filename = filename;
    m_staging_directory = staging_directory;
    m_inter_node = inter_node;
    m_use_ior = use_ior;
    m_use_axl = use_axl;
    m_producer_only = producer_only;
    m_consumer_only = consumer_only;
    m_enable_staging = enable_staging;
    m_num_ranks_per_node = num_ranks_per_node;
    m_num_checkpointing_ranks = num_checkpointing_ranks;
    m_transfer_size = transfer_size;
    m_block_size = block_size;
    m_segment_count = segment_count;
    m_num_ck_files_per_rank = num_ck_files_per_rank;
    m_checkpointing_interval = checkpointing_interval;
    m_stage_out_steps = stage_out_steps;
    m_checkpointing_buffer = checkpointing_buffer;
    m_ck_error_rate = ck_error_rate;
    m_num_checkpoint_iterations = num_checkpoint_iterations;
    m_axl_xfer_type = axl_xfer_type;
    m_num_epochs = num_epochs;
    m_comp_time_per_epoch = comp_time_per_epoch;
    m_dag_workload_type = dag_workload_type;
    m_dag_workload_access_type = dag_workload_access_type;
    m_dag_filepath = dag_filepath;
    m_read_input_dirs = read_input_dirs;
    m_write_input_dirs = write_input_dirs;
    m_read_filenames = read_filenames;
    m_write_filenames = write_filenames;
    m_read_block_size_str = read_block_size_str;
    m_read_segment_count_str = read_segment_count_str;
    m_write_block_size_str = write_block_size_str;
    m_write_segment_count_str = write_segment_count_str;
    m_read_access_type = read_access_type;
    m_write_access_type = write_access_type;
    m_mpiio_type = mpiio_type;
    m_num_procs_per_file_read = num_procs_per_file_read;
    m_num_procs_per_file_write = num_procs_per_file_write;
    m_ranks_per_file_read = ranks_per_file_read;
    m_ranks_per_file_write = ranks_per_file_write;
    parse_config_file();
}

void config_attributes::construct_type_to_str_map()
{
    m_type_to_str_map[(int) config_attr_types::e_config_file] = "config_file";
    m_type_to_str_map[(int) config_attr_types::e_inter_node] = "inter_node";
    m_type_to_str_map[(int) config_attr_types::e_use_ior] = "use_ior";
    m_type_to_str_map[(int) config_attr_types::e_use_axl] = "use_axl";
    m_type_to_str_map[(int) config_attr_types::e_producer_only] = "producer_only";
    m_type_to_str_map[(int) config_attr_types::e_consumer_only] = "consumer_only";
    m_type_to_str_map[(int) config_attr_types::e_enable_staging] = "enable_staging";
    m_type_to_str_map[(int) config_attr_types::e_single_task_single_data] = "single_task_single_data";
    m_type_to_str_map[(int) config_attr_types::e_single_task_multi_data] = "single_task_multi_data";
    m_type_to_str_map[(int) config_attr_types::e_multi_task_single_data] = "multi_task_single_data";
    m_type_to_str_map[(int) config_attr_types::e_multi_task_multi_data] = "multi_task_multi_data";
    m_type_to_str_map[(int) config_attr_types::e_dag_read] = "dag_read";
    m_type_to_str_map[(int) config_attr_types::e_dag_write] = "dag_write";
    m_type_to_str_map[(int) config_attr_types::e_file_per_process_read] = "file_per_process_read";
    m_type_to_str_map[(int) config_attr_types::e_file_per_process_write] = "file_per_process_write";
    m_type_to_str_map[(int) config_attr_types::e_shared_file_read] = "shared_file_read";
    m_type_to_str_map[(int) config_attr_types::e_shared_file_write] = "shared_file_write";
    m_type_to_str_map[(int) config_attr_types::e_enable_profiler] = "enable_profiler";
    m_type_to_str_map[(int) config_attr_types::e_type] = "type";
    m_type_to_str_map[(int) config_attr_types::e_subtype] = "subtype";
    m_type_to_str_map[(int) config_attr_types::e_io_api] = "io_api";
    m_type_to_str_map[(int) config_attr_types::e_input_dir] = "input_dir";
    m_type_to_str_map[(int) config_attr_types::e_filename] = "filename";
    m_type_to_str_map[(int) config_attr_types::e_stage_dir] = "stage_dir";
    m_type_to_str_map[(int) config_attr_types::e_ranks_per_node] = "ranks_per_node";
    m_type_to_str_map[(int) config_attr_types::e_num_ck_ranks] = "num_ck_ranks";
    m_type_to_str_map[(int) config_attr_types::e_transfer_size] = "transfer_size";
    m_type_to_str_map[(int) config_attr_types::e_block_size] = "block_size";
    m_type_to_str_map[(int) config_attr_types::e_segment_count] = "segment_count";
    m_type_to_str_map[(int) config_attr_types::e_num_ck_files_per_rank] = "num_ck_files_per_rank";
    m_type_to_str_map[(int) config_attr_types::e_checkpointing_interval] = "checkpointing_interval";
    m_type_to_str_map[(int) config_attr_types::e_stage_out_steps] = "stage_out_steps";
    m_type_to_str_map[(int) config_attr_types::e_ck_error_rate] = "ck_error_rate";
    m_type_to_str_map[(int) config_attr_types::e_num_ck_iter] = "num_ck_iter";
    m_type_to_str_map[(int) config_attr_types::e_axl_xfer_type] = "axl_xfer_type";
    m_type_to_str_map[(int) config_attr_types::e_num_epochs] = "num_epochs";
    m_type_to_str_map[(int) config_attr_types::e_comp_time_per_epoch] = "comp_time_per_epoch";
    m_type_to_str_map[(int) config_attr_types::e_dag_file] = "dag_file";
    m_type_to_str_map[(int) config_attr_types::e_read_input_dirs] = "read_input_dirs";
    m_type_to_str_map[(int) config_attr_types::e_write_input_dirs] = "write_input_dirs";
    m_type_to_str_map[(int) config_attr_types::e_read_filenames] = "read_filenames";
    m_type_to_str_map[(int) config_attr_types::e_write_filenames] = "write_filenames";
    m_type_to_str_map[(int) config_attr_types::e_read_block_size] = "read_block_size";
    m_type_to_str_map[(int) config_attr_types::e_read_segment_count] = "read_segment_count";
    m_type_to_str_map[(int) config_attr_types::e_write_block_size] = "write_block_size";
    m_type_to_str_map[(int) config_attr_types::e_write_segment_count] = "write_segment_count";
    m_type_to_str_map[(int) config_attr_types::e_profile_out_filepath] = "profile_out_filepath";
    m_type_to_str_map[(int) config_attr_types::e_mpiio_type] = "mpiio_type";
    m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_read] = "num_procs_per_file_read";
    m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_write] = "num_procs_per_file_write";
    m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_read] = "ranks_per_file_read";
    m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_write] = "ranks_per_file_write";
}

void config_attributes::parse_config_file()
{
    if (m_config_file == "") return;
    Json::Value json_root;
    std::ifstream config_file_stream(m_config_file.c_str(), std::ifstream::binary);
    config_file_stream >> json_root;
    for (Json::Value::const_iterator _itr = json_root.begin(); _itr != json_root.end(); _itr++)
    {
        std::string _itr_val = _itr->asString();
        std::for_each(_itr_val.begin(), _itr_val.end(), [](char & _ch)
        {
            _ch = ::tolower(_ch);
        });
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_inter_node])
        {
            m_inter_node = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_use_ior])
        {
            m_use_ior = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_use_axl])
        {
            m_use_axl = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_producer_only])
        {
            m_producer_only = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_consumer_only])
        {
            m_consumer_only = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_enable_staging])
        {
            m_enable_staging = (_itr_val == "true") ? true : false;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_single_task_single_data])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_type = dag_workload_types::e_dwt_single_task_single_data;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_single_task_multi_data])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_type = dag_workload_types::e_dwt_single_task_multi_data;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_multi_task_single_data])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_type = dag_workload_types::e_dwt_multi_task_single_data;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_multi_task_multi_data])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_type = dag_workload_types::e_dwt_multi_task_multi_data;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_dag_read])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_access_type = workload_access_types::e_read;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_dag_write])
        {
            if (_itr_val == "true")
            {
                m_dag_workload_access_type = workload_access_types::e_write;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_file_per_process_read])
        {
            if (_itr_val == "true")
            {
                m_read_access_type = file_access_types::e_file_per_process;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_file_per_process_write])
        {
            if (_itr_val == "true")
            {
                m_write_access_type = file_access_types::e_file_per_process;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_shared_file_read])
        {
            if (_itr_val == "true")
            {
                m_read_access_type = file_access_types::e_shared_file;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_shared_file_write])
        {
            if (_itr_val == "true")
            {
                m_write_access_type = file_access_types::e_shared_file;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_enable_profiler])
        {
            g_profiler.m_enabled = (_itr_val == "true") ? true : false;
        }
        // Start of parameter cases with string value
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_type])
        {
            if (_itr_val == "data")
            {
                m_emulation_type = emulation_types::e_dataflow;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_subtype])
        {
            if (_itr_val == "cr")
                m_dataflow_workload_type = dataflow_workload_types::e_checkpoint_restart;

            if (_itr_val == "pc")
                m_dataflow_workload_type = dataflow_workload_types::e_producer_consumer;

            if (_itr_val == "dl")
                m_dataflow_workload_type = dataflow_workload_types::e_deep_learning;

            if (_itr_val == "dag")
                m_dataflow_workload_type = dataflow_workload_types::e_dag_workload;

            if (_itr_val == "app")
                m_dataflow_workload_type = dataflow_workload_types::e_app_workload;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_io_api])
        {
            if (_itr_val == "posixio")
                m_io_api_type = io_api_types::e_posixio;

            if (_itr_val == "mpiio")
                m_io_api_type = io_api_types::e_mpiio;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_input_dir])
        {
            m_directory = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_filename])
        {
            m_filename = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_stage_dir])
        {
            m_staging_directory = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_ranks_per_node])
        {
            m_num_ranks_per_node = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_ck_ranks])
        {
            m_num_checkpointing_ranks = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_transfer_size])
        {
            m_transfer_size = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_block_size])
        {
            m_block_size = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_block_size])
        {
            m_block_size = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_segment_count])
        {
            m_segment_count = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_ck_files_per_rank])
        {
            m_num_ck_files_per_rank = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_checkpointing_interval])
        {
            m_checkpointing_interval = std::stod(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_stage_out_steps])
        {
            m_stage_out_steps = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_ck_error_rate])
        {
            m_ck_error_rate = std::stof(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_ck_iter])
        {
            m_num_checkpoint_iterations = std::stof(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_axl_xfer_type])
        {
            m_axl_xfer_type = (axl_xfer_types) std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_epochs])
        {
            m_num_epochs = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_comp_time_per_epoch])
        {
            m_comp_time_per_epoch = std::stoi(_itr_val);
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_dag_file])
        {
            m_dag_filepath = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_read_input_dirs])
        {
            m_read_input_dirs = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_write_input_dirs])
        {
            m_write_input_dirs = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_read_filenames])
        {
            m_read_filenames = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_write_filenames])
        {
            m_write_filenames = _itr_val;
        }
        // --read/write_block_size, --read/write_segment_count can take
        // a list of block sizes, e.g., 1213:1233:2133:5456:...
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_read_block_size])
        {
            m_read_block_size_str = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_read_segment_count])
        {
            m_read_segment_count_str = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_write_block_size])
        {
            m_write_block_size_str = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_write_segment_count])
        {
            m_write_segment_count_str = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_profile_out_filepath])
        {
            g_profiler.m_enabled = true;
            g_profiler.m_out_filepath = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_mpiio_type])
        {
            std::string _type = _itr_val;
            if (_type == "noncollective")
            {
                m_mpiio_type = mpiio_types::e_non_collective;
            }
            else if (_type == "collective")
            {
                m_mpiio_type = mpiio_types::e_collective;
            }
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_read])
        {
            // add the ':' separated string to the config attributes
            m_num_procs_per_file_read = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_num_procs_per_file_write])
        {
            // add the ':' separated string to the config attributes
            m_num_procs_per_file_write = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_read])
        {
            // add the ':' then ',' separated string to the config attributes
                m_ranks_per_file_read = _itr_val;
        }
        if (_itr.key().asString() == m_type_to_str_map[(int) config_attr_types::e_ranks_per_file_write])
        {
            // add the ':' then ',' separated string to the config attributes
            m_ranks_per_file_write = _itr_val;
        }
    }
}

void config_attributes::print()
{
    std::cout << "I/O pattern type: " << m_dataflow_workload_type << std::endl;
    std::cout << "Input directory: " << m_directory << std::endl;
    std::cout << "File name: " << m_filename << std::endl;
    std::cout << "Staging directory: " << m_staging_directory << std::endl;
    std::cout << "Internode: " << m_inter_node << std::endl;
    std::cout << "Use IOR: " << m_use_ior << std::endl;
    std::cout << "Use AXL: " << m_use_axl << std::endl;
    std::cout << "Producer only: " << m_producer_only << std::endl;
    std::cout << "Consumer only: " << m_consumer_only << std::endl;
    std::cout << "Enable staging: " << m_enable_staging << std::endl;
    std::cout << "Number of ranks per node: " << m_num_ranks_per_node << std::endl;
    std::cout << "Number of checkpointing ranks: " << m_num_checkpointing_ranks << std::endl;
    std::cout << "Transfer size: " << m_transfer_size << std::endl;
    std::cout << "Block size: " << m_block_size << std::endl;
    std::cout << "Segment count: " << m_segment_count << std::endl;
    std::cout << "Number of checkpointing files per rank: " << m_num_ck_files_per_rank << std::endl;
    std::cout << "Checkpointing interval: " << m_checkpointing_interval << std::endl;
    std::cout << "Stage out interval: " << m_stage_out_steps << std::endl;
    std::cout << "Checkpointing buffer size: " << m_checkpointing_buffer << std::endl;
    std::cout << "Checkpointing error rate: " << m_ck_error_rate << std::endl;
    std::cout << "Checkpoint Iterations: " << m_num_checkpoint_iterations << std::endl;
    std::cout << "AXL Transfer Type: " << m_axl_xfer_type << std::endl;
    std::cout << "Number of Epochs: " << m_num_epochs << std::endl;
    std::cout << "Computation time per Epoch" << m_comp_time_per_epoch << std::endl;
    std::cout << "DAG file path: " << m_dag_filepath << std::endl;
    std::cout << "Read file names: " << m_read_filenames << std::endl;
    std::cout << "Write file names: " << m_write_filenames << std::endl;
    std::cout << "Number of processes reading the file: " << m_num_procs_per_file_read << std::endl;
    std::cout << "Number of processes writing the file: " << m_num_procs_per_file_write << std::endl;
    std::cout << "Ranks reading the file: " << m_ranks_per_file_read << std::endl;
    std::cout << "Ranks writing the file: " << m_ranks_per_file_write << std::endl;
}
