/**
 * �� �� ����ID3V2Tag.c
 * ��    �ܣ�ý���ļ�ID3V2��ǩ��Ϣ��ȡ�����ʹ�ӡ������ʵ��
 * �������ڣ�2009��03��09��
 * �������ڣ�2009��05��21��
 * ������룺huanying
 */
 
 #include "ID3V2Tag.h"
 
 /* ��ȡĿ���ļ��ı�ǩ��Ϣ���洢��ID3V2FL�ṹ�������У��ɹ�����0���򷵻�-1 */
 int get_ID3V2Tag_info(FILE *fp, ID3V2FL *pID3V2FL, int *tagsize_p)
 {
 	int counter = 0;
 	int counter1 = 0;
 	int tag_size = 0;				/* ID3V2��ǩ�Ĵ�С,������ǩͷ��10���ֽں����еı�ǩ֡�Ĵ�С */
 	int read_bytes = 0;
 	unsigned long fbody_size = 0;			/* ��ǩ֡֡��Ĵ�С */
 	char *buffer = NULL;
 	ID3V2H tagHeader;			/* ID3V2��ǩͷ�ṹ�� */
 	ID3V2FH frameHeader;			/* ID3V2��ǩ֡֡ͷ�ṹ�� */
 	ID3V2FL *p1 = NULL;
 	ID3V2FL *p2 = NULL;
 	
	memset(pID3V2FL,0,sizeof(ID3V2FL));

 	if (fp == NULL)
	{
		printf("File stream's pointer is NULL!\n");
		return -1;
	}
	
	if (pID3V2FL == NULL)
	{
		printf("pID3V2FL  is NULL!\n");	
		return -1;
	}
	
 	fseek(fp, 0, SEEK_SET);
 	fread(&tagHeader, sizeof(ID3V2H), 1, fp);				/* ��ȡ��ǩͷ */
 	
 	if (strncmp(tagHeader.header, "ID3", 3) != 0)
 	{
 		printf("This medium has not ID3v2Tag!\n");
 		return -1;
 	}
 	
 	/* ����ID3V2��ǩ�Ĵ�С��������ǩͷ��10�ֽڴ�С */
 	*tagsize_p = tag_size = 	(((tagHeader.size)[0] & 0xff) << 24) |
							(((tagHeader.size)[1] & 0xff) << 16) |
							(((tagHeader.size)[2] & 0xff) << 8) |
							((tagHeader.size)[3] & 0xff);

	
	p2 = pID3V2FL;									/* ����ͷ��㲻�洢��Ϣ */
	p2->pNext = 0;

 	while (read_bytes < (tag_size - 10))
 	{

	 	fread(&frameHeader, sizeof(ID3V2FH), 1, fp);
	 	printf("frameID:%s\t", frameHeader.frameID);
	 	
	 	fbody_size = 	((frameHeader.size)[0] << 24) |
					((frameHeader.size)[1] << 16) |
					((frameHeader.size)[2] << 8) |
					(frameHeader.size)[3];
					
		read_bytes += (10 + fbody_size);
		
		if (fbody_size == 0 || fbody_size > 512)
		{
		     break;
		}
	 
	 	if ( NULL == (p1 = (ID3V2FL *)malloc(TAGFLSIZE)) )
	 	{
	 		perror("Memory allocate for ID3V2FL error");
	 		return -1;
	 	}
	 		 	 	
	 	if ( NULL == (buffer = (char *)malloc(fbody_size + 1)) )
	 	{
	 		perror("Memory allocate for buffer error");
	 		return -1;
	 	}
	 	else
	 	{
	 		memset(buffer, 0, fbody_size + 1);
	 	}
	 	
	 	strcpy(p1->fName, frameHeader.frameID);
	 	
	 	p1->pFBody = buffer;
	 	
 	 	if (fread(p1->pFBody, 1, fbody_size, fp) != fbody_size)
	 	{
	 		fprintf(stderr, "fread error!\n");
	 		break;
	 	}
	 	else
	 	{
	 		printf("read:%d\t", fbody_size);
	 	}
		p1->pFBody[fbody_size-1] = 0;

		
	 	p2->pNext = p1;
	 	p1->pNext = NULL;
	 	p2 = p1;
	 	
	 }/* end of while */
	
	buffer = NULL;
	
	return 0;
 }
 
 /* print_ID3v2Tag:��ӡ��ID3v2��ǩ��Ϣ */
 void print_ID3v2Tag(ID3V2FL *header)
 {
 	ID3V2FL *p = header;
 	
 	if (NULL == p)
 		return;
 	
 	printf("--------------------->ID3v2<---------------------\n");
 	while (p->pNext != NULL)
 	{
 		p = p->pNext;							/* ����ͷ��㲻����Ϣ����������ͷ��� */
 		printf("%s\t%s\n", p->fName, p->pFBody);
 	}
 	printf("--------------------->ID3v2<---------------------\n");
 }
 
 /* free_frame_linkedtable:�ͷ��ɽṹ��(ID3V2FL)��ɵ����� */
  void free_frame_linkedtable(ID3V2FL *header)
  {
  	//int counter = 0;
  	ID3V2FL *p1 = NULL, *p2 = NULL;
  	
  	if (NULL == header)
  		return;
  	
  	p1 = header;
  	
  	do
  	{
  		p2 = p1->pNext;
  		
  		if (p1->pFBody != NULL)
  			free(p1->pFBody);
 		p1->pFBody = NULL;
 		
 		free(p1);
 		//printf("ID3v2-Node[%2d] is free!\n", counter);
 		
 		//++counter;
 		
  	}while ( (p1 = p2) != NULL );
  	
  	p2 = p1 = NULL;
  }
 