//---------------------------------------------------------------------------

#pragma hdrstop

#include "tvmt.h"
#include "Unit1.h"
#include <vcl.h>
#include "globs.h"

#include "Cmachloc.h"

#include <windows.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

const unsigned int BYTEBITS = 8;

CTVMCmdQueue GUIcmdQueue(NTVM_CMD_QUEUEOBJECTS);
CTVMstats TVMstats;
CTVM920Info TVMinfo;
#define OPNP_IPADDR 192<<24|168<<16|30<<8|61<<0	/* HAS to be this */
COPNPServer opnpsvr(OPNP_IPADDR,8888);
/*
$16 is gpio set  + 7 bytes
$17 is gpio clear
$16 a,b,c,d,e,f
Up light  		0,0,0,0   ,1,0,0
down light  	0,0,0,0x80,0,0,0
pick1           0,0,0,1   ,0,0,0
pick2           0,0,0,2   ,0,0,0
pick3           0,0,0,4   ,0,0,0
pick4           0,0,0,4   ,0,0,0

*/
/*
feeders
$ 05  + 11 bytes
front Feeder 1 (first feeder)
0,0,0,0,0,0,0,0,0,1,0 	, bitwise.
0,0,0,0,0,0,0,0,0,2,0 	, bitwise.       etc
0,0,0,0,0,0,0,0,0,4,0 	, bitwise.       etc
Back Feeder 1 (first feeder)
0,0,0,0,0,1,0,0,0,0,0
0,0,0,0,0,2,0,0,0,0,0   etc
0,0,0,0,0,4,0,0,0,0,0   etc

*/
void showhex(uint8_t * buf, int len) {

    logx->AddForm("\t");
	while (len--) {
    	logx->AddForm("%02x ",(int) (*(buf++)));

    }

}
#if 0
#define TVM920_STS_NEGLIMX_BIT ((8*8)+0)
#define TVM920_STS_POSLIMX_BIT ((8*8)+1)
#define TVM920_STS_POSLIMY_RLHS_BIT ((8*8)+2)
#define TVM920_STS_POSLIMY_RRHS_BIT ((8*8)+3)
#define TVM920_STS_NEGLIMY_FLHS_BIT ((8*8)+4)
#define TVM920_STS_NOZZ1_2_NEGZ_BIT ((8*8)+5)
#define TVM920_STS_NOZZ3_4_NEGZ_BIT ((8*8)+6)
#endif

int CTVMT::WaitForTVMResponse(void) {
	int result=udpr->DoRecv();
    if (result>0) {
        //logx->Add("*");
        logx->AddForm("\r\n%d:",result);
        showhex(udpr->rxbuffer,result);
    }
    return result;

}

int CTVMT::DecodeTVM920Status(int len) {

	/* decode */
    if (len!=TVM920_STS_NBYTES) return -1;

    /* push entire thing into a Cbv32 */

    uint8_t * cp = udpr->rxbuffer;
    Cbv32 bv(TVM920_STS_NBYTES*8);

    while (len--) bv.AddBitVectorBits(*(cp++),8);

    cp = udpr->rxbuffer;/* reset this pointer */

    /* decode end stops - count bits and bytes forward from start */

    TVMstats.stats1.NegLimX_1 = bv.GetBitVectorBitExt(TVM920_STS_NEGLIMX_BIT);
    TVMstats.stats1.PosLimX_1 = bv.GetBitVectorBitExt(TVM920_STS_POSLIMX_BIT);
    TVMstats.stats1.PosLimY_1 = bv.GetBitVectorBitExt(TVM920_STS_POSLIMY_RLHS_BIT);
    TVMstats.stats1.PosLimY_2 = bv.GetBitVectorBitExt(TVM920_STS_POSLIMY_RRHS_BIT);
    TVMstats.stats1.NegLimY_1 = bv.GetBitVectorBitExt(TVM920_STS_NEGLIMY_FLHS_BIT);
    TVMstats.stats1.Nozz1_2_NegZ = bv.GetBitVectorBitExt(TVM920_STS_NOZZ1_2_NEGZ_BIT);
    TVMstats.stats1.Nozz3_4_NegZ = bv.GetBitVectorBitExt(TVM920_STS_NOZZ3_4_NEGZ_BIT);

    TVMstats.stats1.Pick1Active = bv.GetBitVectorBitExt(TVM920_STS_PICK1_ACTIVE_BIT);
    TVMstats.stats1.Pick2Active = bv.GetBitVectorBitExt(TVM920_STS_PICK2_ACTIVE_BIT);
    TVMstats.stats1.Pick3Active = bv.GetBitVectorBitExt(TVM920_STS_PICK3_ACTIVE_BIT);
    TVMstats.stats1.Pick4Active = bv.GetBitVectorBitExt(TVM920_STS_PICK4_ACTIVE_BIT);

    TVMstats.stats1.UpLightActive = bv.GetBitVectorBitExt(TVM920_STS_UPLIGHT_ACTIVE_BIT);
    TVMstats.stats1.DownLightActive = bv.GetBitVectorBitExt(TVM920_STS_DOWNLIGHT_ACTIVE_BIT);

    TVMstats.stats1.YellowButton = ~bv.GetBitVectorBitExt(TVM920_STS_BUTTON_YELLOW_BIT);
    TVMstats.stats1.RedButton = ~bv.GetBitVectorBitExt(TVM920_STS_BUTTON_RED_BIT);
    TVMstats.stats1.GreenButton = ~bv.GetBitVectorBitExt(TVM920_STS_BUTTON_GREEN_BIT);

    TVMstats.stats1.Mystery1 = bv.GetBitVectorBitExt(TVM920_STS_MYSTERY1_BIT);
    TVMstats.stats1.Mystery2 = bv.GetBitVectorBitExt(TVM920_STS_MYSTERY2_BIT);
    TVMstats.stats1.Mystery3 = bv.GetBitVectorBitExt(TVM920_STS_MYSTERY3_BIT);

    /* decode HEAD XY position */
    TVMstats.EnterCritical();
    TVMstats.loctik.x =(int32_t)   (*((int32_t *) &cp[TVM920_STS_XLOC_STATUS_BYTE_POS]));
    TVMstats.loctik.y =(int32_t)   (*((int32_t *) &cp[TVM920_STS_YLOC_STATUS_BYTE_POS]));

    TVMstats.motionstate.xstate =  cp[TVM920_STS_XMOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.ystate =  cp[TVM920_STS_YMOVE_STATUS_BYTE_POS] ;

    /* decode NOZZLE A axis rotation */
    TVMstats.loctik.a[0] = (int32_t)   (*((int32_t *) &cp[TVM920_STS_NOZZ_1A_LOC_BYTE_POS]));
    TVMstats.loctik.a[1] = (int32_t)   (*((int32_t *) &cp[TVM920_STS_NOZZ_2A_LOC_BYTE_POS]));
    TVMstats.loctik.a[2] = (int32_t)   (*((int32_t *) &cp[TVM920_STS_NOZZ_3A_LOC_BYTE_POS]));
    TVMstats.loctik.a[3] = (int32_t)   (*((int32_t *) &cp[TVM920_STS_NOZZ_4A_LOC_BYTE_POS]));

    /* decode nozzle A axis state */
    TVMstats.motionstate.a_state[0] =   cp[TVM920_STS_NOZZ_1A_MOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.a_state[1] =   cp[TVM920_STS_NOZZ_2A_MOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.a_state[2] =   cp[TVM920_STS_NOZZ_3A_MOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.a_state[3] =   cp[TVM920_STS_NOZZ_4A_MOVE_STATUS_BYTE_POS] ;


    /* decode NOZZLE Z axis height- but fix it + and - because two cams for four nozzles */
    int32_t z12pos =    (*((int32_t *) &cp[TVM920_STS_NOZZ_1_2Z_LOC_BYTE_POS]));
    int32_t z34pos =    (*((int32_t *) &cp[TVM920_STS_NOZZ_3_4Z_LOC_BYTE_POS]));
    if (z12pos < 0)    {
        /* Z1 is down */
        TVMstats.loctik.z[0] = z12pos;
        TVMstats.loctik.z[1] = 0;

    }
    else    {
        /* Z2 is down */
        TVMstats.loctik.z[0] = 0;
        TVMstats.loctik.z[1] = -z12pos;

    }
    if (z34pos < 0)    {
        /* Z1 is down */
        TVMstats.loctik.z[2] = z34pos;
        TVMstats.loctik.z[3] = 0;

    }
    else    {
        /* Z2 is down */
        TVMstats.loctik.z[2] = 0;
        TVMstats.loctik.z[3] = -z34pos;

    }


    /* deoce nozzle Z state */
    TVMstats.motionstate.z_state[0] =   cp[TVM920_STS_NOZZ1_2ZMOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.z_state[1] =   cp[TVM920_STS_NOZZ1_2ZMOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.z_state[2] =   cp[TVM920_STS_NOZZ3_4ZMOVE_STATUS_BYTE_POS] ;
    TVMstats.motionstate.z_state[3] =   cp[TVM920_STS_NOZZ3_4ZMOVE_STATUS_BYTE_POS] ;

    /* now convert the xyza to actual values */
    TVMstats.UpdateRealNumbers();

    TVMstats.LeaveCritical();
    if (TVMstats.Update()) PostMessage(Application->Handle,UMTVM_STATUSUPDATE,0,0);

  	return 0;

}


int  CTVMT::GetAndDecodeTVMStatus(void) {
    int result;
    uint8_t  buffer0[4]={ 0,0,0,0};
    udpt->send(buffer0,4);
    result=udpr->DoRecv();
    if (result>0) {
    	DecodeTVM920Status(result);
    	logx->Add("*");
        //logx->AddForm("\r\n%d:",result);
        //showhex(udpr->rxbuffer,result);
        return 0;
    }
    else return -1;
}




void CTVMT::SetGPIObit(int bitnumber, int set) {
     /* lookup item */
    int result;
    uint8_t buf[ TVM920_GPIO_CMD_NBYTES ];
    memset(buf,0,TVM920_GPIO_CMD_NBYTES);

    buf[0]  = set ? TVM920_GPIO_CMD_SETBITS_GPIO : TVM920_GPIO_CMD_CLRBITS_GPIO;

    /* resolve bit :  compute byte number from the last byte */
    int bytenum = (TVM920_GPIO_CMD_NBYTES - 1) - (bitnumber / BYTEBITS) ;
    int bitnum =   bitnumber %  BYTEBITS;
    buf[bytenum] = (1<<bitnum);
    udpt->send(buf,TVM920_GPIO_CMD_NBYTES);
    showhex(buf,TVM920_GPIO_CMD_NBYTES);
    result=udpr->DoRecv();
    if (result>0) {
        //logx->Add("*");
        logx->AddForm("\r\n%d:",result);
        showhex(udpr->rxbuffer,result);
    }

}

void CTVMT::SetUpLight(int state) {

    logx->AddForm("<SetUpLight%d>",state);
    SetGPIObit(TVM920_GPIO_CMD_UPLIGHT,state);

}
void CTVMT::SetDownLight(int state) {

    logx->AddForm("<SetDownLight%d>",state);
    SetGPIObit(TVM920_GPIO_CMD_DOWNLIGHT,state);

}

void CTVMT::SetPick(int num, int state) { /* reference 0..3 */

    logx->AddForm("<pick%d %d>",num,state);
    SetGPIObit(TVM920_GPIO_CMD_PICK_1ST_BIT+(num-0),state);

}

/*  feedernumber
is zero based ! */
void CTVMT::SetCLAutoFeeder(int feedernumber,int state) {
    int result;
    int bitnumber;

    /* resolve bit :  compute byte number from the last byte */



    if (feedernumber >= TVM920_FEEDERCMD_NFRONT_FEEDERS ) {
    /* rear feeders */
       bitnumber =    (feedernumber-TVM920_FEEDERCMD_NFRONT_FEEDERS) + TVM920_FEEDERCMD_1STREARFEEDER_BIT;
    }
    else {
       bitnumber =    feedernumber + TVM920_FEEDERCMD_1STFRONTFEEDER_BIT;
    }

    /* manipulate bit vector */
    FeederLatch->SetBitVectorByBit(bitnumber,state);

    /* add the command byte at the 'front'  */
    const int  NBITSCMD = 8;
    FeederLatch->SetBitVectorDWORD_at_Bitnum((TVM920_FEEDERCMD_NBYTES*8)-NBITSCMD,TVM920_FEEDERCMD,NBITSCMD) ;

   	/* now copy into sending buffer */
    FeederLatch->ResetBitVectorExt();
    uint8_t buf[ TVM920_FEEDERCMD_NBYTES ];
    memset(buf,0,TVM920_FEEDERCMD_NBYTES);
    int i =  TVM920_FEEDERCMD_NBYTES * 8;
    unsigned int bitext;
    __int64 feederBitwisestatus=0;
    while (i--) {  /* lowest bit in BV first */

        bitext =  (FeederLatch->GetBitVectorExt() );

        //logx->AddForm("%u:",bitext);
        buf[i / 8] >>= 1;/* fill MSB and shift right */
        buf[i / 8] |=  bitext<<7;  /* fill MSB */

    }

	logx->AddForm("\nFEED: ",result);
    showhex(buf,TVM920_FEEDERCMD_NBYTES);

    udpt->send(buf,TVM920_FEEDERCMD_NBYTES);

    /* update GUI status */
    if (state) {
        feederBitwisestatus |= (1LL<<feedernumber);
    }
    else {
    	feederBitwisestatus &= ~(1LL<<feedernumber);
	}
    TVMstats.UpdateFeederStatus(feederBitwisestatus);
    PostMessage(Application->Handle,UMTVM_FEEDER_STATUS_UPDATE,0,0);

	result=udpr->DoRecv();
    if (result>0) {
        logx->Add("*");

    }



}

int  CTVMT::SendStartupMotionPrefixCommand(void) {
	/* send those 68 bytes */
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
    07,00,04,00,04,00,00,00,04,00,00,00,04,00,00,00,
    04,00,00,00,05,00,00,00,05,00,00,00,04,00,00,00,
    04,00,00,00,0x70,0x17,00,00,0x70,0x17,00,00,0x70,0x17,00,00,
    0x70,0x17,00,00,0xb8,0x0b,00,00,0xb8,0x0b,00,00,0x10,0x27,00,00 ,
    0x10,0x27,00,00
    };
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();

}

int  CTVMT::SendXYFast2MotionPrefixCommand(void) {      /* 100 % */
	/* send those 68 bytes */
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
        07,00,0x09,00,05,00,00,00,05,00,00,00,05,00,00,00 ,
        05,00,00,00,0x0c,00,00,00,0x0c,00,00,00,07,00,00,00  ,
        07,00,00,00,0xe0,0x2e,00,00,0xe0,0x2e,00,00,0xe0,0x2e,00,00 ,
        0xe0,0x2e,00,00,0xc8,0x32,00,00,0xc8,0x32,00,00,0x98,0xb7,00,00  ,
        0xb0,0xb3,00,00

    };
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();

}

int  CTVMT::SendXYFast3MotionPrefixCommand(void) {  /* not really fast ?- no this is different )x11 */
	/* send those 68 bytes */
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
		0x11,00,0x09,00,05,00,00,00,05,00,00,00,05,00,00,00 ,
		05,00,00,00,0x0c,00,00,00,0x0c,00,00,00,07,00,00,00   ,
		07,00,00,00,0xe0,0x2e,00,00,0xe0,0x2e,00,00,0xe0,0x2e,00,00  ,
		0xe0,0x2e,00,00,0xc8,0x32,00,00,0xc8,0x32,00,00,0x98,0xb7,00,00  ,
		0xb0,0xb3,00,00

    };
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();

}

int  CTVMT::SendXYFastMotionPrefixCommand(void) {
	/* send those 68 bytes */
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
      07, 00, 02 ,00, 04, 00, 00, 00, 04, 00, 00, 00, 04, 00, 00, 00 ,
      04, 00, 00, 00, 04 ,00 ,00 ,00, 04, 00 ,00, 00, 04, 00, 00, 00,
      04, 00, 00, 00, 0xd0, 7, 00, 00, 0xd0, 7, 00, 00, 0xd0 ,7, 00, 00 ,
      0xd0, 7, 00, 00 ,0xe8 ,3, 00 ,00, 0xe8, 3, 00, 00, 0xc4, 9, 00, 00 ,
      0xc4, 9, 00, 00
    };
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();

}
int  CTVMT::SendXYSlowMotionPrefixCommand(void) {
	/* slower  */
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
       07, 00, 00, 00, 03, 00, 00, 00, 03, 00, 00, 00, 03, 00 ,00 ,00  ,
        03, 00 ,00, 00, 03, 00, 00, 00, 03, 00, 00, 00, 04, 00 ,00, 00 ,
        04, 00, 00 ,00 ,0xf4, 0x01, 00 ,00, 0xf4, 0x01 ,00, 00 ,0xf4 ,0x01, 00, 00 ,
        0xf4, 0x01 ,00 ,00, 0x64, 00, 00, 00, 0x64, 00, 00, 00, 0x64 ,0 ,00, 00    ,
        0x64, 0x00, 00, 00
    } ;
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();


}

int  CTVMT::SendnozzleSlowAZmotionPrefixCommand(void) {
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
       07, 00, 00, 00, 03, 00, 00, 00, 03, 00, 00, 00, 03, 00 ,00 ,00  ,
        03, 00 ,00, 00, 03, 00, 00, 00, 03, 00, 00, 00, 04, 00 ,00, 00 ,
        04, 00, 00 ,00 ,0xf4, 0x01, 00 ,00, 0xf4, 0x01 ,00, 00 ,0xf4 ,0x01, 00, 00 ,
        0xf4, 0x01 ,00 ,00, 0x64, 00, 00, 00, 0x64, 00, 00, 00, 0x64 ,0 ,00, 00    ,
        0x64, 0x00, 00, 00
    } ;
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();


}



int  CTVMT::SendnozzleFastAZmotionPrefixCommand(void) {
    uint8_t buf[TVM920_MOTION_PREFIX_NBYTES] = {
       07, 00, 03, 00, 04, 00, 00, 00, 04, 00, 00, 00, 04, 00 ,00 ,00  ,
        04, 00 ,00, 00, 04, 00, 00, 00, 04, 00, 00, 00, 04, 00 ,00, 00 ,
        04, 00, 00 ,00 ,0xa0, 0x0f, 00 ,00, 0xa0, 0x0f ,00, 00 ,0xa0 ,0x0f, 00, 00 ,
        0xa0, 0x0f ,00 ,00, 0xd0, 07, 00, 00, 0xd0, 07, 00, 00, 0xa0 ,0x0f ,00, 00    ,
        0xa0, 0x0f, 00, 00
    };
    udpt->send(buf,TVM920_MOTION_PREFIX_NBYTES);
    return WaitForTVMResponse();


}




int  CTVMT::SendXYZAabsmotionCommand(bool IsSetLocationCommand, uint32_t axis_bitwise,
	 Cmachloc<int32_t> & m) {
/*
  	if param = IsSetLocationCommand = true

    this SET location occurs instead

    that is,, the specified XYZ data forces the controller to update its location for that axis with this number

    used after homing.....

*/
    uint8_t buf[TVM920_MOTION_NBYTES];
    memset((void*) buf, 0,TVM920_MOTION_NBYTES);





    if (IsSetLocationCommand) {
    	buf[TVM920_MOTION_CMDBYTE_BYTEPOS] = TVM920_MOTION_SETPOS_CMDBYTE;
    }
    else {
    	buf[TVM920_MOTION_CMDBYTE_BYTEPOS] = TVM920_MOTION_CMDBYTE;

        /* trajectory update for GUI display */
        TVMstats.trajectory_start  = TVMstats.locdub;
        //Cmachloc<double> trajectoryEnddub(TVM920_NNOZZ);
        TVMinfo.xlateTomm(m,TVMstats.trajectory_end);
        PostMessage(Application->Handle,UMTVM_TRAJECTORY_UPDATE,0,0);

    }



    uint32_t ctlbyte=0 ;

    /* now translate control byte of axis to machine bitwise

    since Z1+Z2 and Z3 + Z4 are combined onto same ctl */


    axis_bitwise &= globs.AxisMotionMute; /* GUI axis mute */

    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_A1_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_A1_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_A2_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_A2_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_A3_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_A3_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_A4_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_A4_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_X_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_X_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_Y_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_Y_BIT;

    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_Z1_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_Z1_2_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_Z2_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_Z1_2_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_Z3_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_Z3_4_BIT;
    if (axis_bitwise & (1<<TVMCMD_MOVEXYZA_Z4_BIT))  ctlbyte|=1<<TVM920_MOTION_CTLBYTE_Z3_4_BIT;


    buf[TVM920_MOTION_CTLBYTE_BYTEPOS] = ctlbyte;    /* 8 bit quantity with combined Z1 and Z2 */
    /* now place the values in */
    *((int32_t * ) &buf[TVM920_MOTION_X_BYTEPOS]) = m.x;
    *((int32_t * ) &buf[TVM920_MOTION_Y_BYTEPOS] )= m.y;

    /* comvert nozzle said as they share a cam */

    /* expect NEGATIVE numbers for plunge- zero is all the way up */


	if ( m.z[0] < 0)   *((int32_t * ) &buf[TVM920_MOTION_Z1_2_BYTEPOS])= m.z[0];
    else  if ( m.z[1] < 0)   *((int32_t * ) &buf[TVM920_MOTION_Z1_2_BYTEPOS]) = -m.z[1];

	if ( m.z[2] < 0)   *((int32_t * ) &buf[TVM920_MOTION_Z3_4_BYTEPOS])= m.z[2];
    else  if ( m.z[3] < 0)   *((int32_t * ) &buf[TVM920_MOTION_Z3_4_BYTEPOS]) = -m.z[3];

    *((int32_t * ) &buf[TVM920_MOTION_A1_BYTEPOS]) = m.a[0];
    *((int32_t * ) &buf[TVM920_MOTION_A2_BYTEPOS]) = m.a[1];
    *((int32_t * ) &buf[TVM920_MOTION_A3_BYTEPOS]) = m.a[2];
    *((int32_t * ) &buf[TVM920_MOTION_A4_BYTEPOS]) = m.a[3];

    logx->AddForm("<ct=%02X>",ctlbyte);
    udpt->send(buf,TVM920_MOTION_NBYTES);
    showhex(buf,TVM920_MOTION_NBYTES);

    WaitForTVMResponse();


    return 0;

}



int  CTVMT::TVMSendFinishMotion(uint32_t eventbits) {
    uint8_t buf[TVM920_MOTION_END_NBYTES] = {
        TVM920_MOTION_END_CMDBYTE,
        00,
        00,
        00};

    uint32_t ctlbyte  = eventbits;/* bits are already in the right format for TVM920 */

    buf[TVM920_MOTION_END_INFO_BYTEPOS] = ctlbyte;
    udpt->send(buf,TVM920_MOTION_END_NBYTES);
    WaitForTVMResponse();


    return 0;
}



/* use IsSetPos == true if setting position, not a move command */
int  CTVMT::TVMMoveSetAbsXYZA(bool IsSetPos, uint8_t * cb) {

	/* first send motion start command */
    uint32_t info =   *((uint32_t * ) &cb[TVMCMD_MOVEXYZA_INFO_BYTEPOS]);

    if (info==0) SendXYSlowMotionPrefixCommand();
    else  if (info==1) SendXYFastMotionPrefixCommand();
    else  if (info==2) SendXYFast2MotionPrefixCommand();
    else SendXYSlowMotionPrefixCommand();

    Sleep(2);
    //showhex(cb,64);
    Cmachloc<int32_t> m(TVM920_NNOZZ);

    m.x =   *((int32_t * ) &cb[TVMCMD_MOVEXYZA_X_BYTEPOS]);
    m.y =   *((int32_t* ) &cb[TVMCMD_MOVEXYZA_Y_BYTEPOS]);
    m.z[0] =  *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_Z1_BYTEPOS]);
    m.z[1] =  *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_Z2_BYTEPOS]);
    m.z[2] =  *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_Z3_BYTEPOS]);
    m.z[3] =  *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_Z4_BYTEPOS]);

    m.a[0] =   *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_A1_BYTEPOS]);
    m.a[1] =   *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_A2_BYTEPOS]);
    m.a[2] =   *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_A3_BYTEPOS]);
    m.a[3] =   *((int32_t* ) &cb[TVMCMD_MOVEXYZA_NOZZ_A4_BYTEPOS]);

    SendXYZAabsmotionCommand(IsSetPos, *((uint32_t * ) &cb[TVMCMD_MOVEXYZA_AXIS_BYTEPOS]),m);
    Sleep(2);

    return 0;

}



int  CTVMT::TVMSetEnd_Stop_MotionEnables(uint8_t * cb) {



	uint32_t axisbitwise = *( (uint32_t * ) &cb[TVMCMD_DATASTART_BYTEPOS+0]);
    bool disable = *((uint32_t *) & cb[TVMCMD_DATASTART_BYTEPOS+4]);
	/*   axisbitwise parameter is same bits as TVM920_MOTION_CTLBYTE_BYTEPOS */
    uint32_t addr=0;
    Sleep(2);
    /* one at a time */
    if ( axisbitwise & (1<<TVMCMD_MOVEXYZA_X_BIT) ) {
    	addr = TVM920_RW_REG_ENDSTOPDEFEAT_X;
    }
    else if ( axisbitwise & (1<<TVMCMD_MOVEXYZA_Y_BIT) ) {

    	addr = TVM920_RW_REG_ENDSTOPDEFEAT_Y;
    }
    else return -1;

	WriteData(addr,disable ? TVM920_RW_REG_ENDSTOPDEFEAT_VALUE : 0x0);

    Sleep(2);

    return 0;

}

int CTVMT::DecodeAndExecuteCommand(void * v) {
	/* first three bytes have TVM920 */
    /* next byte is version */

    /* next byte is the command */
    /* next byte is the sub command */
    uint8_t * cb = (uint8_t* ) v;
    if (strncmp(cb,"TVM",3)) return -1;
    if (cb[TVMCMD_VERSION_BYTEPOS]!=0) return -2;

    int command =cb[TVMCMD_CMD_BYTEPOS];
    int subcommand =cb[TVMCMD_SUBCMD_BYTEPOS];
	switch (command) {
        case TVMCMD_GPIOCMD_UPLAMP :
        	SetUpLight(cb[TVMCMD_DATASTART_BYTEPOS]);
        	break;
        case TVMCMD_GPIOCMD_DOWNLAMP :
        	SetDownLight(cb[TVMCMD_DATASTART_BYTEPOS]);
        	break;
        case TVMCMD_GPIOCMD_PICK :
        	SetPick(cb[TVMCMD_DATASTART_BYTEPOS+0],cb[TVMCMD_DATASTART_BYTEPOS+1]);
        	break;
        case TVMCMD_GPIOCMD_CLAUTOFEEDERCTL :
        	SetCLAutoFeeder(cb[TVMCMD_DATASTART_BYTEPOS+0],cb[TVMCMD_DATASTART_BYTEPOS+4]);
            break;
        case TVMCMD_MOVESETABSXYZA :
        	TVMMoveSetAbsXYZA(false,cb);
        	break;
        case TVMCMD_SETXYZA :
        	TVMMoveSetAbsXYZA(true,cb);
        	break;
        case TVMCMD_MOTIONZEROMOTION :
        	TVMSendFinishMotion(~0);
        	break;
        case TVMCMD_SET_DEFEAT_END_STOPS :
        	TVMSetEnd_Stop_MotionEnables(cb);
            break;
         case TVMCMD_GPIOCMD_SRBIT :/* set and reset a gpio bit */
         	SetGPIObit(cb[TVMCMD_DATASTART_BYTEPOS]+0,cb[TVMCMD_DATASTART_BYTEPOS+1]);
        	break;
         case TVMCMD_READREGISTER :
            ConReadRegister(cb);
         	break;
         case TVMCMD_WRITEREGISTER :
            ConWriteRegister(cb);
         	break;
         case TVMCMD_GOHOME :
         	PostMessage(Application->Handle,UMTVM_GOHOME,0,0);
         	break;
         case TVMCMD_MOVEZBIPOLAR :
         	// not implemented TVMMoveZbipolar(cb);
            break;

        default :
        	logx->Add("<cmd not known>");
        	break;
    }

    return 0;



}

int CTVMT::ConReadRegister(uint8_t * cb) {

	int32_t data=0;
    uint32_t reg =   *((uint32_t *) & cb[TVMCMD_DATASTART_BYTEPOS]);
	int result = ReadData(reg,&data);
    char s[128];
    sprintf(s,"\r\nR@:%04x : %02x:%02x:%02x:%02x : s= %d, u= %u",reg,(data>>24)&0xff,(data>>16)&0xff, (data>>8)& 0xff,
    	(data>>0) & 0xff,
    	data,data);

    /* naughty !!!! BAD -write into main form thread !!! */
    Form1->LM(s);    /* naughty !!!! BAD -write into main form thread !!! */

    return 0;
}

int CTVMT::ConWriteRegister(uint8_t * cb) {


    uint32_t reg =   *((uint32_t *) & cb[TVMCMD_DATASTART_BYTEPOS]);
	int32_t data=*((int32_t *) & cb[TVMCMD_DATASTART_BYTEPOS+4]);
	int result = WriteData(reg,data);
    return 0;
}


int CTVMT::WriteData(unsigned int regaddr, int  data)  {
    uint8_t  b[TVM920_RW_CMD_NBYTES]={0,0,0,0,0,0,0,0,0,0,0,0};
    b[TVM920_RW_READ_CMD_BYTE_BYTEPOS] = TVM920_RW_WRITE_CMD_BYTE;
    b[TVM920_RW_BYTE_LOWADDR_BYTEPOS] = regaddr& 0xff;
    b[TVM920_RW_BYTE_HIGHADDR_BYTEPOS] = (regaddr>>8)& 0xff;  /* address ittle endian */

    *((int32_t *) & b[TVM920_RW_BYTE_DATA_BYTEPOS]) = data;

	udpt->send(b,TVM920_RW_CMD_NBYTES);
    showhex(b,TVM920_RW_CMD_NBYTES);
    int rresult= WaitForTVMResponse();
    return rresult;
}


int CTVMT::ReadData(unsigned int regaddr, int * data)  {
    uint8_t  b[TVM920_RW_CMD_NBYTES]={0,0,0,0,0,0,0,0,0,0,0,0};
    b[TVM920_RW_READ_CMD_BYTE_BYTEPOS] = TVM920_RW_READ_CMD_BYTE;
    b[TVM920_RW_BYTE_LOWADDR_BYTEPOS] = regaddr& 0xff;
    b[TVM920_RW_BYTE_HIGHADDR_BYTEPOS] = (regaddr>>8)& 0xff;
	udpt->send(b,TVM920_RW_CMD_NBYTES);
    int rresult= WaitForTVMResponse();

	if (rresult > 0) {
    	/* TO DO , deal with the 0x11 that appears in byte 8 */
        *data = *((int32_t *) & udpr->rxbuffer[TVM920_RW_BYTE_DATA_BYTEPOS]);

    }
    return rresult;
}

int CTVMT::ReadDataAndShow(unsigned int reg) {
    char s[128];
    int data;
    ReadData(reg, &data);
    sprintf(s,"\r\nR@:%04x : %02x:%02x:%02x:%02x : s= %d, u= %u",reg,(data>>24)&0xff,(data>>16)&0xff, (data>>8)& 0xff,
    	(data>>0) & 0xff,
    	data,data);

    /* naughty !!!! BAD -write into main form thread !!! */
    Form1->LM(s);    /* naughty !!!! BAD -write into main form thread !!! */

    logx->Add(s);
    return  0;

}



int CTVMT::GetAllStartupVars(void) {


	int varstart = 0;
    int varstop =  0x2f;


    const int pacingmS = TVM_POLL_RATE_mS;
    Sleep(pacingmS);
    for (int reg = varstart ; reg <= varstop; reg++) {
        ReadDataAndShow(reg);
        Sleep(pacingmS);

    }
    logx->Add("\r\n");
    return 0;

}
int CTVMT::InitTVMstuff(void) {
	int result;
    uint8_t  buffer0[4]={ 0,0,0,0};
    uint8_t  buffer1[8]={0x17,0,0,0,0x10,0,0,0};
    uint8_t  buffer2[12]={2,0,0,0,0xf5,1,0,0,0,0,0,0};   /* end stop overtun permission denied  (?) */
    uint8_t  buffer3[12]={2,0,0,0,0xf6,1,0,0,0,0,0,0};   /* end stop overtun permission denied  (?) */

  	GetAndDecodeTVMStatus();

    Sleep(20);
	udpt->send(buffer1,8);
	WaitForTVMResponse();

    Sleep(20);
	udpt->send(buffer2,12);
	WaitForTVMResponse();

    Sleep(20);
	udpt->send(buffer3,12);
	WaitForTVMResponse();

    Sleep(20);
    SendStartupMotionPrefixCommand();

    /* get all variables */
    GetAllStartupVars();

    Sleep(TVM_POLL_RATE_mS);

    uint8_t  buffer4[4]={ 0xc,0,0,0};   /* what is a $0C command ? */
	udpt->send(buffer4,4);
	WaitForTVMResponse();
    Sleep(20);

    /* sends BITS OFF to  TVM920_GPIO_CMD_UNKNOWN1 and TVM920_GPIO_CMD_UNKNOWN2 */
     /* what is this for - I think it is something about prevent overrun of end stops? */
    uint8_t  buffer5[8]={ 0x17, 00, 00, 00, 0x30 ,00, 00, 00};
	udpt->send(buffer5,8);
	WaitForTVMResponse();

    Sleep(20);
	udpt->send(buffer2,12);
	WaitForTVMResponse();

    Sleep(20);
	udpt->send(buffer3,12);
	WaitForTVMResponse();

  	GetAndDecodeTVMStatus();
    return 0;
}

int CTVMT::ProcessMotionStatus(void) {
/*
	when motion has stopped, we issue a stop-motion command
    We detect when an axis goes from 2 >> 3
    If the axis has been told to move again while it is still moving,
    there will be a motion status supression order issued   so that we dont issue a stop immediately after a new motion command is issued...

*/
	/* extract motion from sttaus packet- status packet is deemed to be still in the rxbuffer of the udp class.
    in fact, it is already done...by GetAndDecodeTVMStatus() */

	/* if change from 3>>0 then issue motion stop */
    /* generate event mask with same bit values TVM920_MOTION_END_INFO_BYTEPOS */

	return TVMSendFinishMotion(TVMstats.MotionStateEndEventRegister);



}
void CTVMT::sktTXthreadmachine(void) {

	int result;
    int startuperrors=0;
    FeederLatch->ClearBits();
    #define TVMINQ_NWAITOBJECTS 2
    HANDLE  waitobjectHandles[TVMINQ_NWAITOBJECTS];
	void * v;

    /* set up the multiple object wait  */

    enum {TVMINQ_NWAITOBJECTS_opnpq,TVMINQ_NWAITOBJECTS_guiq };
    waitobjectHandles[TVMINQ_NWAITOBJECTS_opnpq] = opnpsvr.inboundq->rsema;
    waitobjectHandles[TVMINQ_NWAITOBJECTS_guiq] = GUIcmdQueue.rsema;

    Form1->LM(" TVMT_sktTXthread start ");


    /* initialize UDP sender */

    udpr = new CUDPlistener(TVM_UDP_PORT,TVM_MASTER_IPADDR,3,0,&startuperrors);
    if (startuperrors!=0) throw("sh1t!");
    udpt = new CUDPsender(TVM_MCU_IPADDR,TVM_UDP_PORT);
    Sleep(200);


    InitTVMstuff();
    PostMessage(Application->Handle,UMTVM_INITIALIZE_DONE,0,0);



	while (sktTXthread_terminate==0) {
    	result = WaitForMultipleObjects(TVMINQ_NWAITOBJECTS,waitobjectHandles,
        	false ,TVM_POLL_RATE_mS);
        if (result == WAIT_OBJECT_0 ) {
            v=opnpsvr.inboundq->GetNewAvailable();
            if (v!=NULL) {
                /* we got a command to send */
                DecodeAndExecuteCommand(v);
                 opnpsvr.inboundq->ReleaseQueue();
            }

        }
        else if (result == WAIT_OBJECT_0+1) {
            v=GUIcmdQueue.GetNewAvailable();
            if (v!=NULL) {
                /* we got a command to send */
                DecodeAndExecuteCommand(v);
                 GUIcmdQueue.ReleaseQueue();
            }
        }
        else if (result== WAIT_TIMEOUT) {
        	if (GetAndDecodeTVMStatus()==0) {
                /* if there has been an END event in motion, tell machine, done... */
            	if (TVMstats.MotionStateEndEventRegister) ProcessMotionStatus();
            }

        }
        else {
            /* wait abandoned */
            sktTXthread_terminate = true;/* something died */
        }

    }
    logx->Add("TVMT_terminated !");
    delete  udpt;
    udpt=NULL;
    delete udpr;
    udpr=NULL;
    logx->Add("TVMT_sktTXthread_exit");



}

void CTVMT::sktTXthreadproc(void * v) {

	CTVMT * ptr = (CTVMT *) v;
    ptr->sktTXthreadmachine();

}



void  CTVMT::sktRXthreadmachine(void) {
    Form1->LM(" TVMT_sktRXthread start ");





	logx->Add("recv done");
	while (!sktRXthread_terminate) {
     	Sleep(188);

    }

    logx->Add("TVMT_sktRXthread_exit");
}

void  CTVMT::sktRXthreadproc(void * v) {

	CTVMT * ptr = (CTVMT *) v;
    ptr->sktRXthreadmachine();
}

int  CTVMT::Start(void) {

    sktTXthread_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) sktTXthreadproc,(this),0,&sktTXthread_id);
	//sktRXthread_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) sktRXthreadproc,(this),0,&sktRXthread_id);
  	return 0;
}

int  CTVMT::End(void) {

	if ( sktTXthread_handle!=INVALID_HANDLE_VALUE) {
    	Form1->LM(" EndTVMTX... ");
		sktTXthread_terminate=true;
       	WaitForSingleObject( sktTXthread_handle,INFINITE);
        sktTXthread_handle=INVALID_HANDLE_VALUE ;
        Form1->LM(" EndedTVMTX... ");

    }

	if ( sktRXthread_handle!=INVALID_HANDLE_VALUE) {
    	Form1->LM(" EndTVMRX... ");
		sktRXthread_terminate=true;
    	WaitForSingleObject( sktRXthread_handle,3000);
        sktRXthread_handle=INVALID_HANDLE_VALUE ;
        Form1->LM(" EndedTVMRX... ");
    }
    return 0;

}
