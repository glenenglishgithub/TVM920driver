//---------------------------------------------------------------------------

#ifndef tvmtH
#define tvmtH

#include <windows.h>
#include "Cdatagramblk.h"
#include "tvm920def.h"
#include "CTVMCmdQueue.h"
#include "Cbv32.h"
#include "CTVMstats.h"
#include "CTVM920Info.h"
#include "COPNPServer.h"


#define TVMCMD_TVM_BYTEPOS 0
#define TVMCMD_VERSION_BYTEPOS 3
#define TVMCMD_CMD_BYTEPOS 4
#define TVMCMD_SUBCMD_BYTEPOS 5
#define TVMCMD_DATASTART_BYTEPOS 8

#define  TVMCMD_GPIOCMD_UPLAMP 0
#define  TVMCMD_GPIOCMD_DOWNLAMP 1
#define  TVMCMD_GPIOCMD_PICK 2
#define  TVMCMD_GPIOCMD_CLAUTOFEEDERCTL 3

#define  TVMCMD_MOVESETABSXYZA 4
    #define TVMCMD_MOVEXYZA_INFO_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+0)
    #define TVMCMD_MOVEXYZA_AXIS_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+4)
    #define TVMCMD_MOVEXYZA_X_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+16)
    #define TVMCMD_MOVEXYZA_Y_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+20)
    #define TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+24)
    #define TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+28)
    #define TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+32)
    #define TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+36)
    #define TVMCMD_MOVEXYZA_NOZZ_A1_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+40)
    #define TVMCMD_MOVEXYZA_NOZZ_A2_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+44)
    #define TVMCMD_MOVEXYZA_NOZZ_A3_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+48)
    #define TVMCMD_MOVEXYZA_NOZZ_A4_BYTEPOS (TVMCMD_DATASTART_BYTEPOS+52)

    #define  TVMCMD_MOVEXYZA_A1_BIT 0
    #define  TVMCMD_MOVEXYZA_A2_BIT 1
    #define  TVMCMD_MOVEXYZA_A3_BIT 2
    #define  TVMCMD_MOVEXYZA_A4_BIT 3
    #define  TVMCMD_MOVEXYZA_Z1_BIT 4
    #define  TVMCMD_MOVEXYZA_Z2_BIT 5
    #define  TVMCMD_MOVEXYZA_Z3_BIT 6
    #define  TVMCMD_MOVEXYZA_Z4_BIT 7
    #define  TVMCMD_MOVEXYZA_X_BIT 8
    #define  TVMCMD_MOVEXYZA_Y_BIT 9

#define TVMCMD_GOHOME 5  	/* makes it s way through to GUI to perform */
#define TVMCMD_MOVEZBIPOLAR 6
#define TVMCMD_SETXYZA 7

#define TVMCMD_MOTIONZEROMOTION 8
#define TVMCMD_SET_DEFEAT_END_STOPS 9   /* ? */
#define TVMCMD_GPIOCMD_SRBIT 10
#define TVMCMD_READREGISTER 11
#define TVMCMD_WRITEREGISTER 12
#define TVMCMD_LIGHTING 13

#define NTVM_CMD_QUEUEOBJECTS 8  /* length of cmd queue */


extern CTVMCmdQueue GUIcmdQueue;
extern CTVMstats TVMstats;
extern CTVM920Info TVMinfo;
extern COPNPServer opnpsvr;

class CTVMT {
public :



private :
    DWORD sktTXthread_id;
    DWORD sktRXthread_id;

    HANDLE   sktTXthread_handle;
    HANDLE   sktRXthread_handle;

    volatile int sktTXthread_terminate;
    volatile int sktRXthread_terminate;

    CUDPsender * udpt;
    CUDPlistener * udpr;

    Cbv32 * FeederLatch; /* includes the command */

    uint64_t MotionEndTime;

public :
	CTVMT(void) {
    	sktTXthread_id = NULL;
        sktRXthread_id = NULL;
    	sktTXthread_terminate = false;
        sktRXthread_terminate = false;
        sktTXthread_handle=INVALID_HANDLE_VALUE;
        sktRXthread_handle=INVALID_HANDLE_VALUE ;
        FeederLatch = new Cbv32(TVM920_FEEDERCMD_NBYTES*8); /* includes the command */
        FeederLatch->BVSetLength(TVM920_FEEDERCMD_NBYTES*8);
        MotionEndTime = 0;
    }

    ~CTVMT() {
    	delete FeederLatch;

        }

    int Start(void);
    int End(void);
private:
    static void sktTXthreadproc(void * v);
    void sktTXthreadmachine(void);

    static void sktRXthreadproc(void * v);
    void sktRXthreadmachine(void);

	int InitTVMstuff(void);

 	void SendTVMPacket(void);
 	void RecvTVMPacket(void);

	int GetAndDecodeTVMStatus(void);
	void SetUpLight(int state);
    void SetDownLight(int state);
	void SetGPIObit(int bitnumber, int set);
	int DecodeAndExecuteCommand(void * v);
	void SetPick(int num, int state);
    void SetCLAutoFeeder(int feedernumber,int state);

    int CTVMT::DecodeTVM920Status(int len);
	int  CTVMT::TVMMoveSetAbsXYZA(bool IsSetPos, uint8_t * cb) ;
    int CTVMT::WaitForTVMResponse(void);
	int  CTVMT::SendStartupMotionPrefixCommand(void);
	int  CTVMT::SendXYFastMotionPrefixCommand(void);
    int  CTVMT::SendXYSlowMotionPrefixCommand(void);


    int  CTVMT::SendnozzleSlowAZmotionPrefixCommand(void) ;

	int  CTVMT::SendXYFast2MotionPrefixCommand(void);
	int  CTVMT::SendXYFast3MotionPrefixCommand(void);
	int  CTVMT::SendnozzleFastAZmotionPrefixCommand(void);

	int  CTVMT::SendXYZAabsmotionCommand(bool IsSetLocationCommand,uint32_t axis_bitwise,Cmachloc<int32_t> & m);
    int  CTVMT::TVMSendFinishMotion(uint32_t eventbits);

    int CTVMT::ReadData(unsigned int regaddr, int * data);
    int CTVMT::WriteData(unsigned int regaddr, int  data);

    int CTVMT::GetAllStartupVars(void);
    int CTVMT::ReadDataAndShow(unsigned int reg);
	int CTVMT::ProcessMotionStatus(void);

    int  CTVMT::TVMSetEnd_Stop_MotionEnables(uint8_t * cb);
    int CTVMT::ConReadRegister(uint8_t * cb);
    int CTVMT::ConWriteRegister(uint8_t * cb);
};


//---------------------------------------------------------------------------
#endif
