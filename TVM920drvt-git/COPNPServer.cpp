#include "COPNPServer.h"
#include "CLogHandlerX.h"
#include "Unit1.h"

#ifdef __unix__
	#include <errno.h>
#else
    #define errno WSAGetLastError()
#endif

#include "tvm920def.h"
#include "tvmt.h"
#include "CTVMstats.h"

#include <string>
#include<sstream>
using namespace std  ;

const int COPNPServer::MAXRXPACKETSIZE = 2048;

#if 1
template <typename T>
std::string to_string(T value)
{
    //create an output string stream
    std::ostringstream os ;

    //throw the value into the string stream
    os << value ;

    //convert the string stream into a string and return
    return os.str() ;
}
#endif
int COPNPServer::StartTCP() {
	int iResult;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

    logx->Add("<StartTCP()>");
    /* convert port to a ansistring */
    char cport[8];
    sprintf(cport,"%u",listenport);
    logx->AddForm("Skt constructed and listening on %u ",listenport ) ;

    memset((void *) &hints,0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, cport,  &hints, &result);
    if ( iResult != 0 ) {
        logx->AddForm("getaddrinfo failed with error: %d\n", iResult);
        return -1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        logx->AddForm("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        return -1;
    }



	DWORD timeoutmS =  3000;



	if (setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO,(const char *)  &timeoutmS,sizeof(timeoutmS)) != 0) {
		fprintf(stderr,"\nCError setsockopt failed SO_RCVTIMEO \n");
		return -1;
	}
	int reuse = 1;
	if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR,(const char * )  &reuse, sizeof(int)) !=0 ) {
		fprintf(stderr,"\nCError setsockopt failed SO_REUSEADDR \n");
		return -1;
	}
    #ifdef  SO_REUSEPORT
    if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) !=0)
        logx->Add("setsockopt(SO_REUSEPORT) failed");
    #endif

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        logx->AddForm("bind failed with error: %d\n", WSAGetLastError());
    	freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);
    logx->Add("StartTCP() success ");
    return 0;

}

int COPNPServer::sktListen(void) {
	logx->Add("<sktListen>");
    int iResult = listen(ListenSocket, 0); /* listen, one at a time... */
	if (iResult == SOCKET_ERROR) {
        logx->AddForm("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        return -1;
    }
    logx->Add("sktListen() success ");
    return 0;

}


int COPNPServer::sktWaitForConnect(void) {
	logx->Add("<accept>");
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        logx->AddForm("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        return -1;
    }

    logx->AddForm("connected");

    /* ok connected- shut down listening socket we dont need any more connections */
    //closesocket(ListenSocket);
    //ListenSocket = INVALID_SOCKET;

    logx->Add("sktWaitForConnect() success ");
    return 0;

}

int COPNPServer::sktRecv(void) {
    int iResult = recv(ClientSocket, recvbuf, MAXRXPACKETSIZE, 0);
    //logx->AddForm("sktRecv recv returned: %d\n", iResult);
    if (iResult==0) {

    	/* connection has been gracefully closed. */
        return -1; /* negative returns are fatal */

    }

    if (iResult < 0) {
    	int lasterror =     WSAGetLastError();

    	switch (lasterror) {
            case WSAETIMEDOUT :
            	return 0;/* this one is OK */
            case WSAECONNRESET :
            	logx->Add(" WSAECONNRESET ! ");
                return -1;	/* non fatal err */
            default :
            	logx->AddForm("recv failed with error: %d\n", lasterror);
            	return -2;

        }



    }
    /* if == 0 or < 0, probably socket no good any more */
    return iResult;
}

int COPNPServer::sktSend(uint8_t * txbuf, int nchars) {
	int iSendResult = send( ClientSocket, txbuf, nchars, 0 );
    if (iSendResult == SOCKET_ERROR) {
    	int err  = WSAGetLastError();
        logx->AddForm("send failed with error: %d\n", err);
        closesocket(ClientSocket);
        if (err == WSAEFAULT) logx->Add("which is WSAEFAULT");
		return -1;
    }
    //logx->AddForm(" sktSend Bytes sent: %d\n", iSendResult);
    return nchars;
}

int COPNPServer::CloseClientSocket(void) {
	logx->AddForm("<CloseClientSocket>");
	if (ClientSocket!=INVALID_SOCKET) closesocket(ClientSocket);
	ClientSocket=INVALID_SOCKET;

	return 0;
}
int COPNPServer::CloseListenSocket(void) {
	logx->AddForm("<CloseListenSocket>");
	if (ListenSocket!=INVALID_SOCKET) closesocket(ListenSocket);
	ListenSocket=INVALID_SOCKET;

	return 0;
}


void COPNPServer::sktThreadmachine(void) {

	int result;
    int bytesrecv=0;

    char * replystr = (char *) sendbuf;
    const int   REPLYSTRINGNCHARS = MAXRXPACKETSIZE;
    int  replychars=0;
    enum {START,GO_LISTEN,WAIT4CONNECT,CONNECTED,ENDING} state=START;
    while (sktThread_terminate==0) {
        switch (state) {
            case START :
                if (StartTCP()==0) {
                    state = GO_LISTEN ;
                }
                else {
                    sktThread_terminate = true;	/* forced terminate */
                }
                break;
            case GO_LISTEN :
                if (sktListen()==0) {
                    state = WAIT4CONNECT ;
                }
                else {
                    sktThread_terminate = true;	/* forced terminate */
                }
                break;
            case WAIT4CONNECT :
                if (sktWaitForConnect()==0) {
                    state = CONNECTED;
                }
                else {
                    sktThread_terminate = true;	/* forced terminate */
                }
                break;
            case CONNECTED :
                result =  sktRecv() ;
                bytesrecv = result;
                if (result < -1 ) {
                    sktThread_terminate = true;	 /* connection fatal - terminate */
                }
                else if (result == -1) {
                    /* connection closed with happyness */
                    state =  ENDING;
                }

                else if (result  ==0 ) {
                    /* timeout ---	nothing to do....*/
                    logx->Add(".");

                }

                else {  	/* something to echo */
                	recvbuf[bytesrecv]=NULL;
                    //logx->AddCR(recvbuf) ;
                	/* push into the command queue */
                    replychars = 0;
                    result = HandleRequest((char * )recvbuf,bytesrecv,
                    	 replystr, REPLYSTRINGNCHARS, &replychars);
                    if (replychars) { /* if we have anything to send back */
                        if (sktSend(replystr,replychars) >= 0) {
                            break;
                        }
                        else {

                            sktThread_terminate = true;	/* forced terminate */
                        }
                    }
                }
                break;
            case  ENDING :
                /* connection gracefully closed  - re ignite the listening socket*/
                CloseClientSocket();
                state = WAIT4CONNECT ;
                break;
            default :
                break;
        }
    }
    logx->Add("TVMT_terminated !");
    CloseClientSocket();
    CloseListenSocket();

    logx->Add("TVMT_sktTXthread_exit");
}

void COPNPServer::sktThreadmachineproc(void * v) {

	COPNPServer * ptr = (COPNPServer *) v;
    ptr->sktThreadmachine();

}

int  COPNPServer::KillAcceptor(void) {
    /* runs NOT in skt thread , is there a better way to do this in windows ?*/
    /* nasty ! */
    CloseListenSocket();
 	return 0;
}

int  COPNPServer::Start(void) {

    sktThread_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) sktThreadmachineproc,(this),0,&sktThread_id);
  	return 0;
}

int  COPNPServer::End(void) {

	if ( sktThread_handle!=INVALID_HANDLE_VALUE) {
    	logx->Add(" EndCOPNPServer... ");
		sktThread_terminate=true;
        KillAcceptor();
       	WaitForSingleObject( sktThread_handle,INFINITE);
        sktThread_handle=INVALID_HANDLE_VALUE ;
        logx->Add(" EndedCOPNPServer... ");

    }

    return 0;

}

int COPNPServer::LocateNameValuePair(char * src, const char * name,char ** out ) {
	char * p=NULL;
    p=strstr(src,name);
    if (p!=NULL) {
        /* found ! - now advance to equal sign */
        while ((*p != '=') && (*p !=NULL)) {
            p++;
        }
        if (p==NULL) return -1;/* fail-  no equals sign */
        else {
        	/* advanced past the equals sign */
            p++;
        	*out = p;	/* now assign that pointer */
        	return 0;
        }

    }

    return -1;
}

int COPNPServer::GetNVPdouble(char * src, const char * name,double * out ) {
    int result;
    char * plocation=NULL;
    double value;
    result = LocateNameValuePair(src,name,&plocation);
    if (result==0) {
    	/* sscanf */
        int numscanned = sscanf(plocation, "%lf",&value);
        if (numscanned == 1) {
            *out = value;
            return 0;
        }
        else return -1;
    }
	else return -1;

}

int COPNPServer::GetNVPfloat(char * src, const char * name,float * out ) {
    int result;
    char * plocation=NULL;
    float value;
    result = LocateNameValuePair(src,name,&plocation);
    if (result==0) {
    	/* sscanf */
        int numscanned = sscanf(plocation, "%f",&value);
        if (numscanned == 1) {
            *out = value;
            return 0;
        }
        else return -1;
    }
	else return -1;

}

int COPNPServer::GetNVPsigned(char * src, const char * name,int * out ) {
    int result;
    char * plocation=NULL;
    int value;
    result = LocateNameValuePair(src,name,&plocation);
    if (result==0) {
    	/* sscanf */
        int numscanned = sscanf(plocation, "%d",&value);
        if (numscanned == 1) {
            *out = value;
            return 0;
        }
        else return -1;
    }
	else return -1;

}
int COPNPServer::GetNVPcharstring(char * src, const char * name,char  * out , int maxchars) {
    int result;
    char * plocation=NULL;
    int value;



    if (maxchars < 64) return -1;  /* do not scan more than this */
    result = LocateNameValuePair(src,name,&plocation);
    if (result==0) {
    	/* sscanf */
        int numscanned = sscanf(plocation, "%64s",out);
        if (numscanned == 1) {
            return 0;
        }
        else return -1;
    }
	else return -1;

}

int COPNPServer::HandleMoveToXYZA(char * b, uint32_t * ettc) {
	/* command will be CRLF separate name:value pairs */
    /* if the value is supplied, we will move the machine */
    uint32_t axismask = 0;
    int i;
    Cmachloc<double> loc(TVM920_NNOZZ);
    *ettc = 0;/* impose default of  0


    /* attempt to get all name value pairs */
    if (GetNVPdouble(b,"x=",&loc.x) == 0) {
        axismask |= 1<< TVMCMD_MOVEXYZA_X_BIT;
    }
    else {
        /* x not provided */
        loc.x = TVMstats.locdub.x;
    }

     /* if x or y is not provided, use current location to determine distance to travel */
    if (GetNVPdouble(b,"y=",&loc.y)==0) {
        axismask |= 1<< TVMCMD_MOVEXYZA_Y_BIT;
    }
    else {
        /* y not provided */
        loc.y = TVMstats.locdub.y;
    }

    /* if x or y is not provided, use current location to determine distance to travel */


	#if TVM920_NNOZZ !=4
        no good ! needs work
    #endif

    if (GetNVPdouble(b,"z1=",&loc.z[0])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_Z1_BIT;
    if (GetNVPdouble(b,"z2=",&loc.z[1])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_Z2_BIT;
    if (GetNVPdouble(b,"z3=",&loc.z[2])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_Z3_BIT;
    if (GetNVPdouble(b,"z4=",&loc.z[3])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_Z4_BIT;

    if (GetNVPdouble(b,"a1=",&loc.a[0])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_A1_BIT;
    if (GetNVPdouble(b,"a2=",&loc.a[1])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_A2_BIT;
    if (GetNVPdouble(b,"a3=",&loc.a[2])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_A3_BIT;
    if (GetNVPdouble(b,"a4=",&loc.a[3])==0)   axismask |= 1<< TVMCMD_MOVEXYZA_A4_BIT;


    /* TO DO * - be smarter at preventing OpenPNP from ging where it cannot */
    TVMinfo.ImposeBoundaryLimit(loc);

    int speed = 0;
    GetNVPsigned(b,"speed=",&speed);/* default slow */
    /* TO DO */
    if (speed > 95) speed = 3;
    else if (speed > 50) speed  = 2;
    else if (speed > 10) speed = 1;
    else speed = 0;
    double speed_mm_per_second=1;
    double acceldecceltime = 1;
    /* provide the caller SOME estimate of how long this is going to take to finish */
    switch (speed) {

        case 0 :
        speed_mm_per_second = 1;
        acceldecceltime = 1;
        break;
        case 1 :
        speed_mm_per_second = 20;
        acceldecceltime = 1;
        break;
        case 2 :
        speed_mm_per_second = 100;
        acceldecceltime = 1;
        break;
        case 3 :
        speed_mm_per_second = 200;
        acceldecceltime = 1;
        break;
        default :
        speed_mm_per_second = 1;
        break;
    }



    /* figure out time to completion */
    Cmachloc<double> distances(TVM920_NNOZZ);
    distances.CalcABSDiffs(loc,TVMstats.locdub);


    /* calculate distance for XY move */
    double distanceXYmm = sqrt( (distances.x *   distances.x) +
         ( distances.y *   distances.y));

    double timetravelXY = (distanceXYmm /speed_mm_per_second) + acceldecceltime  ;



    /* calc max nozzle movement */
    double  maxNozzZtrav = -1;
    for (i=0;i< TVM920_NNOZZ;i++)  {
        if (distances.z[i] > maxNozzZtrav) maxNozzZtrav = distances.z[i];
    }
    double timetravelZ =  ( maxNozzZtrav /   speed_mm_per_second ) + acceldecceltime  ;

    double  maxNozzAtrav = -1;
    for (i=0;i< TVM920_NNOZZ;i++)  {
        if (distances.a[i] > maxNozzAtrav) maxNozzAtrav = distances.a[i];
    }
    double timetravelA =  maxNozzAtrav  /   speed_mm_per_second + acceldecceltime   ;/*  TO DO is rottion  */

    const double DMINMM = 0.05;	/* deal with floating point ZERO */

    if ((distanceXYmm < DMINMM) && ( maxNozzZtrav < DMINMM ) && ( maxNozzAtrav < DMINMM) ) {

    	/* there is nothing to do */
        *ettc = 0 ;/* nothng to do */
        return 0;
    }

    double timetravel;
    /* do max */
    if (timetravelXY > timetravelZ) timetravel =    timetravelXY;
    else  timetravel =    timetravelZ;

    if (timetravelA > timetravel) timetravel =    timetravelA;
    else  timetravel =    timetravel;


    const double OTHERACTUATORTIME = 1.0; /* minimum to be worried about */
    if (timetravel < OTHERACTUATORTIME) timetravel=OTHERACTUATORTIME;/* minimum to move anything */

    *ettc = timetravel * 1000;/* milli seconds */
    /* use this to figure out if we are where we are going to yet*/
    logx->AddFormA( " t=%u ",*ettc) ;
    CheckCommandComplete(true);
	return inboundq->MoveXYZA(loc,axismask,speed );
}


int COPNPServer::NozzVacCtl(char * b,char * reply, int maxreplychars, int *  nreplychars) {


    int state,n;

    GetNVPsigned(b,"STATE=",&state);
    GetNVPsigned(b,"INDEX=",&n);    /* 0,1,2,3 */
	logx->AddForm(" NZ%d:%d ",n,state);
    string s;
    s= "NOZZVACCTL ";
    //s+="x=" + to_string(locf.x) + " " ;
    strncpy(reply,s.c_str(),maxreplychars);
    *nreplychars = strlen(reply);
    return inboundq->NozzleVacCtl(n,state);
}

int COPNPServer::HandleFeederCtl(char * b, char * reply, int maxreplychars, int *  nreplychars) {


    int state,n;

    GetNVPsigned(b,"STATE=",&state);
    GetNVPsigned(b,"INDEX=",&n);

	logx->AddForm(" F%d:%d ",n,state);

    string s;
    s= "FEEDERCTL ";
    //s+="x=" + to_string(locf.x) + " " ;
    strncpy(reply,s.c_str(),maxreplychars);
    *nreplychars = strlen(reply);
    return inboundq->FeederCtl(n,state);

}

int COPNPServer::HandleLightingCtl(char * b, char * reply, int maxreplychars, int *  nreplychars) {


    int state,n;

    GetNVPsigned(b,"STATE=",&state);
    GetNVPsigned(b,"INDEX=",&n);

	logx->AddForm(" L ",n,state);

    string s;
    s= "LIGHTINGCTL ";
    //s+="x=" + to_string(locf.x) + " " ;
    strncpy(reply,s.c_str(),maxreplychars);
    *nreplychars = strlen(reply);
    return inboundq->LightingCtl(n,state);
}

int COPNPServer::DoGoHome(char * reply, int maxreplychars, int *  nreplychars) {
	logx->Add(" GOHOME ");

    string s;
    s= "GOHOME ";
    //s+="x=" + to_string(locf.x) + " " ;
    strncpy(reply,s.c_str(),maxreplychars);
    *nreplychars = strlen(reply);
    return inboundq->GoHome();

}

int COPNPServer::GetXYZAFast(char * reply, int maxreplychars, int *  nreplychars) {
	/* get from status class object - do not issue command to queue - for speed*/
    Cmachloc<double> locf(TVM920_NNOZZ);
    Cmachloc<int32_t> loci(TVM920_NNOZZ);

    loci.x = TVMstats.loctik.x;
    loci.y = TVMstats.loctik.y;

    int n =  TVM920_NNOZZ;
    while (n--) {
        loci.a[n] = TVMstats.loctik.a[n];
        loci.z[n] = TVMstats.loctik.z[n];
    }
    /* xlate from ticks to distance  */
    TVMinfo.xlateTomm(loci, locf);

    /* construct ASCII reponse */
    string s;
    s= "GETFAST_XYZA ";
    s+="x=" + to_string(locf.x) + " " ;
    s+="y=" + to_string(locf.y) + " ";

	n =  TVM920_NNOZZ;
    while (n--) {
    	s+="z"+to_string(n+1)+"="+to_string(locf.z[n])+ " ";
    }
	n =  TVM920_NNOZZ;
    while (n--) {
    	s+="a"+to_string(n+1)+"="+to_string(locf.a[n]) + " " ;
    }


    strncpy(reply,s.c_str(),maxreplychars);

    *nreplychars = strlen(reply);

    return 0;




}

int  COPNPServer::CheckCommandComplete(int reset) {
	/* run a FSM to check fi we are finished moving */
    /* but because of timing, we shoudl check we actually moved, first....
    before we use he motion stopped outputs from the TVM */

	static enum  {IDLE,WAIT,STARTMOVE,DONE} state = IDLE;
    if (reset==true) {
    	TVMstats.CopyPresentLocationGuarded(&lastKnownLocation);
        TVMstats.MotionChangeSticky_OPNP=false;
        logx->Add(" FSM reset ") ;
        state = WAIT;
        return false;
    }
    switch (state) {
    	case IDLE :

        	break;
        case  WAIT :
        	if ( TVMstats.CompareWithGuard(lastKnownLocation,TVMstats.locdub)==false) {
            	logx->Add(" FSM WAIT>>STARTMOVE ");
                	state = STARTMOVE;
            }
            break;
        case  STARTMOVE :/* the first LASTDONE from OPNP will hit it here */
        	/* check for motion status sticky bit */
            /* OR check for position is the  same for two consecutive calls */
            //logx->AddForm("locdub.x=%lf .y=%lf lkl.x=%lf .y=%lf ",
            	//TVMstats.locdub.x, TVMstats.locdub.y, lastKnownLocation.x,lastKnownLocation.y);
        	if (TVMstats.MotionChangeSticky_OPNP) {
            	logx->Add(" MotionChangeSticky_OPNP ");
                state = IDLE;
            	return true;
            }
            else if  (TVMstats.locdub.CompareWithLimit(lastKnownLocation,0.10)==true) {   /* moved within 100 um? */
                logx->Add(" TVMstats== "  );
                state = DONE;

            	return true;
            }

            else {
            	/* take  a snap shot of current location  for comparign to see if we stop*/
                TVMstats.CopyPresentLocationGuarded(&lastKnownLocation);
            }
            break;
        case DONE:
            return true;
        	break;
        default :
        	break;
    }
    return false;
}

int COPNPServer::HandleRequest(char * b, int nchars,
	 char * reply, int maxreplychars, int *  nreplychars) {
	/* Push the socket packet into a command processing queue */
    /* they are all going to be text packets fromopenPNP, so put NULL on end to terminate,
    for the moment at least until we do something better.... */
	b[nchars]=NULL;
    int result=0;
    char s[32];

    int  callerTaskId;
    GetNVPsigned(b,"TASKID=",&callerTaskId);


    if ( strstr(b,"MOVETO_XYZA") != NULL) {
        /* get name, pairs */
        uint32_t ettc;/* estimated time to completion, milliseconds */
        result = HandleMoveToXYZA(b, &ettc);
        sprintf(s,"%u ",ettc);
        strncpy(reply,"MOVETO_XYZA TIME=",maxreplychars);
        strncat(reply,s,maxreplychars);


    }
    else if ( strstr(b,"GETFAST_XYZA") != NULL) {
        /* get  location  of machine from most recent status - dont ask, just get it  */
        result = GetXYZAFast(reply,maxreplychars,nreplychars);
    }
    else if (strstr(b,"NOZZVACCTL") != NULL) {
        result = NozzVacCtl(b,reply,maxreplychars,nreplychars);
    }
    else if (strstr(b,"HELO")!=NULL) {
        /* get name, pairs */
        logx->Add(" Got HELO ");
        strncpy(reply,"HELO ACK 1.0 ",maxreplychars);

        result = 0;
    }
    else if (strstr(b,"LASTDONE?") != NULL) {
    	/* done checker- check for motion stop.... */
        if (CheckCommandComplete(false)==true) {

            /* we are done */
        	strncpy(reply,"YESDONE ",maxreplychars);
            logx->Add(" <done> ");
        }
        else {
            /* or not */
        	strncpy(reply,"NOTDONE ",maxreplychars);
            logx->Add(" <NOTdone> ");
        }


        result = 0;

    }
    else if ( strstr(b,"FEEDERCTL") != NULL) {
        result = HandleFeederCtl(b,reply,maxreplychars,nreplychars);
    }
    else if ( strstr(b,"LIGHTINGCTL") != NULL) {
        result = HandleLightingCtl(b,reply,maxreplychars,nreplychars);
    }
    else if ( strstr(b,"GOHOME") != NULL ) {
        result = DoGoHome(reply,maxreplychars,nreplychars);
    }
    else {
    	logx->Add("!");  /* MUST send something back */
        strncpy(reply,"HELO ",maxreplychars);

        result = 0;
    }
    sprintf(s,"%d ",callerTaskId);
    strncat(reply," TASKID=",maxreplychars);
    strncat(reply,s,maxreplychars);
    strncat(reply," \r\n",maxreplychars);
    *nreplychars = strlen(reply);
	return result;
}




