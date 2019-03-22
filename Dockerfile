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

RUN git clone git://github.com/VowpalWabbit/vowpal_wabbit.git /opt/vowpal_wabbit/
WORKDIR /opt/vowpal_wabbit

RUN git -c advice.detachedHead=false checkout e833199eb89a752524159a4a1f7145d4357e63e8

RUN libtoolize -f -c
RUN aclocal -I ./acinclude.d -I /usr/share/aclocal
RUN autoheader
RUN touch README
RUN automake -ac -Woverride
RUN autoconf
RUN ./configure --with-boost-libdir=/usr/lib/x86_64-linux-gnu CXX=g++

RUN make -j4

COPY revert-e833199e.patch .
RUN patch -p1 < revert-e833199e.patch

RUN make -j4
RUN make install
RUN ldconfig

WORKDIR /root

COPY main.c /root/main.c

RUN gcc -c main.c $(pkg-config --cflags libvw_c_wrapper)
RUN g++ main.o -o test $(pkg-config --libs libvw_c_wrapper)

CMD ["./test"]
