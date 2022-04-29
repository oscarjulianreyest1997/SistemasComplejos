/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-
Autores: Oscar Julian Reyes Torres, Valeria Bermudez Carvajal, Santiago Gutierrez Orjuela
Profesor: Joaquin Fernando Sanchez Cifuentes
Tema:  Simulacion de red con dos nodos y un enlace wifi en NS-3
Fecha: 29 de abril de 2022
 */


//Librerias necesarias para su correcto funcionamiento 
#include "ns3/core-module.h" //Libreria de nucleo
#include "ns3/network-module.h" //Libreria de red
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h" //Libreria de mobilidad
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h" //Libreria Wifi
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"//Libreria de simulacion con NetAnim

// Default Network Topology
//
//   Wifi 10.1.3.0
//            AP   AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n1   n2   n3   n0 

using namespace ns3; //Etiquetas de c++ para ns3

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");


//FUncion principal
int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nPkt = 1; //Numero de paquetes
  uint32_t nWifi = 2; //Numero de Nodos wifi mobiles
  bool tracing = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nPkt", "Numero de Paquetes", nPkt); //Estructura para utilizar directamente en la linea de comandos o terminal para settear valores
  cmd.AddValue ("nWifi", "Numero de dispositvos wifi", nWifi); //Numero de nodos wifi mobiles
  cmd.AddValue ("verbose", "Decir a las aplicaciones de eco que inicien sesion si es cierto", verbose);
  cmd.AddValue ("tracing", "Habilitar seguimiento de pcap", tracing); //Si se desea guardar capturas de trafico que se puden leer como wireshark  

  cmd.Parse (argc,argv);

  if (nWifi > 250) //Se conficiona a que si se crean mas de 250 nodos mobiles retorne un error, dado que el maximo de direcciones ip disponibles son 250
    {
      std::cout << "Los nodos mobiles nWifi no pueden exceder los 250, dado que se acabarian las direcciones ip para esta simulacion" << std::endl;
      return 1;
    }

  if (verbose) //Si se activa esta opcion veremos:
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO); //Lo que sucede con la aplicacion udp del cliente
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO); //Lo que sucede con la aplicacion udp del Servidor
    }


  NodeContainer wifiStaNodes; //Se crean los nodos y se le asigna el nombre de wifiStaNodes -> STA por ser estation (moniles)
  wifiStaNodes.Create (nWifi); //Creacion de estaciones mobiles haciendo uso de la variable nWifi por defecto y segun requerimiento es de 2 
  NodeContainer wifiApNode; 
  wifiApNode.Create(1); //Se crea el nodo de Access point (solo hay 1)

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default (); //Configurar el canal 
  YansWifiPhyHelper phy; //Configurar la interfaz fisica del wifi
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager"); //COnfigurar la estacion

  WifiMacHelper mac;
  Ssid ssid = Ssid ("SistemasComplejos2022"); //Nombre de la red wifi 
  mac.SetType ("ns3::StaWifiMac", //Se asigna la configuracion de la capa Mac
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices; //Dispositivos que se instalan (tarjetas de red y controladores) de cada nodo 
  staDevices = wifi.Install (phy, mac, wifiStaNodes); //Se instala la coneccion fisica, la direccion Mac en los nodos wifi

  mac.SetType ("ns3::ApWifiMac", //Se instala la tarjeta en los nodos de Access point
               "Ssid", SsidValue (ssid)); //Se instala la coneccion fisica, la direccion Mac en el Access Point 

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility; //Se crea la Mobilidad

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (1.1), //Coordenadas minimas en X
                                 "MinY", DoubleValue (1.1), //Coordenadas minimas en Y
                                 "DeltaX", DoubleValue (5.0), //Lo que se va a mover en X
                                 "DeltaY", DoubleValue (10.0), //Lo que se va a mover en Y
                                 "GridWidth", UintegerValue (5), //El ancho
                                 "LayoutType", StringValue ("RowFirst")); //Se le asigna la posicion dependiendo de la grilla 

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", //Sea aleatorio la mobilidad
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes); //Se instala en los nodos mobiles "wifiStaNodes"

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); //Se declara una posicion constante para el Access Point
  mobility.Install (wifiApNode);

  InternetStackHelper stack; //Pila de protocolos de internet que creamos
  stack.Install (wifiApNode);//Se instala en el nodo creado como Access Point 
  stack.Install (wifiStaNodes); //Se instala en los nodos creados en este caso los nodos mobilies wifi

  Ipv4AddressHelper address; //Se  hace uso del protocolo de enrutamiento ipv4

  //Asigna direcciones ipv4 a los nodos empezando por los nodos mobiles, por lo tanto el nodo 0 va a tener la direccion 10.1.3.1
  address.SetBase ("10.1.3.0", "255.255.255.0");

  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces=address.Assign (staDevices);
  address.Assign (apDevices);

  UdpEchoServerHelper echoServer (9); //Se invoca el servicio 

  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes.Get (0)); //Para el servidor el contenedor de la Aplicacion
  serverApps.Start (Seconds (1.0)); //Para que inicie en 1 segundo
  serverApps.Stop (Seconds (10.0)); //Para que termine en 10 segundos

  UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (0), 9); //Se incluye el trafico para el cliente
  echoClient.SetAttribute ("MaxPackets", UintegerValue (nPkt)); //Se define el numero de paquetes
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); //El intervalo en que se va a transmitir
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024)); ///El tamano de paquetes

  //Se crea la aplicacion aque leera el cliente que se consume cada 2 segundos mientras que parsa el servidor es cada 1 segundo
  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0)); //Tambien se termina a los 10 segundos

  Simulator::Stop (Seconds (10.0)); //Detener la simulacion

  //Habilitar la captura del trafico para ser leida por ejemplo con wireshark
  if (tracing == true)
    {
      phy.EnablePcap ("third", apDevices.Get (0));
    }
    
  //Se crea el archivo .xml para su visualizacion en NetAnim  
  AnimationInterface anim("Red2022.xml");

  //Constructores del Programa
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
