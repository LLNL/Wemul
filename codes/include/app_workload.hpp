#ifndef __APP_WORKLOAD_HPP__
#define __APP_WORKLOAD_HPP__

#include <mpi.h>
#include <string>
#include <vector>

#include "dataflow_workload.hpp"
#include "types.hpp"


class app_workload : public dataflow_workload
{
public:
    app_workload(std::string directory,
        int block_size,
        int segment_count,
        std::string read_input_dirs,
        std::string write_input_dirs,
        std::string read_filenames,
        std::string write_filenames,
        int read_block_size = -1,
        int read_segment_count = -1,
        int write_block_size = -1,
        int write_segment_count = -1,
        file_access_types read_access_type = file_access_types::e_shared_file,
        file_access_types write_access_type = file_access_types::e_shared_file);

    virtual ~app_workload() override;
    virtual void emulate(int argc, char** argv) override;

private:
    // TODO: make it directory list for heterogeneous storage
    //       or take list of absolute file paths and extract directory
    //       now all the files are read/written in the same mountpoint
    std::string m_directory;
    int m_block_size;
    int m_segment_count;
    int m_world_rank;
    int m_world_size;

    // Colon separated input directories
    std::string m_read_input_dirs;
    // Colon separated filenames
    std::string m_read_filenames;
    int m_read_block_size;
    int m_read_segment_count;
    file_access_types m_read_access_type;
    std::vector<std::string> m_read_input_dir_list;
    std::vector<std::string> m_read_filename_list;

    std::string m_write_input_dirs;
    std::string m_write_filenames;
    int m_write_block_size;
    int m_write_segment_count;
    file_access_types m_write_access_type;
    std::vector<std::string> m_write_input_dir_list;
    std::vector<std::string> m_write_filename_list;

    void prepare_input_dir_list();
    void prepare_filename_list();
    void initialize_MPI(int argc, char** argv);
    void finalize_MPI();
};

#endif // __APP_WORKLOAD_HPP__
