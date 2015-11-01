#include"common.h"
#include"comx.h"

#include<stdio.h>
#include<conio.h>
#include<process.h>

volatile BOOL fExit = 0;

unsigned int WINAPI ReadInfo(PVOID Hcom)
{
	char Buf;
	DWORD ReadNum;
	while (!fExit)
	{
		if (ReadFile(Hcom, &Buf, 1, &ReadNum, NULL) == FALSE)
		{
			printf("串口可能已被拔出\n输出线程终止\n");
			return 0;
		}
		if (ReadNum != 0)
		{
			printf("%c", Buf);
		}
	}
	return 0;
}

unsigned int WINAPI WriteInfo(PVOID Hcom)
{
	char Buf;
	DWORD WriteNum;
	printf("超级终端启动完毕,需要关闭时请按ESC\n");
	while (1)
	{
		Buf = _getch();
		if (Buf == 27)
		{
			fExit = 1;
			break;
		}
		if (WriteFile(Hcom, &Buf, 1, &WriteNum, NULL) == FALSE)
		{
			printf("串口可能已被拔出\n输入线程终止\n");
			return 0;
		}
	}
	return 0;
}

unsigned int WINAPI ExitMonitor(PVOID PM)
{
	int x;
	while (1)
	{
		x = _getch();
		if (x == 27)
		{
			fExit = 1;
			break;
		}
	}
	return 0;
}

static char* SetPra05(int Choose)
{
	static char Buf[50];
	switch (Choose)
	{
		CASE NAME:sprintf(Buf, "AT+NAME="); printf("请输入新名字(enter结束)\n");
		CASE PSWD:sprintf(Buf, "AT+PSWD="); printf("请输入新密码(enter结束)\n");
		CASE UART:sprintf(Buf, "AT+UART="); printf("请依照文档格式输入新传输模式(enter结束)\n");
	}
	scanf("%s", &Buf[8]);
	Buf[49] = '\0';
	int NameLen;
	NameLen = strlen(Buf);
	NameLen == 49 ? sprintf(&Buf[47], "\r\n") :
					sprintf(&Buf[strlen(Buf)], "\r\n");
	return Buf;
}

void Set05AT(void)
{
	HANDLE Hcom;
	Hcom = Set05ATInit();
	CheckComHandle(Hcom);
	_beginthreadex(NULL, 0, ReadInfo, Hcom, 0, NULL);
	printf("请选择指令:\n");
	printf("1.设置名字\t\t2.设置密码\n"\
		   "3.设为主机\t\t4.设为从机\n"\
		   "5.设为任意连接\t\t6.设置通信参数\n"\
		   "7.清空配对列表\t\t8.测试AT模式\n"\
		   "9.退出AT模式\n"\
		   "显示OK表示设置成功,并可以继续发送指令\n");
	char *Order;
	int Choose = 0;
	DWORD WriteNum;
	while (Choose != '9')
	{
		Choose = _getch();
		switch (Choose)
		{
		default:printf("指令错误!\n"); system("pause"); exit(EXIT_SUCCESS);
			CASE '1':Order = SetPra05(NAME);
			CASE '2':Order = SetPra05(PSWD);
			CASE '3':Order = "AT+ROLE=0\r\n";
			CASE '4':Order = "AT+ROLE=1\r\n";
			CASE '5':Order = "AT+CMODE=0\r\n";
			CASE '6':Order = SetPra05(UART);
			CASE '7':Order = "AT+RMAAD\r\n";
			CASE '8':Order = "AT\r\n";
			CASE '9':Order = "AT+RESET\r\n";
		}
		if (WriteFile(Hcom, Order, strlen(Order), &WriteNum, NULL) == FALSE)
		{
			printf("串口可能已被拔出\n指令发送失败\n");
			return;
		}
	}
	fExit = 1;
	CloseHandle(Hcom);
	Sleep(500);
}

static void SetPra06(HANDLE Hcom, int Choose)
{
	char Order[50] = { 0 };
	DWORD WriteNum;
	int BAUDnum;
	if (Choose != UART)
	{
		if (Choose == AT)
		{
			sprintf(Order, "AT");
		}
		else
		{
			switch (Choose)
			{
				CASE NAME : sprintf(Order, "AT+NAME"); printf("请输入新名字(enter结束)\n");
				CASE PSWD : sprintf(Order, "AT+PIN"); printf("请输入新密码(enter结束)\n");
			}
			scanf("%s", &Order[strlen(Order)]);
			Order[49] = '\0';
		}
	}
	else
	{
		printf("请选择需要的波特率\n");
		for (int count = 1; count <= 8; count++)
		{
			printf("%d.%ld\n", count, BAUD[count]);
		}
		BAUDnum = _getch();
		BAUDnum -= 48;
		sprintf(Order, "AT+BAUD%d", BAUDnum);
	}
	if (WriteFile(Hcom, Order, strlen(Order), &WriteNum, NULL) == FALSE)
	{
		printf("串口可能已被拔出\n指令发送失败\n");
		return;
	}
	if (Choose == UART)
	{
		Sleep(2000);
		if (SetDcb(Hcom, BAUD[BAUDnum], 8, NOPARITY, ONESTOPBIT) == FALSE)
		{
			printf("DCB信息修改失败\n");
			system("pause");
			exit(EXIT_SUCCESS);
		}
	}
}

void Set06AT(void)
{
	HANDLE Hcom;
	Hcom = Set06ATInit();
	CheckComHandle(Hcom);
	_beginthreadex(NULL, 0, ReadInfo, Hcom, 0, NULL);
	printf("请选择指令:\n");
	printf("1.测试AT模式\t\t2.设置波特率\n"\
		   "3.设置蓝牙名字\t\t4.设置密码\n"\
		   "5.退出AT设置\n");
	int Choose = 0;
	while (Choose != '5')
	{
		Choose = _getch();
		switch (Choose)
		{
		default:printf("指令错误!\n"); system("pause"); exit(EXIT_SUCCESS);
			CASE '1':SetPra06(Hcom, AT);
			CASE '2':SetPra06(Hcom, UART);
			CASE '3':SetPra06(Hcom, NAME);
			CASE '4':SetPra06(Hcom, PSWD);
			CASE '5':;
		}
		Sleep(1000);
		putchar('\n');
	}
	fExit = 1;
	CloseHandle(Hcom);
	Sleep(500);
}