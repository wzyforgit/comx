#ifndef COMX_H
#define COMX_H

#include<windows.h>

#define NAME 1
#define PSWD 2
#define UART 3
#define AT   4

extern volatile BOOL fExit;
extern const DWORD BAUD[];
extern const char SectionEnd;

extern BOOL OpenCom(HANDLE *Hcom, char *ComName, DWORD DesAcc, DWORD Flags);
extern BOOL SetBufAndTimeOut(HANDLE Hcom);
extern BOOL SetDcb(HANDLE Hcom, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
extern HANDLE HyperTerminalInit(void);
extern HANDLE ReceivePictureInit(void);
extern HANDLE Set05ATInit(void);
extern HANDLE Set06ATInit(void);

#define CheckComHandle(Hcom) {if (Hcom == NULL)return;}
extern unsigned int WINAPI WriteInfo(PVOID Hcom);
extern unsigned int WINAPI ReadInfo(PVOID Hcom);
extern unsigned int WINAPI ExitMonitor(PVOID PM);
extern void Set05AT(void);
extern void Set06AT(void);
extern void ExPic(void);
extern void PIDtest(void);

#endif