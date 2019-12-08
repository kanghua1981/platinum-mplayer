/**
 * �� �� ����APEV2Tag.h
 * ��    �ܣ�ý���ļ�APEV2��ǩ��Ϣ��ȡ�����ʹ�ӡ�����Ķ���
 * �������ڣ�2009��05��10��
 * �������ڣ�2009��05��10��
 * ������룺huanying
 */
 
 #ifndef 	_APEV2TAG_H
 #define	_APEV2TAG_H
 
 #include "define.h"
 
 /* ý���ļ�APEv2��ǩ�ı�ǩͷ���ǩβ�ṹ�壬��СΪ32�ֽ� */
 typedef struct APEV2TagHF
 {
 	char	preamble[8];			/* APEv2��ǩ�Ƿ���ڵı�־��ռ8�ֽ� */	
 	char	version[4];			/* APEv2��ǩ�İ汾��ռ4�ֽ� */
 	char	tagsize[4];			/* APEv2��ǩ�Ĵ�С��������ǩβ����������ǩͷ�Ĵ�С��ռ4�ֽ� */
 	char	itemcount[4];			/* APEv2��ǩԪ�صĸ�����ռ4�ֽ� */
 	char	tagflags[4];			/* APEv2��ǩ�ı�ʶ��ռ4�ֽ� */
 	char	reserved[8];			/* APEv2��ǩ������ֵȫΪ�㣬ռ8�ֽ� */
 	
 }APEv2HF;
 
 /* ý���ļ�APEv2��ǩ�ı�ǩԪ�� */
 typedef struct APEV2TagItem
 {
 	char *itemvalue_p;				/* APEv2��ǩ��ǩԪ�ص����� */
 	unsigned char itemvaluesize[4];	/* APEv2��ǩ��ǩԪ�����ݵĴ�С��ռ4�ֽ� */
 	char  itemflags[4];				/* APEv2��ǩ��ǩԪ�صı�ʶ��ռ4�ֽ� */
 	char itemkey[256];				/* APEv2��ǩ��ǩԪ�عؼ��֣���С����Ϊ2���ɼ��ַ���255�ɼ��ַ� */
 	struct APEV2TagItem *nextp;
 	
 }APEv2Item;
 
  /* get_APEv2Tag_info:��ȡĿ���ļ���APEv2��ǩ��Ϣ���ɹ�����0��ӷ���-1 */
 int get_APEv2Tag_info(FILE *fp, APEv2Item *header, int tagflag, int ID3v2Size);
 
  /* print_APEv2Tag: ��ӡAPEv2��ǩ��Ϣ */
 void print_APEv2Tag(APEv2Item *header);
 
  /* free_item_linkedtable:�ͷ��б�ǩԪ�ؽṹ��(APEv2Item)�ṹ����ɵ����� */
 void free_item_linkedtable(APEv2Item *item_stp);
 
 #endif
 