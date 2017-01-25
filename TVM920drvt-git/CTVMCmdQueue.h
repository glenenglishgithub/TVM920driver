/* CTVMCmdQueue.h */
#ifndef CTVMCMDQUEUEH
#define  CTVMCMDQUEUEH

#include "CCqueue.h"
#include "Cmachloc.h"
#include <stdint.h>
class CTVMCmdQueue : public CCQueue {
public :
    #define TVMcmdbuffers_NBYTES 128
    uint8_t ** cbufs;
    unsigned int Nobjects;


public :
	CTVMCmdQueue(unsigned int Nobj) :   CCQueue(Nobj) {
    	cbufs = new  uint8_t * [ Nobj ];

    	for (int i=0;i<Nobj;i++) {
            cbufs[i] = new uint8_t[TVMcmdbuffers_NBYTES];
            memset(cbufs[i],0,TVMcmdbuffers_NBYTES);
        }


        Nobjects = Nobj;
    }
    ~CTVMCmdQueue(){
    	for (int i=0;i<Nobjects;i++) {
            delete cbufs[i] ;
            cbufs[i] = NULL;
        }
        delete cbufs;
    }
public :
	int MoveXYZA(Cmachloc<double> & locf, uint32_t axismask, int speed);
    int MoveXYZA(Cmachloc<int32_t>   & loc, uint32_t axismask, int speed);
    int FeederCtl(uint32_t feederindexZeroBased, uint32_t isopenair);
    int NozzleVacCtl(uint32_t nozzleZeroBased, uint32_t VacOn);
    int CTVMCmdQueue::GoHome(void);
    int CTVMCmdQueue::LightingCtl(uint32_t lightindex, uint32_t state);
    int CTVMCmdQueue::MoveZBIPOLAR(Cmachloc<int32_t>   & loc, uint32_t axismask, int speed);/* for homing */
public :
    void * GetPtrToNextBuffer(void) {
        return (void *)  cbufs[queuewriteindex];
    }
    void * GetPtrToNextBufferNice(int command, int subcommand) {
    	uint8_t * p = (uint8_t *)  GetPtrToNextBuffer();
        p[0]= 'T'; p[1] = 'V'; p[2] = 'M' ; p[3] = 0;
        p[4] = command;
        p[5] = subcommand;
        return (void *) p;

    }
    void Post(int timeout) {
		WriteToQueue(cbufs[queuewriteindex],timeout);

    }


};

#endif


