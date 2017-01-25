#include "CTVMCmdQueue.h"
#include "tvm920def.h"
#include "tvmt.h"
#include "Unit1.h"

/* accept move XYZA in ticks , and prepare for queue in binary format */
int CTVMCmdQueue::MoveZBIPOLAR(Cmachloc<int32_t>   & loc, uint32_t axismask, int speed) {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_MOVEZBIPOLAR,0);

    *((uint32_t *) &p[TVMCMD_MOVEXYZA_INFO_BYTEPOS]) =  speed ;
    *((uint32_t *) &p[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]) =  axismask ;


    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS]) = loc.z[0];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS]) = loc.z[1];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS]) = loc.z[2];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS]) = loc.z[3];


    logx->Add(" OPNP sending MOVE BIPOLAR sp: ");

 	Post(stdtimeout);
    return 0;
}

/* accept move XYZA in ticks , and prepare for queue in binary format */
int CTVMCmdQueue::MoveXYZA(Cmachloc<int32_t>   & loc, uint32_t axismask, int speed) {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_MOVESETABSXYZA,0);

    *((uint32_t *) &p[TVMCMD_MOVEXYZA_INFO_BYTEPOS]) =  speed ;
    *((uint32_t *) &p[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]) =  axismask ;
    *((int32_t *) &p[TVMCMD_MOVEXYZA_X_BYTEPOS]) = loc.x ;
    *((int32_t *) &p[TVMCMD_MOVEXYZA_Y_BYTEPOS]) = loc.y;


    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS]) = loc.z[0];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS]) = loc.z[1];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS]) = loc.z[2];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS]) = loc.z[3];

    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A1_BYTEPOS]) = loc.a[0];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A2_BYTEPOS]) = loc.a[1];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A3_BYTEPOS]) = loc.a[2];
    *((int32_t *) &p[TVMCMD_MOVEXYZA_NOZZ_A4_BYTEPOS]) = loc.a[3];

    logx->Add(" OPNP sending MOVE XYZA sp: ");

 	Post(stdtimeout);
    return 0;
}


/* accept XYZA move in millimeters, degrees etc */
int CTVMCmdQueue::MoveXYZA(Cmachloc<double> & locf, uint32_t axismask, int speed) {
    /* now  push into queue */


    Cmachloc<int32_t> loc(TVM920_NNOZZ);
    /* translation layer for distances */
    TVMinfo.xlateToTicks(locf,&loc);

    MoveXYZA(loc,axismask,speed);

    return  0;
}



/* generate a feeder control command - one at a time !*/
int CTVMCmdQueue::FeederCtl(uint32_t feederindexZeroBased, uint32_t isopenair) {

    const int stdtimeout = 100;
	/* toggle UP light */
    uint8_t * p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_GPIOCMD_CLAUTOFEEDERCTL,0);
    int feedernumber =  feederindexZeroBased;

    p[TVMCMD_DATASTART_BYTEPOS+0] = feedernumber ;
    p[TVMCMD_DATASTART_BYTEPOS+4] = isopenair;
	Post(stdtimeout);
}

 int CTVMCmdQueue::NozzleVacCtl(uint32_t nozzleZeroBased, uint32_t VacOn)  {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_GPIOCMD_PICK,0);
    p[TVMCMD_DATASTART_BYTEPOS+0] =  nozzleZeroBased;
    p[TVMCMD_DATASTART_BYTEPOS+1] =  VacOn;
	Post(stdtimeout);
 }

  int CTVMCmdQueue::LightingCtl(uint32_t lightindex, uint32_t state)  {
    const int stdtimeout = 100;
    enum {UPLIGHTINDEX, DOWNLIGHTINDEX};
    uint8_t * p=NULL;
    switch (lightindex) {
    	case UPLIGHTINDEX :
        	p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_GPIOCMD_UPLAMP,0);
            break;
        case DOWNLIGHTINDEX :
        	p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_GPIOCMD_DOWNLAMP,0);
            break;
        default :
            return -1;
        	break;
    }

    p[TVMCMD_DATASTART_BYTEPOS+0] =  state;
	Post(stdtimeout);
 }


 /* send home command */
 int CTVMCmdQueue::GoHome(void)  {
    const int stdtimeout = 100;
    uint8_t * p = (uint8_t *) GetPtrToNextBufferNice(TVMCMD_GOHOME,0);
	Post(stdtimeout);
 }
