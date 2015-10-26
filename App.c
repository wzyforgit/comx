#include"comx.h"

void HyperTerminal(void)
{
	HANDLE Hcom;
	Hcom = HyperTerminalInit();
	CheckComHandle(Hcom);

	HANDLE Wait[2];
	Wait[0] = (HANDLE)_beginthreadex(NULL, 0, WriteInfo, Hcom, 0, NULL);
	Wait[1] = (HANDLE)_beginthreadex(NULL, 0, ReadInfo, Hcom, 0, NULL);
	WaitForMultipleObjects(2, Wait, TRUE, INFINITE);
	CloseHandle(Hcom);
	CloseHandle(Wait[0]);
	CloseHandle(Wait[1]);
	fExit = 0;
}

void ReceivePicture(void)
{
	HANDLE Hcom;
	Hcom = ReceivePictureInit();
	CheckComHandle(Hcom);
	/*准备存储文件*/
	FILE *pSave;
	char FileName[MAX_PATH];
	printf("请输入存储文件的完整文件名,按enter确认\n");
	scanf("%s", FileName);
	FileName[MAX_PATH - 1] = '\0';
	pSave = fopen(FileName, "w");
	if (pSave == NULL)
	{
		printf("存储文件创建失败\n");
		system("pause");
		exit(EXIT_FAILURE);
	}
	/*传输数据*/
	printf("传输准备就绪,请开启下位机开始进行数据传输\n需要结束传输请按ESC\n");
	_beginthreadex(NULL, 0, ExitMonitor, NULL, 0, NULL);

	/*传输数据起始符,由上位机和下位机统一*/
	const char *DataStart = "tsD";
	/*传输数据终止符,由上位机和下位机统一*/
	const char *DataEnd = "deD";
	/*段结束符,由上位机和下位机统一*/
	const char SectionEnd = '#';
	char SEbuf[4];
	DWORD ReadNum;
	memset(SEbuf, 0, sizeof(char)* 4);
	while (1)
	{
		if (ReadFile(Hcom, &SEbuf[0], 1, &ReadNum, NULL) == FALSE)
		{
			printf("串口可能已被拔出\n输出线程终止\n");
			system("pause");
		}
		if (ReadNum != 0)
		{
			if (STRCMP(SEbuf, == , DataStart))
				break;
			SEbuf[2] = SEbuf[1];
			SEbuf[1] = SEbuf[0];
		}
	}
	printf("检测到数据起始符,开始保存数据至文件\n");
	memset(SEbuf, 0, sizeof(char)* 3);
	while (1)
	{
		if (ReadFile(Hcom, &SEbuf[0], 1, &ReadNum, NULL) == FALSE)
		{
			printf("串口可能已被拔出\n输出线程终止\n");
			system("pause");
		}
		if (ReadNum != 0)
		{
			fprintf(pSave, "%c", SEbuf[0]);
			if (SEbuf[0] == SectionEnd&&fExit == 1)
			{
				printf("同时检测到段终止符和退出\n");
				break;
			}
			if (STRCMP(SEbuf, == , DataEnd))
			{
				printf("检测到数据终止符\n");
				break;
			}
			SEbuf[2] = SEbuf[1];
			SEbuf[1] = SEbuf[0];
		}
	}

	CloseHandle(Hcom);
	fclose(pSave);
	fExit = 0;
}

void SetAT(void)
{
	printf("请先接好线路,确认无误后继续\n");
	system("pause");
	system("cls");
	printf("请选择芯片型号\n1.HC-05\t2.HC-06\n");
	switch (_getch())
	{
		default:printf("输入错误!\n"); system("pause"); return;
		CASE '1':Set05AT();
		CASE '2':Set06AT();
	}
	fExit=0;
}

void ExportPicture(void)
{
	printf("请选择数据种类\n1.图片\n2.PID测试\n");
	switch (_getch())
	{
		default:printf("输入错误!\n"); system("pause"); return;
		CASE '1':ExPic();
		CASE '2':PIDtest();
	}
}