A Custom NFD Strategy Layer 
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

	# Compile NS-3 with the ndnSIM module
	cd <ns-3-folder>
	git checkout ns-3.21-ndnSIM-2.0
	./waf configure --disable-python --enable-examples -d debug
	./waf
```

## Example Scenario

An example scenario is included in "examples/multipath-example.cc" together with its topology "examples/topologies/topo-multipath.txt"

```bash
	# Link example scenario to scratch folder
	cd <ns-3-folder>
	ln -s ../src/ndnSIM/examples/multipath-example.cc scratch/multipath-example.cc

	# Optinal: Enable logging
	export NS_LOG=nfd.LowestCostStrategy
	
	# Run 
	./waf --run multipath-example

```
