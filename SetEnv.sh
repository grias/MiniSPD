#!/bin/bash

export SIMPATH=/home/gribowski/soft/FairSoft/install
export FAIRROOTPATH=/home/gribowski/soft/FairRoot/install

export PATH="${SIMPATH}/bin:$PATH"
export LD_LIBRARY_PATH="${SIMPATH}/lib:$LD_LIBRARY_PATH"

if [ -f ../build/config.sh ]
then
source ../build/config.sh -a
fi

export GEOMPATH="${VMCWORKDIR}/geometry"
export PATH="${VMCWORKDIR}:$PATH"


