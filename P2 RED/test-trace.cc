#include <iostream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

using namespace std;
using namespace ns3;


NS_LOG_COMPONENT_DEFINE("trace");

class MyObject : public Object
{
public:
    static TypeId GetTypeId(void)
    {
        static TypeId tid = TypeId("MyObject")
        .SetParent(Object::GetTypeId())
        .AddConstructor<MyObject>()
        .AddTraceSource("MyInteger", "an intefer value to trace",
        MakeTraceSourceAccessor(&MyObject::m_myInt));  //使用AddTraceSource加入
        return tid;
    }

    MyObject () {}
    TracedValue<int32_t> m_myInt; //定义要跟踪的变量
};

void IntTrace(int32_t oldValue, int32_t newValue)   //回调函数
{
    std::cout << "old:" << oldValue << "\tnew:" << newValue << std::endl;
}

int main(int argc, char *argv[])
{
    Ptr<MyObject> myObject = CreateObject<MyObject>();
    myObject->TraceConnectWithoutContext("MyInteger", MakeCallback(&IntTrace)); //关联跟踪变量和回调函数

    myObject->m_myInt = 1; //变量变化时会自动调用回调函数
    myObject->m_myInt = 2;

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}