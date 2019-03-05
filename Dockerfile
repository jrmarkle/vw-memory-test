# Note: use https://www.fromlatest.io/ for linting Dockerfiles

FROM ubuntu:16.04 AS vw

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
	zlib1g-dev \
	netcat \
	python \
	default-jdk \
	libtool \
	m4 \
	autoconf \
	automake \
	valgrind

RUN git clone git://github.com/JohnLangford/vowpal_wabbit.git /opt/vowpal_wabbit/
RUN git config --global user.email nobody
RUN git config --global user.name nobody

WORKDIR /opt/vowpal_wabbit

RUN git checkout 276e0da6f0d21be617a854b0169d5e8ac0832225

# jni.h build fix
ENV JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64

# hash.h build fix
RUN git cherry-pick 77733e0e46b0c37e3ea638b771249b1aff596bd2

RUN libtoolize -f -c
RUN aclocal -I ./acinclude.d -I /usr/share/aclocal
RUN autoheader
RUN touch README
RUN automake -ac -Woverride
RUN autoconf
RUN ./configure --with-boost-libdir=/usr/lib/x86_64-linux-gnu CXX=g++

RUN make -j6
RUN make install
RUN ldconfig

WORKDIR /root

COPY main.c /root/main.c

RUN gcc \
	main.c -o test \
	-Wall -Werror \
	-lvw_c_wrapper

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./test"]
