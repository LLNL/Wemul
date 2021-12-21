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

#include "graph.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

// // Hint: For now, we do not need an abstraction for vertex.
// vertex::vertex(std::string vertex_id)
// {
//     m_vertex_id = vertex_id;
//     m_parent_vertex = nullptr;
// }

// vertex::~vertex()
// {
// }

// void vertex::set_parent(std::shared_ptr<vertex> parent_vertex)
// {
//     m_parent_vertex = parent_vertex;
// }

// std::shared_ptr<vertex> vertex::get_parent() const
// {
//     return m_parent_vertex;
// }

// void vertex::set_id(std::string vertex_id)
// {
//     m_vertex_id = vertex_id;
// }

// std::string vertex::get_id() const
// {
//     return m_vertex_id;
// }

// void print_colors(std::unordered_map<std::string, colors> map)
// {
//     for (auto item : map)
//     {
//         std::cout << item.first << ":" << item.second << std::endl;
//     }
// }

// // Hint: This is iterative DFS instead of recursive call.
// //       Use this if the callstack size is too big for the system
// std::string dag_parser::find_backedge(std::string start_vertex)
// {
//     std::string _tail_vertex_id;
//     for (auto _task : m_tasks)
//     {
//         m_colors[_task] = colors::white;
//         m_parents[_task] = "";
//         m_discovery_time[_task] = 0;
//     }
//     for (auto _data : m_data_units)
//     {
//         m_colors[_data] = colors::white;
//         m_parents[_data] = "";
//         m_discovery_time[_data] = 0;
//     }

//     std::stack<std::string> _dfs_stack;
//     _dfs_stack.push(start_vertex);
//     std::cout << std::endl << "pushed: " << start_vertex << std::endl;
//     while (!_dfs_stack.empty())
//     {
//         // for (auto _item : m_colors)
//         // {
//         //     std::cout << _item.first << ":" << _item.second << ",";
//         // }
//         // std::cout << std::endl;
//         // std::cout << "top of the stack: " << _dfs_stack.top() << std::endl;
//         std::string _current_vertex = _dfs_stack.top(); _dfs_stack.pop();
//         std::cout << "popped: " << _current_vertex << ", color: " << m_colors[_current_vertex] << std::endl;
//         m_colors[_current_vertex] = colors::gray;
//         if (m_parents[_current_vertex] != "")
//         {
//             m_discovery_time[_current_vertex] = m_discovery_time[m_parents[_current_vertex]] + 1;
//         }
//         for (auto _child_vertex_id : m_dataflow_dag[_current_vertex])
//         {
//             // std::cout << "child vertex id: " << _child_vertex_id << std::endl;
//             m_parents[_child_vertex_id] = _current_vertex;
//             if (m_colors[_child_vertex_id] != colors::white)
//             {
//                 std::cout << "Cycle found" << std::endl;
//                 std::cout << ">> Back edge: " << m_parents[_current_vertex] << "," << _current_vertex << std::endl;
//                 _tail_vertex_id = _current_vertex;
//             }
//             else
//             {
//                 _dfs_stack.push(_child_vertex_id);
//                 std::cout << "pushed: " << _child_vertex_id << std::endl;
//             }
//         }
//         // make it black if all children are pushed to stack already
//         m_colors[_current_vertex] = colors::black;
//         std::cout << _current_vertex << ": all children covered" << std::endl;
//     }
//     return _tail_vertex_id;
// }

graph::graph()
{
    m_dataflow_graph = std::make_shared<GraphDS>();
    // m_dataflow_dag = std::make_shared<GraphDS>();
    m_vertices = std::make_shared<std::unordered_set<std::string>>();
    m_non_strict_edges = std::make_shared<std::unordered_set<std::string>>();
}

graph::~graph()
{
}

void graph::generate_dag()
{
    m_dataflow_dag = m_dataflow_graph;
    while (find_back_edge())
    {
        remove_non_strict_edge_from_cycle(m_current_tail_vertex);
        m_child_to_parent.clear();
    }
    // while(self.find_back_edge() and self.max_treatable_cycle_count):
    //     # remove back edge from the graph
    //     # TODO: remove only non-strict edges
    //     #       if there is no non-strict edge in the cycle path
    //     #       there should be a deadlock and should be handled exceptionally
    //     # Hint: keep track of the parents while finding a loop, reuse this path
    //             information to find parallelizability afterward
    //     start_vertex_id = self.tail_vertex.vertex_id
    //     current_vertex = self.tail_vertex.parent
    //     has_deadlock = True
    //     if (self.cycle_end, self.cycle_start) in self.nonstrict_edge_set:
    //         self.remove_child_from_vertex(self.cycle_end, self.cycle_start)
    //         self.starting_vertices.add(self.cycle_start)
    //         self.ending_vertices.add(self.cycle_end)
    //         self.max_treatable_cycle_count -= 1
    //         has_deadlock = False
    //     else:
    //         while(True):
    //             if (current_vertex.parent.vertex_id, current_vertex.vertex_id) in self.nonstrict_edge_set:
    //                 self.remove_child_from_vertex(current_vertex.parent.vertex_id, current_vertex.vertex_id)
    //                 self.starting_vertices.add(current_vertex.vertex_id)
    //                 self.ending_vertices.add(current_vertex.parent.vertex_id)
    //                 self.max_treatable_cycle_count -= 1
    //                 has_deadlock = False
    //             current_vertex = current_vertex.parent
    //             if current_vertex.vertex_id == start_vertex_id:
    //                 break
    //         if has_deadlock:
    //             print("error: dataflow has a deadlock")
    //             exit(1)
    // # check if there is still any cycle with strict edges
    // if self.max_treatable_cycle_count <= 0 and self.find_back_edge():
    //     print("error: dataflow has a deadlock")
    //     exit(1)

}
    // def set_vertices(self, vertices):
    //     self.vertices = vertices

    // def set_max_treatable_cycle_count(self, count):
    //     self.max_treatable_cycle_count = count

    // def set_nonstrictness_edge_set(self, nonstrict_edge_set):
    //     self.nonstrict_edge_set = nonstrict_edge_set

    // def print_graph(self):
    //     print(self.graph)
    //     print(self.dag)

void graph::add_edge(std::string vertex_a, std::string vertex_b)
{
    if (m_dataflow_graph->find(vertex_a) == m_dataflow_graph->end())
    {
        (*m_dataflow_graph)[vertex_a] = std::vector<std::string>();
    }
    (*m_dataflow_graph)[vertex_a].push_back(vertex_b);
}

void graph::add_children(std::string parent_vertex,
                         std::vector<std::string> children_vertices)
{
    if (m_dataflow_graph->find(parent_vertex) == m_dataflow_graph->end())
    {
        (*m_dataflow_dag)[parent_vertex] = std::vector<std::string>();
    }
    for (std::string _child_vertex : children_vertices)
    {
        (*m_dataflow_dag)[parent_vertex].push_back(_child_vertex);
    }
}

bool graph::dfs_cycle(std::string current_vertex)
{
    if (m_dataflow_dag->find(current_vertex) == m_dataflow_dag->end())
    {
        m_statuses[current_vertex] = status::removed;
        m_vertices->erase(current_vertex);
        return false;
    }
    m_statuses[current_vertex] = status::visiting;
    std::cout << current_vertex << ",";
    if (!(*m_dataflow_dag)[current_vertex].size())
    {
        m_ending_vertices.insert(current_vertex);
    }
    for (auto _child_vertex : (*m_dataflow_dag)[current_vertex])
    {
        m_child_to_parent[_child_vertex] = current_vertex;
        if (m_statuses[_child_vertex] == status::visited) continue;
        if (m_statuses[_child_vertex] == status::visiting)
        {
            std::cout << std::endl;
            std::cout << "found cycle: " << current_vertex <<
                " " << _child_vertex << std::endl;
            m_current_tail_vertex = _child_vertex;
            return true;
        }
        if (dfs_cycle(_child_vertex)) return true;
    }
    m_statuses[current_vertex] = status::visited;
    return false;
}

// # For finding cycle in a graph in linear time inspired by algorithm from CLRS book
// # Keep three sets for faster and cleaner graph coloring
// # UNVISITED : Initial status of all the vertices. Vertex is yet to be processed.
// # VISITING: Vertex is being processed. Means DFS for this vertex has started, but not finished yet.
// #       All descendants of the vertex are not processed yet or vertex is in the function call stack
// # VISITED : Vertex and all its descendants have been processed.
// #         While doing DFS, if an edge goes from a current vertex to a GRAY vertex
// #         then the edge creates a cycle
// # Time complexity: O(V+E)

bool graph::find_back_edge()
{
    std::cout << std::endl << "DEBUG:::: graph::find_back_edge" << std::endl;
    for (auto _current_vertex : *m_vertices)
    {
        m_statuses[_current_vertex] = status::unvisited;
    }
    for (auto _status_map_item : m_statuses)
    {
        std::cout << std::endl << "DEBUG:::: in loop graph::find_back_edge" << std::endl;
        std::string _current_vertex = _status_map_item.first;
        status _current_vertex_status = _status_map_item.second;
        std::cout << _current_vertex << " - " << _current_vertex_status << std::endl;
        if (_current_vertex_status == status::unvisited)
        {
            if (dfs_cycle(_current_vertex))
            {
                return true;
            }
        }
    }
    return false;
}

void graph::remove_child_from_vertex(std::string parent_vertex,
                                     std::string rm_child_vertex)
{
    if (m_dataflow_dag->find(parent_vertex) == m_dataflow_dag->end())
    {
        return;
    }
    std::vector<std::string> _children = (*m_dataflow_dag)[parent_vertex];
    std::vector<std::string>::iterator _rm_child_itr =
        std::find(_children.begin(), _children.end(), rm_child_vertex);
    _children.erase(_rm_child_itr);
    if (!_children.size()) m_dataflow_dag->erase(parent_vertex);
}

bool graph::remove_non_strict_edge_from_cycle(std::string tail_vertex_id)
{
    std::string _current_vertex_id = tail_vertex_id;
    while(_current_vertex_id != "")
    {
        if (m_child_to_parent.find(_current_vertex_id) == m_child_to_parent.end())
        {
            break;
        }
        std::string _parent_id = m_child_to_parent[_current_vertex_id];
        if (m_dataflow_dag->find(_parent_id) != m_dataflow_dag->end() &&
            has_non_strict_edge(_parent_id, _current_vertex_id))
        {
            // using erase remove idiom
            (*m_dataflow_dag)[_parent_id].erase(
                std::remove((*m_dataflow_dag)[_parent_id].begin(),
                (*m_dataflow_dag)[_parent_id].end(), _current_vertex_id),
                (*m_dataflow_dag)[_parent_id].end());
            // if there is no child, remove the entry
            if (m_dataflow_dag->find(_parent_id) != m_dataflow_dag->end() &&
                (*m_dataflow_dag)[_parent_id].size() == 0)
            {
                std::cout << std::endl;
                std::cout << "1 - No child for " << _parent_id << std::endl;
                m_dataflow_dag->erase(_parent_id);
            }
            return true;
        }
        // if there is no child, remove the entry
        if (m_dataflow_dag->find(_current_vertex_id) != m_dataflow_dag->end() &&
            (*m_dataflow_dag)[_current_vertex_id].size() == 0)
        {
            std::cout << std::endl;
            std::cout << "2 - No child for " << _current_vertex_id << std::endl;
            m_dataflow_dag->erase(_current_vertex_id);
        }
        _current_vertex_id = _parent_id;
    }
    return false;
}
