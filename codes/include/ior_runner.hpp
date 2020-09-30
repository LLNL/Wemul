#ifndef __IOR_RUNNER_HPP__
#define __IOR_RUNNER_HPP__

#include <string>
#include <vector>

class ior_runner
{
public:
    ior_runner(std::vector<std::string> params);
    ~ior_runner();
    int run();
private:
    char** convert_params_to_argv();
    std::vector<std::string> m_params;
};

#endif // __IOR_RUNNER_HPP__
