/**
 * �� �� ����ID3V1Tag.c
 * ��    �ܣ�ý���ļ�ID3V1��ǩ��Ϣ��ȡ�����ʹ�ӡ������ʵ��
 * �������ڣ�2009��03��09��
 * �������ڣ�2009��03��09��
 * ������룺huanying
 */
 
#include "ID3V1Tag.h"


/* ��ȡĿ���ļ��ı�ǩ��Ϣ���洢��ID3V1�ṹ���У��ɹ�����0���򷵻�-1 */
int get_ID3V1Tag_info(FILE *fp, ID3V1 *pID3V1)
{
	memset(pID3V1,0,sizeof(ID3V1));
	if (fp == NULL)
	{
		printf("File stream's pointer is NULL!\n");
		return -1;
	}
	else if (pID3V1 == NULL)
	{
		printf("Tag struct's pointer is NULL!\n");	
		return -1;
	}
	
	fseek(fp, -128L, SEEK_END);
	fread(pID3V1->Header, 3, 1, fp);
	
	/* �жϱ�ǩͷ�е������Ƿ�Ϊ��TAG����������Ϊ��ý���ļ�û��ID3V1��ǩ */
	if (memcmp(pID3V1->Header, "TAG", 3) == 0)
	{
		fread(pID3V1->Title, 30, 1, fp);
		fread(pID3V1->Artist, 30, 1, fp);
		fread(pID3V1->Album, 30, 1, fp);
		fread(pID3V1->Year, 4, 1, fp);
		fread(pID3V1->Comment, 28, 1, fp);
		fread(pID3V1->Reserve, 1, 1, fp);
		fread(&pID3V1->Track, 1, 1, fp);
		fread(&pID3V1->Genre, 1, 1, fp);
	}
	else
	{
		printf("This medium has not ID3v1Tag!\n");
		return -1;
	}
	
	return 0;
}

/* print_ID3v1Tag:��ӡ����ǽṹ���еĳ�Ա��Ϣ */
void print_ID3v1Tag(ID3V1 *pID3V1)
{
	printf("--------------------->ID3v1<---------------------\n");
	printf("Header:\t%s\n", pID3V1->Header);
	printf("Title:\t%s\n", pID3V1->Title);
	printf("Artist:\t%s\n", pID3V1->Artist);
	printf("Album:\t%s\n", pID3V1->Album);
	printf("Year:\t%s\n", pID3V1->Year);
	printf("Comment:\t%s\n", pID3V1->Comment);
	//printf("Reserve:\t%s\n", pID3V1->Reserve);
	printf("Track:\t%d\n", pID3V1->Track);
	printf("Genre:\t%d\n", pID3V1->Genre);
	printf("--------------------->ID3v1<---------------------\n");
}
