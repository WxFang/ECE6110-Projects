#!/bin/bash
list="TcpNewReno TcpHybla TcpWestwood TcpWestwoodPlus"
for one_thing in $list
do for ((i=1;i<64;i=i*2))
do cd "scratch" 
./waf --run "p1 --nSpokes=$i --Protocol=$one_thing" > "scratch" /$one_thing$i.txt
done
done