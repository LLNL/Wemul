/*
 * Copyright (c) 2021, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2021, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ior_runner.hpp"

// TODO: better parameterization later
ior_runner::ior_runner(std::vector<std::string> params)
{
    m_params = params;
}

ior_runner::~ior_runner()
{
}

int ior_runner::run()
{
    pid_t _pid;
    int _status;
    char** _argv = convert_params_to_argv();
    if (0 == (_pid = fork()))
    {
        extern char** _environ;
        if (-1 == execve(_argv[0], (char **)_argv, _environ))
        {
            perror("ERROR: %m process failed to execute");
            return -1;
        }
    }

    // Wait until the forked program finishes
    while (0 == waitpid(_pid, &_status, WNOHANG))
    {
        sleep(1);
    }

    printf("%s WEXITSTATUS %d WIFEXITED %d STATUS %d\n",
           _argv[0], WEXITSTATUS(_status), WIFEXITED(_status), _status);

    if (1 != WIFEXITED(_status) || 0 != WEXITSTATUS(_status))
    {
        perror("ERROR: %s");
        return -1;
    }

    return 0;
}

char** ior_runner::convert_params_to_argv()
{
    char** _argv = (char **)malloc((m_params.size() + 1) * sizeof(char *));
    int i = 0;
    for (; i < m_params.size(); i++)
    {
        _argv[i] = (char *)m_params[i].c_str();
    }
    _argv[i] = NULL;

    return _argv;
}
