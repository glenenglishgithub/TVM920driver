#ifndef CDATAGRAMBLKH
#define CDATAGRAMBLKH
    #include <stdint.h>
#ifdef __unix__
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <stdio.h>

    #include <errno.h>
    #include <assert.h>
    #include <string.h>
#else
	//#define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <stdio.h>
#endif

void perror(char * p) {


}

#if 0
class CdgramsktBlk {
public :
	int sock;
	struct sockaddr_in sktin;	
	unsigned int sockaddr_in_length;
			
public:
	CdgramsktBlk(uint32_t ipv4addr, uint32_t sendtoPort) {
	
		sktin.sin_addr.s_addr = htonl((unsigned long ) ipv4addr);
		sock= socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) perror("socket");

		sktin.sin_family = AF_INET;
		sktin.sin_port = htons((uint16_t)sendtoPort);
		sockaddr_in_length=sizeof(struct sockaddr_in);
		

	}
	~CdgramsktBlk() {
		closesocket(sock);
		sock=0;/* kill so cannot be accidently used */
	}
	

	int send(uint16_t * buffer, int length);

};
#endif
#define UDPLISTENERRXBUFFERSIZE 2048


class CUDPlistener {
	
public :

	uint8_t  rxbuffer[UDPLISTENERRXBUFFERSIZE] ;/* incoming buffer */
	int RXlenNbytes;
    SOCKET ListenSocket;
    struct sockaddr from;

public :
	CUDPlistener(uint16_t dstport,uint32_t listening_ipaddr, int timeoutsecs,  int timeoutms, int * starterrors);
	

	~CUDPlistener() {
		fprintf(stdout,"<~CUDPlistener()>");
		fflush(stdout);
        ListenSocket = INVALID_SOCKET;
		//fprintf(stdout,"<~CUDPlistener() closing socket>");
		//fflush(stdout);
		closesocket(ListenSocket);

	}
	
	
	int DoRecv(void);
	uint32_t SetNewTimeout(int timeoutsecs, int timeoutms);

	
	
};


class CUDPsender {
public :
	SOCKET sock;
	struct sockaddr_in addr;	
	struct sockaddr_in srcaddr;
	unsigned int sockaddr_in_length;
	struct sockaddr fskt;	/* for recv from function */

			
public:
	CUDPsender(uint32_t ipv4addr, uint32_t dstPort);
    CUDPsender(uint32_t ipv4addr, uint32_t dstPort, int timeoutsecs,int timeoutms );
    CUDPsender(uint32_t ipv4addr, uint32_t srcport, uint32_t dstPort);

	~CUDPsender() {
		closesocket(sock);
		sock=0;/* kill so cannot be accidently used */
	}
	int send(uint32_t * buffer, int length);
	int send(uint16_t * buffer, int length) ;
	int send(uint8_t * buffer, int length) ;
	void AssignIPv4addr(uint32_t ipv4addr);
	void AssignDstPort(uint32_t dstport);
	uint32_t  GetSrcPort(void) ;/* does htis work ??? */
	int rxfrom(uint8_t * buf);
	int sendfkt(uint8_t * buffer, int length);
	

};

#endif
