/**
 * �� �� ����common.c
 * ��    �ܣ�ͨ�õĺ������ʵ��
 * �������ڣ�2009��03��06��
 * �������ڣ�2009��03��06��
 * ������룺huanying
 */
 #include "common.h"

/* ��ָ��·���ļ� */
FILE *open_file(char *file_name)
{
	FILE *fp = NULL;
	
	fp = fopen(file_name, "rb");				/* ��ֻ���������Ƶķ�ʽ���ļ� */
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

/* �ر�ָ���Ѵ򿪵��ļ��� */
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
