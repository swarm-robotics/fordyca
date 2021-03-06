#!/bin/bash
#
# Do not try to run this script somewhere other than on MSI, as it probably
# will not work.

export GINIROOT=/home/gini
export SWARMROOT=$GINIROOT/shared/swarm

# Load modules
module load cmake/3.16.2
module load qt/5.9.1
module load boost/1.72.0/gnu-9.2.0
module load gcc/9.2.0
module load llvm/5.0.0
module load python/3.6.3
module load parallel
module unload gcc/6.1.0

# Set compiler vars so that cmake uses the correct version of the
# compiler. You would think that this would not be required...
export CC=gcc
export CXX=g++

# I should NOT have to do this, but the MSI help staff have not been helpful on
# this at all.
if lscpu | grep -q AuthenticAMD; then
    export MSIARCH=amd
    echo "Environment: AMD EPYC"
else
    export MSIARCH=intel
    echo "Environment: Intel Xeon"
fi

# Add argos to our path
gcc_prefix=$(gcc -v 2>&1  |grep prefix | awk -F'=' '{print $2}' | awk '{print $1}')
export PATH=$PATH:$SWARMROOT/bin
export ARGOS_PLUGIN_PATH=$SWARMROOT/$MSIARCH/lib/argos3
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SWARMROOT/$MSIARCH/lib/argos3:$gcc_prefix/lib64

# Always generate core dumps if they happen
ulimit -c unlimited
