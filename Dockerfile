# Note: use https://www.fromlatest.io/ for linting Dockerfiles

FROM ubuntu:16.04 AS vw

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y \
	cmake \
	git \
	build-essential \
	libboost-program-options-dev \
	libboost-system-dev \
	libboost-test-dev \
	libboost-thread-dev \
	zlib1g-dev \
	valgrind \
	zlib1g-dev

WORKDIR /opt/vowpal_wabbit
RUN git clone git://github.com/VowpalWabbit/vowpal_wabbit.git . && \
	git -c advice.detachedHead=false checkout 170bba585b012db74e7e6949692141bade933c9e
WORKDIR /opt/vowpal_wabbit/build
RUN cmake .. -DSTATIC_LINK_VW=On -DBUILD_TESTS=Off -DCMAKE_BUILD_TYPE=Release
RUN make -j$(cat nprocs.txt) install
RUN ldconfig

WORKDIR /root

COPY main.c .

RUN gcc -c main.c -I/usr/local/include/vowpalwabbit
RUN g++ main.o -o test -lvw_c_wrapper -lvw -lallreduce -pthread -lboost_program_options -lz

CMD ["valgrind", "./test"]
