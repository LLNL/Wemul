#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

#include "dag_workload.hpp"
#include "ior_runner.hpp"
#include "utils.hpp"

dag_workload::dag_workload(bool use_ior,
    std::string directory,
    std::string filename,
    dag_workload_types dag_workload_type,
    workload_access_types dag_workload_access_type,
    std::string dag_filepath,
    int block_size,
    int segment_count)
    : dataflow_workload()
{
    m_use_ior = use_ior;
    m_directory = directory;
    m_filename = filename;
    m_dag_workload_type = dag_workload_type;
    m_dag_workload_access_type = dag_workload_access_type;
    m_dag_filepath = dag_filepath;
    m_block_size = block_size;
    m_segment_count = segment_count;
}

dag_workload::~dag_workload()
{
}

void dag_workload::emulate(int argc, char** argv)
{
    if (m_use_ior)
    {
        switch (m_dag_workload_type)
        {
            case dag_workload_types::e_single_task_single_data:
            {
                single_task_single_data();
                break;
            }
            case dag_workload_types::e_single_task_multi_data:
            {
                single_task_multi_data();
                break;
            }
            case dag_workload_types::e_multi_task_single_data:
            {
                multi_task_single_data();
                break;
            }
            case dag_workload_types::e_multi_task_multi_data:
            {
                multi_task_multi_data();
                break;
            }
            default:
            {
                std::cout << "Unsupported workload!" << std::endl;
            }
        }
    }
    else
    {
        initialize_MPI(argc, argv);
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        // with staging
        for (auto _itr : m_data_units_per_stage)
        {
            std::unordered_set<std::string> _data_units = _itr.second;
            for (auto _itr2 : _data_units)
            {
                std::string _filepath = m_directory + "/" + _itr2;
                bool read_check = (m_non_strict_data_ranks[_itr2].find(world_rank) == m_non_strict_data_ranks[_itr2].end());
                MPI_Comm _mpi_write_comm = MPI_COMM_NULL;
                if (m_write_comms.find(_itr2) != m_write_comms.end())
                    _mpi_write_comm = m_write_comms[_itr2];
                if (MPI_COMM_NULL != _mpi_write_comm)
                {
                    int _rank;
                    MPI_Comm_rank(_mpi_write_comm, &_rank);
                    // std::cout << _rank << ":" << world_rank << std::endl;
                    utils::read_or_write(_filepath, m_block_size, m_segment_count, true, read_check);
                }
                MPI_Comm _mpi_read_comm = MPI_COMM_NULL;
                if (m_read_comms.find(_itr2) != m_read_comms.end())
                    _mpi_read_comm = m_read_comms[_itr2];
                if (MPI_COMM_NULL != _mpi_read_comm)
                {
                    int _rank;
                    MPI_Comm_rank(_mpi_read_comm, &_rank);
                    utils::read_or_write(_filepath, m_block_size, m_segment_count, false, read_check);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        // // without staging information
        // for (auto _itr : m_read_comms)
        // {
        //     MPI_Comm _mpi_comm = _itr.second;
        //     if (MPI_COMM_NULL != _mpi_comm)
        //     {
        //         int _rank;
        //         MPI_Comm_rank(_mpi_comm, &_rank);
        //         read_or_write(world_rank, _rank, m_block_size, m_segment_count,
        //             m_directory, _itr.first);
        //         // std::cout << world_rank << ":" << _rank << " reads " << _itr.first << std::endl;
        //     }
        // }
        // for (auto _itr : m_write_comms)
        // {
        //     MPI_Comm _mpi_comm = _itr.second;
        //     if (MPI_COMM_NULL != _mpi_comm)
        //     {
        //         int _rank;
        //         MPI_Comm_rank(_mpi_comm, &_rank);
        //         read_or_write(world_rank, _rank, m_block_size, m_segment_count,
        //             m_directory, _itr.first, true);
        //         // std::cout << world_rank << ":" << _rank << " writes " << _itr.first << std::endl;
        //     }
        // }

        finalize_MPI();
    }
}

void dag_workload::single_task_single_data()
{
    std::vector<std::string> _params = {"ior", "-a", "MPIIO"};
    if (m_dag_workload_access_type == workload_access_types::e_read)
    {
        _params.push_back("-r");
    }
    else if (m_dag_workload_access_type == workload_access_types::e_write)
    {
        _params.push_back("-w");
    }
    _params.push_back("-k");
    _params.push_back("-o");
    _params.push_back(m_directory + std::string("/") + m_filename);
    ior_runner _ior_runner(_params);
    _ior_runner.run();
}

void dag_workload::single_task_multi_data()
{
    // std::vector<std::string> _params = {"ior", "-a", "MPIIO", "-$", "-#", m_directory};
    // ior_runner _ior_runner(_params);
    // _ior_runner.run();
}

void dag_workload::multi_task_single_data()
{
    std::vector<std::string> _params = {"ior", "-a", "MPIIO"};
    if (m_dag_workload_access_type == workload_access_types::e_read)
    {
        _params.push_back("-r");
    }
    else if (m_dag_workload_access_type == workload_access_types::e_write)
    {
        _params.push_back("-w");
    }
    _params.push_back("-k");
    _params.push_back("-o");
    _params.push_back(m_directory + std::string("/") + m_filename);
    ior_runner _ior_runner(_params);
    _ior_runner.run();
}

void dag_workload::multi_task_multi_data()
{
    // std::vector<std::string> _params = {"ior", "-a", "MPIIO", "-$", "-#", m_directory};
    // ior_runner _ior_runner(_params);
    // _ior_runner.run();
}

void dag_workload::create_groups_from_dag_file()
{
    std::fstream _infile(m_dag_filepath, std::ios::in);
    if(!_infile.is_open())
    {
        std::cout << "File not found!" << std::endl;
        return;
    }
    std::string _line;
    bool _is_read = false;
    bool _is_non_strict = false;
    bool _is_stages = false;
    std::unordered_map<std::string, int> _task_id_to_rank;
    int _current_rank = 0;
    while(std::getline(_infile, _line))
    {
        if (_line == "READ")
        {
            _is_read = true;
            continue;
        }
        else if (_line == "WRITE")
        {
            _is_read = false;
            continue;
        }
        if (_line == "NONSTRICT")
        {
            _is_non_strict = true;
            continue;
        }
        if (_line == "STAGES")
        {
            _is_stages = true;
            continue;
        }
        if (!_is_non_strict && !_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _data_id;
            std::getline(_linestream, _data_id, ':');
            std::string _tasks_line;
            std::getline(_linestream, _tasks_line, ':');
            std::stringstream _tasks_linestream(_tasks_line);
            std::string _task_id;
            std::vector<int> _task_ranks;
            MPI_Group _mpi_group;
            MPI_Comm _mpi_comm;
            int _group_size = 0;
            while(std::getline(_tasks_linestream, _task_id, ','))
            {
                if(_task_id_to_rank.find(_task_id) == _task_id_to_rank.end())
                {
                    _task_id_to_rank[_task_id] = _current_rank;
                    _current_rank++;
                }
                _task_ranks.push_back(_task_id_to_rank[_task_id]);
                _group_size++;
            }
            int *_task_ranks_array = new int[_task_ranks.size()];
            int _id = 0;
            for (int _itr : _task_ranks)
            {
                _task_ranks_array[_id] = _itr;
                _id++;
            }
            MPI_Group_incl(m_world_group, _group_size, _task_ranks_array, &_mpi_group);
            MPI_Comm_create_group(MPI_COMM_WORLD, _mpi_group, 0, &_mpi_comm);
            delete[] _task_ranks_array;
            if(_is_read) m_read_comms[_data_id] = _mpi_comm;
            else m_write_comms[_data_id] = _mpi_comm;
        }
        else if (!_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _data_id;
            std::getline(_linestream, _data_id, ':');
            std::string _task_id;
            std::getline(_linestream, _task_id, ':');
            if(m_non_strict_data_ranks.find(_data_id) == m_non_strict_data_ranks.end())
            {
                m_non_strict_data_ranks[_data_id] = std::unordered_set<int>();
            }
            m_non_strict_data_ranks[_data_id].insert(_task_id_to_rank[_task_id]);
        }
        if (_is_stages)
        {
            std::stringstream _linestream(_line);
            std::string _stage_str;
            std::getline(_linestream, _stage_str, ':');
            int _stage = std::stoi(_stage_str);
            std::string _data_units_str;
            std::getline(_linestream, _data_units_str, ':');
            if(m_data_units_per_stage.find(_stage) == m_data_units_per_stage.end())
            {
                m_data_units_per_stage[_stage] = std::unordered_set<std::string>();
            }
            std::stringstream _data_units_stream(_data_units_str);
            std::string _data_id;
            while(std::getline(_data_units_stream, _data_id, ','))
            {
                m_data_units_per_stage[_stage].insert(_data_id);
            }
        }
    }
}

void dag_workload::print_dag()
{
    // for (auto _itr : m_read_ranks)
    // {
    //     std::cout << _itr.first << ":";
    //     for (auto _itr2 : _itr.second)
    //     {
    //         std::cout << _itr2 << ",";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "WRITE" << std::endl;
    // for (auto _itr : m_write_ranks)
    // {
    //     std::cout << _itr.first << ":";
    //     for (auto _itr2 : _itr.second)
    //     {
    //         std::cout << _itr2 << ",";
    //     }
    //     std::cout << std::endl;
    // }
    std::cout << "NONSTRICT" << std::endl;
    for (auto _itr : m_non_strict_data_ranks)
    {
        std::cout << _itr.first << ":";
        for (auto _itr2 : _itr.second)
        {
            std::cout << _itr2 << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "STAGES" << std::endl;
    for (auto _itr : m_data_units_per_stage)
    {
        std::cout << _itr.first << ":";
        for (auto _itr2 : _itr.second)
        {
            std::cout << _itr2 << ",";
        }
        std::cout << std::endl;
    }
}

void dag_workload::initialize_MPI(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // m_rank_reading = new bool[world_size];
    // for(int i=0; i<world_size; i++)
    // {
    //     m_rank_reading[i] = false;
    // }

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Comm_group(MPI_COMM_WORLD, &m_world_group);

    create_groups_from_dag_file();
}

void dag_workload::finalize_MPI()
{
    // delete[] m_rank_reading;
    MPI_Group_free(&m_world_group);
    MPI_Finalize();
}
