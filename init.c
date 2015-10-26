#include"comx.h"

BOOL OpenCom(HANDLE *Hcom, char *ComName,DWORD DesAcc,DWORD Flags)
{
	*Hcom = CreateFileA(
		ComName,//串口逻辑名
		DesAcc,//访问类型,GENERIC_READ为可读,GENERIC_WRITE为可写
		0,//是否可以共享,因为串口不可共享,必须为0
		NULL,//引用安全属性结构,缺省为NULL
		OPEN_EXISTING,//创建标志,对串口必须为OPEN_EXISTING(打开而不是创建)
		Flags,//是否进行异步操作,0为同步,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED为异步
		NULL//选用模板文件,对串口必须为NULL
		);
	if (*Hcom == (HANDLE)-1)
	{
		printf("打开串口失败!\n该串口不存在或可能正在被占用\n");
		return FALSE;
	}
	return TRUE;
}

BOOL SetBufAndTimeOut(HANDLE Hcom)
{
	/*配置串口*/
	//设置I/O缓冲区(根据通信速率调整)
	if (SetupComm(Hcom,//设备句柄
		1024,//输入缓冲区大小(字节)
		1024 //输出缓冲区大小(字节)
		) == FALSE)
	{
		printf("I/O缓冲区设置失败\n");
		return FALSE;
	}
	//定义设置超时的结构体  注:超时的作用是若在设置的超时时间内未完成读/写操作,读/写操作仍会结束
	COMMTIMEOUTS TimeOuts;
	/*
	1.读/写总超时=系数*字符数+常量
	2.系数与常量均为0则表示不使用该超时
	3.若读间隔超时为MAXDWORD且读系数和常量均为0,那么读入后就会立即返回,不管是否读入要求的字符
	4.异步通信时,读/写可能会在完成操作前返回,此时超时规定的是操作的完成时间
	5.时间单位均为ms
	*/
	//设定读超时
	TimeOuts.ReadIntervalTimeout = MAXDWORD;//读间隔超时
	TimeOuts.ReadTotalTimeoutMultiplier = 1;//读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 10;//读时间常量
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 50;//写时间系数
	TimeOuts.WriteTotalTimeoutConstant = 1000;//写时间常量
	//写入超时设置,读取当前超时设置使用GetCommTimeouts()
	if (SetCommTimeouts(Hcom, &TimeOuts) == FALSE)
	{
		printf("写入超时设置失败\n");
		return FALSE;
	}
	return TRUE;
}

BOOL SetDcb(HANDLE Hcom, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	//传输设置
	DCB dcb;
	//获取当前DCB信息
	if (GetCommState(Hcom, &dcb) == FALSE)
	{
		printf("获取串口初始DCB信息失败\n");
		return FALSE;
	}
	dcb.BaudRate = BaudRate;//设置波特率,格式为CBR_xxx
	dcb.ByteSize = ByteSize;//字节长度(4-8)
	dcb.Parity = Parity;//奇偶校验方法,ODDPARITY 奇校验 EVENPARITY 偶校验 MARKPARITY 标记校验 NOPARITY 无校验
	dcb.StopBits = StopBits;//停止位位数,ONESTOPBIT 1位停止位 TWOSTOPBITS 2位停止位 ONE5STOPBITS 1.5位停止位
	if (Parity == NOPARITY)
		dcb.fParity = 0;
	else
		dcb.fParity = 1;
	//写入DCB信息
	if (SetCommState(Hcom, &dcb) == FALSE)
	{
		printf("写入DCB信息失败\n");
		return FALSE;
	}
	return TRUE;
}

static char* GetComName(void)
{
	static char ComName[7] = { 'C', 'O', 'M' };
	int ComNum;
	printf("请输入需要打开的串口号(enter结束输入)\n");
	scanf("%d", &ComNum);
	sprintf(&ComName[3], "%d", ComNum);
	return ComName;
}

HANDLE HyperTerminalInit(void)
{
	HANDLE Hcom = NULL;
	/*打开串口*/
	char *ComName = NULL;
	char InfoBuf[30];
	ComName = GetComName();
	if (OpenCom(&Hcom, ComName, GENERIC_READ | GENERIC_WRITE, 0) == FALSE)
	{
		return NULL;
	}
	/*设置缓冲区与超时*/
	if (SetBufAndTimeOut(Hcom) == FALSE)
	{
		return NULL;
	}
	/*设置DCB结构体*/
	DWORD BaudRate;
	BYTE ByteSize;
	BYTE Parity;
	BYTE StopBits;
	printf("请按文档格式输入波特率,校验方式,数据位,停止位\n");
	scanf("%ld-%c-%c-%c", &BaudRate, &Parity, &ByteSize, &StopBits);
	sprintf(InfoBuf, ComName);
	sprintf(&InfoBuf[strlen(ComName)], ",参数:%ld-%c-%c-%c\n", BaudRate, Parity, ByteSize, StopBits);
	switch (StopBits - 48)
	{
		default:StopBits = ONE5STOPBITS;
		CASE 1:StopBits = ONESTOPBIT;
		CASE 2:StopBits = TWOSTOPBITS;
	}
	switch (Parity)
	{
		default:Parity = NOPARITY;
		CASE 'N':Parity = NOPARITY;
		CASE 'E':Parity = EVENPARITY;
		CASE 'O':Parity = ODDPARITY;
	}
	ByteSize -= 48;
	if (SetDcb(Hcom, BaudRate, ByteSize, Parity, StopBits) == FALSE)
	{
		return NULL;
	}
	//清理缓冲区
	/*
	清理函数操作参数(可一起使用):
	PURGE_TXABORT         中断所有写操作并立即返回，即使写操作还没有完成。
	PURGE_RXABORT         中断所有读操作并立即返回，即使读操作还没有完成。
	PURGE_TXCLEAR         清除输出缓冲区
	PURGE_RXCLEAR         清除输入缓冲区
	*/
	if (PurgeComm(Hcom, PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE)
	{
		printf("初始化缓冲区失败\n");
		return NULL;
	}
	system("cls");
	printf(InfoBuf);
	return Hcom;
}

HANDLE ReceivePictureInit(void)
{
	HANDLE Hcom = HyperTerminalInit();
	return Hcom;
}

HANDLE Set05ATInit(void)
{
	HANDLE Hcom = NULL;
	char *ComName = NULL;
	ComName = GetComName();
	if (OpenCom(&Hcom, ComName, GENERIC_READ | GENERIC_WRITE, 0) == FALSE)
	{
		return NULL;
	}
	if (SetBufAndTimeOut(Hcom) == FALSE)
	{
		return NULL;
	}
	if (SetDcb(Hcom, 38400, 8, NOPARITY, ONESTOPBIT) == FALSE)
	{
		return NULL;
	}
	if (PurgeComm(Hcom, PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE)
	{
		printf("初始化缓冲区失败\n");
		return NULL;
	}
	system("cls");
	printf("初始化完成\n串口号:%s,参数:38400-N-8-1\n", ComName);
	return Hcom;
}

const DWORD BAUD[] = {
	0, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};

static BOOL TryBAUD(HANDLE Hcom)
{
	int count;
	char *SendBuf = "AT";
	char ReBuf[3];
	DWORD WriteNum;
	DWORD ReadNum;
	ReBuf[2] = '\0';
	for (count = 1; count <= 8; count++)
	{
		if (SetDcb(Hcom, BAUD[count], 8, NOPARITY, ONESTOPBIT) == FALSE)
		{
			return FALSE;
		}
		if (WriteFile(Hcom, SendBuf, 2, &WriteNum, NULL) == FALSE)
		{
			printf("信息发送失败,串口可能已被拔出\n");
			return FALSE;
		}
		Sleep(800);
		if (ReadFile(Hcom, ReBuf, 2, &ReadNum, NULL) == FALSE)
		{
			printf("信息读取失败,串口可能已被拔出\n");
			return FALSE;
		}
		if (ReadNum != 0)
		{
			if (STRCMP(ReBuf, == , "OK"))
			{
				system("cls");
				printf("检测到初始波特率为%ld\n", BAUD[count]);
				return TRUE;
			}
		}
	}
	printf("波特率高于115200,无法测试!\n");
	return FALSE;
}

HANDLE Set06ATInit(void)
{
	HANDLE Hcom = NULL;
	char *ComName = NULL;
	ComName = GetComName();
	if (OpenCom(&Hcom, ComName, GENERIC_READ | GENERIC_WRITE, 0) == FALSE)
	{
		return NULL;
	}
	if (SetBufAndTimeOut(Hcom) == FALSE)
	{
		return NULL;
	}
	if (PurgeComm(Hcom, PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE)
	{
		printf("初始化缓冲区失败\n");
		return NULL;
	}
	system("cls");
	printf("正在测试初始波特率...\n");
	if (TryBAUD(Hcom) == FALSE)
	{
		return NULL;
	}
	return Hcom;
}