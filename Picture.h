#ifndef PICTURE_H
#define PICTURE_H

#define _CRT_SECURE_NO_WARNINGS 1

#define PIDW 1024
#define PIDH 768

typedef struct StepResponseData
{
	int Data[PIDW / 2];	//文件内部数据
	int DataNum;		//数据数量
	int MaxData;		//数据最大值
	int td;				//延迟时间
	int tr;				//上升时间
	int tp;				//峰值时间
	int ts;				//调节时间
	int MaxOutput;		//最大输出量
	int base;			//输入值
	int T;				//采样周期
	double Overshoot;	//超调量
}StepResponseData;

extern void CreatPIDPicture(int Data[], int DataNum, int Base, int max);

#endif