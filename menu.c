#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

#include"common.h"
#include"App.h"
#include"comx.h"

typedef void(*V_FP_V)(void);

void show(int Choose)
{
	switch (Choose)
	{
	default:printf("fatal error!!!\n");
		CASE MainMenu :
		printf("欢迎使用多功能串口调试助手(By 海魂水晶)\n"\
			"请在使用前阅读说明文档\n"\
			"1.超级终端\n"\
			"2.接收大量数据\n"\
			"3.蓝牙AT模式设置\n"\
			"4.根据数据导出图片\n"\
			"Q.退出程序\n"
			);
		CASE SetATMenu :
		printf("请先接好线路,确认无误后继续\n");
		system("pause");
		system("cls");
		printf("请选择芯片型号\n1.HC-05\t2.HC-06\nQ.退出当前任务\n");
		CASE ExportPictureMenu :
		printf("请选择数据种类\n1.图片\n2.PID测试\nQ.退出当前任务\n");
	}
}

void Menu(int Choose, int Num)
{
	static V_FP_V const arpf[][9] =
	{
		{ HyperTerminal, ReceivePicture, SetAT, ExportPicture },
		{ Set05AT, Set06AT },
		{ ExPic, PIDtest }
	};
	int option = 0;
	show(Choose);
	while (1)
	{
		option = _getch();
		if (option > Num + 48 || option < '1')
		{
			if (option == 'Q' || option == 'q')
			{
				break;
			}
			else
			{
				printf("数据错误,请重新输入!\n");
				continue;
			}
		}
		option -= 49;
		system("cls");
		arpf[Choose][option]();
		system("pause");
		system("cls");
		show(Choose);
	}
}