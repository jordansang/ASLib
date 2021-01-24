#!/bin/bash

echo "Build ASLib 0.0.1"

OUTDIR=/opt/as/ASLib

rm -rf $OUTDIR
mkdir $OUTDIR

make clean

make distclean

../../mnt/hgfs/WorkSpace/as/configure --prefix=$OUTDIR --enable-xml2=/usr/local --disable-allwarning

make

make install

cp -r ASLib /mnt/hgfs/WorkSpace/

echo "Build Finish"
