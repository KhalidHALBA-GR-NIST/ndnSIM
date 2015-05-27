A custom strategy layer for NFD
==========

This is a custom version of NFD integrated into ndnSIM 2.0 with following additions:

- Ability to use strategy-specific parameters
- Additional forwarding strategies
- Additional interface estimator classes

The source code is currently still in alpha stage and for experimentation purposes only.

## Installation Procedure

The installation procedure is straight-forward and similar to the one of [ndnSIM](http://ndnsim.net/2.0/getting-started.html):

```bash
		# Download sources from github
		mkdir ndnSIM
		cd ndnSIM
		git clone https://github.com/named-data/ndn-cxx.git ndn-cxx
		git clone https://github.com/cawka/ns-3-dev-ndnSIM.git ns-3
		git clone https://github.com/cawka/pybindgen.git pybindgen
		git clone https://github.com/schneiderklaus/ndnSIM ns-3/src/ndnSIM
		
		# Compile and install the ndn-cxx library
		cd ndnSIM/ndn-cxx
		./waf configure
		./waf
		sudo ./waf install

		# Compile NS-3 with ndnSIM module
		cd <ns-3-folder>
		./waf configure --disable-python --enable-examples -d debug
		./waf
```

## Tutorial

Coming soon!
