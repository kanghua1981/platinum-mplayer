/**
 * 文 件 名：cpmain.c
 * 功    能：CPlayer入口函数
 * 创建日期：2009年03月09日
 * 最新日期：2009年05月21日
 * 程序编码：huanying
 */
 
#include "common.h"
#include "ID3V1Tag.h"
#include "ID3V2Tag.h"
#include "APEV2Tag.h"

int main(int argc, char **argv)
{
	int tagflag = 0;						/* 标签存在标志 */
	int ID3v2Size = 0;						/* ID3v2标签大小 */
	FILE *fp = NULL;
	ID3V1 *pID3V1 = NULL;				/* ID3V1标签结构体指针 */
	ID3V2FL *pID3V2FL = NULL;
	APEv2Item *item_hp = NULL;			/* APEv2标签结构体指针 */
	
	if (argc != 2)
	{
		printf("Usage:%s	<filename>\n", argv[0]);
		return -1;
	}

	//argv[1] = "e:\\music\\乡间的小路.wma";
	
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
	
	/* 读取ID3v1标签 */
	if (0 == get_ID3V1Tag_info(fp, pID3V1))
	{
		tagflag += HAS_ID3v1;
	}
	
	/* 读取ID3v2标签 */
	if (0 == get_ID3V2Tag_info(fp, pID3V2FL, &ID3v2Size))
	{
		tagflag += HAS_ID3v2;
	}
	
	/* 读取APEv2标签 */
	if (0 == get_APEv2Tag_info(fp, item_hp, tagflag, ID3v2Size))
	{
		tagflag += HAS_APEv2;
	}
	
	fclose(fp);
	
	print_ID3v1Tag(pID3V1);
 	print_ID3v2Tag(pID3V2FL);
 	print_APEv2Tag(item_hp);
 	
	/* 释放ID3v1标签结构体 */
	if (pID3V1 != NULL)
	{
		free(pID3V1);
		pID3V1 = NULL;
	}
	
	/* 释放由ID3v2结构体组成的链表 */
	free_frame_linkedtable(pID3V2FL);
	pID3V2FL = NULL;
	
	/* 释放由APEv2Item结构体组成的链表 */
	free_item_linkedtable(item_hp);
	item_hp = NULL;
	
	return 0;
}
