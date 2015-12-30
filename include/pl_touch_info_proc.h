 /************************************************************************
* File Name : pl_touch_info_proc.h
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description :  Deal with Touch Message From HMI
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		     Description
* 1        V0.1     2015/01/26      CaoLinfeng 	    CMFB Project      	 Deal with Touch Message From HMI
*
************************************************************************/

/************************************************************************
* Multi-Include-Prevent Section
************************************************************************/
#ifndef PL_TOUCH_INFO_PROC_H_
#define PL_TOUCH_INFO_PROC_H_

/************************************************************************
* Prototype Declare Section
************************************************************************/
//Deal with Touch Message From HMI
int proc_touch_info(int x,int y,unsigned char action);

#endif /* TOUCH_INFO_PROC_H_ */
