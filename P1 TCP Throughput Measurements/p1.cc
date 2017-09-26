/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("P1");

int 
main (int argc, char *argv[])
{
  SeedManager::SetSeed(1);

  uint32_t nSpokes = 8;
  double endTime = 60.0;
  std::string protocol = "TcpHybla";

  //Process the command line

  CommandLine cmd;
  cmd.AddValue("nSpokes", "Number of spokes to place in each star", nSpokes);
  cmd.AddValue("Protocol", "TCP Protocol to use", protocol);
  cmd.Parse(argc, argv);

  if(protocol == "TcpNewReno") {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpNewReno::GetTypeId()));
  }else if(protocol == "TcpHybla") {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHybla::GetTypeId()));
  }else if(protocol == "TcpWestwood"){ 
  // the default protocol type in ns3::TcpWestwood is WESTWOOD
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
    Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
  }else if("TcpWestwoodPlus"){
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
    Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
  }else{
    NS_LOG_DEBUG ("Invalid TCP version");
    exit (1);
  }

  //Use the given p2p attributes for each star
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));
  
  //Use the pointToPoint helper to create point to point stars
  PointToPointStarHelper star1(nSpokes, pointToPoint);
  PointToPointStarHelper star2(nSpokes, pointToPoint);

  InternetStackHelper internet;
  star1.InstallStack(internet);
  star2.InstallStack(internet);

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  star1.AssignIpv4Addresses(address);
  address.SetBase("10.2.1.0", "255.255.255.0");
  star2.AssignIpv4Addresses(address);
  address.SetBase("10.3.1.0", "255.255.255.0");

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("20ms"));

  NetDeviceContainer devices;
  devices = p2p.Install(star1.GetHub(), star2.GetHub());
  Ipv4InterfaceContainer interfaces;
  interfaces = address.Assign(devices);
  
  Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), 5000));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApps;
  for(uint32_t i = 0; i < nSpokes; ++i) {
    sinkApps.Add(sinkHelper.Install(star1.GetSpokeNode(i)));
  }

  sinkApps.Start(Seconds(1.0));

  BulkSendHelper bulkSender ("ns3::TcpSocketFactory", Address());
  ApplicationContainer sourceApps;
  for(uint32_t i = 0; i < nSpokes; ++i) 
  {
    AddressValue remoteAddress(InetSocketAddress(star1.GetSpokeIpv4Address(i), 5000));
    bulkSender.SetAttribute("Remote", remoteAddress);
    sourceApps.Add(bulkSender.Install(star2.GetSpokeNode(i)));
  }

  sourceApps.Start(Seconds(2.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  Simulator::Stop(Seconds(endTime));

  Simulator::Run ();

  uint64_t totalRx = 0;

  for(uint32_t i = 0; i < nSpokes; ++i) {

    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(i));
    uint32_t bytesReceived = sink->GetTotalRx();
    totalRx += bytesReceived;
    std::cout << "Sink " << i << "\tTotalRx: " << bytesReceived * 1e-6 * 8 << "Mb";
    std::cout << "\tGoodput: " << (bytesReceived * 1e-6 * 8) / endTime << "Mbps" << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Totals\tTotalRx: " << totalRx * 1e-6 * 8 << "Mb";
  std::cout << "\tGoodput: " << (totalRx * 1e-6 * 8) / endTime << "Mbps" << std::endl;

  Simulator::Destroy ();
}
