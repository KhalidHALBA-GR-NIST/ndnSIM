/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Klaus Schneider, University of Bamberg, Germany
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Klaus Schneider <klaus.schneider@uni-bamberg.de>
 */
#include <ns3/error-model.h>
#include <ns3/node.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <string>
#include <ns3/names.h>
#include <ns3/string.h>
#include "../src/ndnSIM/helper/ndn-app-helper.hpp"
#include "../src/ndnSIM/helper/ndn-fib-helper.hpp"
#include "../src/ndnSIM/helper/ndn-stack-helper.hpp"
#include "../src/ndnSIM/helper/ndn-strategy-choice-helper.hpp"
#include "../src/ndnSIM/utils/topology/annotated-topology-reader.hpp"
#include "../src/ndnSIM/utils/tracers/l2-rate-tracer.hpp"
#include "../src/ndnSIM/utils/tracers/ndn-app-delay-tracer.hpp"
#include "../src/ndnSIM/utils/tracers/ndn-l3-rate-tracer.hpp"
#include <ns3/event-id.h>
#include <ns3/net-device.h>
#include <ns3/channel.h>

namespace ns3 {

// Set the link error rate of a given channel
void setLinkError(Ptr<NetDevice> nd, double errorRate)
{
  Ptr < RateErrorModel > error = CreateObject<RateErrorModel>();
  error->SetUnit(RateErrorModel::ERROR_UNIT_PACKET);
  error->SetRate(errorRate);
  nd->SetAttribute("ReceiveErrorModel", PointerValue(error));
}

// Set the one-way delay of a given channel.
void setDelay(Ptr<NetDevice> nd, std::string timeString)
{
  Ptr < Channel > chan = nd->GetChannel();
  chan->SetAttribute("Delay", TimeValue(Time(timeString)));
}

int main(int argc, char* argv[])
{
  // Set up trace results
  std::string folder = "./";
  std::string resultsFile = folder + "rates.txt";
  std::string dropFile = folder + "drop.txt";
  std::string delayFile = folder + "delay.txt";

  // Read Topology
  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-multipath.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  Ptr < Node > client = Names::Find < Node > ("client");
  Ptr < Node > router1 = Names::Find < Node > ("router1");
  Ptr < Node > router2 = Names::Find < Node > ("router2");
  Ptr < Node > backbone = Names::Find < Node > ("backbone");
  Ptr < Node > server = Names::Find < Node > ("server");

  // Set link cost
  ndn::FibHelper::AddRoute("client", "/", "router1", 150);
  ndn::FibHelper::AddRoute("client", "/", "router2", 100);
  ndn::FibHelper::AddRoute("client", "/", "router3", 50);
  ndn::FibHelper::AddRoute("router1", "/", "backbone", 100);
  ndn::FibHelper::AddRoute("router2", "/", "backbone", 100);
  ndn::FibHelper::AddRoute("router3", "/", "backbone", 100);
  ndn::FibHelper::AddRoute("backbone", "/", "server", 100);

  // Choosing a forwarding strategy
  std::string strategy = "lowest-cost";
  std::string params = "maxdelay=100";
  ndn::StrategyChoiceHelper::Install(client, "/prefix",
      "/localhost/nfd/strategy/" + strategy + "/%FD%01/" + params);

  // Installing consumer application
  ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
  consumerHelper1.SetPrefix("/prefix/A");
  consumerHelper1.SetAttribute("Frequency", StringValue("300"));
  consumerHelper1.SetAttribute("StopTime", StringValue("20"));
  consumerHelper1.Install(client);

  // Installing producer application
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix("/prefix");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(server);

  // Installing tracers
  L2RateTracer::InstallAll(dropFile, Seconds(0.1));
  ndn::L3RateTracer::InstallAll(resultsFile, Seconds(0.1));
  ndn::AppDelayTracer::InstallAll(delayFile);

  // Setting link quality
  setLinkError(client->GetDevice(0), 0.2);
  setLinkError(client->GetDevice(1), 0.1);
  setLinkError(client->GetDevice(2), 0);

  // Increase delay of link 0 after 10 seconds.
  Simulator::Schedule(Seconds(10.0), setDelay, client->GetDevice(0), "200ms");

  Simulator::Stop(Seconds(23.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

}  // namespace ns3

int main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
