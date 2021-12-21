/*
 * Copyright (c) 2021, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2021, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "posix_io_api.hpp"
#include "profiler.hpp"
#include "utils.hpp"

extern profiler g_profiler;

posix_io_api::posix_io_api() : base_io_api()
{
}

posix_io_api::~posix_io_api()
{
}

void posix_io_api::read_file(std::string filepath)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    int _fd_oflag = O_RDONLY;
    int _fd = open(filepath.c_str(), _fd_oflag, 0664);
    if (_fd < 0)
    {
        std::cout << filepath << " : does not exist!" << std::endl;
    }
    else
    {
        std::ifstream _file_stream(filepath, std::ios::binary | std::ios::ate);
        int _filesize = _file_stream.tellg();
        char* _buf = (char*) malloc(_filesize * sizeof(char));
        int _bytes_read = read(_fd, _buf, _filesize);
        if (g_profiler.m_enabled) g_profiler.m_total_bytes_read += _bytes_read;
        free(_buf);
        close(_fd);
    }
    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_read_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void posix_io_api::read_file(std::string filepath, int block_size,
    int segment_count)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    int _fd_oflag = O_RDONLY;
    int _fd = open(filepath.c_str(), _fd_oflag, 0664);
    if (_fd < 0)
    {
        std::cout << filepath << " : does not exist!" << std::endl;
    }
    else
    {
        char* _read_buf = (char*) malloc(block_size * sizeof(char));
        for (int i = 0; i < segment_count; i++)
        {
            int _bytes_read = read(_fd, _read_buf, block_size);
            if (g_profiler.m_enabled) g_profiler.m_total_bytes_read += _bytes_read;
            free(_read_buf);
            _read_buf = (char*) malloc(block_size * sizeof(char));
        }
        free(_read_buf);
        close(_fd);
    }

    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_write_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void posix_io_api::write_file(std::string filepath, int block_size,
    int segment_count, bool is_fsync)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    int _fd_oflag = O_CREAT | O_WRONLY;
    int _fd = open(filepath.c_str(), _fd_oflag, 0664);
    if (_fd < 0)
    {
        std::cout << filepath << " : does not exist!" << std::endl;
    }
    else
    {
        // calloc to initialize the buffer with 0's
        char* _write_buf = (char*) calloc(block_size, sizeof(char));
        for (int i = 0; i < segment_count; i++)
        {
            int _bytes_written = write(_fd, _write_buf, block_size);
            if (g_profiler.m_enabled) g_profiler.m_total_bytes_written += _bytes_written;
            free(_write_buf);
            _write_buf = (char*) calloc(block_size, sizeof(char));
        }
        free(_write_buf);
        if (is_fsync)
            if (fsync(_fd) != 0)
                std::cout << "ERROR: fsync failed!" << std::endl;

        close(_fd);
    }
    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        g_profiler.m_write_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}

void posix_io_api::read_or_write(std::string filepath, int block_size,
    int segment_count, bool is_write, bool read_check, bool is_fsync)
{
    double _start_time, _end_time;
    if (g_profiler.m_enabled)
    {
        _start_time = g_profiler.m_timer.get_current_time();
    }
    int _fd_oflag = (is_write) ? O_CREAT | O_WRONLY : O_RDONLY;
    if (!is_write && read_check) utils::file_ready(filepath, block_size, segment_count);

    int _fd = open(filepath.c_str(), _fd_oflag, 0664);
    if (_fd < 0)
    {
        std::cout << filepath << " : does not exist yet, might be a non-strict file" << std::endl;
    }
    else
    {
        char* _buf = (is_write) ? (char*) calloc(block_size, sizeof(char))
            : (char*) malloc(block_size * sizeof(char));

        for (int i = 0; i < segment_count; i++)
        {
            int _bytes_transferred = 0;
            if(is_write)
                _bytes_transferred = write(_fd, _buf, block_size);
            else
                _bytes_transferred = read(_fd, _buf, block_size);
            if (g_profiler.m_enabled)
            {
                if (is_write) g_profiler.m_total_bytes_written += _bytes_transferred;
                else g_profiler.m_total_bytes_read += _bytes_transferred;
            }
            free(_buf);
            _buf = (is_write) ? (char*) calloc(block_size, sizeof(char))
                : (char*) malloc(block_size * sizeof(char));
        }
        free(_buf);
        if (is_write && is_fsync)
            if (fsync(_fd) != 0)
                std::cout << "ERROR: fsync failed!" << std::endl;

        close(_fd);
    }

    if (g_profiler.m_enabled)
    {
        _end_time = g_profiler.m_timer.get_current_time();
        if (is_write) g_profiler.m_write_intervals.push_back(std::make_pair(_start_time, _end_time));
        else g_profiler.m_read_intervals.push_back(std::make_pair(_start_time, _end_time));
    }
}
