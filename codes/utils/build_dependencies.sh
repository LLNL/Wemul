## Following commands are required for running Wemul with stable Flux.
# module_file=/usr/global/tools/flux/blueos_3_ppc64le_ib/modulefiles
# shim_module=pmi-shim
# mpi_module=spectrum-mpi/2019.06.24-flux
# export FLUX_FLAGS=-Wl,-rpath=/usr/global/tools/flux/blueos_3_ppc64le_ib/spectrum-mpi-2019.06.24/lib

# module use $module_file
# module load $shim_module flux
# module load $mpi_module

export WEMUL_HOME=/g/g91/chowdhur/codes/hawk_gitlab/hpc_dataflow_manager/workflow_emulator/codes
pushd $WEMUL_HOME

mkdir -p libs
cd libs

echo "Building Wemul dependencies..."
## Download and build KVTree
echo "Downloading KVTree..."
git clone git@github.com:llnl/kvtree.git
cd kvtree
mkdir build
mkdir install
cd build
echo "Building KVTree..."
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make
make install
make test
cd ../.. # currently on libs dir
echo "KVTree build finished [1/3]"

## Download and build AXL
# export KVTREE_LIBRARIES=$WEMUL_HOME/libs/kvtree/install/lib64
# export KVTREE_INCLUDE_DIRS=$WEMUL_HOME/libs/kvtree/install/include
echo "Downloading AXL..."
git clone https://github.com/ECP-VeloC/AXL.git
cd AXL
mkdir build
mkdir install
cd build
echo "Building AXL..."
cmake -DCMAKE_INSTALL_PREFIX=../install -DAXL_ASYNC_API=IBM_BBAPI -DWITH_BBAPI_PREFIX=/opt/ibm/bb .. -DWITH_KVTREE_PREFIX=$WEMUL_HOME/libs/kvtree/install
make
make install
make test
cd ../.. # currently on libs dir
echo "AXL build finished [2/3]"

echo "Preparing build environment..."
cd $WEMUL_HOME
export KVTREE_DIR=$WEMUL_HOME/libs/kvtree/install
export AXL_DIR=$WEMUL_HOME/libs/AXL/install
echo "KVTree installed in: "$KVTREE_DIR
echo "AXL installed in: "$AXL_DIR
export LD_LIBRARY_PATH=/opt/ibm/bb/lib:$AXL_DIR/lib64:$KVTREE_DIR/lib64:$LD_LIBRARY_PATH
echo "Build environment preparation done! [3/3]"

popd

