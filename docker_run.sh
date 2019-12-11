#!/bin/bash

docker run -it \
		   -p 8000:8000 \
		   -p 7890:7890 \
		   -p 7891:7891 \
		   --rm \
		   --ip 172.17.0.2 \
		   emp