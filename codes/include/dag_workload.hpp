#ifndef __DAG_WORKLOAD_HPP__
#define __DAG_WORKLOAD_HPP__

#include <map>
#include <memory>
#include <mpi.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "dataflow_workload.hpp"
#include "types.hpp"

// forward declaration
class dag_parser;

class dag_workload : public dataflow_workload
{
public:
    dag_workload(bool use_ior,
        std::string directory,
        std::string filename,
        dag_workload_types dag_workload_type,
        workload_access_types dag_workload_access_type,
        std::string dag_filepath,
        int block_size,
        int segment_count);

    virtual ~dag_workload() override;
    virtual void emulate(int argc, char** argv) override;

private:
    // For IOR enabled dags
    bool m_use_ior;
    std::string m_directory;
    std::string m_filename;
    dag_workload_types m_dag_workload_type;
    workload_access_types m_dag_workload_access_type;
    std::shared_ptr<dag_parser> m_dag_parser;

    void single_task_single_data();
    void single_task_multi_data();
    void multi_task_single_data();
    void multi_task_multi_data();

    // For DAG definition file enabled
    std::string m_dag_filepath;
    int m_block_size;
    int m_segment_count;
    std::unordered_map<std::string, std::unordered_set<int>> m_non_strict_data_ranks;
    std::unordered_map<std::string, MPI_Comm> m_read_comms;
    std::unordered_map<std::string, MPI_Comm> m_write_comms;
    std::map<int, std::unordered_set<std::string>> m_data_units_per_stage;
    // bool* m_rank_reading;
    MPI_Group m_world_group;

    void create_groups_from_dag_file();
    void print_dag();
    void initialize_MPI(int argc, char** argv);
    void finalize_MPI();
    void read_or_write(int world_rank, int comm_rank, int block_size, int segment_count,
        std::string directory, std::string filename, bool write = false);
};

#endif // __DAG_WORKLOAD_HPP__
