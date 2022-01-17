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

#include <fstream>
#include <iostream>
#include <mpi.h>

#include "mpi_io_api.hpp"
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

mpi_io_api::mpi_io_api() : base_io_api()
{
}

mpi_io_api::~mpi_io_api()
{
}

void mpi_io_api::read_file(std::string filepath)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    // std::cout << "Call read_file" << std::endl;
    MPI_File file_handle;

    int err = MPI_File_open(MPI_COMM_SELF, filepath.c_str(),
        MPI_MODE_RDONLY, MPI_INFO_NULL, &file_handle);

    if (!err)
    {
        std::ifstream file_stream(filepath, std::ios::binary | std::ios::ate);
        int filesize = file_stream.tellg();
        char* buf = (char*) malloc(filesize * sizeof(char));
        //std::cout << "File size: " << filesize << std::endl;
        MPI_Status status;
        MPI_File_read(file_handle, buf, filesize, MPI_BYTE, &status);
        if (g_profiler.m_enabled) g_profiler.m_total_bytes_read += filesize;
        //std::cout << buf << std::endl;
        //int count;
        //MPI_Get_count(&status, MPI_BYTE, &count);
        //printf("rank read %d bytes\n", count);
        free(buf);
        MPI_File_close(&file_handle);
    }
    else
    {
        std::cout << filepath << " : does not exist!" << std::endl;
    }
    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_read_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void mpi_io_api::write_file(std::string filepath, int block_size, int segment_count, bool is_fsync)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }

    MPI_File file_handle;

    MPI_File_open(MPI_COMM_SELF, filepath.c_str(),
        MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL,
        &file_handle);

    // calloc to initialize the buffer with 0's
    char* write_buf = (char*) calloc(block_size, sizeof(char));
    // MPI_Status status;
    // MPI_File_write(file_handle, write_buf, filesize, MPI_BYTE, &status);
    // std::cout << write_buf << std::endl;
    // int count;
    // MPI_Get_count(&status, MPI_BYTE, &count);
    // printf("rank wrote %d bytes\n", count);
    MPI_Offset offset = 0;
    for (int i = 0; i < segment_count; i++)
    {
        MPI_Status status;
        MPI_File_write_at(file_handle, offset, write_buf, block_size, MPI_BYTE, &status);
        if (g_profiler.m_enabled) g_profiler.m_total_bytes_written += block_size;
        // std::cout << write_buf << std::endl;
        int count;
        MPI_Get_count(&status, MPI_BYTE, &count);
        // printf("rank wrote %d bytes\n", count);
        offset += block_size;
        free(write_buf);
        write_buf = (char*) calloc(block_size, sizeof(char));
    }
    free(write_buf);
    if (is_fsync)
        if (MPI_File_sync(file_handle) != MPI_SUCCESS)
            std::cout << "ERROR: fsync failed!" << std::endl;

    MPI_File_close(&file_handle);

    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_write_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void mpi_io_api::read_file(std::string filepath, int block_size, int segment_count)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }

    MPI_File file_handle;

    MPI_File_open(MPI_COMM_SELF, filepath.c_str(),
        MPI_MODE_RDONLY, MPI_INFO_NULL,
        &file_handle);

    char* read_buf = (char*) malloc(block_size * sizeof(char));
    MPI_Offset offset = 0;
    for (int i = 0; i < segment_count; i++)
    {
        MPI_Status status;
        MPI_File_read_at(file_handle, offset, read_buf, block_size, MPI_BYTE, &status);
        if (g_profiler.m_enabled) g_profiler.m_total_bytes_read += block_size;
        // std::cout << read_buf << std::endl;
        int count;
        MPI_Get_count(&status, MPI_BYTE, &count);
        // printf("rank read %d bytes\n", count);
        offset += block_size;
        free(read_buf);
        read_buf = (char*) malloc(block_size * sizeof(char));
    }
    free(read_buf);
    MPI_File_close(&file_handle);

    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_read_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void mpi_io_api::read_or_write(std::string filepath, int block_size, int segment_count, bool is_write,
    bool read_check, bool is_fsync)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }

    MPI_File file_handle;
    // std::cout << "File Path: " << filepath << std::endl;

    int mode = (is_write) ? MPI_MODE_CREATE | MPI_MODE_WRONLY : MPI_MODE_RDONLY;

    if (!is_write && read_check) utils::file_ready(filepath, block_size, segment_count);

    int err = MPI_File_open(MPI_COMM_SELF, filepath.c_str(),
        mode, MPI_INFO_NULL, &file_handle);

    if (!err)
    {
        // MPI collective I/O
        char* buf = (is_write) ? (char*) calloc(block_size, sizeof(char))
            : (char*) malloc(block_size * sizeof(char));

        for (int i = 0; i < segment_count; i++)
        {
            if(is_write)
                MPI_File_write_ordered(file_handle, buf, block_size, MPI_BYTE, MPI_STATUS_IGNORE);
            else
                MPI_File_read_ordered(file_handle, buf, block_size, MPI_BYTE, MPI_STATUS_IGNORE);
            if (g_profiler.m_enabled)
            {
                if (is_write) g_profiler.m_total_bytes_written += block_size;
                else g_profiler.m_total_bytes_read += block_size;
            }
            free(buf);
            buf = (is_write) ? (char*) calloc(block_size, sizeof(char))
                : (char*) malloc(block_size * sizeof(char));
        }
        free(buf);
        if (is_write && is_fsync)
        if (MPI_File_sync(file_handle) != MPI_SUCCESS)
            std::cout << "ERROR: fsync failed!" << std::endl;

        MPI_File_close(&file_handle);
    }
    else
    {
        std::cout << filepath << " : does not exist yet, might be a non-strict file" << std::endl;
    }

    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        if (is_write) g_profiler.m_write_intervals.push_back(std::make_pair(_start_time, _end_time));
        else g_profiler.m_read_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}
