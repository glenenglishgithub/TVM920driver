#ifndef COPNPSERVERH
#define COPNPSERVERH
#include <stdint.h>
#include "Cdatagramblk.h"
#include "Cmachloc.h"
#include "tvm920def.h"
#include "CTVMCmdQueue.h"
//#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#pragma comment (lib, "Ws2_32.lib")

/*

	TCP listener
    OPNP must open a socket
    we will listen and BLOCK on that socket  and process commands.

    so there are two queues
    - one inbound queue
    - that shares the same decoding engine as the GUI
    - this will process commands and requests from the host PNP controller like Open PNP

    - one outbound
    - which is all the return info- acks or data requests.

    - host commands can either be async - that is they REPLY ACK immediately , and then you have to ask for finished,
    or
    -they can be sync- REPLY is not generated until the job, whatever it is, is done

    UDP Talker.
    - also, there are asynchronous updates.
    Async updates will be on a separate UDP backchannel.
    Whenever ANY status, location, anything on the machine changes, a status update will be issued
    this way, the status updates are stateless.

*/
class COPNPServer {
private :
    DWORD sktThread_id;
    HANDLE   sktThread_handle;
    volatile int sktThread_terminate;

public :
	static const int MAXRXPACKETSIZE;
	uint8_t *sendbuf, * recvbuf;
    SOCKET ListenSocket;
    SOCKET ClientSocket;
    struct sockaddr from;

    uint32_t listening_ipaddr;
    uint32_t listenport;

public :

    /* open PNP shouldnt do any more than this many in a queue */

	CTVMCmdQueue * inboundq;
	Cmachloc<double>  lastKnownLocation;   /* used for figuring out if we are done */
public :
	COPNPServer(uint32_t ip4listenaddr, uint32_t port)  :  listening_ipaddr( ip4listenaddr ) ,
    	  listenport(port), lastKnownLocation(TVM920_NNOZZ) {
        ListenSocket=INVALID_SOCKET;
        ClientSocket=INVALID_SOCKET;
		sendbuf = new uint8_t[MAXRXPACKETSIZE];
		recvbuf = new uint8_t[MAXRXPACKETSIZE];
        sktThread_handle = INVALID_HANDLE_VALUE;
        sktThread_terminate = false;
        sktThread_id = NULL;
		#define  CTVMCmdQueue_inboundq_INQUEUELENGTH 4
        inboundq = new CTVMCmdQueue(CTVMCmdQueue_inboundq_INQUEUELENGTH);
       // lastKnownLocation = new Cmachloc<double>(TVM920_NNOZZ);

    }
    ~COPNPServer() {
    	delete sendbuf;
    	delete recvbuf;
        delete inboundq;
        //delete lastKnownLocation;
    }
	static void COPNPServer::sktThreadmachineproc(void * v);
    void COPNPServer::sktThreadmachine(void);
    int  COPNPServer::Start(void);
    int  COPNPServer::End(void);

    int COPNPServer::StartTCP();
    int COPNPServer::sktListen(void);

    int COPNPServer::sktWaitForConnect(void);
    int COPNPServer::sktRecv(void);
	int COPNPServer::sktSend(uint8_t * txbuf, int nchars);
    int COPNPServer::CloseClientSocket(void);
    int COPNPServer::CloseListenSocket(void);
    int  COPNPServer::KillAcceptor(void);

    int COPNPServer::HandleRequest(char * b, int nchars,
	 	char * reply, int maxreplychars, int *  nreplychars);
	int COPNPServer::LocateNameValuePair(char * src, const char * name,char ** out );
    int COPNPServer::HandleMoveToXYZA(char * b, uint32_t * ettc) ;
    int COPNPServer::GetNVPfloat(char * src, const char * name,float * out );
    int COPNPServer::GetNVPsigned(char * src, const char * name,int * out );
    int COPNPServer::GetNVPdouble(char * src, const char * name,double * out );
    int COPNPServer::GetXYZAFast(char * reply, int maxreplychars, int *  nreplychars);
    int  COPNPServer::CheckCommandComplete(int reset);
    int COPNPServer::NozzVacCtl(char * b, char * reply, int maxreplychars, int *  nreplychars);
    int COPNPServer::HandleFeederCtl(char * b, char * reply, int maxreplychars, int *  nreplychars);
    int COPNPServer::DoGoHome(char * reply, int maxreplychars, int *  nreplychars);
    int COPNPServer::GetNVPcharstring(char * src, const char * name,char  * out , int maxchars);
    int COPNPServer::HandleLightingCtl(char * b, char * reply, int maxreplychars, int *  nreplychars);
};
#endif
