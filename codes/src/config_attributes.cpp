#include <iostream>

#include "config_attributes.hpp"

config_attributes::config_attributes(dataflow_workload_types dataflow_workload_type,
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
        int read_block_size,
        int read_segment_count,
        int write_block_size,
        int write_segment_count,
        file_access_types read_access_type,
        file_access_types write_access_type)
{
    m_dataflow_workload_type = dataflow_workload_type;
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
    m_read_block_size = read_block_size;
    m_read_segment_count = read_segment_count;
    m_write_block_size = write_block_size;
    m_write_segment_count = write_segment_count;
    m_read_access_type = read_access_type;
    m_write_access_type = write_access_type;
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
}
