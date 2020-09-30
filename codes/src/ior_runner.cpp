#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>              /* tolower() */
#include <errno.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#include <sys/stat.h>           /* struct stat */
#include <time.h>

#ifndef _WIN32
# include <sys/time.h>           /* gettimeofday() */
# include <sys/utsname.h>        /* uname() */
#endif

#include <assert.h>

extern "C"
{
#include "ior.h"
#include "ior-internal.h"
#include "aiori.h"
#include "utilities.h"
#include "parse_options.h"
}

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
    return ior_main(m_params.size(), convert_params_to_argv());
}

char** ior_runner::convert_params_to_argv()
{
    char **_argv = (char**) malloc((m_params.size() + 1) * sizeof(char*));
    int i = 0;
    for(; i < m_params.size(); i++)
    {
        _argv[i] = (char*) m_params[i].c_str();
    }
    _argv[i] = NULL;

    return _argv;
}
