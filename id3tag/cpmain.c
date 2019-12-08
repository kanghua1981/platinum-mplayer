/**
 * �� �� ����cpmain.c
 * ��    �ܣ�CPlayer��ں���
 * �������ڣ�2009��03��09��
 * �������ڣ�2009��05��21��
 * ������룺huanying
 */
 
#include "common.h"
#include "ID3V1Tag.h"
#include "ID3V2Tag.h"
#include "APEV2Tag.h"

int main(int argc, char **argv)
{
	int tagflag = 0;						/* ��ǩ���ڱ�־ */
	int ID3v2Size = 0;						/* ID3v2��ǩ��С */
	FILE *fp = NULL;
	ID3V1 *pID3V1 = NULL;				/* ID3V1��ǩ�ṹ��ָ�� */
	ID3V2FL *pID3V2FL = NULL;
	APEv2Item *item_hp = NULL;			/* APEv2��ǩ�ṹ��ָ�� */
	
	if (argc != 2)
	{
		printf("Usage:%s	<filename>\n", argv[0]);
		return -1;
	}

	//argv[1] = "e:\\music\\����С·.wma";
	
	if ( NULL == (fp = fopen(argv[1], "rb")) )
	{
		fprintf(stderr, "Open file '%s' failure!\n", argv[1]);
		return -1;
	}
	else
	{
		printf("Open file '%s' success!\n", argv[1]);
	}
	
	if ( NULL == (pID3V1 = (ID3V1 *)malloc(TAGSIZE)) )
	{
		perror("Memory allocate for ID3V1 error");
		return -1;
	}
	
	if ( NULL == (pID3V2FL = (ID3V2FL *)malloc(TAGFLSIZE)) )
	{
		free(pID3V1);
		pID3V1 = NULL;
		
		perror("Memory allocate for ID3V2FL error");
		return -1;
	}
	
	if ( NULL == (item_hp = (APEv2Item *)malloc(sizeof(APEv2Item))) )
	{
		free(pID3V1);
		pID3V1 = NULL;
		
		free(pID3V2FL);
		pID3V2FL = NULL;
		
		perror("Memory allocate for APEv2Item error");
		return -1;
	}
	
	/* ��ȡID3v1��ǩ */
	if (0 == get_ID3V1Tag_info(fp, pID3V1))
	{
		tagflag += HAS_ID3v1;
	}
	
	/* ��ȡID3v2��ǩ */
	if (0 == get_ID3V2Tag_info(fp, pID3V2FL, &ID3v2Size))
	{
		tagflag += HAS_ID3v2;
	}
	
	/* ��ȡAPEv2��ǩ */
	if (0 == get_APEv2Tag_info(fp, item_hp, tagflag, ID3v2Size))
	{
		tagflag += HAS_APEv2;
	}
	
	fclose(fp);
	
	print_ID3v1Tag(pID3V1);
 	print_ID3v2Tag(pID3V2FL);
 	print_APEv2Tag(item_hp);
 	
	/* �ͷ�ID3v1��ǩ�ṹ�� */
	if (pID3V1 != NULL)
	{
		free(pID3V1);
		pID3V1 = NULL;
	}
	
	/* �ͷ���ID3v2�ṹ����ɵ����� */
	free_frame_linkedtable(pID3V2FL);
	pID3V2FL = NULL;
	
	/* �ͷ���APEv2Item�ṹ����ɵ����� */
	free_item_linkedtable(item_hp);
	item_hp = NULL;
	
	return 0;
}
