#!/bin/bash

# install semi-honest 2-party circuits
git clone https://github.com/emp-toolkit/emp-sh2pc.git


# add our files to compile scripts
cp source/* emp-sh2pc/test/
cd emp-sh2pc
mv test/geninput.py .
echo "add_test (mult3)" >> CMakeLists.txt
echo "add_test (innerprod)" >> CMakeLists.txt
echo "add_test (alob)" >> CMakeLists.txt
echo "add_test (editdist)" >> CMakeLists.txt
echo "add_test (editdist-socket)" >> CMakeLists.txt
echo "add_test (xtabs)" >> CMakeLists.txt

# build project
mkdir build
cd build
cmake ..
make
make install