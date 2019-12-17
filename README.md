# CSC 281 Final Project

By Avi Saven and Olivia Morton

## Build

This project uses docker to manage building/runtime. The following commands can be used to get the code and build the necessary image:

```
$ git clone github.com/block8437/csc281-final
$ cd csc281-final
$ docker build -t emp .
```

## Run

Once the image is build, it can be run using `docker_run.sh`. This takes a while as the alob.singles container requires the exposure of many ports, which ends up spawning a lot of docker proxies. Once spawned the following commands can be executed:

```
$ cd web/
$ python3 server.py
```

This will expose on the host's port 80 the ALOB server. 
