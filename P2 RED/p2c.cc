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

// i hate typing _t so I'm making shortcuts
using i32 = int32_t;
using u32 = uint32_t;

std::stringstream filePlotQueue;
std::stringstream filePlotQueueAvg;

uint32_t checkTimes;
double avgQueueSize;
uint32_t port = 5000;

constexpr uint32_t packetSize = 1000 - 42;

NodeContainer n0n4;
NodeContainer n1n4;
NodeContainer n2n4;
NodeContainer n3n4;
NodeContainer n4n5;
NodeContainer n5n6;
NodeContainer n5n7;
NodeContainer n5n8;
NodeContainer n5n9;

Ipv4InterfaceContainer i0i4;
Ipv4InterfaceContainer i1i4;
Ipv4InterfaceContainer i2i4;
Ipv4InterfaceContainer i3i4;
Ipv4InterfaceContainer i4i5;
Ipv4InterfaceContainer i5i6;
Ipv4InterfaceContainer i5i7;
Ipv4InterfaceContainer i5i8;
Ipv4InterfaceContainer i5i9;

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

	//TODO:  Need to figure out how to print the time this packet arrived and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get
	//           nice consequtive sequence numbers to graph
	if(tcp.GetDestinationPort () == 50001) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50002) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50003) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50004) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50005) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50006) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50007) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50008) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;

}

void DequeueAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

	//TODO:  Need to figure out how to print the time this packet left and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get
	//           nice consequtive sequence numbers to graph
	// if(tcp.GetDestinationPort () == 50002) std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;

}

void DroppedAtRed(Ptr<const QueueItem> item) {
	TcpHeader tcp;
	Ptr<Packet> pkt = item->GetPacket();
	pkt->PeekHeader(tcp);

	//TODO:  Need to figure out how to print the time this packet was dropped and which flow it belongs to.  Hint below in app setup.
	//REMEMBER:  The sequence number is actually in bytes not packets so to make it graph nice you'll need to manipulate to get
	//           nice consequtive sequence numbers to graph
	// std::cout << "\tSequenceNumber\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	// std::cout << "\tDestinationPort\t" << tcp.GetDestinationPort () << "\t" << std::endl;
	if(tcp.GetDestinationPort () == 50001) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50002) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50003) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50004) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50005) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50006) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50007) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
	else if(tcp.GetDestinationPort () == 50008) std::cout << Simulator::Now() << "\t" << tcp.GetDestinationPort () << "\t" << tcp.GetSequenceNumber() << "\t" << std::endl;
}


//This code is fine for printing average and actual queue size
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

	//allow these to be varied via command line
	uint32_t runNumber = 0;
	uint32_t maxPackets = 100;

	std::string leafLinkBW = "10Mbps";
	std::string leafLinkDelay = "1ms";

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

	//RED setup
	Config::SetDefault("ns3::RedQueueDisc::Mode", StringValue("QUEUE_MODE_PACKETS"));
	Config::SetDefault("ns3::RedQueueDisc::MeanPktSize", UintegerValue(packetSize));
  	Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (minTh));
  	Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (maxTh));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkBandwidth", StringValue (bottleneckLinkBW));
  	Config::SetDefault ("ns3::RedQueueDisc::LinkDelay", StringValue (bottleneckLinkDelay));
  	Config::SetDefault ("ns3::RedQueueDisc::QueueLimit", UintegerValue (maxPackets));
	Config::SetDefault ("ns3::RedQueueDisc::MeanPktSize", UintegerValue (packetSize));

	SeedManager::SetSeed(1);
	SeedManager::SetRun(runNumber);

	NodeContainer c;
	c.Create (10);
	Names::Add ( "N0", c.Get (0));
	Names::Add ( "N1", c.Get (1));
	Names::Add ( "N2", c.Get (2));
	Names::Add ( "N3", c.Get (3));
	Names::Add ( "N4", c.Get (4));
	Names::Add ( "N5", c.Get (5));
	Names::Add ( "N6", c.Get (6));
	Names::Add ( "N7", c.Get (7));
	Names::Add ( "N8", c.Get (8));
	Names::Add ( "N9", c.Get (9));
	n0n4 = NodeContainer (c.Get (0), c.Get (4));
	n1n4 = NodeContainer (c.Get (1), c.Get (4));
	n2n4 = NodeContainer (c.Get (2), c.Get (4));
	n3n4 = NodeContainer (c.Get (3), c.Get (4));
	n4n5 = NodeContainer (c.Get (4), c.Get (5));
	n5n6 = NodeContainer (c.Get (5), c.Get (6));
	n5n7 = NodeContainer (c.Get (5), c.Get (7));
	n5n8 = NodeContainer (c.Get (5), c.Get (8));
	n5n9 = NodeContainer (c.Get (5), c.Get (9));


	InternetStackHelper stack;
  	stack.Install (c);

	// TrafficControlHelper tchBottleneck;
	// tchBottleneck.SetRootQueueDisc("ns3::RedQueueDisc");
	// //this `install` below returns a QueueDiscContainer.  Since I'm only installing one queue, I'm just going to grab the first one
	// Ptr<QueueDisc> redQueue = (tchBottleneck.Install(n4n5.Get(0)->GetDevice(0))).Get(0);
	// tchBottleneck.Install(n4n5.Get(1)->GetDevice(0));

	//NOTE:  If I don't do the above with the RedQueueDisc, I'll end up with default queues on the bottleneck link.
	TrafficControlHelper tchPfifo;
	uint16_t handle = tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
	tchPfifo.AddInternalQueues (handle, 3, "ns3::DropTailQueue", "MaxPackets", UintegerValue (1000));

	TrafficControlHelper tchRed;
	tchRed.SetRootQueueDisc ("ns3::RedQueueDisc", "LinkBandwidth", StringValue (bottleneckLinkBW),
                           "LinkDelay", StringValue (bottleneckLinkDelay));


	//ASSIGN IP
	PointToPointHelper p2p;

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
	NetDeviceContainer devn0n4 = p2p.Install (n0n4);
	tchPfifo.Install (devn0n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
	NetDeviceContainer devn1n4 = p2p.Install (n1n4);
	tchPfifo.Install (devn1n4);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("3ms"));
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
	// only backbone link has RED queue disc
	QueueDiscContainer queueDiscs = tchRed.Install (devn4n5);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
	NetDeviceContainer devn5n6 = p2p.Install (n5n6);
	tchPfifo.Install (devn5n6);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
	NetDeviceContainer devn5n7 = p2p.Install (n5n7);
	tchPfifo.Install (devn5n7);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("5ms"));
	NetDeviceContainer devn5n8 = p2p.Install (n5n8);
	tchPfifo.Install (devn5n8);

	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
	NetDeviceContainer devn5n9 = p2p.Install (n5n9);
	tchPfifo.Install (devn5n9);

	Ptr<QueueDisc> redQueue = queueDiscs.Get(0);
	Ptr<QueueDisc> redQueue1 = queueDiscs.Get(1);

	//setup traces
	redQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&EnqueueAtRed));
	redQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&DequeueAtRed));
	redQueue->TraceConnectWithoutContext("Drop", MakeCallback(&DroppedAtRed));

	redQueue1->TraceConnectWithoutContext("Enqueue", MakeCallback(&EnqueueAtRed));
	redQueue1->TraceConnectWithoutContext("Dequeue", MakeCallback(&DequeueAtRed));
	redQueue1->TraceConnectWithoutContext("Drop", MakeCallback(&DroppedAtRed));



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

	ipv4.SetBase ("10.1.6.0", "255.255.255.0");
	i5i6 = ipv4.Assign (devn5n6);

	ipv4.SetBase ("10.1.7.0", "255.255.255.0");
	i5i7 = ipv4.Assign (devn5n7);

	ipv4.SetBase ("10.1.8.0", "255.255.255.0");
	i5i8 = ipv4.Assign (devn5n8);

	ipv4.SetBase ("10.1.9.0", "255.255.255.0");
	i5i9 = ipv4.Assign (devn5n9);


	//APPLICATION

	//Configure Sinks

	// SINKs
	// #1
	uint16_t port1 = 50001;
	Address sinkLocalAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port1));
	PacketSinkHelper sinkHelper1 ("ns3::TcpSocketFactory", sinkLocalAddress1);
	ApplicationContainer sinkApp1 = sinkHelper1.Install (n5n6.Get (1));
	sinkApp1.Start (Seconds (sink_start_time));
	sinkApp1.Stop (Seconds (sink_stop_time));
	// #2
	uint16_t port2 = 50002;
	Address sinkLocalAddress2 (InetSocketAddress (Ipv4Address::GetAny (), port2));
	PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", sinkLocalAddress2);
	ApplicationContainer sinkApp2 = sinkHelper2.Install (n5n7.Get (1));
	sinkApp2.Start (Seconds (sink_start_time));
	sinkApp2.Stop (Seconds (sink_stop_time));
	// #3
	uint16_t port3 = 50003;
	Address sinkLocalAddress3 (InetSocketAddress (Ipv4Address::GetAny (), port3));
	PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", sinkLocalAddress3);
	ApplicationContainer sinkApp3 = sinkHelper3.Install (n5n8.Get (1));
	sinkApp3.Start (Seconds (sink_start_time));
	sinkApp3.Stop (Seconds (sink_stop_time));
	// #4
	uint16_t port4 = 50004;
	Address sinkLocalAddress4 (InetSocketAddress (Ipv4Address::GetAny (), port4));
	PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory", sinkLocalAddress4);
	ApplicationContainer sinkApp4 = sinkHelper4.Install (n5n9.Get (1));
	sinkApp4.Start (Seconds (sink_start_time));
	sinkApp4.Stop (Seconds (sink_stop_time));

	// #5
	uint16_t port5 = 50005;
	Address sinkLocalAddress5 (InetSocketAddress (Ipv4Address::GetAny (), port5));
	PacketSinkHelper sinkHelper5 ("ns3::TcpSocketFactory", sinkLocalAddress5);
	ApplicationContainer sinkApp5 = sinkHelper5.Install (n0n4.Get (0));
	sinkApp5.Start (Seconds (sink_start_time));
	sinkApp5.Stop (Seconds (sink_stop_time));
	// #6
	uint16_t port6 = 50006;
	Address sinkLocalAddress6 (InetSocketAddress (Ipv4Address::GetAny (), port6));
	PacketSinkHelper sinkHelper6 ("ns3::TcpSocketFactory", sinkLocalAddress6);
	ApplicationContainer sinkApp6 = sinkHelper6.Install (n1n4.Get (0));
	sinkApp6.Start (Seconds (sink_start_time));
	sinkApp6.Stop (Seconds (sink_stop_time));
	// #7
	uint16_t port7 = 50007;
	Address sinkLocalAddress7 (InetSocketAddress (Ipv4Address::GetAny (), port7));
	PacketSinkHelper sinkHelper7 ("ns3::TcpSocketFactory", sinkLocalAddress7);
	ApplicationContainer sinkApp7 = sinkHelper7.Install (n2n4.Get (0));
	sinkApp7.Start (Seconds (sink_start_time));
	sinkApp7.Stop (Seconds (sink_stop_time));
	// #8
	uint16_t port8 = 50008;
	Address sinkLocalAddress8 (InetSocketAddress (Ipv4Address::GetAny (), port8));
	PacketSinkHelper sinkHelper8 ("ns3::TcpSocketFactory", sinkLocalAddress8);
	ApplicationContainer sinkApp8 = sinkHelper8.Install (n3n4.Get (0));
	sinkApp8.Start (Seconds (sink_start_time));
	sinkApp8.Stop (Seconds (sink_stop_time));


	//Configure Sources

	// Connenction #1
	OnOffHelper clientHelper1 ("ns3::TcpSocketFactory", Address ());
	clientHelper1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper1.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper1.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps1;
	AddressValue remoteAddress1 (InetSocketAddress (i5i6.GetAddress (1), port1));
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
	AddressValue remoteAddress2 (InetSocketAddress (i5i7.GetAddress (1), port2));
	clientHelper2.SetAttribute ("Remote", remoteAddress2);
	clientApps2.Add (clientHelper2.Install (n1n4.Get (0)));
	clientApps2.Start (Seconds (client_start_time));
	clientApps2.Stop (Seconds (client_stop_time));

	// Connection #3
	OnOffHelper clientHelper3 ("ns3::TcpSocketFactory", Address ());
	clientHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper3.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper3.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps3;
	AddressValue remoteAddress3 (InetSocketAddress (i5i8.GetAddress (1), port3));
	clientHelper3.SetAttribute ("Remote", remoteAddress3);
	clientApps3.Add (clientHelper3.Install (n2n4.Get (0)));
	clientApps3.Start (Seconds (client_start_time));
	clientApps3.Stop (Seconds (client_stop_time));

	// Connection #4
	OnOffHelper clientHelper4 ("ns3::TcpSocketFactory", Address ());
	clientHelper4.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper4.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper4.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper4.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps4;
	AddressValue remoteAddress4 (InetSocketAddress (i5i9.GetAddress (1), port4));
	clientHelper4.SetAttribute ("Remote", remoteAddress4);
	clientApps4.Add (clientHelper4.Install (n3n4.Get (0)));
	clientApps4.Start (Seconds (client_start_time));
	clientApps4.Stop (Seconds (client_stop_time));

	// Connenction #5
	OnOffHelper clientHelper5 ("ns3::TcpSocketFactory", Address ());
	clientHelper5.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper5.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper5.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper5.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps5;
	AddressValue remoteAddress5 (InetSocketAddress (i0i4.GetAddress (0), port5));
	clientHelper5.SetAttribute ("Remote", remoteAddress5);
	clientApps5.Add (clientHelper5.Install (n5n6.Get (1)));
	clientApps5.Start (Seconds (client_start_time));
	clientApps5.Stop (Seconds (client_stop_time));

	// Connection #6
	OnOffHelper clientHelper6 ("ns3::TcpSocketFactory", Address ());
	clientHelper6.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper6.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper6.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper6.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps6;
	AddressValue remoteAddress6 (InetSocketAddress (i1i4.GetAddress (0), port6));
	clientHelper6.SetAttribute ("Remote", remoteAddress6);
	clientApps6.Add (clientHelper6.Install (n5n7.Get (1)));
	clientApps6.Start (Seconds (client_start_time));
	clientApps6.Stop (Seconds (client_stop_time));

	// Connection #7
	OnOffHelper clientHelper7 ("ns3::TcpSocketFactory", Address ());
	clientHelper7.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper7.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper7.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper7.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps7;
	AddressValue remoteAddress7 (InetSocketAddress (i2i4.GetAddress (0), port7));
	clientHelper7.SetAttribute ("Remote", remoteAddress7);
	clientApps7.Add (clientHelper7.Install (n5n8.Get (1)));
	clientApps7.Start (Seconds (client_start_time));
	clientApps7.Stop (Seconds (client_stop_time));

	// Connection #8
	OnOffHelper clientHelper8 ("ns3::TcpSocketFactory", Address ());
	clientHelper8.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	clientHelper8.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	clientHelper8.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
	clientHelper8.SetAttribute ("PacketSize", UintegerValue (packetSize));

	ApplicationContainer clientApps8;
	AddressValue remoteAddress8 (InetSocketAddress (i3i4.GetAddress (0), port8));
	clientHelper8.SetAttribute ("Remote", remoteAddress8);
	clientApps8.Add (clientHelper8.Install (n5n9.Get (1)));
	clientApps8.Start (Seconds (client_start_time));
	clientApps8.Stop (Seconds (client_stop_time));


	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	std::cout << "Here we go" << std::endl;

	AnimationInterface* anim = nullptr;
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
		Simulator::ScheduleNow(&CheckQueueSize, redQueue1);
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

