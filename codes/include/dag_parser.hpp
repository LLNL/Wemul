/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2020, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 *
 * LLNL-CODE-816239. All rights reserved.
 *
 * This is the license for Wemul.
 * For details, see https://github.com/LLNL/Wemul
 * Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.
 */

#ifndef __DAG_PARSER_HPP__
#define __DAG_PARSER_HPP__

#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class dag_parser
{
public:
    dag_parser(std::string filepath);
    ~dag_parser();
    void parse_file();
    void extract_dag();
    void print();

private:
    void update_apps(std::string line);
    void update_tasks(std::string line);
    void update_data(std::string line);
    void update_relations(std::string line);

    // extracting dag
    bool has_cycle(std::string start_vertex);
    bool find_cycle(std::string current_vertex);

    std::string m_filepath;

    std::regex m_app_regex;
    std::regex m_task_regex;
    std::regex m_data_regex;
    std::regex m_strict_parent_regex;
    std::regex m_non_strict_parent_regex;
    std::regex m_strict_child_regex;
    std::regex m_non_strict_child_regex;
    std::regex m_access_regex;

    std::unordered_set<std::string> m_apps;
    std::unordered_map<std::string, std::vector<std::string>> m_app_to_tasks;

    std::unordered_set<std::string> m_tasks;
    std::unordered_map<std::string, std::string> m_task_names;
    std::unordered_map<std::string, float> m_task_est_times;

    std::unordered_set<std::string> m_data_units;
    std::unordered_map<std::string, std::string> m_data_unit_names;
    std::unordered_map<std::string, float> m_data_unit_sizes;

    // These are stages; assign values to these maps according to the access type
    std::unordered_map<std::string, std::vector<std::string>> m_data_read_to_tasks;
    std::unordered_map<std::string, std::vector<std::string>> m_data_write_to_tasks;
    std::set<std::pair<std::string, std::string>> m_non_strict_edges;

    // maintaining dataflow graph
    std::unordered_map<std::string, std::vector<std::string>> m_dataflow_graph;
    std::unordered_map<std::string, std::vector<std::string>> m_dataflow_dag;
    std::unordered_set<std::string> m_white_vertices;
    std::unordered_set<std::string> m_gray_vertices;
    std::unordered_set<std::string> m_black_vertices;
    std::unordered_set<std::string> m_starting_vertices;
    std::unordered_set<std::string> m_ending_vertices;

    int m_current_task_id;
    int m_current_line_no;
    bool m_app_based;
};

#endif // __DAG_PARSER_HPP__
