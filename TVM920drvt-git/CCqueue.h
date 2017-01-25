//---------------------------------------------------------------------------

#ifndef CCqueueH
#define CCqueueH
//---------------------------------------------------------------------------

#include <windows.h>
class CCQueue  {
public :
    void ** qobjects;
    int queuewriteindex;
    int queuereadindex;
    int queueNobjects;
    HANDLE rsema;
    HANDLE wsema;
public :
    CCQueue(int nobjects);
    ~CCQueue();
    int WriteToQueue(void * obj, int ms);
    int ReleaseQueue(void);
    void *  WaitWithTO(int ms);
    void *  GetNewAvailable(void);


};


#endif
