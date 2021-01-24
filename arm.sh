#!/bin/bash

echo "Build ASLib ARM 0.0.1"

TOOLDIR=/opt/bcm_sdk/iproc/buildroot/host/usr/bin
OUTDIR=/opt/as/ASLib_arm

rm -rf $OUTDIR
mkdir $OUTDIR

make clean

make distclean

export PATH=$PATH:$TOOLDIR

../../mnt/hgfs/WorkSpace/as/configure --prefix=$OUTDIR --host=arm-broadcom-linux-uclibcgnueabi --enable-xml2=/opt/qlibs --disable-allwarning

make

make install

cp -r ASLib_arm /mnt/hgfs/WorkSpace/

echo "Build Finish"
