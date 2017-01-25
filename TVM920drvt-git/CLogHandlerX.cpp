#include "CLogHandlerX.h"
//handler for the logger


void CLogHandlerX::Add(char * s) {
    unsigned long conwritten=0;

    if (Hcon==NULL) return;
    EnterCriticalSection(&cs);
    WriteConsole(Hcon,s,strlen(s),&conwritten,0);
    LeaveCriticalSection(&cs);
}
void CLogHandlerX::AddCR(char * s) {
    unsigned long conwritten=0;
    if (Hcon==NULL) return;
    strcat(s,"\r\n");
    WriteConsole(Hcon,s,strlen(s),&conwritten,0);
}
void CLogHandlerX::Add(char * s1,unsigned int escalationvalue, char * s2) {
    unsigned long conwritten=0;
    if (escalationvalue<MinErrorLevelToDisplay) return ;

    if (Hcon==NULL) return;
    WriteConsole(Hcon,s1,strlen(s1),&conwritten,0);
    WriteConsole(Hcon,s2,strlen(s2),&conwritten,0);
}
void CLogHandlerX::Add(char * s,unsigned escalationvalue) {

    if (escalationvalue<MinErrorLevelToDisplay) return ;
    unsigned long conwritten=0;
    if (Hcon==NULL) return;
    WriteConsole(Hcon,s,strlen(s),&conwritten,0);
}

void CLogHandlerX::AddTime(void) {
    SYSTEMTIME st;
    char s[16];
    unsigned long conwritten=0;
    GetSystemTime(&st);
    sprintf(s,"<%02u:%02u:%02u>",
        (int)st.wHour,(int)st.wMinute,(int)st.wSecond);
    WriteConsole(Hcon,s,strlen(s),&conwritten,0);



}

void CLogHandlerX::AddForm(char * format, ...) {
    char s[256];
    unsigned long conwritten=0;
    va_list args;
    va_start (args, format);
    vsprintf (s,format, args);
    va_end (args);
    if (Hcon==NULL) return;
    WriteConsole(Hcon,s,strlen(s),&conwritten,0);
}
