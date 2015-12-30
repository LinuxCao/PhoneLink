 /************************************************************************
* File Name : pl_main.h
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description : phoneLink Main Process
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		Description
* 1        V0.1     2015/01/26      CaoLinfeng 	   CMFB Project                 phoneLink Main Process
*
************************************************************************/

/************************************************************************
* Multi-Include-Prevent Section
************************************************************************/
#ifndef PL_MAIN_H_
#define PL_MAIN_H_

int gPhoneLinkLogLevel ;

/************************************************************************
* Macro Define Section
************************************************************************/
/*PhoneLink LOG Set*/
#define PHONELINK_LEVEL_ERROR 	       0
#define PHONELINK_LEVEL_WARN 		1
#define PHONELINK_LEVEL_INFO 		2
#define PHONELINK_LEVEL_DEBUG		3

//ERROR
#define ERROR(format,...)  \
		do {						\
			if(gPhoneLinkLogLevel>=PHONELINK_LEVEL_ERROR) \
			{			\
				fprintf(stderr, "[Thread-%ld][PHONELINK_ERROR]",(long)pthread_self());\
				fprintf(stderr,format,##__VA_ARGS__); \
			}						\
			}							\
		while(0)
//WARN
#define WARN(format,...)  \
	do {						\
		if(gPhoneLinkLogLevel>=PHONELINK_LEVEL_WARN) \
		{									\
			fprintf(stdout, "[Thread-%ld][PHONELINK_WARN]", (long)pthread_self());\
			fprintf(stdout,format,##__VA_ARGS__); \
		}						\
	}							\
	while(0)


//INFO
#define INFO(format,...)  \
	do {				\
		if(gPhoneLinkLogLevel>=PHONELINK_LEVEL_INFO) \
		{									\
			fprintf(stdout, "[Thread-%ld][PHONELINK_INFO]",(long)pthread_self());\
			fprintf(stdout,format,##__VA_ARGS__); \
		}												\
	}													\
	while(0)


//DEBUG
#define DEBUG(format, ...)		\
		do {				\
			if(gPhoneLinkLogLevel>=PHONELINK_LEVEL_DEBUG) \
			{									\
				fprintf(stdout,format,##__VA_ARGS__); \
			}												\
		}													\
		while(0)


#endif /* PL_MAIN_H_ */
