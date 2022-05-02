#!/bin/bash

#Modo 
modprobe batman-adv
#Se para el proseso de red de la maquina
sudo pkill NetworkManager
#se apaga la interfaz wan de la maquina y se asigna el modo adhoc
ifconfig wlp6s0 down
iwconfig wlp6s0 mode ad-hoc 

ifconfig wlp6s0 mtu 1532
#Asignacion del acces point y el channel
iwconfig wlp6s0 mode ad-hoc essid RED-Adhoc ap 02:1B:55:AD:0C:02  channel 1 
sleep 1
#Se enciende la interfaz wan de la maquina
ip link set wlp6s0 up 
sleep 1
#Se asigna la interfaz virtual de BATMAN a la WAN
batctl if add wlp6s0
ifconfig bat0 up
#Se asigna la IP a la interfaz virtual
ifconfig bat0 192.168.0.2

echo "Configuracion realizada exitosamente"
