#!/bin/bash

set -e

gwsocket -p 7890 \
		 --pipein=/tmp/tmpinA.fifo   \
		 --pipeout=/tmp/tmpoutA.fifo \
		 --strict &
gwsocket -p 7891 \
		 --pipein=/tmp/tmpinB.fifo   \
		 --pipeout=/tmp/tmpoutB.fifo \
		 --strict &


sleep 1

cd emp-sh2pc/build/
./bin/$1 /tmp/tmpoutA.fifo /tmp/tmpinA.fifo 12345 &
./bin/$1 /tmp/tmpoutB.fifo /tmp/tmpinB.fifo 12345 &

cd ../../web/
python -m SimpleHTTPServer 8000