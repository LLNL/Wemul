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

#include <iostream>

#include "dag_generator.hpp"
#include "dag_parser.hpp"
#include "graph.hpp"

dag_generator::dag_generator(std::string filepath)
{
    m_graph = std::make_shared<graph>();
    m_dag_parser = std::make_shared<dag_parser>(filepath, m_graph);
    m_dag_parser->parse_file();
    // m_dag_parser->print();
    m_graph->generate_dag();
    m_dag_parser->print();
}

void dag_generator::print()
{
    std::cout << "std::shared_ptr<graph>::std::shared_ptr<GraphDS> m_graph.m_dataflow_dag;" << std::endl;
    std::shared_ptr<GraphDS> dataflow_dag = m_graph->get_dataflow_dag();
    for (auto itr = dataflow_dag->begin(); itr != dataflow_dag->end(); itr++)
    {
        std::cout << itr->first << std::endl;
        for (auto child : itr->second)
        {
            std::cout << child << " ";
        }
        std::cout << std::endl << "-" << std::endl;
    }
}

dag_generator::~dag_generator()
{
}
