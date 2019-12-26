#pragma once
void udpattack(unsigned char *target, int port, int secs, int packetsize);
void tcpattack(unsigned char *target, int port, int secs, int packetsize, char *flags);
void greattack(unsigned char *target, int port, int secs, int packetsize);
