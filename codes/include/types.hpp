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

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

enum emulation_types
{
    emulation_types_start,
    e_dataflow,
    emulation_types_end
};

enum io_api_types
{
    io_api_types_start, // variable for looping
    e_posixio,
    e_mpiio,
    io_api_types_end // variable for looping
};

enum mpiio_types
{
    mpiio_types_start,
    e_non_collective,
    e_collective,
    mpiio_types_end
};

enum dataflow_workload_types
{
    dataflow_workload_types_start, // variable for looping
    e_checkpoint_restart,
    e_producer_consumer,
    e_deep_learning,
    e_dag_workload,
    e_app_workload,
    dataflow_workload_types_end // variable for looping
};

enum dag_workload_types
{
    dag_workload_types_start, // variable for looping
    e_dwt_single_task_single_data,
    e_dwt_single_task_multi_data,
    e_dwt_multi_task_single_data,
    e_dwt_multi_task_multi_data,
    dag_workload_types_end // variable for looping
};

enum workload_access_types
{
    workload_access_types_start, // variable for looping
    e_read,
    e_write,
    e_read_write,
    workload_access_types_end // variable for looping
};

enum file_access_types
{
    file_access_types_start,
    e_file_per_process,
    e_shared_file,
    e_custom,
    file_access_types_end
};

enum dependency_types
{
    e_strict,
    e_non_strict
};

enum axl_xfer_types
{
    axl_transfer_types_start,
    e_default,       /* Autodetect and use the fastest API for this node
                        * type that supports all AXL transfers.
                        */
    e_sync,          /* synchronous copy */
    e_async_daemon,  /* async daemon process */
    e_async_dw,      /* Cray Datawarp */
    e_async_bbapi,   /* IBM Burst Buffer API */
    e_async_cppr,    /* Intel CPPR */
    e_native,        /* Autodetect and use the native API (BBAPI, DW,
                        * etc) for this node type.  It may or may not
                        * be compatible with all AXL transfers (like
                        * shmem).  If there is no native API, fall back
                        * to AXL_XFER_DEFAULT.
                        */
    e_pthread,      /* parallel copy using pthreads */
    axl_transfer_types_end
};

enum config_attr_types
{
    e_config_file,
    e_inter_node,
    e_use_ior,
    e_use_axl,
    e_producer_only,
    e_consumer_only,
    e_enable_staging,
    e_single_task_single_data,
    e_single_task_multi_data,
    e_multi_task_single_data,
    e_multi_task_multi_data,
    e_dag_read,
    e_dag_write,
    e_file_per_process_read,
    e_file_per_process_write,
    e_shared_file_read,
    e_shared_file_write,
    e_enable_profiler,
    e_type,
    e_subtype,
    e_io_api,
    e_input_dir,
    e_filename,
    e_stage_dir,
    e_ranks_per_node,
    e_num_ck_ranks,
    e_transfer_size,
    e_block_size,
    e_segment_count,
    e_num_ck_files_per_rank,
    e_checkpointing_interval,
    e_stage_out_steps,
    e_ck_error_rate,
    e_num_ck_iter,
    e_axl_xfer_type,
    e_num_epochs,
    e_comp_time_per_epoch,
    e_dag_file,
    e_read_input_dirs,
    e_write_input_dirs,
    e_read_filenames,
    e_write_filenames,
    e_read_block_size,
    e_read_segment_count,
    e_write_block_size,
    e_write_segment_count,
    e_profile_out_filepath,
    e_mpiio_type,
    e_num_procs_per_file_read,
    e_num_procs_per_file_write,
    e_ranks_per_file_read,
    e_ranks_per_file_write
};

#endif // __TYPES_HPP__
