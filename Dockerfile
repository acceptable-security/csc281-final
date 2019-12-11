FROM ubuntu:16.04
WORKDIR /root
RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  git \
  libgmp-dev \
  libssl-dev \
  sudo \
  software-properties-common \
  nano
ADD source/ /root/source
ADD README.md .
ADD install.sh .
ADD install_sock.sh .
RUN ["bash", "install.sh"]
EXPOSE 8123
RUN ["bash", "install_sock.sh"]
CMD ["/bin/bash"]

