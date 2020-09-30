#ifndef __DATAFLOW_WORKLOAD_HPP__
#define __DATAFLOW_WORKLOAD_HPP__

class dataflow_workload
{
public:
    dataflow_workload();
    virtual ~dataflow_workload();
    virtual void emulate(int argc, char** argv);
};

#endif // __DATAFLOW_WORKLOAD_HPP__
