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

#include <fstream>
#include <iostream>
#include <stack>
#include <sstream>

#include "dag_parser.hpp"

dag_parser::dag_parser(std::string filepath)
{
    m_filepath = filepath;
    m_app_regex = "\\b(APP)";
    m_task_regex = "\\b(TASK)";
    m_data_regex = "\\b(DATA)";
    m_strict_parent_regex = "\\b(PARENT)";
    m_non_strict_parent_regex = "\\b(NS_PARENT)";
    m_strict_child_regex = "\\b(CHILD)";
    m_non_strict_child_regex = "\\b(NS_CHILD)";
    m_access_regex = "\\b(ACCESS)";
    m_current_task_id = 0;
    m_current_line_no = 1;
    m_app_based = false;
}

dag_parser::~dag_parser()
{
}

void dag_parser::parse_file()
{
    std::fstream _infile(m_filepath, std::ios::in);
    if(!_infile.is_open())
    {
        std::cout << "File not found!" << std::endl;
        return;
    }
    std::string _line;
    while(std::getline(_infile, _line))
    {
        update_apps(_line);
        update_tasks(_line);
        update_data(_line);
        update_relations(_line);
        m_current_line_no++;
    }
}

void dag_parser::extract_dag()
{
    remove_backedges();
}

void dag_parser::remove_backedges()
{
    // for (auto _edge : m_non_strict_edges)
    // {
    //     std::string _start_vertex = _edge.first;
    //     std::cout << "start vertex: " << _start_vertex
    //         << " results from remove_backedge: "
    //         << remove_backedge(_start_vertex)
    //         << std::endl;
    // }
    for (auto _task : m_tasks)
    {
        remove_backedge(_task);
    }
    for (auto _data : m_data_units)
    {
        remove_backedge(_data);
    }
}

void dag_parser::print()
{
    std::cout << "std::unordered_set<std::string> m_apps;" << std::endl;
    for (auto app : m_apps)
    {
        std::cout << app << std::endl;
    }

    std::cout << "std::unordered_map<std::string, std::vector<std::string>> m_app_to_tasks;" << std::endl;
    for (auto app = m_app_to_tasks.begin(); app != m_app_to_tasks.end(); app++)
    {
        std::cout << app->first << std::endl;
        for (auto task : app->second)
        {
            std::cout << task << " ";
        }
        std::cout << std::endl << "-" << std::endl;
    }
    std::cout << "std::unordered_set<std::string> m_tasks;" << std::endl;
    for (auto task : m_tasks)
    {
        std::cout << task << std::endl;
    }
    std::cout << "std::unordered_map<std::string, std::string> m_task_names;" << std::endl;
    for (auto task = m_task_names.begin(); task != m_task_names.end(); task++)
    {
        std::cout << task->first << " : " << task->second << std::endl;
    }
    std::cout << "std::unordered_map<std::string, float> m_task_est_times;" << std::endl;
    for (auto task = m_task_est_times.begin(); task != m_task_est_times.end(); task++)
    {
        std::cout << task->first << " : " << task->second << std::endl;
    }
    std::cout << "std::unordered_set<std::string> m_data_units;" << std::endl;
    for (auto data_unit : m_data_units)
    {
        std::cout << data_unit << std::endl;
    }
    std::cout << "std::unordered_map<std::string, std::string> m_data_unit_names;" << std::endl;
    for (auto data = m_data_unit_names.begin(); data != m_data_unit_names.end(); data++)
    {
        std::cout << data->first << " : " << data->second << std::endl;
    }
    std::cout << "std::unordered_map<std::string, float> m_data_unit_sizes;" << std::endl;
    for (auto data = m_data_unit_sizes.begin(); data != m_data_unit_sizes.end(); data++)
    {
        std::cout << data->first << " : " << data->second << std::endl;
    }
    std::cout << "std::unordered_map<std::string, std::vector<std::string>> m_data_read_to_tasks;" << std::endl;
    for (auto data = m_data_read_to_tasks.begin(); data != m_data_read_to_tasks.end(); data++)
    {
        std::cout << data->first << std::endl;
        for (auto task : data->second)
        {
            std::cout << task << " ";
        }
        std::cout << std::endl << "-" << std::endl;
    }
    std::cout << "std::unordered_map<std::string, std::vector<std::string>> m_data_write_to_tasks;" << std::endl;
    for (auto data = m_data_write_to_tasks.begin(); data != m_data_write_to_tasks.end(); data++)
    {
        std::cout << data->first << std::endl;
        for (auto task : data->second)
        {
            std::cout << task << " ";
        }
        std::cout << std::endl << "-" << std::endl;
    }
    std::cout << "std::unordered_set<std::string> m_non_strict_relations;" << std::endl;
    for (auto _edge : m_non_strict_edges)
    {
        std::cout << _edge.first << ", " << _edge.second << std::endl;
    }
    std::cout << "std::unordered_map<std::string, std::vector<std::string>> m_relations_graph;" << std::endl;
    for (auto parent = m_dataflow_graph.begin(); parent != m_dataflow_graph.end(); parent++)
    {
        std::cout << parent->first << std::endl;
        for (auto child : parent->second)
        {
            std::cout << child << " ";
        }
        std::cout << std::endl << "-" << std::endl;
    }
}

void dag_parser::update_apps(std::string line)
{
    std::smatch _string_itr;
    if (!std::regex_search(line, _string_itr, m_app_regex)) return;
    m_app_based = true;
    std::string _app_info_line = _string_itr.suffix().str();
    std::stringstream _linestream(_app_info_line);
    std::string _blank;
    std::getline(_linestream, _blank, ' ');
    std::string _app_id;
    std::getline(_linestream, _app_id, ' ');
    m_apps.insert(_app_id);
    std::string _app_name;
    std::getline(_linestream, _app_name, ' ');
    std::string _app_num_procs_str;
    std::getline(_linestream, _app_num_procs_str, ' ');
    int _app_num_procs = std::stoi(_app_num_procs_str);
    std::string _app_est_time_str;
    std::getline(_linestream, _app_est_time_str, ' ');
    float _app_est_time = std::stof(_app_est_time_str);
    while(_app_num_procs--)
    {
        std::string _task_id_prefix = "T";
        std::string _task_id_suffix = std::to_string(++m_current_task_id);
        std::string _task_id = _task_id_prefix + _task_id_suffix;
        m_tasks.insert(_task_id);
        m_task_names[_task_id] = _app_name + "." + _task_id_suffix;
        m_task_est_times[_task_id] = _app_est_time;
        m_app_to_tasks[_app_id].push_back(_task_id);
    }
}

void dag_parser::update_tasks(std::string line)
{
    std::smatch _string_itr;
    if (!std::regex_search(line, _string_itr, m_task_regex)) return;
    std::string _task_info_line = _string_itr.suffix().str();
    std::stringstream _linestream(_task_info_line);
    std::string _blank;
    std::getline(_linestream, _blank, ' ');
    std::string _task_id;
    std::getline(_linestream, _task_id, ' ');
    m_tasks.insert(_task_id);
    std::string _task_name;
    std::getline(_linestream, _task_name, ' ');
    m_task_names[_task_id] = _task_name;
    std::string _task_est_time_str;
    std::getline(_linestream, _task_est_time_str, ' ');
    m_task_est_times[_task_id] = std::stof(_task_est_time_str);
}

void dag_parser::update_data(std::string line)
{
    std::smatch _string_itr;
    if (!std::regex_search(line, _string_itr, m_data_regex)) return;
    std::string _data_info_line = _string_itr.suffix().str();
    std::stringstream _linestream(_data_info_line);
    std::string _blank;
    std::getline(_linestream, _blank, ' ');
    std::string _data_id;
    std::getline(_linestream, _data_id, ' ');
    m_data_units.insert(_data_id);
    std::string _data_unit_name;
    std::getline(_linestream, _data_unit_name, ' ');
    m_data_unit_names[_data_id] = _data_unit_name;
    std::string _data_unit_size;
    std::getline(_linestream, _data_unit_size, ' ');
    m_data_unit_sizes[_data_id] = std::stof(_data_unit_size);
}

void dag_parser::update_relations(std::string line)
{
    std::smatch _string_itr, _strict_string_itr, _non_strict_string_itr;
    int _strict_relation_found = std::regex_search(line, _strict_string_itr, m_strict_parent_regex);
    int _non_strict_relation_found = std::regex_search(line, _non_strict_string_itr, m_non_strict_parent_regex);
    int _access_type = 0;
    if (!_strict_relation_found && !_non_strict_relation_found) return;

    if (_strict_relation_found) _string_itr = _strict_string_itr;
    if (_non_strict_relation_found) _string_itr = _non_strict_string_itr;
    std::string _relations_str = _string_itr.suffix().str();
    if(std::regex_search(_relations_str, _string_itr, m_strict_child_regex)
        || std::regex_search(_relations_str, _string_itr, m_non_strict_child_regex))
    {
        std::string _parents_str = _string_itr.prefix().str();
        std::string _suffix_str = _string_itr.suffix().str();
        std::string _children_str;
        if(std::regex_search(_suffix_str, _string_itr, m_access_regex))
        {
            _children_str = _string_itr.prefix().str();
            std::string _access_type_str = _string_itr.suffix().str();
            _access_type = std::stoi(_access_type_str.substr(1, _access_type_str.length()-1));
        }
        else
        {
            _children_str = _suffix_str;
        }
        std::vector<std::string> _parents, _children;
        std::stringstream _parent_linestream(_parents_str);
        std::string _blank;
        std::getline(_parent_linestream, _blank, ' ');
        std::string _parent_id;
        while(std::getline(_parent_linestream, _parent_id, ' '))
        {
            if (m_app_based)
            {
                if (m_data_units.find(_parent_id) != m_data_units.end())
                    _parents.push_back(_parent_id);
                else
                {
                    for(auto _task_id : m_app_to_tasks[_parent_id])
                    {
                        _parents.push_back(_task_id);
                    }
                }
            }
            else _parents.push_back(_parent_id);
        }
        std::stringstream _children_linestream(_children_str);
        std::getline(_children_linestream, _blank, ' ');
        std::string _child_id;
        while(std::getline(_children_linestream, _child_id, ' '))
        {
            if (m_app_based)
            {
                if (m_data_units.find(_child_id) != m_data_units.end())
                    _children.push_back(_child_id);
                else
                {
                    for(auto _task_id : m_app_to_tasks[_child_id])
                    {
                        _children.push_back(_task_id);
                    }
                }
            }
            else _children.push_back(_child_id);
        }
        // shared access
        if(_access_type == 0)
        {
            for(auto _parent_itr : _parents)
            {
                m_dataflow_graph[_parent_itr] = _children;
                m_dataflow_dag[_parent_itr] = _children;
                if (_non_strict_relation_found)
                {
                    for (auto _child_itr : _children)
                    {
                        std::pair<std::string, std::string> _edge = 
                            std::make_pair(_parent_itr, _child_itr);
                        m_non_strict_edges.insert(_edge);
                    }
                }
                if (m_data_units.find(_parent_itr) != m_data_units.end())
                {
                    for (auto _child : _children)
                    {
                        m_data_read_to_tasks[_parent_itr].push_back(_child);
                    }
                }
                else if(m_tasks.find(_parent_itr) != m_tasks.end())
                {
                    for (auto _child : _children)
                    {
                        m_data_write_to_tasks[_parent_itr].push_back(_child);
                    }
                }
            }
        }
        // file-per-process access
        else if(_access_type == 1)
        {
            if (_parents.size() == _children.size())
            {
                for (int i = 0; i < _parents.size(); i++)
                {
                    m_dataflow_graph[_parents[i]].push_back(_children[i]);
                    m_dataflow_dag[_parents[i]].push_back(_children[i]);
                    if (m_data_units.find(_parents[i]) != m_data_units.end())
                    {
                        m_data_read_to_tasks[_parents[i]].push_back(_children[i]);
                    }
                    else if(m_tasks.find(_parents[i]) != m_tasks.end())
                    {
                        m_data_write_to_tasks[_parents[i]].push_back(_children[i]);
                    }
                    if (_non_strict_relation_found)
                    {
                        std::pair<std::string, std::string> _edge = 
                            std::make_pair(_parents[i], _children[i]);
                        m_non_strict_edges.insert(_edge);
                        // m_non_strict_edges.insert(_parents[i] + "," + _children[i]);
                    }
                }
            }
            else
            {
                std::cout << "ERROR: Line: " << m_current_line_no << ": task and data number do not match" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "ERROR: Line: " << m_current_line_no << ": no child for parent" << std::endl;
    }
}

void print_colors(std::unordered_map<std::string, colors> map)
{
    for (auto item : map)
    {
        std::cout << item.first << ":" << item.second << std::endl;
    }
}

bool dag_parser::remove_backedge(std::string start_vertex)
{
    for (auto _task : m_tasks)
    {
        m_colors[_task] = colors::white;
        m_parents[_task] = "";
    }
    for (auto _data : m_data_units)
    {
        m_colors[_data] = colors::white;
        m_parents[_data] = "";
    }
    m_colors[start_vertex] = colors::gray;
    std::stack<std::string> _dfs_stack;
    _dfs_stack.push(start_vertex);
    while (!_dfs_stack.empty())
    {
        std::string _current_vertex = _dfs_stack.top(); _dfs_stack.pop();
        for (auto _child_vertex_id : m_dataflow_dag[_current_vertex])
        {
            m_parents[_child_vertex_id] = _current_vertex;
            if (m_colors[_child_vertex_id] == colors::white)
            {
                m_colors[_child_vertex_id] = colors::gray;
                _dfs_stack.push(_child_vertex_id);
            }
            else if (m_colors[_child_vertex_id] == colors::gray
                || m_colors[_child_vertex_id] == colors::black)
            {
                std::cout << "Cycle found" << std::endl;
                if (!remove_non_strict_edge_from_cycle(start_vertex, _child_vertex_id))
                {
                    std::cout << "ERROR: dataflow has a deadlock!" << std::endl;
                    exit(-1);
                }
                return true;
            }
        }
        m_colors[_current_vertex] = colors::black;
    }
    return false;
}

bool dag_parser::remove_non_strict_edge_from_cycle(std::string start_vertex_id,
    std::string end_vertex_id)
{
    std::pair<std::string, std::string> _edge =
        std::make_pair(end_vertex_id, start_vertex_id);
    if (m_non_strict_edges.find(_edge) != m_non_strict_edges.end())
    {
        // using erase remove idiom
        m_dataflow_dag[end_vertex_id].erase(
            std::remove(m_dataflow_dag[end_vertex_id].begin(),
            m_dataflow_dag[end_vertex_id].end(), start_vertex_id),
            m_dataflow_dag[end_vertex_id].end());
        return true;
    }
    std::string _current_vertex_id = end_vertex_id;
    while(_current_vertex_id != "")
    {
        std::pair<std::string, std::string> _edge =
        std::make_pair(m_parents[_current_vertex_id], _current_vertex_id);
        if (m_non_strict_edges.find(_edge) != m_non_strict_edges.end())
        {
            // using erase remove idiom
            m_dataflow_dag[m_parents[_current_vertex_id]].erase(
                std::remove(m_dataflow_dag[m_parents[_current_vertex_id]].begin(),
                m_dataflow_dag[m_parents[_current_vertex_id]].end(), _current_vertex_id),
                m_dataflow_dag[m_parents[_current_vertex_id]].end());
            return true;
        }
        _current_vertex_id = m_parents[_current_vertex_id];
    }
    return false;
}
