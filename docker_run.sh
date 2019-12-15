#!/bin/bash

docker run -it \
		   -p 8000:8000 \
		   -p 49100-50000:49100-50000 \
		   --rm \
		   --ip 172.17.0.2 \
		   emp