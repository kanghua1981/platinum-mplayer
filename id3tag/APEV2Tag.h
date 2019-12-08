/**
 * 文 件 名：APEV2Tag.h
 * 功    能：媒体文件APEV2标签信息提取函数和打印函数的定义
 * 创建日期：2009年05月10日
 * 最新日期：2009年05月10日
 * 程序编码：huanying
 */
 
 #ifndef 	_APEV2TAG_H
 #define	_APEV2TAG_H
 
 #include "define.h"
 
 /* 媒体文件APEv2标签的标签头或标签尾结构体，大小为32字节 */
 typedef struct APEV2TagHF
 {
 	char	preamble[8];			/* APEv2标签是否存在的标志，占8字节 */	
 	char	version[4];			/* APEv2标签的版本，占4字节 */
 	char	tagsize[4];			/* APEv2标签的大小，包括标签尾但不包括标签头的大小，占4字节 */
 	char	itemcount[4];			/* APEv2标签元素的个数，占4字节 */
 	char	tagflags[4];			/* APEv2标签的标识，占4字节 */
 	char	reserved[8];			/* APEv2标签保留，值全为零，占8字节 */
 	
 }APEv2HF;
 
 /* 媒体文件APEv2标签的标签元素 */
 typedef struct APEV2TagItem
 {
 	char *itemvalue_p;				/* APEv2标签标签元素的内容 */
 	unsigned char itemvaluesize[4];	/* APEv2标签标签元素内容的大小，占4字节 */
 	char  itemflags[4];				/* APEv2标签标签元素的标识，占4字节 */
 	char itemkey[256];				/* APEv2标签标签元素关键字，大小可以为2个可见字符到255可见字符 */
 	struct APEV2TagItem *nextp;
 	
 }APEv2Item;
 
  /* get_APEv2Tag_info:读取目标文件的APEv2标签信息，成功返回0否从返回-1 */
 int get_APEv2Tag_info(FILE *fp, APEv2Item *header, int tagflag, int ID3v2Size);
 
  /* print_APEv2Tag: 打印APEv2标签信息 */
 void print_APEv2Tag(APEv2Item *header);
 
  /* free_item_linkedtable:释放有标签元素结构体(APEv2Item)结构体组成的链表 */
 void free_item_linkedtable(APEv2Item *item_stp);
 
 #endif
 