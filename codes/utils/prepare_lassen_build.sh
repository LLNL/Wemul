export KVTREE_DIR=/g/g91/chowdhur/codes/KVTree/install
export AXL_DIR=/g/g91/chowdhur/codes/AXL/install

module_file=/usr/global/tools/flux/blueos_3_ppc64le_ib/modulefiles
shim_module=pmi-shim
mpi_module=spectrum-mpi/2019.06.24-flux
export FLUX_FLAGS=-Wl,-rpath=/usr/global/tools/flux/blueos_3_ppc64le_ib/spectrum-mpi-2019.06.24/lib

module use $module_file
module load $shim_module flux
module load $mpi_module

