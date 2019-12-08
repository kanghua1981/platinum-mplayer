/**
 * �� �� ����ID3V2Tag.h
 * ��    �ܣ�ý���ļ�ID3V2��ǩ��Ϣ��ȡ�����ʹ�ӡ�����Ķ���
 * �������ڣ�2009��03��09��
 * �������ڣ�2009��05��21��
 * ������룺huanying
 */
 #ifndef _ID3V2TAG_H
 #define _ID3V2TAG_H
 
 #include "define.h"
 
 #define TAGHSIZE sizeof(ID3V2H)
 #define TAGFHSIZE sizeof(ID3V2FH)
 #define TAGFLSIZE sizeof(ID3V2FL)
 
#pragma  pack(1)
 /* ý���ļ�ID3V2��ǩͷ�ṹ�� */
 typedef struct ID3V2TagH
 {
 	char header[3]; 			/* ����Ϊ"ID3"������Ϊ��ǩ������,3�ֽ� */
	char version[1]; 			/* �汾��ID3V2.3 �ͼ�¼3��1�ֽ� */
	char revision[1]; 			/* ���汾�Ŵ˰汾��¼Ϊ0��1�ֽ� */
	char flag[1]; 				/* ��ű�־���ֽڣ�����汾ֻ��������λ��1�ֽ� */
	unsigned char  size[4]; 				/* ��ǩ��С��������ǩͷ��10���ֽں����еı�ǩ֡�Ĵ�С��4�ֽ� */
 }ID3V2H;
 
 /* ý���ļ�ID3V2H��ǩ֡֡ͷ�ṹ�� */
 typedef struct ID3V2FrameH
 {
 	char frameID[4]; 			/* ���ĸ��ַ���ʶһ��֡��˵��������,4�ֽ� */
	unsigned char size[4]; 				/* ֡���ݵĴ�С��������֡ͷ������С��1��4�ֽ� */
	char flags[2]; 				/* ��ű�־��ֻ������6 λ��2�ֽ� */
 }ID3V2FH;
 
 /* ý���ļ�ID3V2��ǩ֡����ṹ�� */
 typedef struct ID3V2TagFL
 {
 	char fName[4+1];			/* �ദ��һ���ֽ��������'\0' */
 	char *pFBody;
 	struct ID3V2TagFL *pNext;
 }ID3V2FL;
#pragma  pack()

 /* ��ȡĿ���ļ��ı�ǩ��Ϣ���洢��ID3V2FL�ṹ�������У��ɹ�����0���򷵻�-1 */
 int get_ID3V2Tag_info(FILE *fp, ID3V2FL *pID3V2FL, int *tagsize);
 
  /* print_ID3v2Tag:��ӡ��ID3v2��ǩ��Ϣ */
 void print_ID3v2Tag(ID3V2FL *header);
 
  /* free_frame_linkedtable:�ͷ��ɽṹ��(ID3V2FL)��ɵ����� */
  void free_frame_linkedtable(ID3V2FL *header);
  
 #endif
 