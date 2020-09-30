input_file=$1
output_dir=$2
max_subdir_count=$3
max_file_count=$4 # per subdir

subdir_prefix=subdir
filename_prefix=data

mkdir -p $output_dir

subdir_count=0
while [ $subdir_count -lt $max_subdir_count ]
do
    current_subdir=$subdir_prefix.$subdir_count
    mkdir -p $output_dir/$current_subdir
    echo "Creating $current_subdir..."
    file_count=0
    while [ $file_count -lt $max_file_count ]
    do
        cp $input_file $output_dir/$current_subdir/$filename_prefix.$file_count
        file_count=$((file_count+1))
    done
    echo "Created $current_subdir"
    subdir_count=$((subdir_count+1))
done

