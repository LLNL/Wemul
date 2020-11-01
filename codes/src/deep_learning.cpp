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

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <stdlib.h>
#include <thread>

#include "ior_runner.hpp"
#include "deep_learning.hpp"
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

deep_learning::deep_learning(std::string directory, bool use_ior,
        int num_epochs, int comp_time_per_epoch)
    : dataflow_workload()
{
    m_directory = directory;
    m_use_ior = use_ior;
    m_num_epochs = num_epochs;
    m_comp_time_per_epoch = comp_time_per_epoch;
    populate_list_of_all_filepaths();
    // std::cout << "file path list populated with size: " << m_filepath_list.size() << std::endl;
}

deep_learning::~deep_learning()
{
}

void deep_learning::emulate(int argc, char** argv)
{
    if (m_use_ior)
    {
        char* _ior_path_env = getenv("IOR_PATH");
        std::string _ior_path;
        if(_ior_path_env) _ior_path = _ior_path_env;
        if (_ior_path.empty())
        {
            std::cout << "ERROR: IOR_PATH not set!" << std::endl;
        }
        std::vector<std::string> _params = {_ior_path, "-a", "MPIIO", "-$", "-#", m_directory};
        ior_runner _ior_runner(_params);
        for(int i = 0; i < m_num_epochs; i++)
        {
            _ior_runner.run();
            // emulate some computation; later make it configurable
            std::this_thread::sleep_for(std::chrono::milliseconds((long) (5 * 1000)));
        }
    }
    else
    {
        MPI_Init(&argc, &argv);
        g_profiler.m_timer.adjust_time_deviation();

        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        for (int i = 0; i < m_num_epochs; i++)
        {
            // shuffling per epoch
            std::random_shuffle(m_file_ids.begin(), m_file_ids.end());

            int file_count = m_file_ids.size();
            int num_files_per_proc = ceil(file_count / (double) world_size);
            if (num_files_per_proc == 0) // if file_count is less than world_size
                    num_files_per_proc = 1;

            int start_id = world_rank * num_files_per_proc;
            int end_id = start_id + num_files_per_proc - 1;
            end_id = (end_id >= file_count) ? file_count - 1 : end_id;
            std::cout << "Rank: " << world_rank << " : " << start_id << "," << end_id << std::endl;
            std::cout << file_count << std::endl;
            std::cout << num_files_per_proc << std::endl;
            for (int i = start_id; i <= end_id; i++)
            {
                utils::read_file(m_filepath_list[m_file_ids[i]]);
            }
            // emulate some computation; later make it configurable
            std::this_thread::sleep_for(std::chrono::milliseconds((long) (m_comp_time_per_epoch * 1000)));
        }

        if (g_profiler.m_enabled)
        {
            g_profiler.write_to_file();
        }
        MPI_Finalize();
    }
}

void deep_learning::set_directory(std::string directory)
{
    m_directory = directory;
}

void deep_learning::populate_list_of_all_filepaths()
{
    utils::recursive_listfiles(m_directory, m_filepath_list);
    // // Save time in the experiments by hard-coded directory structure
    // // Hint: In ImageNet package there is a file that has all information
    // // about the directory structure; no need to recursively traverse
    // for (int i = 0; i < 320; i++)
    // {
    //    for (int j = 0; j < 1024; j++)
    //    {
    //        m_filepath_list.push_back(m_directory + "/subdir." + std::to_string(i) + "/data." + std::to_string(j));
    //    }
    // }
    for (int i = 0; i < m_filepath_list.size(); i++)
    {
        m_file_ids.push_back(i);
    }
}
