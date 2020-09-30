#ifndef __WORKFLOW_EMULATOR_HPP__
#define __WORKFLOW_EMULATOR_HPP__

class workflow_emulator
{

public:
    workflow_emulator();
    virtual ~workflow_emulator();
    virtual void run(int argc, char** argv);
};

#endif // __WORKFLOW_EMULATOR_HPP__
