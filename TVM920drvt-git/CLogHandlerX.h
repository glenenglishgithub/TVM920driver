//CLogHandlerX.h

#ifndef CLOGHANDLERXH
#define CLOGHANDLERXH

#include <windows.h>
#include <stdio.h>

class CLogHandlerX {
private:
	HANDLE Hcon;
    unsigned MinErrorLevelToDisplay;
    CRITICAL_SECTION cs;
public:
	CLogHandlerX(char * filename, unsigned debugconsole,unsigned displaylevel){
    	//start console if there
        InitializeCriticalSection(&cs);
        MinErrorLevelToDisplay=displaylevel;
        if(debugconsole) {
	    	AllocConsole();
		    Hcon=GetStdHandle(STD_OUTPUT_HANDLE);
        }
        else Hcon==NULL;
    }
    ~CLogHandlerX(){
		if (Hcon!=NULL) FreeConsole();
        DeleteCriticalSection(&cs);
    }
    void Add(char * s);
    void AddCR(char * s);
    void Add(char * s1,unsigned int escalationvalue, char * s2);
    void Add(char * s,unsigned escalationvalue);
    void AddForm(char * format, ...);
    void AddTime(void) ;
};

#endif
