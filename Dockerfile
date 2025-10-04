FROM ubuntu:20.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3 \
    git \
    build-essential \
    libopenblas-dev \
    libprotobuf-dev \
    protobuf-compiler \
    cmake \
    libopencv-dev \
    gfortran \
    python3-pip \
    lsof

RUN pip install numpy==1.23.5

# RUN echo '1'

# RUN git clone --recursive --depth 1 --shallow-submodules https://github.com/Kaiser-Yang/mxnet

COPY ./tmp /mxnet

RUN cd /mxnet && \
    cmake -S . -B build -DUSE_CUDA=OFF \
    -DUSE_ONEDNN=OFF \
    -DUSE_DIST_KVSTORE=ON \
    -DUSE_OPENMP=OFF \
    -DUSE_OPERATOR_TUNING=OFF \
    -DUSE_SSE=OFF && \
    cd build && \
    cmake --build . -j 8

RUN cd mxnet/python && pip install -e .

CMD ["/bin/bash"]
