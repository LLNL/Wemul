#ifndef __DATAFLOW_EMULATOR_HPP__
#define __DATAFLOW_EMULATOR_HPP__

#include <string>
#include "types.hpp"
#include "workflow_emulator.hpp"
#include "config_attributes.hpp"

class dataflow_workload;

class dataflow_emulator : public workflow_emulator
{
public:
    dataflow_emulator(config_attributes* conf_attrib);

    virtual ~dataflow_emulator() override;
    virtual void run(int argc, char** argv) override;

protected:
    virtual dataflow_workload* create_dataflow_workload(dataflow_workload_types dataflow_workload_type);

private:
    config_attributes* m_config_attributes;
    dataflow_workload* m_dataflow_workload;
};

#endif // __DATAFLOW_EMULATOR_HPP__
