#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS 1

#define CASE break;case
#define STRCMP(a,R,b) (strcmp(a,b) R 0)

#define MainMenu				0
#define MainMenuNum				6

#define SetATMenu				1
#define SetATMenuNum			2

#define ExportPictureMenu		2
#define ExportPictureMenuNum	2

void show(int Choose);
void Menu(int Choose, int Num);

#endif