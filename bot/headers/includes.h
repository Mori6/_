#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <netinet/in.h>

#define FALSE   0
#define TRUE    1

static int comsocket;
int getip(unsigned char *toGet, struct in_addr *i);
int sockprintf(int sock, char *str, int botname);

typedef char BOOL;
typedef uint32_t ipv4_t;
typedef uint16_t port_t;

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))

ipv4_t LOCAL_ADDR;


