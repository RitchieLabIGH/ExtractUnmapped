FROM ubuntu:bionic
MAINTAINER claudio.lorenzi

LABEL version=v0.1

COPY ./filter /EUnmapped/filter
COPY ./scripts /EUnmapped/scripts
COPY ./external /EUnmapped/external

ENV PATH="/EUnmapped/external:/EUnmapped/external/bin:/EUnmapped/scripts:$PATH"
ENV LD_LIBRARY_PATH="/usr/local/lib/:$LD_LIBRARY_PATH"

RUN apt-get clean all && \
    apt-get update && \
    apt-get -y upgrade && \
    export DEBIAN_FRONTEND=noninteractive && \ 
    apt-get install -qy make build-essential gcc g++ \
    	 libopenmpi-dev zlib1g-dev  \
    	libarmadillo-dev libboost-test-dev \
    	libboost-dev libboost-system-dev libboost-math-dev \
    	libboost-program-options-dev libboost-serialization-dev \
    	bedtools ncbi-entrez-direct  \
    	gzip gawk libz-dev wget libseqan2-dev cmake git perl locales && \
    rm -rf /var/lib/apt/lists/* /var/log/dpkg.log && \
    apt-get clean && \
    apt-get purge && \
    apt-get autoclean	
    
### mlpack v3.0.4
### wget http://www.mlpack.org/files/mlpack-3.0.4.tar.gz

RUN cd /EUnmapped/external/ && \
 	tar -xvzpf mlpack-3.0.4.tar.gz && \
 	mkdir -p ./mlpack-3.0.4/build && \
 	cd ./mlpack-3.0.4/build/ && \
 	cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_CLI_EXECUTABLES=OFF -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_TESTS=OFF -DUSE_OPENMP=ON  ../ && \
 	make -j4 && \
 	make install && \
 	cd /EUnmapped/external/ && \
 	rm -fr ./mlpack-3.0.4 ./mlpack-3.0.4.tar.gz
 		
### KMC
## wget https://github.com/refresh-bio/KMC/releases/download/v3.0.0/KMC3.linux.tar.gz 

RUN	cd /EUnmapped/external/ && \
 	tar -xvzpf KMC3.linux.tar.gz && \
 	rm ./KMC3.linux.tar.gz 
 	
RUN	cd /EUnmapped/filter/Release && \
	make clean && \
	make -j4 all && \
	mv ./ExtractUnampped /EUnmapped/scripts/
	
ENTRYPOINT ["ExtractUnampped"]