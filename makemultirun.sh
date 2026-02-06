#!/bin/bash

touch multirun
rm -rf multirun
mkdir multirun
cwd=`pwd`
cd multirun
for ((i=1; i<=$1; i++)); do
    dirname=wibopcua$i
    echo "creating directory: " ${cwd}/multirun/${dirname}
    mkdir ${dirname}
    cd ${dirname}
    cp -r ${cwd}/wib_opcua/* .
    rm -rf build
    ./quasar.py build
    iport=$((4841+$i))
    sed -i -e "s/4841/${iport}/g" bin/ServerConfig.xml
    cd ..
done
