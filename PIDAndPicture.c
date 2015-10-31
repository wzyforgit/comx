#include"Picture.h"
#include"comx.h"

void ExPic(void)
{
	printf("Expic not open\n");
}

void PIDtest(void)
{
	system("cls");
	printf("请输入完整的文件名\n");
	char FileName[MAX_PATH];
	scanf("%s", FileName);
	FileName[MAX_PATH - 1] = '\0';

	FILE *ReFile;
	ReFile = fopen(FileName, "r");
	if (ReFile == NULL)
	{
		printf("文件打开失败!\n");
		system("pause");
		return;
	}

	StepResponseData SRD;
	SRD.td = SRD.tr = SRD.tp = SRD.ts = 0;
	SRD.Overshoot = 0;

	printf("请输入本次测试的上限值\n");
	scanf("%d", &SRD.MaxOutput);
	printf("请输入本次测试的输入值\n");
	scanf("%d", &SRD.base);
	printf("请输入本次测试的采样周期(ms)\n");
	scanf("%d", &SRD.T);

	printf("计算中...\n");
	int ch;
	for (SRD.DataNum = 0, SRD.MaxData = 0; SRD.DataNum < PIDW / 2; SRD.DataNum++)
	{
		/*数据读取*/
		fscanf(ReFile, "%d", &SRD.Data[SRD.DataNum]);

		/*计算超调量,峰值时间*/
		if (SRD.Data[SRD.DataNum]>SRD.MaxData)
		{
			SRD.MaxData = SRD.Data[SRD.DataNum];
			if (SRD.MaxData > SRD.base)
			{
				SRD.tp = (SRD.DataNum - 1) * SRD.T;
				SRD.Overshoot = (SRD.MaxData - SRD.base) / (double)SRD.base;
			}
		}

		/*计算调节时间*/
		if (SRD.ts == 0 && abs(SRD.Data[SRD.DataNum] - SRD.base) / (double)SRD.base <= 0.02)
		{
			SRD.ts = (SRD.DataNum - 1) * SRD.T;
		}
		if (SRD.ts != 0 && abs(SRD.Data[SRD.DataNum] - SRD.base) / (double)SRD.base > 0.02)
		{
			SRD.ts = 0;
		}

		/*计算延迟时间*/
		if (SRD.td == 0 && SRD.Data[SRD.DataNum] >= SRD.base / 2)
		{
			SRD.td = (SRD.DataNum - 1) * SRD.T;
		}

		/*计算上升时间*/
		if (SRD.tr == 0 && SRD.Data[SRD.DataNum] >= SRD.base)
		{
			SRD.tr = (SRD.DataNum - 1) * SRD.T;
		}

		/*读取段终值符(根据通信协议设计)及判断EOF*/
		ch = getc(ReFile);
		ch = getc(ReFile);
		if (ch == EOF)
			break;
		else
			ungetc(ch, ReFile);
	}

	printf("计算完成!\n");
	printf("td=%dms\ntr=%dms\ntp=%dms\nts=%dms\nσ=%g\n", SRD.td, SRD.tr, SRD.tp, SRD.ts, SRD.Overshoot);
	printf("按任意键开始创建本次测试的阶跃响应图像\n");
	_getch();

	printf("正在生成图片...\n");
	CreatPIDPicture(SRD.Data, SRD.DataNum, SRD.base, SRD.MaxOutput);
}