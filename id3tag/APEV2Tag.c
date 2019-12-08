/**
 * 文 件 名：APEV2Tag.c
 * 功    能：媒体文件APEV2标签信息提取函数和打印函数的实现
 * 创建日期：2009年05月10日
 * 最新日期：2009年05月10日
 * 程序编码：huanying
 */
 
 #include "APEV2Tag.h"
 
  /* get_APEv2Tag_info:读取目标文件的APEv2标签信息，成功返回0否从返回-1 */
 int get_APEv2Tag_info(FILE *fp, APEv2Item *header, int tagflag, int ID3v2Size)
 {
 	int c;
 	int i;
 	int item_num;
 	int item_size;
 	long tag_size;
 	APEv2HF HF_st;
 	APEv2Item  *p1 = NULL, *p2 = NULL;
 	
	memset(header,0,sizeof(APEv2Item));

 	if (NULL == fp)
 	{
 		printf("File stream's pointer is NULL!\n");
 		return -1;
 	}
 	
 	if (NULL == fp)
 	{
 		printf("APEv2HF's pointeris NULL!\n");
 		return -1;
 	}
 	
	
	/* 从文件尾部开始搜寻APEv2标签标签尾 */
	if (tagflag == 1 || tagflag == 3)
 		fseek(fp, -(128L + 32L), SEEK_END);
 	else
 		fseek(fp, -32L, SEEK_END);
 		
 	fread(&HF_st, sizeof(APEv2HF), 1, fp);

 	if (0 == strncmp(HF_st.preamble, "APETAGEX", 8))
 	{
 		printf("This medium has APEv2Tag footer!\n");
 		
 		tag_size =	((HF_st.tagsize)[3] << 24) | 
	 				((HF_st.tagsize)[2] << 16) | 
	 				((HF_st.tagsize)[1] << 8) | 
	 				(HF_st.tagsize)[0];
			 	
		item_num = 	((HF_st.itemcount)[3] << 24) | 
	 				((HF_st.itemcount)[2] << 16) | 
	 				((HF_st.itemcount)[1] << 8) | 
	 				(HF_st.itemcount)[0];
	 				
		if (tagflag == 1 || tagflag == 3)
			fseek(fp, -(128L + tag_size), SEEK_END);
		else
			fseek(fp, -tag_size, SEEK_END);
		
	 	//printf("TagSize:%ld\t", tag_size); 
	 	//printf("Itemcount:%d\n", item_num);
 	}
 	else
 	{ 
 		 /* 从文件头部开始搜寻APEv2标签标签头 */
 		 if (tagflag == 2 || tagflag == 3)				/* 判断是否有ID3v2标签 */
 		 	fseek(fp, ID3v2Size, SEEK_SET);
 		 else
 		 	fseek(fp, 0L, SEEK_SET);
 		 
 		 fread(&HF_st, sizeof(APEv2HF), 1, fp);
 		 if (0 == strncmp(HF_st.preamble, "APETAGEX", 8))
	 	{
	 		printf("This medium has APEv2Tag header!\n");
	 		
	 		tagflag += HAS_APEv2;
	 		
	 		tag_size =	((HF_st.tagsize)[3] << 24) | 
		 				((HF_st.tagsize)[2] << 16) | 
		 				((HF_st.tagsize)[1] << 8) | 
		 				(HF_st.tagsize)[0];
				 	
			item_num = 	((HF_st.itemcount)[3] << 24) | 
		 				((HF_st.itemcount)[2] << 16) | 
		 				((HF_st.itemcount)[1] << 8) | 
		 				(HF_st.itemcount)[0];
		 	
		 	if (tagflag == 2 || tagflag == 3)				/* 判断是否有ID3v2标签 */
				fseek(fp, ID3v2Size + 32L, SEEK_SET);
			else
				fseek(fp, 32L, SEEK_SET);
			
		 	//printf("TagSize:%ld\t", tag_size); 
		 	//printf("Itemcount:%d\n", item_num);
	 	}
	 	else
	 	{
	 		 printf("This medium has not APEv2Tag!\n");
	 		 return -1;
	 	}
 	}/* end of else */
 	
 	/* 头结点不存储信息 */
 	p2 = header;
 	p2->nextp = 0;
 	/* 搜寻标签元素中的关键字和其对应的值 */
 	for (i=0; i<item_num; i++)
 	{
		int i = 0;
 		
 		if ( NULL == (p1 = (APEv2Item *)malloc(sizeof(APEv2Item))) )
 		{
 			perror("Memory allocate for item error");
 			break;
 		}
 		
 		fread(p1->itemvaluesize, 4, 1, fp);
 		fread(p1->itemflags, 4, 1, fp);
 		
 		item_size = 	((p1->itemvaluesize)[3] << 24) | 
 					((p1->itemvaluesize)[2] << 16) | 
 					((p1->itemvaluesize)[1] << 8) | 
 					(p1->itemvaluesize)[0];
		
		/* 当标签元素的大小为负的时候，表示提取APEv2标签元素有问题，终止提取 */
		if (item_size < 0)
		{
			break;
		}
		 
		while ( (c = fgetc(fp) ) != 0x00)
		{
			(p1->itemkey)[i++] = c;
		}
		p1->itemkey[i] = 0;

 		
 		
 		if ( NULL == (p1->itemvalue_p = (char *)malloc(item_size + 1)) )
 		{
 			perror("Memory allocate for itemvalue error");
 			break;
 		}
 		else
 		{
 			memset(p1->itemvalue_p, 0, item_size + 1);
 		}
 		
 		fread(p1->itemvalue_p, item_size, 1, fp);
 	 	

 		p2->nextp = p1;
 		p2 = p1;
 		p1->nextp = NULL;
 	}/* end of for */

 	return 0;
 }
 
 /* print_APEv2Tag: 打印APEv2标签信息 */
 void print_APEv2Tag(APEv2Item *header)
 {
 	APEv2Item *p = header;
 	
 	if (NULL == p)
 		return;
 	
 	printf("--------------------->APEv2<---------------------\n");
 	while (p->nextp != NULL)
 	{
 		p = p->nextp;					/* 由于头结点不存信息，所以跳过头结点 */
 		printf("%s\t%s\n", p->itemkey, p->itemvalue_p);
 	}
 	printf("--------------------->APEv2<---------------------\n");
 }
 
 /* free_item_linkedtable:释放有标签元素结构体(APEv2Item)结构体组成的链表 */
 void free_item_linkedtable(APEv2Item *item_stp)
 {
 	//int counter = 0;
 	APEv2Item *p1 = NULL, *p2 = NULL;
 	
 	p1 = item_stp;
 	
 	if (NULL == p1)
 		return;
 		
 	do
 	{
 		p2 = p1->nextp;
 		
 		free(p1->itemvalue_p);
 		p1->itemvalue_p = NULL;
 		
 		free(p1);
 		//printf("APEv2-Node[%2d] is free!\n", counter);
 		//counter++;
 		
 	}while ( (p1 = p2) != NULL);
 	
 	p2 = p1 = item_stp = NULL;
 	
 }
 