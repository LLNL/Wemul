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

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

enum emulator_types
{
    emulator_types_start,
    e_dataflow,
    emulator_types_end
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
    e_single_task_single_data,
    e_single_task_multi_data,
    e_multi_task_single_data,
    e_multi_task_multi_data,
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
    file_access_types_end
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

#endif // __TYPES_HPP__
