/**
 * 文 件 名：common.c
 * 功    能：通用的函数库的实现
 * 创建日期：2009年03月06日
 * 最新日期：2009年03月06日
 * 程序编码：huanying
 */
 #include "common.h"

/* 打开指定路径文件 */
FILE *open_file(char *file_name)
{
	FILE *fp = NULL;
	
	fp = fopen(file_name, "rb");				/* 以只读方二进制的方式打开文件 */
	if (fp == NULL)
	{
		printf("Open file is failed!\n");
		return NULL;
	}
	else
	{
		printf("Open file is successful!\n");
		return fp;
	}
}

/* 关闭指定已打开的文件流 */
void close_file(FILE *fp)
{
	int close_flag;
	
	if (fp != NULL)
	{
		close_flag = fclose(fp);
	}
	
	if ( close_flag != 0)
	{
		printf("Close file is failed!\n");
		return;
	}
	else
	{
		printf("Close file is successful!\n");
	}
}
