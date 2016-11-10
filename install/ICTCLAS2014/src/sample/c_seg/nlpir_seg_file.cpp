// nlpir_seg_file.cpp : 定义控制台应用程序的入口点。
//

#include "../../include/NLPIR.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef OS_LINUX
#pragma comment(lib, "../../../bin/ICTCLAS2013/NLPIR.lib")
#else
#include<pthread.h>
#endif

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <time.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef OS_LINUX
#include <io.h>
#include <process.h>
#include <direct.h>
#include <assert.h>
#include <conio.h>
#define makeDirectory _mkdir
#pragma warning(disable:4786)
#define PATH_DELEMETER  "\\"
#define LONG64U __int64
#define DATE_DELEMETER  "/"
//#include <windows.h>
#define SLEEP(x) Sleep(x*1000)
#else
#include <dirent.h>
#include <ctype.h>
#include <unistd.h> 
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#define makeDirectory(x) mkdir(x, 0777)    // 这步骤必须，否则文件夹只读
//#define stricmp strcasecmp
//#define strnicmp strncasecmp
#define Sleep(x) sleep(x/1000)
#define min(a,b) (((a)<(b)) ? (a) : (b))
#define _stricmp(X,Y) strcasecmp((X),(Y))
#define stricmp(X,Y) strcasecmp((X),(Y))
#define strnicmp(X,Y,Z) strncasecmp((X),(Y),(Z))
#define _fstat(X,Y)     fstat((X),(Y))
#define _fileno(X)     fileno((X))
#define _stat           stat
#define _getcwd         getcwd
#define _off_t          off_t
#define PATH_DELEMETER  "/"
#define DATE_DELEMETER  "/"
#define LONG64U long long
#include <unistd.h>
#define SLEEP(x) sleep(x)
#endif

void SplitGBK(const char *sInput);
void SplitUTF8();
void testNewWord(int code);
long ReadFile(const char *sFilename,char **pBuffer);

/*********************************************************************
 *
 *  Func Name  : Read(const char *sFilename,char *pBuffer)
 *  Description: 
 *        Read file to a buffer and return the file size
 *              
 *
 *  Parameters : 
 *               sFilename: filename;
 *               pBuffer: buffer
 *
 *  Returns    : _off_t file size
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2003-11-28
 *********************************************************************/
long ReadFile(const char *sFilename,char **pBuffer)
{
	FILE *fp;
    struct _stat buf;
    if((fp=fopen(sFilename,"rb"))==NULL)
	   return 0;//read file false
    int nRet=_fstat( _fileno(fp), &buf );//Get file information
    if(nRet != 0)//File length of the lexicon file
	{
	   return 0;
	}
    *pBuffer = new char[buf.st_size+1];
    //The buffer for file
    if(*pBuffer == 0)
	{
	   return 0;
	}
    fread(*pBuffer, buf.st_size, 1, fp);//Read 
	(*pBuffer)[buf.st_size]=0;
    fclose(fp);
	return buf.st_size;
}



void testNewWord(int nCode)
{
	//初始化分词组件
	if(!NLPIR_Init("../../../",nCode))//数据在上一层目录下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	char sInputFile[1024]="../../../test/test.TXT",sResultFile[1024];
	if (nCode==UTF8_CODE)
	{
		strcpy(sInputFile,"../../../test/test-utf8.TXT");
	}

	//////////////////////////////////////////////////////////////////////////
	//下面开始测试关键词提取功能
	//////////////////////////////////////////////////////////////////////////
	const char *sResult=NLPIR_GetFileKeyWords(sInputFile,50,true);//从文本文件中提取关键词
	FILE *fp=fopen("Result.txt","wb");
	fprintf(fp,sResult);//将结果写入文件
	fclose(fp);
	//////////////////////////////////////////////////////////////////////////
	//下面开始测试从单个文件中提取新词的功能
	//////////////////////////////////////////////////////////////////////////
	sResult=NLPIR_GetFileNewWords(sInputFile);//从文本文件中提取新词
	fp=fopen("ResultNew.txt","wb");//将结果写入文件
	fprintf(fp,sResult);
	fclose(fp);



	//////////////////////////////////////////////////////////////////////////
	//下面开始测试从单个文件中提取新词的功能
	//////////////////////////////////////////////////////////////////////////
	NLPIR_NWI_Start();//新词识别开始
	NLPIR_NWI_AddFile(sInputFile);//批量增加输入文件，可以不断循环调用NLPIR_NWI_AddFile或者NLPIR_NWI_AddMem
	NLPIR_NWI_Complete();//新词识别导入文件结束
	const char *pNewWordlist=NLPIR_NWI_GetResult();//获取本次批量导入文本文件中识别的新词结果
	printf("识别出的新词为：%s\n",pNewWordlist);//打印输出新词识别结果
	
	strcpy(sResultFile,sInputFile);
	strcat(sResultFile,"_result1.txt");
	clock_t  lStart,lEnd;
	long lTime=0;
	int nLine=0;

	lStart=clock();//Record the time
	double speed=NLPIR_FileProcess(sInputFile,sResultFile,0);//普通的文本文件分词功能
	lEnd=clock();//Record the time
	lTime+=lEnd-lStart;

	float fTime=(float)lTime/(float)CLOCKS_PER_SEC;//Time cost
	printf("speed=%.2lfKB/s,time=%.2f\n",speed,fTime);
	
	NLPIR_NWI_Result2UserDict();//将上次新词识别的结果作为用户词典导入系统中，导入后的分词结果为自适应分词结果

	strcpy(sResultFile,sInputFile);
	strcat(sResultFile,"_result2.txt");
	NLPIR_FileProcess(sInputFile,sResultFile);//再次调用同样的函数，但是分词结果将自动适应新词结果
	
	NLPIR_Exit();//识别完成，系统退出，释放资源
}

void SplitGBK(const char *sInput)
{//分词演示

	//初始化分词组件
	if(!NLPIR_Init("../../../"))//数据在当前路径下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	NLPIR_SetPOSmap(PKU_POS_MAP_SECOND);//设置词性标注集合的类型，默认为计算所二级标注体系
	 const char* sample1 = "开心成长的烦恼学习深造中国证券市场";
	 const char* sResult1 = ICTCLAS_ParagraphProcess(sample1); 
	 printf("%s\n", sResult1); 
	 printf("%d\n", ICTCLAS_AddUserWord("开心 n"));
	 sResult1 = ICTCLAS_ParagraphProcess(sample1, 1); 
	 printf("%s\n", sResult1); 
	 printf("%d\n", ICTCLAS_DelUsrWord("开心"));
	 sResult1 = ICTCLAS_ParagraphProcess(sample1, 1); 
	 printf("%s\n", sResult1); 
	 printf("%d\n", ICTCLAS_AddUserWord("开心 n"));
	 sResult1 = ICTCLAS_ParagraphProcess(sample1, 1); 
	 printf("%s\n", sResult1); 


	 const char* sample2 = "不雅视频网上流传";
	 const char* sResult2 = ICTCLAS_ParagraphProcess(sample2);
	 printf("%s\n", sResult2); 
	 printf("%d\n", ICTCLAS_AddUserWord("不雅 ag")); 
	 sResult2 = ICTCLAS_ParagraphProcess(sample2, 1); 
	 printf("%s\n", sResult2);


	char sSentence[5000]="ICTCLAS在国内973专家组组织的评测中活动获得了第一名，在第一届国际中文处理研究机构SigHan组织的评测中都获得了多项第一名。";
	const char * sResult;


	//////////////////////////////////////////////////////////////////////////
	//下面开始测试NLPIR_ParagraphProcessA功能
	//////////////////////////////////////////////////////////////////////////
	int nCount;
	strcpy(sSentence,"张长虹指出，中国证券市场的下跌有特殊的原因，例如经济发展方向的调整和非流通股的消化问题，以及市场情绪的变化等。");
	const result_t *pResult=NLPIR_ParagraphProcessA(sSentence,&nCount);
	//分词的高级功能
	//该函数功能为：输入字符串，输出分词结果的result_t结构体数组，nCout为数组大小
	//返回的数组结果由系统自行维护，用户直接调用即可

	printf("nCount=%d\n",nCount);//分词结果数目

	int i=1;
	char *sWhichDic;
	char sWord[100];
	for(i=0;i<nCount;i++)
	{//逐个答应分词结果的内容
		sWhichDic="";
		switch (pResult[i].word_type)
		{
		case 0:
			sWhichDic = "核心词典";
			break;
		case 1:
			sWhichDic = "用户词典";
			break;
		case 2:
			sWhichDic = "专业词典";
			break;
		default:
			break;
		}
		strncpy(sWord,sSentence+pResult[i].start,pResult[i].length);
		sWord[pResult[i].length]=0;
		printf("No.%d:start:%d, length:%d,POS_ID:%d,Word_ID:%d, UserDefine:%s, Word:%s,Weight:%d\n",
			i+1, pResult[i].start, pResult[i].length, pResult[i].iPOS, pResult[i].word_ID, sWhichDic,
			sWord,pResult[i].weight );
	}

	//////////////////////////////////////////////////////////////////////////
	//下面开始测试用户词典功能
	//////////////////////////////////////////////////////////////////////////
	//导入用户词典前

	printf("未导入用户词典：\n");
	sResult = NLPIR_ParagraphProcess(sSentence, 1);
	printf("%s\n", sResult);

	//导入用户词典后
	printf("\n导入用户词典后：\n");
	nCount = NLPIR_ImportUserDict("userdic.txt");//userdic.txt覆盖以前的用户词典
	//保存用户词典
	printf("导入%d个用户词。\n", nCount);
	sResult = NLPIR_ParagraphProcess(sSentence, 1);
	printf("%s\n", sResult);

	strcpy(sSentence,"不雅视频东莞聚众淫乱");
	sResult = NLPIR_ParagraphProcess(sSentence, 1);
	printf("%s\n", sResult);

	//动态添加用户词
	printf("\n动态添加用户词后：\n");
	NLPIR_AddUserWord("不雅   ag");
	sResult = NLPIR_ParagraphProcess(sSentence, 1);
	printf("%s\n", sResult);


	//////////////////////////////////////////////////////////////////////////
	//下面开始测试对文件进行分词功能
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//下面开始测试基本的常用功能NLPIR_ParagraphProcess功能
	//////////////////////////////////////////////////////////////////////////
	while(_stricmp(sSentence,"q")!=0)
	{
		sResult = NLPIR_ParagraphProcess(sSentence);
		//测试最基本的功能，输入一个字符串，输出分词结果字符串，
		//如果不需要标注，调用的时候可以为： NLPIR_ParagraphProcess(sSentence,0)
		printf("%s\nInput string now('q' to quit)!\n", sResult);// 
		gets(sSentence);
	}
	//对文件进行分词
	NLPIR_FileProcess("test2.txt","test2_result.txt",1);
	NLPIR_FileProcess("testGBK.txt","testGBK_result.txt",1);

	//释放分词组件资源
	NLPIR_Exit();
}

void SplitGBK_Fanti_Old(const char *sInput)
{//分词演示

	//初始化分词组件
	if(!NLPIR_Init("",GBK_FANTI_CODE))//数据在当前路径下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}

	NLPIR_SetPOSmap(ICT_POS_MAP_SECOND);

	char sSentence[5000]="ICTCLAS在国内专家组组织的评测中活动获得了第一名，在第一届国际中文处理研究机构SigHan组织的评测中都获得了多项第一名。ICTCLAS在國內專家組組織的評測中活動獲得了第一名，在第一屆國際中文處理研究機構SigHan組織的評測中都獲得了多項第一名。";
	const char * sResult;


	int nCount;
	NLPIR_ParagraphProcessA(sSentence,&nCount);
	printf("nCount=%d\n",nCount);
	int count = NLPIR_GetParagraphProcessAWordCount(sSentence);
	const result_t *pResult=NLPIR_ParagraphProcessA(sSentence,&nCount);

	int i=1;
	char *sWhichDic;
	for(i=0;i<nCount;i++)
	{
		sWhichDic="";
		switch (pResult[i].word_type)
		{
		case 0:
			sWhichDic = "核心词典";
			break;
		case 1:
			sWhichDic = "用户词典";
			break;
		case 2:
			sWhichDic = "专业词典";
			break;
		default:
			break;
		}
		printf("No.%d:start:%d, length:%d,POS_ID:%d,Word_ID:%d, UserDefine:%s, Word:%s\n",
			i+1, pResult[i].start, pResult[i].length, pResult[i].iPOS, pResult[i].word_ID, sWhichDic,sSentence+pResult[i].start );
	}
	while(_stricmp(sSentence,"q")!=0)
	{
		sResult = NLPIR_ParagraphProcess(sSentence,0);
		printf("%s\nInput string now('q' to quit)!\n", sResult);// 
		scanf("%s",sSentence);
	}
	
	//导入用户词典前
	printf("未导入用户词典：\n");
	sResult = NLPIR_ParagraphProcess(sInput, 0);
	printf("%s\n", sResult);

	//导入用户词典后
	printf("\n导入用户词典后：\n");
	nCount = NLPIR_ImportUserDict("userdic.txt");//userdic.txt覆盖以前的用户词典
	//保存用户词典
	NLPIR_SaveTheUsrDic();
	printf("导入%d个用户词。\n", nCount);
	
	sResult = NLPIR_ParagraphProcess(sInput, 1);
	printf("%s\n", sResult);

	//动态添加用户词
	printf("\n动态添加用户词后：\n");
	NLPIR_AddUserWord("计算机学院   xueyuan");
	NLPIR_SaveTheUsrDic();
	sResult = NLPIR_ParagraphProcess(sInput, 1);
	printf("%s\n", sResult);


	//对文件进行分词
	NLPIR_FileProcess("test2.txt","test2_result.txt",1);
	NLPIR_FileProcess("testGBK.txt","testGBK_result.txt",1);


	//释放分词组件资源
	NLPIR_Exit();
}
void SplitBIG5()
{
	//初始化分词组件
	if(!NLPIR_Init("",BIG5_CODE))//数据在当前路径下，设置为BIG5编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	NLPIR_FileProcess("testBIG.txt","testBIG_result.txt");
	NLPIR_Exit();
}

void SplitUTF8()
{
	//初始化分词组件
	if(!NLPIR_Init("../../../",UTF8_CODE))//数据在当前路径下，设置为UTF8编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	printf("Hello 4\n");
	NLPIR_FileProcess("testUTF.txt","testUTF_result.txt");
//	NLPIR_FileProcess("test.xml","testUTF_result.xml");
	printf("Hello 3\n");

	FILE *fp=fopen("/home/yanfa/yanqiwei/proj/NLPIR/ICTCLAS2014/test/utf-8-offset.txt2","rt");
	if (fp==NULL)
	{
		printf("Error Open TestUTF8-bigfile.txt\n");
		NLPIR_Exit();
		return ;
	}
	FILE *fpResult=fopen("utf-8-offset-result.txt","wt");
	printf("Hello 2");
	char sLine[10241];
	int i,nCount,nDocID=1;
	result_t res[1000];
	//while (fgets(sLine,10240,fp))
	{
		CICTCLAS *pICTCLAS=new CICTCLAS;
		/*
		int nCountA=pICTCLAS->GetParagraphProcessAWordCount(sLine);
		pICTCLAS->ParagraphProcessAW(nCountA,res);
		for(i=0;i<nCountA;i++)
		{
			fprintf(fpResult,"No.%d:start:%d, length:%d,POS_ID:%d,Word_ID:%d\n",
				i+1, res[i].start, res[i].length, res[i].iPOS, res[i].word_ID);
		}
		*/
	
		fseek(fp,0,SEEK_END);
		int nSize=ftell(fp);

		printf("Hello 1");

		fseek(fp,0,SEEK_SET);
		fread(sLine,nSize,1,fp);
		sLine[nSize]=0;
		const result_t *pResult=pICTCLAS->ParagraphProcessA(sLine,&nCount);
	    i=1;

		for(i=0;i<500&&i<nCount;i++)
		{
 			fprintf(fpResult,"\nNo.%d:start:%d, length:%d,POS_ID:%d,Word_ID:%d\n",
 				i+1, pResult[i].start, pResult[i].length, pResult[i].iPOS, pResult[i].word_ID);
			fwrite(sLine+pResult[i].start,sizeof(char),pResult[i].length,fpResult);
		}
		delete pICTCLAS;
		fclose(fpResult);
		printf("Processed docs %d\r",nDocID++);
	}

	NLPIR_Exit();
}


void ImportUserDict(const char *sFilename)
{
	//初始化分词组件
	if(!NLPIR_Init("../../../"))//数据在上一层目录下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	int nCount=NLPIR_ImportUserDict(sFilename);//再次调用同样的函数，但是分词结果将自动适应新词结果
	printf("Import %d user defined items from file %s\n",nCount,sFilename);
	NLPIR_Exit();//识别完成，系统退出，释放资源
}
//int main(int argc,char *argv[])
//{
//	char *pFile;
//	if (argc<2)
//	{
//		SplitGBK("");
//		return 1;
//	}
//	switch(argv[1][0])
//	{
//
//	case 'b'://thread
//	case 'B'://thread
//		testBatchSpeed(argv[1]);
//		break;
//	case 'f'://thread
//	case 'F'://thread
//		SplitBIG5();
//		break;
//	case 'g'://thread
//	case 'G'://thread
//		SplitGBK("Test");
//		break;
//	case 'i'://import user dictionary
//	case 'I'://
//		pFile="../../../Data/Userdict.txt";
//		if (argc>2)
//		{
//			pFile=argv[2];
//		}
//		ImportUserDict(pFile);
//		break;
//	case 's'://thread
//	case 'S'://thread
//		testSpeed(0);
//		break;
//	case 'm'://thread
//	case 'M'://thread
//		//testMultiThread();
//		break;
//	case 'n'://thread
//	case 'N'://thread
//		testNewWord(0);
//		break;
//	case 'u'://thread
//	case 'U'://thread
//		printf("Hello Begin U");
//		SplitUTF8();
//		break;
//	}
//	return 0;
//}	

void SegGBK_File(const char *Input_file_path, const char *Output_file_path, int POStagged)
{
	//初始化分词组件
	if(!NLPIR_Init("../../../"))//数据在当前路径下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	NLPIR_FileProcess(Input_file_path, Output_file_path, POStagged);
	printf("Complete file %s !\n", Input_file_path);

	//释放分词组件资源
	NLPIR_Exit();
}

void SegGBK_Fanti(const char*Input_file_path, const char*Output_file_path, int POStagged)
{
	//初始化分词组件
	if(!NLPIR_Init("../../../",GBK_FANTI_CODE))//数据在当前路径下，默认为GBK编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	NLPIR_FileProcess(Input_file_path, Output_file_path, POStagged);
	printf("Complete file %s !\n", Input_file_path);
	//
	NLPIR_Exit();
}

void SegUTF8_Fanti(const char*Input_file_path, const char*Output_file_path, int POStagged)
{
	if(!NLPIR_Init("../../../",UTF8_FANTI_CODE))//数据在当前路径下，设置为UTF8编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return ;
	}
	NLPIR_FileProcess(Input_file_path, Output_file_path, POStagged);
	printf("Complete file %s !\n", Input_file_path);
	NLPIR_Exit();
}


void SegUTF8_File(const char *Input_file_path, const char *Output_file_path, int POStagged)
{
	//初始化分词组件
	if(!NLPIR_Init("../../../",UTF8_CODE))//数据在当前路径下，设置为UTF8编码的分词
	{
		printf("ICTCLAS INIT FAILED!\n");
		return	;
	}
	NLPIR_FileProcess(Input_file_path, Output_file_path, POStagged);
	printf("Complete file %s !\n", Input_file_path);

	NLPIR_Exit();
}


int main(int argc, char *argv[])
{
	if (argc<6)
	{
		printf("Usage: %s input_file_path output_file_path POStagged(0/1) file_lang file_encoding userdict_path\n", argv[0]);
		return -1;
	}
	int POStagged = atoi(argv[3])-1;
	if (POStagged!=0 && POStagged!=1){
		POStagged = 1;
		printf("1,seg;2,pos; get %s",POStagged);
	}
	if (strcmp(argv[4], "simplified") == 0)
	{
		if (strcmp(argv[5], "UTF8") == 0)
		{
			SegUTF8_File(argv[1], argv[2], POStagged);
			return 0;
		}else if(strcmp(argv[5], "GBK") == 0){
			SegGBK_File(argv[1], argv[2], POStagged);
			return 0;
		}
	}else if(strcmp(argv[4], "traditional") == 0)
	{
		if (strcmp(argv[5], "UTF8") == 0)
		{
			SegUTF8_Fanti(argv[1], argv[2], POStagged);
			return 0;
		}else if(strcmp(argv[5], "GBK") == 0)
		{
			SegGBK_Fanti(argv[1], argv[2], POStagged);
			return 0;
		} 
	}
	SegUTF8_File(argv[1], argv[2], POStagged);
}
