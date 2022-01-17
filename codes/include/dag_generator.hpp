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

#ifndef __DAG_GENERATOR_HPP__
#define __DAG_GENERATOR_HPP__

#include <memory>
#include <string>

class dag_parser;
class graph;

class dag_generator
{
public:
    dag_generator(std::string filepath);
    ~dag_generator();

    void print();

private:
    std::shared_ptr<dag_parser> m_dag_parser;
    std::shared_ptr<graph> m_graph;
};

#endif // __DAG_GENERATOR_HPP__
