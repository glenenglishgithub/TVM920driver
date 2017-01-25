#ifndef TVMSTATSH
#define TVMSTATSH

#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "Cmachloc.h"
#include "CTVM920Info.h"
#include "Unit1.h"


class CTVMstats {

public :
	typedef struct {
    	unsigned int NegLimX_1: 1 ;
    	unsigned int NegLimX_2: 1 ; /* unused */
    	unsigned int PosLimX_1: 1 ;
    	unsigned int PosLimX_2: 1 ;    /* unused */
    	unsigned int NegLimY_1: 1 ;
    	unsigned int NegLimY_2: 1 ; /* unused */
    	unsigned int PosLimY_1: 1 ;
    	unsigned int PosLimY_2: 1 ;
    	unsigned int Nozz1_2_NegZ: 1 ;
    	unsigned int Nozz3_4_NegZ: 1 ;

    	unsigned int UpLightActive : 1;
    	unsigned int DownLightActive : 1;

    	unsigned int Pick1Active : 1;
    	unsigned int Pick2Active : 1;
    	unsigned int Pick3Active : 1;
    	unsigned int Pick4Active : 1;

    	unsigned int YellowButton : 1;
    	unsigned int RedButton : 1;
    	unsigned int GreenButton : 1;

    	unsigned int Mystery1 : 1;
    	unsigned int Mystery2 : 1;
    	unsigned int Mystery3 : 1;

   	} stats1_t;

    stats1_t stats1;
    stats1_t stats1_zm1;




    typedef struct {
        uint32_t  xstate;
        uint32_t ystate;
        uint32_t a_state[TVM920_NNOZZ];
        uint32_t z_state[TVM920_NNOZZ];
    } motionstate_t;


    Cmachloc<double> trajectory_start;
    Cmachloc<double> trajectory_end;

    motionstate_t motionstate;
    motionstate_t motionstate_zm1;

    Cmachloc<int32_t>  loctik;
    Cmachloc<int32_t> loctik_zm1;
    Cmachloc<double>  locdub;

    uint32_t MotionStateEventRegister;/* bits are set to 1 if state has changed */
    uint32_t MotionStateEndEventRegister;/* bits are set to 1 if state has changed from NZ from Z */
    bool MotionChangeSticky_OPNP ;

    __int64 FeederActuators;

    CRITICAL_SECTION cs;
public :
    CTVMstats(void) : loctik(TVM920_NNOZZ), loctik_zm1(TVM920_NNOZZ), locdub(TVM920_NNOZZ),
      	trajectory_start(TVM920_NNOZZ),trajectory_end(TVM920_NNOZZ) {

        memset((void * ) &stats1,0, sizeof(stats1));
        memset((void * ) &stats1_zm1,0, sizeof(stats1_zm1));
        memset((void * ) &motionstate,0, sizeof(motionstate));
        memset((void * ) &motionstate_zm1,0, sizeof(motionstate_zm1));
		MotionStateEventRegister=0;       /* clear motion change bits on entry */
        MotionStateEndEventRegister = 0;
        MotionChangeSticky_OPNP = false;
        FeederActuators = 0;

        InitializeCriticalSection(&cs);

    }
    ~CTVMstats(){
            DeleteCriticalSection(&cs);
    }
public :
	void CTVMstats::UpdateRealNumbers(void);
    bool CTVMstats::CompareWithGuard(Cmachloc<double> & t1,Cmachloc<double> & t2);
    int CTVMstats::Update(void);

    void CopyPresentLocationGuarded(Cmachloc<double> * c) {
        EnterCriticalSection(&cs);
        /* take a snapshot */
        *c = locdub;

        LeaveCriticalSection(&cs);

    }

    void EnterCritical(void) {
    	EnterCriticalSection(&cs);
    }
    void LeaveCritical(void) {
    	LeaveCriticalSection(&cs);
    }

    void CTVMstats::UpdateFeederStatus(__int64 feeders);
    __int64 CTVMstats::ReadFeederStatus(void);



};

#endif
