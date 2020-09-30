#ifndef __CONFIG_ATTRIBUTES_HPP__
#define __CONFIG_ATTRIBUTES_HPP__

#include <string>

#include "types.hpp"

class config_attributes
{
public:
    config_attributes(
        dataflow_workload_types dataflow_workload_type = dataflow_workload_types::dataflow_workload_types_start,
        std::string directory = ".",
        std::string filename = "testFile",
        std::string staging_directory = ".",
        bool inter_node = false,
        bool use_ior = false,
        bool use_axl = false,
        bool producer_only = false,
        bool consumer_only = false,
        bool enable_staging = false,
        int num_ranks_per_node = 1,
        int num_checkpointing_ranks = 1,
        int transfer_size = 1000000,
        int block_size = 1000000,
        int segment_count = 10,
        int num_ck_files_per_rank = 1,
        double checkpointing_interval = 0.5,
        int stage_out_steps = 10,
        int checkpointing_buffer = -1,
        float ck_error_rate = 20.0,
        int num_checkpoint_iterations = -1,
        axl_xfer_types axl_xfer_type = axl_xfer_types::e_default,
        int num_epochs = 1,
        int comp_time_per_epoch = 2,
        dag_workload_types dag_workload_type = dag_workload_types::dag_workload_types_start,
        workload_access_types dag_workload_access_type = workload_access_types::workload_access_types_start,
        std::string dag_filepath = "./test_dag",
        std::string read_input_dirs = "",
        std::string write_input_dirs = "",
        std::string read_filenames = "",
        std::string write_filenames = "",
        int read_block_size = -1,
        int read_segment_count = -1,
        int write_block_size = -1,
        int write_segment_count = -1,
        file_access_types read_access_type = file_access_types::e_shared_file,
        file_access_types write_access_type = file_access_types::e_shared_file);

    void print();

    dataflow_workload_types m_dataflow_workload_type;
    std::string m_directory;
    std::string m_filename;
    std::string m_staging_directory;
    bool m_inter_node;
    bool m_use_ior;
    bool m_use_axl;
    bool m_producer_only;
    bool m_consumer_only;
    bool m_enable_staging;
    int m_num_ranks_per_node;
    int m_num_checkpointing_ranks;
    int m_transfer_size;
    int m_block_size;
    int m_segment_count;
    int m_num_ck_files_per_rank;
    // TODO: rename to interval
    double m_checkpointing_interval;
    int m_stage_out_steps;
    int m_checkpointing_buffer;
    float m_ck_error_rate;
    int m_num_checkpoint_iterations;
    axl_xfer_types m_axl_xfer_type;
    int m_num_epochs;
    int m_comp_time_per_epoch;
    dag_workload_types m_dag_workload_type;
    workload_access_types m_dag_workload_access_type;
    std::string m_dag_filepath;
    std::string m_read_input_dirs;
    std::string m_write_input_dirs;
    std::string m_read_filenames;
    std::string m_write_filenames;
    int m_read_block_size;
    int m_read_segment_count;
    int m_write_block_size;
    int m_write_segment_count;
    file_access_types m_read_access_type;
    file_access_types m_write_access_type;
};

#endif // __CONFIG_ATTRIBUTES_HPP__
