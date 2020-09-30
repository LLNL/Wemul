#ifndef __DEEP_LEARNING_HPP__
#define __DEEP_LEARNING_HPP__

#include <string>
#include <cstring>
#include <vector>

#include "dataflow_workload.hpp"

// TODO: Rename it to Distributed DL IO later?
class deep_learning : public dataflow_workload
{
public:
    deep_learning(std::string directory = ".", bool use_ior = false,
        int num_epochs = 1, int comp_time_per_epoch = 2);
    virtual ~deep_learning() override;
    virtual void emulate(int argc, char** argv) override;
    void set_directory(std::string directory);

private:
    void populate_list_of_all_filepaths();

    std::string m_directory;
    std::vector<std::string> m_filepath_list;
    std::vector<int> m_file_ids;
    bool m_use_ior;
    int m_num_epochs;
    int m_comp_time_per_epoch;
};

#endif // __DEEP_LEARNING_HPP__
