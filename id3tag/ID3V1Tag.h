/**
 * �� �� ����ID3V1Tag.h
 * ��    �ܣ�ý���ļ�ID3V1��ǩ�ṹ���Լ���ǩ��Ϣ��ȡ�����ʹ�ӡ�����Ķ���
 * �������ڣ�2009��03��09��
 * �������ڣ�2009��05��21��
 * ������룺huanying
 */
#ifndef _ID3V1TAG_H
#define _ID3V1TAG_H

#include "define.h"
#define TAGSIZE sizeof(ID3V1)		/* ��ǩ�ṹ��Ĵ�С */

/* �˽ṹ���еĳ�Ա�ַ�����ĳ��ȱ���ඨ��һ�����ȣ�ת���ַ���ʱ�� */
/* �ദ��һ���ֽ��������'\0' */
typedef struct tagID3V1
{
	char Header[4]; 				/* ��ǩͷ������"TAG"������Ϊû�б�ǩ,3�ֽ� */
	char Title[30]; 				/* ����,30�ֽ� */
	char Artist[30]; 				/* ����,30�ֽ� */
	char Album[30]; 			/* ר��,30�ֽ� */
	char Year[4]; 				/* ��Ʒ���,4�ֽ� */
	char Comment[28]; 			/* ��ע,28�ֽ� */
	char Reserve[1]; 				/* ����,1�ֽ� */
	unsigned char Track; 				/* ����,1�ֽ� */
	unsigned char Genre; 				/* ����,1�ֽ� */
}ID3V1;

/* ��ȡĿ���ļ��ı�ǩ��Ϣ���洢��ID3V1�ṹ���У��ɹ�����0���򷵻�-1 */
int get_ID3V1Tag_info(FILE *fp, ID3V1 *pID3V1);

/* print_ID3v1Tag:��ӡ����ǽṹ���еĳ�Ա��Ϣ */
void print_ID3v1Tag(ID3V1 *pID3V1);

#endif
