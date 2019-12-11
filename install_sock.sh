#!/bin/bash

set -e

wget http://tar.gwsocket.io/gwsocket-0.3.tar.gz
tar -xzvf gwsocket-0.3.tar.gz
cd gwsocket-0.3/
./configure
make
make install
