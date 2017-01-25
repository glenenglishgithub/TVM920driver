#include <stdint.h>
#include "Cdatagramblk.h"

#ifdef __unix__
	#include <errno.h>
#else
    #define errno WSAGetLastError()
#endif


#if 0
int CdgramsktBlk::send(uint16_t * buffer, int length) {
	int n;
	n=sendto(sock,buffer,length*sizeof(uint16_t),
	0,(const struct sockaddr *)&sktin,sockaddr_in_length);
	if (n < 0) perror("Sendto");
	return n;
}
#endif


CUDPlistener::CUDPlistener(uint16_t dstport, uint32_t listening_ipaddr, int timeoutsecs,  int timeoutms, int * starterrors) {


    struct addrinfo *result = NULL, *ptr = NULL, hints;
    sockaddr_in recvSockAddr;

    int iResult;
    memset(&recvSockAddr,0, sizeof (recvSockAddr));
    recvSockAddr.sin_family = AF_INET;
    recvSockAddr.sin_port = htons(dstport);
    //recvSockAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    recvSockAddr.sin_addr.s_addr=htonl(listening_ipaddr);/* use listening IP address to tell this machine which interface to listen on...*/

    ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(AF_INET,SOCK_DGRAM ,0);

    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);

        *starterrors = true;
    }
    SetNewTimeout( timeoutsecs, timeoutms);

    // Setup the UDP listening socket
    iResult = bind( ListenSocket,(SOCKADDR *)  &recvSockAddr,sizeof(recvSockAddr));
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);

        *starterrors = true;
    }
    *starterrors = 0;

}

int CUDPlistener::DoRecv(void) {
		
		socklen_t fromlen;
		fromlen = sizeof(struct sockaddr_in	);
		/* read into intermediate buffer */
		RXlenNbytes = recvfrom(ListenSocket,rxbuffer,
        	 UDPLISTENERRXBUFFERSIZE  ,0,(struct sockaddr *)&from,&fromlen);
		
		//if (RXlenNbytes < 0) perror("recvfrom");
		return RXlenNbytes;
}

uint32_t CUDPlistener::SetNewTimeout(int timeoutsecs, int timeoutms) {

	DWORD timeoutmS =    timeoutms + (1000 *  timeoutsecs);

	return setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO,(const char *)  &timeoutmS,sizeof(timeoutmS));

}


CUDPsender::CUDPsender(uint32_t ipv4addr, uint32_t dstPort) {
	int wsaerr;
    addr.sin_addr.s_addr = htonl((unsigned long ) ipv4addr);
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
    	wsaerr=WSAGetLastError();

    }
    else {
        addr.sin_family = AF_INET;
        addr.sin_port = htons((uint16_t)dstPort);
        sockaddr_in_length=sizeof(struct sockaddr_in);
    }
}

CUDPsender::CUDPsender(uint32_t ipv4addr, uint32_t dstPort, int timeoutsecs,int timeoutms ) {/* with timeout- used for two way socket  */
    int timeoutmS = timeoutsecs* 1000 + timeoutms;
    int wsaerr;
    addr.sin_addr.s_addr = htonl((unsigned long ) ipv4addr);
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
    	wsaerr=WSAGetLastError();

    }
    else {
        int result=setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeoutmS,sizeof(timeoutmS));

        int reuse = 1;
        result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,(char *) &reuse, sizeof(int));

        addr.sin_family = AF_INET;
        addr.sin_port = htons((uint16_t)dstPort);
        sockaddr_in_length=sizeof(struct sockaddr_in);
    }
}



/* added this one for specifying the source port 1/10/2015 */
CUDPsender::CUDPsender(uint32_t ipv4addr, uint32_t srcport, uint32_t dstPort) {
    int wsaerr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl((unsigned long ) ipv4addr);
    addr.sin_port = htons((uint16_t)dstPort);

    memset(&srcaddr,0,sizeof(srcaddr));
    srcaddr.sin_family = AF_INET;
    srcaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srcaddr.sin_port = htons((uint16_t)srcport);

    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
    	wsaerr=WSAGetLastError();
    }
    else {
        sockaddr_in_length=sizeof(struct sockaddr_in);

        /* now bind source port specific */
        if (bind(sock,(struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
            perror("bind");

        }
    }

}
	

int CUDPsender::send(uint32_t * buffer, int length) {
	int n;

	n=sendto(sock,(char * )buffer,length*sizeof(uint32_t),NULL,(const struct sockaddr *)&addr,sockaddr_in_length);
	if (n < 0) {
		fprintf(stdout,"<sendto(32) errno=%s >",strerror(errno));
		//perror("Sendto");
	}
	return n;
}


int CUDPsender::send(uint16_t * buffer, int length) {
	int n;

	n=sendto(sock,(char * )buffer,length*sizeof(uint16_t),NULL,(const struct sockaddr *)&addr,sockaddr_in_length);
	if (n < 0) {
		fprintf(stdout,"<sendto(16) errno=%s >",strerror(errno));
		//perror("Sendto");

		
	}
	return n;
}

int CUDPsender::send(uint8_t * buffer, int length) {
	int n;
	n=sendto(sock,(char * )buffer,length*sizeof(uint8_t),NULL,(const struct sockaddr *)&addr,sockaddr_in_length);
	if (n < 0) {
		fprintf(stdout,"<sendto(8) errno=%s >",strerror(errno));
		//perror("Sendto");
	}
	return n;
}

int CUDPsender::sendfkt(uint8_t * buffer, int length) {/* send aginst receiver credentials */
	int n;

	socklen_t  fskt_len = sizeof(fskt);
	n=sendto(sock,buffer,length*sizeof(uint8_t),NULL,(const struct sockaddr *)&fskt,fskt_len);
	if (n < 0) {
		fprintf(stdout,"<sendtofkt(9) errno=%s >",strerror(errno));
		//perror("Sendto");
	}
	return n;
}

int CUDPsender::rxfrom(uint8_t * buf) {/* use this to receive from who we tx and rx from on same port */
	int result;
	socklen_t  fskt_len = sizeof(fskt);
	result = recvfrom(sock,buf,sizeof(buf),0,&fskt,&fskt_len);
	return result;
}


uint32_t  CUDPsender::GetSrcPort(void) {
	struct sockaddr addr;
	socklen_t  addrlen=sizeof(struct sockaddr);
	memset(&addr,0,sizeof(addr));	
	getsockname(sock,&addr,&addrlen);
	return (uint32_t) ntohs( (((struct sockaddr_in *) addr.sa_data)->sin_port ));
	
}

void CUDPsender::AssignIPv4addr(uint32_t ipv4addr) {
	addr.sin_addr.s_addr = htonl((unsigned long ) ipv4addr);	
}
void CUDPsender::AssignDstPort(uint32_t dstport) {
	addr.sin_port = htons((uint16_t)dstport);
}