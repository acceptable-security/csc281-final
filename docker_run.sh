#!/bin/bash

docker run -it \
		   -p 443:8443 \
		   -p 49100-50000:49100-50000 \
		   --rm \
		   --ip 172.17.0.2 \
		   emp