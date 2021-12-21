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

#ifndef __DATAFLOW_EMULATOR_HPP__
#define __DATAFLOW_EMULATOR_HPP__

#include <memory>
#include <string>
#include "types.hpp"
#include "workflow_emulator.hpp"
#include "config_attributes.hpp"
#include "base_io_api.hpp"

class dataflow_workload;

class dataflow_emulator : public workflow_emulator
{
public:
    dataflow_emulator(config_attributes* conf_attrib);

    virtual ~dataflow_emulator() override;
    virtual void run(int argc, char** argv) override;

protected:
    virtual std::shared_ptr<base_io_api> create_io_api(io_api_types io_api_type);
    virtual dataflow_workload* create_dataflow_workload(dataflow_workload_types dataflow_workload_type);

private:
    config_attributes* m_config_attributes;
    dataflow_workload* m_dataflow_workload;
    std::shared_ptr<base_io_api> m_io_api;
};

#endif // __DATAFLOW_EMULATOR_HPP__
