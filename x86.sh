#!/bin/bash

echo "Build ASLib 0.0.1"

OUTDIR=/home/jsang/workspace/as/ASLib

rm -rf $OUTDIR
mkdir $OUTDIR

make clean

make distclean

./configure --prefix=$OUTDIR --disable-allwarning

make

make install

# cp -r ASLib /mnt/hgfs/WorkSpace/

echo "Build Finish"
