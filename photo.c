#include"Picture.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <pshpack2.h>
#include <poppack.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#pragma pack(push)
#pragma pack(1)

/*定义位图文件头*/
typedef struct
{
	WORD bfType;//文件类型,必须为"BM",ASCII值为0x4d42
	DWORD bfSize;//文件的大小
	WORD bfReserved1;//为0
	WORD bfReserved2;//为0
	DWORD bfOffBits;//存储的像素阵列相对于文件头的偏移量(文件开头到调色区距离)
} BIT_MAP_FILE_HEADER;

/*定义信息头*/
typedef struct
{
	DWORD biSize;//信息头大小
	DWORD biWidth;//位图宽度(像素)
	DWORD biHeight;//位图高度(像素)
	WORD biPlanes;//必须为1
	WORD biBitCount;//每个像素的位数,即二进制数位数24为每一色8位(1,4,8,16,24,32)
	DWORD biCompression;//压缩方式(0或BI_RGB(未压缩))
	DWORD biSizeImage;//图像大小(字节,不包括信息头),仅用于压缩位图
	DWORD biXPelsPerMeter;//以目标设备每米的像素数来说明位图的水平分辨率
	DWORD biYPelsPerMeter; //以目标设备每米的像素数来说明位图的垂直分辨率
	DWORD biClrUsed;//颜色表的颜色数,若为0则位图使用由biBitCount指定的最大颜色数
	DWORD biClrImportant;//重要颜色的数目，若该值为0则所有颜色都重要
} BIT_MAP_INFO_HEADER;

/*调色板*/
typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
} RGB_QUAD;

#pragma pack(pop)
RGB_QUAD *bmp_data = NULL;
RGB_QUAD white = { 255, 255, 255 };
RGB_QUAD black = { 0, 0, 0 };
RGB_QUAD red = { 0, 0, 255 };

static void printWord(int s, int t,const unsigned char *word)
{
	int count;
	int x, y;
	for (y = t, count = 0; y > t - 16; y--, count++)
	{
		for (x = s; x < s + 8; x++)
		{
			if (word[count] & (1 << (s + 7 - x)))
			{
				bmp_data[y * 320 + x] = white;
			}
			else
			{
				bmp_data[y * 320 + x] = black;
			}
		}
		for (count++; x < s + 16; x++)
		{
			if (word[count] & (1 << (s + 15 - x)))
			{
				bmp_data[y * 320 + x] = white;
			}
			else
			{
				bmp_data[y * 320 + x] = black;
			}
		}
	}
}

static void printAllWord(int s, int t, int num, const unsigned char *word)
{
	int x, y;
	int count;
	for (x = s, y = t, count = 0; x<s + num * 16; x += 16, count += 32)
	{
		printWord(x, y, &word[count]);
	}
}

static FILE* PictureInit(long int width, long int height)
{
	FILE *bmp_file;
	BIT_MAP_FILE_HEADER bmp_map_file;
	BIT_MAP_INFO_HEADER bmp_map_info;

	if ((bmp_file = fopen("cubs.bmp", "wb+")) == NULL)
	{
		printf("创建图片失败!\n");
		system("pause");
		exit(1);
	}

	/*写入图位文件头*/
	bmp_map_file.bfType = 0x4d42;
	bmp_map_file.bfSize = \
		sizeof(BIT_MAP_FILE_HEADER)+sizeof(BIT_MAP_INFO_HEADER)+width * height * sizeof(RGB_QUAD);
	bmp_map_file.bfReserved1 = 0;
	bmp_map_file.bfReserved2 = 0;
	bmp_map_file.bfOffBits = sizeof(BIT_MAP_FILE_HEADER)+sizeof(BIT_MAP_INFO_HEADER);
	fwrite(&bmp_map_file, sizeof(BIT_MAP_FILE_HEADER), 1, bmp_file);

	/*写入信息头*/
	bmp_map_info.biSize = sizeof(BIT_MAP_INFO_HEADER);
	bmp_map_info.biPlanes = 1;
	bmp_map_info.biHeight = height;
	bmp_map_info.biWidth = width;
	bmp_map_info.biBitCount = 24;
	bmp_map_info.biClrImportant = 0;
	bmp_map_info.biClrUsed = 0;
	bmp_map_info.biSizeImage = width * height * sizeof(RGB_QUAD);
	bmp_map_info.biCompression = 0;
	bmp_map_info.biXPelsPerMeter = 0;
	bmp_map_info.biYPelsPerMeter = 0;
	fwrite(&bmp_map_info, sizeof(BIT_MAP_INFO_HEADER), 1, bmp_file);

	return bmp_file;
}

void CreatPIDPicture(int Data[],int DataNum,int Base,int max)
{
	FILE* bmp_file = NULL;
	bmp_file = PictureInit(PIDW, PIDH);

	/*给图像分配内存*/
	if ((bmp_data = (RGB_QUAD*)malloc(PIDW * PIDH * sizeof(RGB_QUAD))) == NULL)
	{
		printf("图片申请内存失败！\n");
		exit(0);
	}

	/*初始化图像*/
	int i, j;
	for (j = 0; j < PIDH; j++)
	{
		for (i = 0; i < PIDW; i++)
		{
			bmp_data[j * PIDW + i] = black;
		}
	}

	/*处理数据*/
	double Wstep = PIDW / (double)DataNum;
	double Hstep = PIDH / (double)max;

	/*画出输入值*/
	Base = (int)(Base*Hstep);
	for (i = 0; i < PIDW; i++)
	{
		bmp_data[Base*PIDW + i] = red;
		bmp_data[(Base + 1)*PIDW + i] = red;
		bmp_data[(Base - 1)*PIDW + i] = red;
	}

	/*描点*/
	int count;
	int temp;
	for (i = 0, count = 0; i < PIDW&&count <= DataNum; count++)
	{
		temp = (int)(Data[count] * Hstep);
		bmp_data[temp * PIDW + i] = white;
		bmp_data[(temp + 1) * PIDW + i] = white;
		bmp_data[(temp + 1) * PIDW + i + 1] = white;
		bmp_data[temp * PIDW + i + 1] = white;
		if (DataNum < PIDW / 4)
		{
			bmp_data[(temp + 2) * PIDW + i] = white;
			bmp_data[(temp + 2) * PIDW + i + 1] = white;
			bmp_data[(temp + 2) * PIDW + i + 2] = white;
			bmp_data[(temp + 1) * PIDW + i + 2] = white;
			bmp_data[temp * PIDW + i + 2] = white;
		}
		i = (int)(i + Wstep);
	}

	fwrite(bmp_data, sizeof(RGB_QUAD), PIDW*PIDH, bmp_file);
	printf("BMP图像生成成功\n");
	fclose(bmp_file);
	free(bmp_data);
	bmp_data = NULL;
	system("start cubs.bmp");
}