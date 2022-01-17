#!/bin/bash

#BSUB -G asccasc
#BSUB -nnodes 32
#BSUB -q pbatch
#BSUB -W 360

clean_cache()
{
    printf "\nCleaning cache...\n"
    cache_num_nodes=$1
    cache_tasks_per_node=$2
    ior_executable=$3
    cache_aggregated_file_size=262144 # 256g
    cache_number_of_segments=$((cache_aggregated_file_size/(cache_tasks_per_node*cache_num_nodes)))
    lrun -N $cache_num_nodes -T $cache_tasks_per_node $ior_executable -a MPIIO -k -r -o /p/gpfs1/chowdhur/huge_file_for_cache_cleaning/256g_file -s $cache_number_of_segments -b 1M -t 1M
}

min_nodes=1
max_nodes=32
min_tasks_per_node=8
max_tasks_per_node=8
max_iteration=25
gpfs_dir=/p/gpfs1/chowdhur/io_playground
bb_dir=$BBPATH
tmpfs_dir=/dev/shm
ramdisk_dir=/tmp
vast_dir=/p/vast1/chowdhur

export LD_LIBRARY_PATH=/opt/ibm/bb/lib:$LD_LIBRARY_PATH

ior_executable=/g/g91/chowdhur/codes/ior_original/install/bin/ior
number_of_segments=5120 # 5 GiB per process
is_fixed_aggregated_file_size=0
aggregated_file_size=262144 # 270336 # 264 GiB # 262144
aggregated_file_size_unit=M # MiB
block_size=1
block_size_unit=M # MiB
transfer_size=1
transfer_size_unit=M # MiB
test_file_name=ior_test_file

# for storage_dir in $gpfs_dir $tmpfs_dir
for storage_dir in $gpfs_dir $bb_dir $tmpfs_dir $vast_dir
do
    printf "\nStorage dir: "
    echo $storage_dir
    printf "\n"
    num_nodes=$min_nodes
    while [ $num_nodes -le $max_nodes ]
    do
        tasks_per_node=$min_tasks_per_node
        while [ $tasks_per_node -le $max_tasks_per_node ]
        do
            printf "\nNumber of clients: "
            echo $num_nodes
            printf "\nTasks per node: "
            echo $tasks_per_node
            printf "\n"

            iteration=1
            while [ $iteration -le $max_iteration ]
            do
                if [ $is_fixed_aggregated_file_size -eq 1 ]
                then
                    number_of_segments=$((aggregated_file_size/(block_size*tasks_per_node*num_nodes)))
                fi
                printf "\nIteration: "
                echo $iteration
                printf "\n"
                # with darshan
                # lrun -N$num_nodes -T$tasks_per_node --env "LD_PRELOAD=/g/g91/chowdhur/codes/darshan/build/lib/libdarshan.so" $ior_executable -a MPIIO -w -r -o $storage_dir/$test_file_name -F -C -s $number_of_segments -b $block_size$block_size_unit -t $transfer_size$transfer_size_unit
                lrun -N$num_nodes -T$tasks_per_node $ior_executable -a MPIIO -w -r -e -o $storage_dir/$test_file_name -F -C -s $number_of_segments -b $block_size$block_size_unit -t $transfer_size$transfer_size_unit
                clean_cache $num_nodes $tasks_per_node $ior_executable
                iteration=$((iteration+1))
            done
            tasks_per_node=$((tasks_per_node*2))
        done
        num_nodes=$((num_nodes*2))
    done
done

# write a 128g file
# lrun -N 1 -T 8 /g/g91/chowdhur/codes/ior_original/install/bin/ior -a MPIIO -k -w -o /p/gpfs1/chowdhur/huge_file_for_cache_cleaning/128g_file -s 16384 -b 1M -t 1M
