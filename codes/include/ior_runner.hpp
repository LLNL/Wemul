/*
 * Copyright (c) 2022, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2022, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

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
