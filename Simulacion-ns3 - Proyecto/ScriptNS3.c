/*Importa librerias*/
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/core-module.h"
#include "ns3/netwoek-module.h"
#include "ns3/internet-module.h"
#include "ns3/aplications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Adhoc");
int main (int argc, char *argv[]){
    int verbose=1;
    
    //Activar trazas de dispositivos
    WifiHelper wifi;

    if(verbose){
        wifi.EnableLogComponents();
            }
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

    //Creacion de nodos
    NodeContainer nodes;
    nodes.Create(2);

    //Elección capa fisica y MAC
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager","DataMode", StringValue (phyMode), "ControlMode", StringValue (phyMode));
    wifiMac.SetType("ns3::AdhocWifiMac");

    YansWifiPhyHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FixedRssLossModel","Rss", DoubleValue(rss));
    wifiPhy.SerChannel (wifiChannel.Create());
    NetDeviceContainer devies = wifi.Install(wifiPhy, wifiMac,nodes);
    
    //Ajustes de Movilidad
    MobilityHelper mobility;

    //Posición Inicial
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc ->Add (Vector(0.0,0.0,0.0));
    positionAlloc ->Add (Vector(5.0,0.0,,0.0));
    mobility.SetPositionAllocator (positionAlloc);

    //Elección del modelo de movilidad
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install (c);

    //Trazas
    AsciiTraceHelper ascii;
    wifiPhy.EnableAsciiAll(ascii.CreateFileStream("net-adhoc.tr"));
    wifiPhy.EnnablePcap("net-adhoc", devices);

    Simulator::Run():
    Simulator::Destroy();


}
