# PCON Source Code

## Installation Procedure

The installation procedure is straight forward and similar to the one of [ndnSIM](http://ndnsim.net/2.1/getting-started.html):

```bash
  # Download sources from github
  mkdir ndnSIM
  cd ndnSIM
  git clone https://github.com/schneiderklaus/ns-3-dev.git ns-3
  git clone -b pcon-0.1.0 --recursive https://github.com/schneiderklaus/ndnSIM.git ns-3/src/ndnSIM

  # Compile and install 
  cd <ns-3-folder>
  make configure 
  make

```

## Running Experiments

The repository contains the scenarios from section 4.1 to section 4.4 of the paper [A Practical Congestion Control Scheme for Named Data Networking](http://conferences2.sigcomm.org/acm-icn/2016/slides/Session1/schneider.pdf). You can run them as follows:

```bash
  # Switch to Ns3 folder
  cd <ns-3-folder>

  #Run any of the following:
  ./waf --run pcon_scen1
  ./waf --run pcon_scen2
  ./waf --run pcon_scen3
  ./waf --run pcon_scen4.1
  ./waf --run pcon_scen4.2

  #The tracing results will be writting into the folder /results

```

