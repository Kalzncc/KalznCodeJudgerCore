#!/bin/bash

cd ..

cd ../judger
make
if [ $? -ne 0 ]; then
    echo "failed"
else
    cd ../sh
    sh run.sh
fi
