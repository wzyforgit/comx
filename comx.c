#include"App.h"
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

typedef void(*V_FP_V)(void);

void show(void)
{
	printf("欢迎使用多功能串口调试助手(By 海魂水晶)\n"\
		"请在使用前阅读说明文档\n"\
		"1.超级终端\n"\
		"2.接收大量数据\n"\
		"3.蓝牙AT模式设置\n"\
		"4.根据数据导出图片\n"\
		"Q.退出程序\n"
		);
}

int main(void)
{
	static V_FP_V const arpf[] =
	{ HyperTerminal, ReceivePicture, SetAT, ExportPicture };
	int option = 0;
	show();
	while (1)
	{
		option = _getch();
		if (option > '4' || option < '1')
		{
			if (option == 'Q' || option == 'q')
				break;
			printf("数据错误,请重新输入!\n");
			continue;
		}
		option -= 49;
		system("cls");
		arpf[option]();
		system("pause");
		system("cls");
		show();
	}
	return EXIT_SUCCESS;
}