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
