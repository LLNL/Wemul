#ifndef __PRODUCER_CONSUMER_HPP__
#define __PRODUCER_CONSUMER_HPP__

#include <mpi.h>

#include "dataflow_workload.hpp"

class producer_consumer : public dataflow_workload
{
public:
    producer_consumer(std::string directory, bool inter_node,
        int num_ranks_per_node, bool producer_only = false,
        bool consumer_only = false, int block_size = 1000000,
        int segment_count = 1000000);

    virtual ~producer_consumer() override;
    virtual void emulate(int argc, char** argv) override;

private:
    void initialize_MPI(int argc, char** argv);
    void finalize_MPI();
    MPI_Group m_world_group;
    MPI_Group m_producer_group;
    MPI_Group m_consumer_group;
    // MPI_Group m_both_group;
    MPI_Comm m_producer_comm;
    MPI_Comm m_consumer_comm;
    // MPI_Comm m_both_comm;
    std::string m_directory;
    bool m_inter_node;
    int m_num_ranks_per_node;
    bool m_producer_only;
    bool m_consumer_only;
    int m_block_size;
    int m_segment_count;
};

#endif // __PRODUCER_CONSUMER_HPP__
