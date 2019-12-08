/**
 * 文 件 名：ID3V1Tag.h
 * 功    能：媒体文件ID3V1标签结构体以及标签信息提取函数和打印函数的定义
 * 创建日期：2009年03月09日
 * 最新日期：2009年05月21日
 * 程序编码：huanying
 */
#ifndef _ID3V1TAG_H
#define _ID3V1TAG_H

#include "define.h"
#define TAGSIZE sizeof(ID3V1)		/* 标签结构体的大小 */

/* 此结构体中的成员字符数组的长度必须多定义一个长度，转换字符串时用 */
/* 多处的一个字节用来存放'\0' */
typedef struct tagID3V1
{
	char Header[4]; 				/* 标签头必须是"TAG"否则认为没有标签,3字节 */
	char Title[30]; 				/* 标题,30字节 */
	char Artist[30]; 				/* 作者,30字节 */
	char Album[30]; 			/* 专集,30字节 */
	char Year[4]; 				/* 出品年代,4字节 */
	char Comment[28]; 			/* 备注,28字节 */
	char Reserve[1]; 				/* 保留,1字节 */
	unsigned char Track; 				/* 音轨,1字节 */
	unsigned char Genre; 				/* 类型,1字节 */
}ID3V1;

/* 读取目标文件的标签信息并存储到ID3V1结构体中，成功返回0否则返回-1 */
int get_ID3V1Tag_info(FILE *fp, ID3V1 *pID3V1);

/* print_ID3v1Tag:打印出标记结构体中的成员信息 */
void print_ID3v1Tag(ID3V1 *pID3V1);

#endif
