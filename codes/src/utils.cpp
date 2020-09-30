#include <dirent.h>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <stdlib.h>

#include "utils.hpp"

void utils::recursive_listfiles(const std::string& directory, std::vector<std::string>& filepath_list,const bool show_hidden_entries)
{
    DIR *input_direntry;
    struct dirent *output_direntry;
    input_direntry = opendir(directory.c_str());
    // std::cout << "input dir: " << directory << std::endl;
    if (input_direntry != NULL)
    {
        while ((output_direntry = readdir(input_direntry)) != NULL)
        {
            // std::cout << "in the loop: " << DT_DIR << std::endl;
            // printf("%p %d \n", output_direntry, output_direntry->d_type);
            if(show_hidden_entries ?
                (output_direntry->d_type == DT_DIR 
                    && std::string(output_direntry->d_name) != ".."
                    && std::string(output_direntry->d_name) != "." )
                : (output_direntry->d_type == DT_DIR
                    && std::string(output_direntry->d_name) != ".."
                    && std::string(output_direntry->d_name) != "."
                    && std::string(output_direntry->d_name).rfind(".", 0) != 0 ))
            {
                // std::string _current_subdir = directory + "/" + output_direntry->d_name;
                // std::cout << "current subdir: " << _current_subdir << std::endl;
                recursive_listfiles(directory + "/" + output_direntry->d_name + "/", filepath_list, show_hidden_entries);
            }
            if(output_direntry->d_type == DT_REG)
            {
                // std::string _current_file = directory + "/" + output_direntry->d_name;
                // std::cout << "current file: " << _current_file << std::endl;
                filepath_list.push_back(directory + "/" + output_direntry->d_name);
            }
        }
    }

    closedir(input_direntry);
}

void utils::read_file(std::string filepath)
{
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
}

void utils::write_file(std::string filepath, int block_size, int segment_count, bool fsync)
{
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
        // std::cout << write_buf << std::endl;
        int count;
        MPI_Get_count(&status, MPI_BYTE, &count);
        // printf("rank wrote %d bytes\n", count);
        offset += block_size;
        free(write_buf);
        write_buf += block_size;
        write_buf = (char*) malloc(block_size * sizeof(char));
    }
    free(write_buf);
    if (fsync)
        if (MPI_File_sync(file_handle) != MPI_SUCCESS)
            std::cout << "ERROR: fsync failed!" << std::endl;


    MPI_File_close(&file_handle);
}

void utils::read_file(std::string filepath, int block_size, int segment_count)
{
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
        // std::cout << read_buf << std::endl;
        int count;
        MPI_Get_count(&status, MPI_BYTE, &count);
        // printf("rank read %d bytes\n", count);
        offset += block_size;
        free(read_buf);
        read_buf += block_size;
        read_buf = (char*) malloc(block_size * sizeof(char));
    }
    free(read_buf);
    MPI_File_close(&file_handle);
}

bool utils::file_ready(std::string filepath, int block_size, int segment_count)
{
    long _expected_filesize = (long) block_size * segment_count;
    while(true)
    {
        std::ifstream _file_stream(filepath, std::ios::binary | std::ios::ate);
        if (_file_stream.good() && _file_stream.tellg() == _expected_filesize)
        {
            std::cout << "File " << filepath << " is GOOD!" << std::endl;
            std::cout << "File size: " << _file_stream.tellg() << std::endl;
            break;
        }
    }

    return true;
}

void utils::read_or_write(std::string filepath, int block_size, int segment_count, bool write,
    bool read_check, bool fsync)
{
    MPI_File file_handle;
    // std::cout << "File Path: " << filepath << std::endl;

    int mode = (write) ? MPI_MODE_CREATE | MPI_MODE_WRONLY : MPI_MODE_RDONLY;

    if (!write && read_check) utils::file_ready(filepath, block_size, segment_count);

    int err = MPI_File_open(MPI_COMM_SELF, filepath.c_str(),
        mode, MPI_INFO_NULL, &file_handle);

    if (!err)
    {
        // MPI collective I/O
        char* buf = (write) ? (char*) calloc(block_size, sizeof(char))
            : (char*) malloc(block_size * sizeof(char));

        for (int i = 0; i < segment_count; i++)
        {
            if(write)
                MPI_File_write_ordered(file_handle, buf, block_size, MPI_BYTE, MPI_STATUS_IGNORE);
            else
                MPI_File_read_ordered(file_handle, buf, block_size, MPI_BYTE, MPI_STATUS_IGNORE);
            free(buf);
            buf += block_size;
            buf = (write) ? (char*) calloc(block_size, sizeof(char))
                : (char*) malloc(block_size * sizeof(char));
        }
        free(buf);
        if (write && fsync)
        if (MPI_File_sync(file_handle) != MPI_SUCCESS)
            std::cout << "ERROR: fsync failed!" << std::endl;

        MPI_File_close(&file_handle);
    }
    else
    {
        std::cout << filepath << " : does not exist yet, might be a non-strict file" << std::endl;
    }
}
