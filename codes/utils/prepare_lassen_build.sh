export DEP_LIBS_DIR=/g/g91/chowdhur/codes/Wemul/codes/libs
export KVTREE_DIR=${DEP_LIBS_DIR}/kvtree/install
export AXL_DIR=${DEP_LIBS_DIR}/AXL/install
export JSONCPP_DIR=${DEP_LIBS_DIR}/jsoncpp

# module_file=/usr/global/tools/flux/blueos_3_ppc64le_ib/modulefiles
# shim_module=pmi-shim
# mpi_module=spectrum-mpi/2019.06.24-flux
# export FLUX_FLAGS=-Wl,-rpath=/usr/global/tools/flux/blueos_3_ppc64le_ib/spectrum-mpi-2019.06.24/lib

# module use $module_file
# module load $shim_module flux
# module load $mpi_module

export LD_LIBRARY_PATH=/opt/ibm/bb/lib:${AXL_DIR}/lib64:${KVTREE_DIR}/lib64:${JSONCPP_DIR}/build/lib:$LD_LIBRARY_PATH
