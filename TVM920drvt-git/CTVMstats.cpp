#include "CTVMstats.h"
#include "tvmt.h"
void CTVMstats::UpdateRealNumbers(void) {
    /* using scale factors, update the local class object locdub */
    TVMinfo.xlateTomm(loctik,locdub);

}
void CTVMstats::UpdateFeederStatus(__int64 feeders) {
	FeederActuators = feeders;
}
__int64 CTVMstats::ReadFeederStatus(void) {
	return FeederActuators;
}

bool CTVMstats::CompareWithGuard(Cmachloc<double> & t1,Cmachloc<double> & t2) {
    bool result =false;
    EnterCritical();
    result = (t1==t2);

    LeaveCritical();
    return result;
}
int CTVMstats::Update(void) {   /* return NZ if something has changed */
    int changed = false;
    /* detect changes */
    /* clear motion state bitwise flags */
    MotionStateEventRegister=0;       /* clear motion change bits on entry */
    MotionStateEndEventRegister = 0;

    if ((*(uint32_t *) &stats1) != (*(uint32_t *) & stats1_zm1))  changed=true;

    //logx->AddForm(" z[0]=%d zz0=%d ",loctik.z[0],loctik_zm1.z[0]);
    if (loctik.ChkEqual(loctik_zm1)==false) {
        changed=true;

        /* set the motion change bits */
        if (motionstate.xstate != motionstate_zm1.xstate) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_X_BIT);
            MotionStateEndEventRegister |= ((motionstate.xstate==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_X_BIT;
        }
        if (motionstate.ystate != motionstate_zm1.ystate) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_Y_BIT);
            MotionStateEndEventRegister |= ((motionstate.ystate==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_Y_BIT;
        }
        if (motionstate.a_state[0] != motionstate_zm1.a_state[0]) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_A1_BIT);
            MotionStateEndEventRegister |= ((motionstate.a_state[0]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_A1_BIT;
        }
        if (motionstate.a_state[1] != motionstate_zm1.a_state[1]) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_A2_BIT);
            MotionStateEndEventRegister |= ((motionstate.a_state[1]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_A2_BIT;
        }
        if (motionstate.a_state[2] != motionstate_zm1.a_state[2]) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_A3_BIT);
            MotionStateEndEventRegister |= ((motionstate.a_state[2]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_A3_BIT;
        }
        if (motionstate.a_state[3] != motionstate_zm1.a_state[3]) {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_A4_BIT);
            MotionStateEndEventRegister |= ((motionstate.a_state[3]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_A4_BIT;
        }
        if ((motionstate.z_state[0] != motionstate_zm1.z_state[0])||
           (motionstate.z_state[1] != motionstate_zm1.z_state[1]))	 {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_Z1_2_BIT);
            MotionStateEndEventRegister |= ((motionstate.z_state[0]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_Z1_2_BIT;
            MotionStateEndEventRegister |= ((motionstate.z_state[1]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_Z1_2_BIT;
        }
        if ((motionstate.z_state[2] != motionstate_zm1.z_state[2])||
           (motionstate.z_state[3] != motionstate_zm1.z_state[3]))	 {
            MotionStateEventRegister |= (1<<TVM920_MOTION_END_BYTE2_Z3_4_BIT);
            MotionStateEndEventRegister |= ((motionstate.z_state[2]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_Z3_4_BIT;
            MotionStateEndEventRegister |= ((motionstate.z_state[3]==0) ? 1 : 0 )<<TVM920_MOTION_END_BYTE2_Z3_4_BIT;
        }

        if (MotionStateEndEventRegister) {
            MotionChangeSticky_OPNP=true;/* OPNP must reset this otherwise info gone next update */
        }
    }

    if (memcmp( (void *) &motionstate,(void *) &motionstate_zm1, sizeof(motionstate)))  changed=true;


    /* update "last" values */
    loctik_zm1=loctik;
    stats1_zm1 = stats1;
    motionstate_zm1 = motionstate;
    return changed;
}


