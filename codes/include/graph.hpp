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

#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum status
{
    unvisited,
    visiting,
    visited,
    removed
};

// class vertex
// {
// public:
//     vertex(std::string vertex_id);
//     vertex()
//     {
//         m_vertex_id = "";
//         m_parent_vertex = nullptr;
//     }
//     ~vertex();
//     void set_parent(std::shared_ptr<vertex> parent_vertex);
//     std::shared_ptr<vertex> get_parent() const;
//     void set_id(std::string vertex_id);
//     std::string get_id() const;

//     bool operator==(const vertex& v) const
//     {
//         return m_vertex_id == v.get_id();
//     }

//     vertex operator=(const vertex& v) const
//     {
//         vertex _new_vertex;
//         _new_vertex.set_parent(v.get_parent());
//         _new_vertex.set_id(v.get_id());
//         return _new_vertex;
//     }

// private:
//     std::string m_vertex_id;
//     std::shared_ptr<vertex> m_parent_vertex;
// };

// namespace std
// {
// template <>
// struct hash<vertex>
// {
//     std::size_t operator() (const vertex& v) const
//     {
//         std::string _id_str = v.get_id();
//         int _ret = 0;
//         for(auto _id_ch : _id_str)
//         {
//             _ret ^= (int) _id_ch;
//         }

//         return _ret;
//     }
// };
// }

// type definition of graph data structure
typedef std::unordered_map<std::string, std::vector<std::string>> GraphDS;

class graph
{
public:
    graph();
    ~graph();

    void generate_dag();

    std::shared_ptr<GraphDS> get_dataflow_graph()
    {
        return m_dataflow_graph;
    }

    std::shared_ptr<GraphDS> get_dataflow_dag()
    {
        return m_dataflow_dag;
    }

    std::shared_ptr<std::unordered_set<std::string>> get_vertices()
    {
        return m_vertices;
    }

    void set_dataflow_graph(const std::shared_ptr<GraphDS> dataflow_graph)
    {
        m_dataflow_graph = dataflow_graph;
    }

    void set_dataflow_dag(const std::shared_ptr<GraphDS> dataflow_dag)
    {
        m_dataflow_dag = dataflow_dag;
    }

    void set_vertices(const std::shared_ptr<std::unordered_set<std::string>> vertices)
    {
        m_vertices = vertices;
    }

    std::shared_ptr<std::unordered_set<std::string>> get_non_strict_edges()
    {
        return m_non_strict_edges;
    }

    void add_non_strict_edge(std::string src, std::string dest)
    {
        std::string edge_str = src + ":" + dest;
        m_non_strict_edges->insert(edge_str);
    }

    bool has_non_strict_edge(std::string src, std::string dest)
    {
        std::string edge_str_1 = src + ":" + dest;
        std::string edge_str_2 = src + ":" + dest;
        return m_non_strict_edges->find(edge_str_1) != m_non_strict_edges->end() ||
               m_non_strict_edges->find(edge_str_2) != m_non_strict_edges->end();
    }

private:
    std::shared_ptr<GraphDS> m_dataflow_graph;
    std::shared_ptr<GraphDS> m_dataflow_dag;
    std::unordered_map<std::string, std::string> m_child_to_parent;
    std::unordered_map<std::string, status> m_statuses;
    std::shared_ptr<std::unordered_set<std::string>> m_vertices;
    std::unordered_set<std::string> m_starting_vertices;
    std::unordered_set<std::string> m_ending_vertices;
    std::string m_current_tail_vertex;
    // element structure -> src_vertex:dest_vertex
    std::shared_ptr<std::unordered_set<std::string>> m_non_strict_edges;

    void add_edge(std::string vertex_u, std::string vertex_v);
    void add_children(std::string parent_vertex,
                      std::vector<std::string> children_vertices);
    bool dfs_cycle(std::string current_vertex);
    bool find_back_edge();
    void remove_child_from_vertex(std::string parent_vertex,
                                  std::string rm_child_vertex);
    bool remove_non_strict_edge_from_cycle(std::string tail_vertex_id);
};

#endif // __GRAPH_HPP__
