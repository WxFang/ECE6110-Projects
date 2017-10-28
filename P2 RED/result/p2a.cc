#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

using i32 = int32_t;
using u32 = uint32_t;

std::stringstream filePlotQueue;
std::stringstream filePlotQueueAvg;

uint32_t checkTimes;
double avgQueueSize;

constexpr uint32_t packetSize = 1000 - 42;

NodeContainer n0n4;
NodeContainer n1n4;
NodeContainer n2n4;
NodeContainer n3n4;
NodeContainer n4n5;

Ipv4InterfaceContainer i0i4;
Ipv4InterfaceContainer i1i4;
Ipv4InterfaceContainer i2i4;
Ipv4InterfaceContainer i3i4;
Ipv4InterfaceContainer i4i5;

double global_start_time;
double global_stop_time;
double sink_start_time;
double sink_stop_time;
double client_start_time;
double client_stop_time;

void EnqueueAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

	if(tcp.GetDestinationPort () == 60001) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60002) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60003) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60004) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;

}

void DequeueAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

	if(tcp.GetDestinationPort () == 60001) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60002) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60003) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60004) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;

}

void DroppedAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

	if(tcp.GetDestinationPort () == 60001) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60002) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60003) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 60004) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
}

void CheckQueueSize(Ptr<QueueDisc> queue) {
	uint32_t qsize = StaticCast<RedQueueDisc>(queue)->GetQueueSize();
	avgQueueSize += qsize;
	checkTimes++;

	Simulator::Schedule(Seconds(0.01), &CheckQueueSize, queue);

	std::ofstream fPlotQueue(filePlotQueue.str().c_str(), std::ios::out | std::ios::app);
	fPlotQueue << Simulator::Now().GetSeconds() << " " << qsize << std::endl;
	fPlotQueue.close();

	std::ofstream fPlotQueueAvg(filePlotQueueAvg.str().c_str(), std::ios::out | std::ios::app);
	fPlotQueueAvg << Simulator::Now().GetSeconds() << " " << avgQueueSize / checkTimes << std::endl;
	fPlotQueueAvg.close();
}


int main(int argc, char* argv[]) {

	std::string pathOut = ".";
	bool writeForPlot = true;
	bool createVis = false;
	bool useFlowMon = true;

	uint32_t runNumber = 0;
	uint32_t maxPackets = 100;

	std::string bottleneckLinkBW = "45Mbps";
	std::string bottleneckLinkDelay = "2ms";

	std::string animationFile = "demo.xml";

	double minTh = 5;
	double maxTh = 15;

	global_start_time = 0.0;
	global_stop_time = 5.0;
	sink_start_time = 0.0;
	sink_stop_time = global_stop_time + 3.0;
	client_start_time = sink_start_time;
	client_stop_time = global_stop_time - 2.0;

	CommandLine cmd;
	cmd.AddValue("runNumber", "run number for random variable generation", runNumber);
	cmd.AddValue("animationFile", "File name for animation output", animationFile);
	cmd.AddValue("createVis", "<0/1> to create animation output", createVis);
	cmd.AddValue("writeForPlot", "<0/1> to write results for queue plot", writeForPlot);
	cmd.AddValue("useFlowMon", "<0/1> to use the flowmonitor", useFlowMon);
	cmd.AddValue("maxPackets", "Max packets allowed in the device queue", maxPackets);

	cmd.Parse(argc, argv);

	Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
	Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(packetSize));
	Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(1));

	Config::SetDefault("ns3::RedQueueDisc::Mode", StringValue("QUEUE_MODE_PACKETS"));
	Config::SetDefault("ns3::RedQueueDisc::MeanPktSize", UintegerValue(packetSize));
    Config::SetDefault ("ns3::RedQueueDisc::Wait", BooleanValue (true));
    Config::SetDefault ("ns3::RedQueueDisc::Gentle", BooleanValue (true));
  	Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (minTh));
  	Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (maxTh));
  	Config::SetDefault ("ns3::RedQueueDisc::QW", DoubleValue (0.002));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkBandwidth", StringValue (bottleneckLinkBW));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkDelay", StringValue (bottleneckLinkDelay));
  	Config::SetDefault ("ns3::RedQueueDisc::QueueLimit", UintegerValue (maxPackets));
	Config::SetDefault ("ns3::RedQueueDisc::MeanPktSize", UintegerValue (packetSize));

	SeedManager::SetSeed(2);
	SeedManager::SetRun(runNumber);

	NodeContainer c;
	c.Create (6);
	Names::Add ( "N0", c.Get (0));
	Names::Add ( "N1", c.Get (1));
	Names::Add ( "N2", c.Get (2));
	Names::Add ( "N3", c.Get (3));
	Names::Add ( "N4", c.Get (4));
	Names::Add ( "N5", c.Get (5));
	n0n4 = NodeContainer (c.Get (0), c.Get (4));
	n1n4 = NodeContainer (c.Get (1), c.Get (4));
	n2n4 = NodeContainer (c.Get (2), c.Get (4));
	n3n4 = NodeContainer (c.Get (3), c.Get (4));
	n4n5 = NodeContainer (c.Get (4), c.Get (5));

	InternetStackHelper stack;
  	stack.Install (c);

	TrafficControlHelper tchPfifo;
	uint16_t handle = tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
	tchPfifo.AddInternalQueues (handle, 3, "ns3::DropTailQueue", "MaxPackets", UintegerValue (1000));

	TrafficControlHelper tchRed;
	tchRed.SetRootQueueDisc ("ns3::RedQueueDisc", "LinkBandwidth", StringValue (bottleneckLinkBW),
                           "LinkDelay", StringValue (bottleneckLinkDelay));

	PointToPointHelper p2p;

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
	NetDeviceContainer devn0n4 = p2p.Install (n0n4);
	tchPfifo.Install (devn0n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("4ms"));
	NetDeviceContainer devn1n4 = p2p.Install (n1n4);
	tchPfifo.Install (devn1n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("8ms"));
	NetDeviceContainer devn2n4 = p2p.Install (n2n4);
	tchPfifo.Install (devn2n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("5ms"));
	NetDeviceContainer devn3n4 = p2p.Install (n3n4);
	tchPfifo.Install (devn3n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue (bottleneckLinkBW));
	p2p.SetChannelAttribute ("Delay", StringValue (bottleneckLinkDelay));
	NetDeviceContainer devn4n5 = p2p.Install (n4n5);
	QueueDiscContainer queueDiscs = tchRed.Install (devn4n5);

	Ptr<QueueDisc> redQueue = queueDiscs.Get(0);

	redQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&EnqueueAtRed));
	redQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&DequeueAtRed));
	redQueue->TraceConnectWithoutContext("Drop", MakeCallback(&DroppedAtRed));

	Ipv4AddressHelper ipv4;

	ipv4.SetBase ("10.1.1.0", "255.255.255.0");
	i0i4 = ipv4.Assign (devn0n4);

	ipv4.SetBase ("10.1.2.0", "255.255.255.0");
	i1i4 = ipv4.Assign (devn1n4);

	ipv4.SetBase ("10.1.3.0", "255.255.255.0");
	i2i4 = ipv4.Assign (devn2n4);

	ipv4.SetBase ("10.1.4.0", "255.255.255.0");
	i3i4 = ipv4.Assign (devn3n4);

	ipv4.SetBase ("10.1.5.0", "255.255.255.0");
	i4i5 = ipv4.Assign (devn4n5);

	// SINKs

	// #1
	uint16_t port1 = 60001;
	Address sinkLocalAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port1));
	PacketSinkHelper sinkHelper1 ("ns3::TcpSocketFactory", sinkLocalAddress1);
	ApplicationContainer sinkApp1 = sinkHelper1.Install (n4n5.Get (1));
	sinkApp1.Start (Seconds (sink_start_time));
	sinkApp1.Stop (Seconds (sink_stop_time));

	// #2
	uint16_t port2 = 60002;
	Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), port2));
	PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", sinkLocalAddress2);
	ApplicationContainer sinkApp2 = sinkHelper2.Install (n4n5.Get (1));
	sinkApp2.Start (Seconds (sink_start_time));
	sinkApp2.Stop (Seconds (sink_stop_time));
	// #3
	uint16_t port3 = 60003;
	Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), port3));
	PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", sinkLocalAddress3);
	ApplicationContainer sinkApp3 = sinkHelper3.Install (n4n5.Get (1));
	sinkApp3.Start (Seconds (sink_start_time));
	sinkApp3.Stop (Seconds (sink_stop_time));
	// #4
	uint16_t port4 = 60004;
	Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), port4));
	PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory", sinkLocalAddress4);
	ApplicationContainer sinkApp4 = sinkHelper4.Install (n4n5.Get (1));
	sinkApp4.Start (Seconds (sink_start_time));
	sinkApp4.Stop (Seconds (sink_stop_time));

	//Configure Sources

	OnOffHelper clientHelper1 ("ns3::TcpSocketFactory", Address ());
	clientHelper1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper1.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper1.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps1;
	AddressValue remoteAddress1 (InetSocketAddress (i4i5.GetAddress (1), port1));
	clientHelper1.SetAttribute ("Remote", remoteAddress1);
	clientApps1.Add (clientHelper1.Install (n0n4.Get (0)));
	clientApps1.Start (Seconds (client_start_time));
	clientApps1.Stop (Seconds (client_stop_time));

	// Connection #2
	OnOffHelper clientHelper2 ("ns3::TcpSocketFactory", Address ());
	clientHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper2.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper2.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps2;
	AddressValue remoteAddress2 (InetSocketAddress (i4i5.GetAddress (1), port2));
	clientHelper2.SetAttribute ("Remote", remoteAddress2);
	clientApps2.Add (clientHelper2.Install (n1n4.Get (0)));
	clientApps2.Start (Seconds (client_start_time + 0.2));
	clientApps2.Stop (Seconds (client_stop_time));

	// Connection #3
	OnOffHelper clientHelper3 ("ns3::TcpSocketFactory", Address ());
	clientHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper3.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper3.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps3;
	AddressValue remoteAddress3 (InetSocketAddress (i4i5.GetAddress (1), port3));
	clientHelper3.SetAttribute ("Remote", remoteAddress3);
	clientApps3.Add (clientHelper3.Install (n2n4.Get (0)));
	clientApps3.Start (Seconds (client_start_time + 0.4));
	clientApps3.Stop (Seconds (client_stop_time));

	// Connection #4
	OnOffHelper clientHelper4 ("ns3::TcpSocketFactory", Address ());
	clientHelper4.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper4.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper4.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper4.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps4;
	AddressValue remoteAddress4 (InetSocketAddress (i4i5.GetAddress (1), port4));
	clientHelper4.SetAttribute ("Remote", remoteAddress4);
	clientApps4.Add (clientHelper4.Install (n3n4.Get (0)));
	clientApps4.Start (Seconds (client_start_time + 0.6));
	clientApps4.Stop (Seconds (client_stop_time));


	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	std::cout << "Here we go" << std::endl;

	AnimationInterface* anim = NULL;
	FlowMonitorHelper flowMonHelper;
	Ptr<FlowMonitor> flowmon;

	if(createVis) {
		anim = new AnimationInterface(animationFile);
		anim->EnablePacketMetadata();
		anim->EnableIpv4L3ProtocolCounters(Seconds(0), Seconds(global_stop_time));
	}

	if(writeForPlot) {
		filePlotQueue << pathOut << "/" << "redQueue.plot";
		filePlotQueueAvg << pathOut << "/" << "redQueueAvg.plot";

		remove(filePlotQueue.str().c_str());
		remove(filePlotQueueAvg.str().c_str());
		Simulator::ScheduleNow(&CheckQueueSize, redQueue);
	}

	if(useFlowMon) {
		flowmon = flowMonHelper.InstallAll();
	}

	Simulator::Stop(Seconds(global_stop_time));
	Simulator::Run();

	uint32_t totalBytes = 0;

	if(useFlowMon) {
		std::stringstream flowOut;
		flowOut << pathOut << "/" << "red.flowmon";
		remove(flowOut.str().c_str());
		flowmon->SerializeToXmlFile(flowOut.str().c_str(), true, true);
	}


		Ptr<Application> app = sinkApp3.Get(0);
		Ptr<PacketSink> pktSink = DynamicCast<PacketSink>(app);
		u32 recieved = pktSink->GetTotalRx();
		std::cout << "\tBytes\t" << recieved << std::endl;
		totalBytes += recieved;

	RedQueueDisc::Stats st = StaticCast<RedQueueDisc> (queueDiscs.Get (0))->GetStats ();
	std::cout << "*** RED stats from Node 4 queue ***" << std::endl;
	std::cout << "\t " << st.unforcedDrop << " drops due prob mark" << std::endl;
	std::cout << "\t " << st.forcedDrop << " drops due hard mark" << std::endl;
	std::cout << "\t " << st.qLimDrop << " drops due queue full" << std::endl;


	std::cout << std::endl << "\tTotal\t\tBytes\t" << totalBytes << std::endl;

	std::cout << "Done" << std::endl;

	if(anim)
		delete anim;

	Simulator::Destroy();

	return 0;
}

