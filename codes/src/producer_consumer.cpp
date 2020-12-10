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
#include <math.h>

#include "producer_consumer.hpp"
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

producer_consumer::producer_consumer(std::shared_ptr<base_io_api> io_api,
    std::string directory, bool inter_node,
    int num_ranks_per_node, bool producer_only, bool consumer_only,
    int block_size, int segment_count) : dataflow_workload()
{
    m_io_api = io_api;
    m_directory = directory;
    m_inter_node = inter_node;
    m_num_ranks_per_node = num_ranks_per_node;
    m_producer_only = producer_only;
    m_consumer_only = consumer_only;
    m_block_size = block_size;
    m_segment_count = segment_count;
}

producer_consumer::~producer_consumer()
{
}

void producer_consumer::emulate(int argc, char** argv)
{
    initialize_MPI(argc, argv);

    int rank;
    // int size;
 
    if (!(m_producer_only || m_consumer_only))
    {
        if (MPI_COMM_NULL != m_producer_comm)
        {
            MPI_Comm_rank(m_producer_comm, &rank);
            std::string _file_path = m_directory + "/file." + std::to_string(rank);
            m_io_api->read_or_write(_file_path, m_block_size, m_segment_count, true);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (MPI_COMM_NULL != m_consumer_comm)
        {
            MPI_Comm_rank(m_consumer_comm, &rank);
            std::string _file_path = m_directory + "/file." + std::to_string(rank);
            m_io_api->read_or_write(_file_path, m_block_size, m_segment_count);
        }
    }
    else if (m_producer_only)
    {
        if (MPI_COMM_NULL != m_producer_comm)
        {
            MPI_Comm_rank(m_producer_comm, &rank);
            std::string _file_path = m_directory + "/file." + std::to_string(rank);
            m_io_api->read_or_write(_file_path, m_block_size, m_segment_count, true);
        }
    }
    else if (m_consumer_only)
    {
        if (MPI_COMM_NULL != m_consumer_comm)
        {
            MPI_Comm_rank(m_consumer_comm, &rank);
            std::string _file_path = m_directory + "/file." + std::to_string(rank);
            m_io_api->read_or_write(_file_path, m_block_size, m_segment_count);
        }
    }
    finalize_MPI();
}

void producer_consumer::initialize_MPI(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    g_profiler.m_timer.adjust_time_deviation();

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Comm_group(MPI_COMM_WORLD, &m_world_group);
    
    if(!(m_producer_only || m_consumer_only))
    {
        int group_size = world_size / 2;

        int* producer_ranks = (int*) malloc(group_size * sizeof(int));
        int* consumer_ranks = (int*) malloc(group_size * sizeof(int));

        for(int i = 0, j = 0, k = 0; i < world_size; i++)
        {
            if(m_inter_node)
            {
                if(i < group_size)
                {
                    producer_ranks[j] = i;
                    j++;
                }
                else
                {
                    consumer_ranks[k] = i;
                    k++;
                }
            }
            else
            {
                if((i % m_num_ranks_per_node) % 2)
                {
                    consumer_ranks[j] = i;
                    j++;
                }
                else
                {
                    producer_ranks[k] = i;
                    k++;
                }
            }
        }

        MPI_Group_incl(m_world_group, group_size, producer_ranks, &m_producer_group);
        MPI_Comm_create_group(MPI_COMM_WORLD, m_producer_group, 0, &m_producer_comm);

        MPI_Group_incl(m_world_group, group_size, consumer_ranks, &m_consumer_group);
        MPI_Comm_create_group(MPI_COMM_WORLD, m_consumer_group, 0, &m_consumer_comm);
    }
    else if(m_producer_only)
    {
        m_producer_group = m_world_group;
        m_producer_comm = MPI_COMM_WORLD;
    }
    else if(m_consumer_only)
    {
        m_consumer_group = m_world_group;
        m_consumer_comm = MPI_COMM_WORLD;
    }

    // Hint: The code below kept for dividing the world into 3 parts
    // int producer_max_rank = ceil(world_size / 3);
    // int* producer_ranks = (int*) malloc(producer_max_rank * sizeof(int));

    // int i = 0;
    // for(; i < producer_max_rank; i++)
    // {
    //     producer_ranks[i] = i;
    // }

    // MPI_Group_incl(m_world_group, i, producer_ranks, &m_producer_group);
    // MPI_Comm_create_group(MPI_COMM_WORLD, m_producer_group, 0, &m_producer_comm);

    // int consumer_max_rank = producer_max_rank + ceil(world_size / 3);
    // int* consumer_ranks = (int*) malloc((consumer_max_rank-producer_max_rank) * sizeof(int));
    // int j = 0;
    // for(; i < consumer_max_rank; i++, j++)
    // {
    //     consumer_ranks[j] = i;
    // }

    // MPI_Group_incl(m_world_group, j, consumer_ranks, &m_consumer_group);
    // MPI_Comm_create_group(MPI_COMM_WORLD, m_consumer_group, 0, &m_consumer_comm);

    // int* both_ranks = (int*) malloc((world_size-consumer_max_rank) * sizeof(int));
    // int k = 0;
    // for(; i < world_size; i++, k++)
    // {
    //     both_ranks[k] = i;
    // }

    // MPI_Group_incl(m_world_group, k, both_ranks, &m_both_group);
    // MPI_Comm_create_group(MPI_COMM_WORLD, m_both_group, 0, &m_both_comm);
}

void producer_consumer::finalize_MPI()
{
    if (g_profiler.m_enabled)
    {
        g_profiler.write_to_file();
    }
    MPI_Group_free(&m_world_group);

    if (!(m_producer_only || m_consumer_only))
    {
        MPI_Group_free(&m_producer_group);
        MPI_Group_free(&m_consumer_group);
    }
    else if (m_producer_only) MPI_Group_free(&m_producer_group);
    else if (m_consumer_only) MPI_Group_free(&m_consumer_group);

    // MPI_Group_free(&m_both_group);
    MPI_Finalize();
}
