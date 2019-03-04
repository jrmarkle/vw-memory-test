# Note: use https://www.fromlatest.io/ for linting Dockerfiles

FROM ubuntu:16.04 AS vw

ENV DEBIAN_FRONTEND noninteractive

RUN apt update
RUN apt install -y -qq \
	apt-transport-https \
	apt-utils \
	ca-certificates \
	curl \
	git \
	software-properties-common \
	build-essential \
	libboost-program-options-dev \
	zlib1g-dev \
	netcat \
	python \
	default-jdk \
	libtool \
	m4 \
	autoconf \
	automake 

RUN git clone --depth 1 --branch 8.3.2-s-1 git://github.com/jrmarkle/vowpal_wabbit.git /opt/vowpal_wabbit/

WORKDIR /opt/vowpal_wabbit

RUN libtoolize -f -c
RUN aclocal -I ./acinclude.d -I /usr/share/aclocal
RUN autoheader
RUN touch README
RUN automake -ac -Woverride
RUN autoconf
RUN ./configure --with-boost-libdir=/usr/lib/x86_64-linux-gnu CXX=g++

RUN make -j6
RUN make test
RUN make install
RUN ldconfig

RUN apt install -y -qq \
	valgrind

WORKDIR /root

COPY main.c /root/main.c

RUN gcc \
	main.c -o test \
	-Wall -Werror \
	-lvw_c_wrapper

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./test"]
