MKDIR_P=mkdir -p

SOURCE_DIR=src
TEST_DIR=tests
SOURCES=$(SOURCE_DIR)/*.cpp $(TEST_DIR)/*.cpp
INCLUDE_DIR=include
INCLUDES=$(INCLUDE_DIR)/*.hpp
OUTPUT_DIR=build
OUTPUT=$(OUTPUT_DIR)/emulator

# # IOR related information
# IOR_LIB=aiori
# IOR_RELATED_LIB=gpfs
# IOR_SRC_DIR=/g/g91/chowdhur/codes/ior_original/ior/src
# IOR_LIB_DIR=/g/g91/chowdhur/codes/ior_original/install/lib

# KVTree related information
# KVTREE_DIR=/g/g91/chowdhur/codes/KVTree/install
KVTREE_LIB=kvtree
KVTREE_LIB_DIR=${KVTREE_DIR}/lib64
KVTREE_INCLUDE_DIR=${KVTREE_DIR}/include

# AXL related information
# AXL_DIR=/g/g91/chowdhur/codes/AXL/install
AXL_LIB=axl
AXL_LIB_DIR=${AXL_DIR}/lib64
AXL_INCLUDE_DIR=${AXL_DIR}/include

CXX=g++
MPICXX=mpic++
CXXFLAGS=-Wall -ansi -pedantic -std=c++11

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(MKDIR_P) $(OUTPUT_DIR)
	$(MPICXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(AXL_INCLUDE_DIR) -L$(AXL_LIB_DIR) -L$(KVTREE_LIB_DIR) $(SOURCES) -o$(OUTPUT) -l$(AXL_LIB) -l$(KVTREE_LIB)
clean:
	rm -rf $(OUTPUT_DIR)
