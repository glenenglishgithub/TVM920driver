//---------------------------------------------------------------------------

#pragma hdrstop

#include "CCqueue.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)



CCQueue::CCQueue(int nobjects) {
     queuewriteindex=0;
     queuereadindex = 0;
     queueNobjects = nobjects;
     qobjects = new void * [nobjects];
     wsema=CreateSemaphore(NULL,nobjects,nobjects,0);
     rsema=CreateSemaphore(NULL,0,nobjects,0);
}

CCQueue::~CCQueue() {
	delete  [] qobjects;
    CloseHandle(rsema);
    CloseHandle(wsema);
}


int CCQueue::WriteToQueue(void * obj, int ms) {
	long count;
    DWORD wait= WaitForSingleObject(wsema,ms);  /* wait for queue ready to take */
    switch (wait) {
        case  WAIT_ABANDONED :
        case  WAIT_FAILED :
        	return -1;

        case  WAIT_OBJECT_0 :
        	/* signalled remove from the queue */
			qobjects[queuewriteindex++]= obj;
            queuewriteindex &= queueNobjects-1;
            ReleaseSemaphore(rsema,1,&count);	/* permit q to advance */
            return count;

        case  WAIT_TIMEOUT :
        	return NULL;
		default:
        	break;

    }
    return NULL;
}

int CCQueue::ReleaseQueue(void) {

	long count;
	ReleaseSemaphore(wsema,1,&count);	/* permit q to advance */
    return count;
}

void *  CCQueue::GetNewAvailable(void) {
	/* assumption is we know there is one available by semaphore wait */
    void * v = qobjects[queuereadindex++];
    queuereadindex &= queueNobjects-1;
    return v;
}

void *  CCQueue::WaitWithTO(int ms) {
	/* wait on the queue for max time */
	void * v;
    DWORD wait= WaitForSingleObject(rsema,ms);
    switch (wait) {
        case  WAIT_ABANDONED :
        case  WAIT_FAILED :
        	return NULL;

        case  WAIT_OBJECT_0 :
        	/* signalled remove from the queue */

            v = qobjects[queuereadindex++];
            queuereadindex &= queueNobjects-1;
            return v;

        case  WAIT_TIMEOUT :
        	return NULL;

		default:
        	break;
    }
    return NULL;

}

