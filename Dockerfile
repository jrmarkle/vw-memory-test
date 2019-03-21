# Note: use https://www.fromlatest.io/ for linting Dockerfiles

FROM ubuntu:18.04 AS vw

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update
RUN apt-get install -y \
	apt-transport-https \
	apt-utils \
	ca-certificates \
	curl \
	git \
	software-properties-common \
	build-essential \
	libboost-program-options-dev \
	libboost-system-dev \
	libboost-test-dev \
	libboost-thread-dev \
	zlib1g-dev \
	netcat \
	python \
	default-jdk \
	libtool \
	m4 \
	autoconf \
	automake \
	valgrind \
	pkg-config \
	cmake \
	help2man

RUN git clone git://github.com/jrmarkle/vowpal_wabbit.git /opt/vowpal_wabbit/
WORKDIR /opt/vowpal_wabbit

RUN git -c advice.detachedHead=false checkout 0a9122a4ab7effe1fe4bcf04da6192ee4e63bc45

RUN make
RUN make install
RUN ldconfig

WORKDIR /root

COPY main.c /root/main.c

RUN cat /usr/local/lib/pkgconfig/libvw_c_wrapper.pc
RUN cat /usr/local/lib/pkgconfig/libvw.pc
RUN pkg-config --libs libvw_c_wrapper
RUN gcc -c main.c $(pkg-config --cflags libvw_c_wrapper)
RUN g++ main.o -o test $(pkg-config --libs libvw_c_wrapper)

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./test"]
