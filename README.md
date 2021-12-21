# Wemul: Workflow I/O Emulation Framework (Wemul) v0.2

The Workflow I/O Emulation Framework (Wemul) provides an MPI-enabled
C++ interface for emulating HPC workflow I/O workloads with the features
for specifying data-dependencies among workflow applications and tasks.

For details on its usage, see [docs/README.md](https://github.com/LLNL/Wemul/blob/main/docs/README.md).

## Quickstart
### Dependencies
- C++11
- MPI
- [IOR](https://github.com/LLNL/ior)
- [KVTree](https://github.com/LLNL/KVTree)
- [AXL](https://github.com/ECP-VeloC/AXL)
### How to build
#### Build IOR
Build IOR by following the instructions mentioned [here](https://github.com/LLNL/ior/blob/master/README).
Run the following in the terminal
```shell
export IOR_PATH=/path/to/IOR/executable/including/the/executable/file/name
```
#### Build KVTree
Build KVTree by following the instructions mentioned [here](https://github.com/ECP-VeloC/KVTree/blob/master/README.md).
Run the following in the terminal
```shell
export KVTREE_DIR=/path/to/KVTree/installation/dir
```
#### Build AXL
Build AXL by following the instructions mentioned [here](https://github.com/ECP-VeloC/AXL/blob/master/README.md).
Run the following in the terminal
```shell
export AXL_DIR=/path/to/AXL/installation/dir
```
#### Build Wemul
Run the following commands in the terminal to build Wemul from source
```shell
git clone https://github.com/LLNL/Wemul.git
cd codes
make
```
An executable ```wemul``` will be generated in the codes/build directory
### How to run
We recommend running Wemul with srun/jsrun/mpirun (lrun if on an LLNL system).
An example run command with ```srun``` is as follows
```shell
srun -N 2 -n 16 /path/to/Wemul/executable --type data --subtype app  --read_input_dirs /path/to/gpfs:/path/to/bb --read_filenames D1:D2 --read_block_size 1048576 --read_segment_count 32768 --write_input_dirs /path/to/tmpfs:/path/to/bb --write_filenames D3:D4 --write_block_size 1048576 --write_segment_count 32768 --file_per_process_write
```
List of all parameters can be found [here](https://github.com/LLNL/Wemul/blob/main/docs/README.md#functionality-and-usage).

## Authors
Created by
- Fahim Tahmid Chowdhury    (fchowdhu@cs.fsu.edu) -> Email here for any questions and suggestions
- Yue Zhu                   (yzhu@cs.fsu.edu)
- Francesco Di Natale       (dinatale3@llnl.gov)
- Adam Moody                (moody20@llnl.gov)
- Elsa Gonsiorowski         (gonsiorowski1@llnl.gov)
- Kathryn Mohror            (mohror1@llnl.gov)
- Weikuan Yu                (wyu3@fsu.edu)

To reference Wemul in a publication, please cite the following paper:

* F. Chowdhury, Y. Zhu, F. Di Natale, A. Moody, E. Gonsiorowski, K. Mohror, and W. Yu, "Emulating I/O Behavior in Scientific Workflows on High Performance Computing Systems," 5th International Parallel Data Systems Workshop. November 2020, Virtual Workshop.

## Release
Copyright (c) 2021, Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory.
Copyright (c) 2021, Florida State University. Contributions from
the Computer Architecture and Systems Research Laboratory (CASTL)
at the Department of Computer Science.
<br/>
`LLNL-CODE-816239`. All rights reserved.

Please read https://github.com/LLNL/Wemul/blob/main/LICENSE for full license text.


For release details and restrictions, please read the [LICENSE](https://github.com/LLNL/Wemul/blob/main/LICENSE) and [NOTICE](https://github.com/LLNL/Wemul/blob/main/NOTICE) files.