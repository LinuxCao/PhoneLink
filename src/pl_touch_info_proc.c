 /************************************************************************
* File Name : pl_touch_info_proc.c
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
* Include File Section
************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "BtCSMsg.h"
#include "BtClient.h"
#include "pl_bt_hid_fun.h"
#include "pl_bluetooth_recive_thread.h"
#include "pl_phone_app_protocol.h"
#include "pl_touch_info_proc.h"
#include "pl_hmi_communication_proc.h"
#include "pl_main.h"

/************************************************************************
* Static Variable Define Section
************************************************************************/

/************************************************************************
* Function Define Section
************************************************************************/

 unsigned int GetPhoneLinkTickCount()
{
	struct timeval tv;
	if(gettimeofday(&tv,NULL)!=0)
	{
		return 0;
	}
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
 
//Deal with Touch Message From HMI
int proc_touch_info(int x,int y,unsigned char action)
{	
	//INFO("proc_touch_info:x=%d,y=%d,action=%d\n",x,y,action);
	int moveXtemp =0;  //x位移总距离
	int moveYtemp =0;  //y位移总距离
	pthread_mutex_lock(&touch_deal_proctect_mutex);
	pl_screen_operation_param.axisX = x;
	pl_screen_operation_param.axisY = y;

	static unsigned int sTouchMsgClickTick =0;
	unsigned int separateTime =0;
	
	//INFO("recv display_mode:%d,pl_screen_operation_param.phone_width:%d,pl_screen_operation_param.phone_height:%d\n",display_mode, pl_screen_operation_param.phone_width, pl_screen_operation_param.phone_height);
	//INFO("recv display_mode:%d,pl_screen_operation_param.vaild_axis_x_end:%d,pl_screen_operation_param.vaild_axis_y_end:%d\n",display_mode, pl_screen_operation_param.vaild_axis_x_end,   pl_screen_operation_param.vaild_axis_y_end);
	//INFO("recv display_mode:%d,pl_screen_operation_param.moveXfix:%1f,pl_screen_operation_param.moveYfix:%1f\n",display_mode, pl_screen_operation_param.moveXfix, pl_screen_operation_param.moveYfix);
	//INFO("Caolinfeng recv action=%d,axisX=%d,axisY=%d,axisXp=%d, axisYp=%d\n",action,pl_screen_operation_param.axisX,pl_screen_operation_param.axisY,pl_screen_operation_param.axisXp,pl_screen_operation_param.axisYp);
	
	//Reject Invalid TouchMsg From HMI.
	if(x<pl_screen_operation_param.vaild_axis_x_start
		||x>pl_screen_operation_param.vaild_axis_x_end
		||y<pl_screen_operation_param.vaild_axis_y_start
		||y>pl_screen_operation_param.vaild_axis_y_end)
	{
		INFO("Invalid TouchMsg\n");
		goto touch_msg_process_finish;
	}

	//Horizontal Mode
	if(display_mode==PHONE_HORIZONTAL)//横屏
	{
		if(action==0x02 )//move
		{
			separateTime =GetPhoneLinkTickCount() -sTouchMsgClickTick;
			//INFO("mytest separateTime:%d\r\n",separateTime);
			if(separateTime <80)
			{
				//INFO("touchmsg less than system tick(80ms).\r\n");
				goto touch_msg_process_finish;
			}

			//本次与上一次的坐标差，计算出本次x及y轴的位移总量
			moveXtemp=(pl_screen_operation_param.axisX-pl_screen_operation_param.axisXp)*pl_screen_operation_param.moveXfix;
			moveYtemp=(pl_screen_operation_param.axisY-pl_screen_operation_param.axisYp)*pl_screen_operation_param.moveYfix;
			//INFO("Caolinfeng add moveXtemp=%d,moveYtemp=%d\n",moveXtemp,moveYtemp);

			send_hid_mode_cotrol_data(2,0,moveXtemp*pl_screen_operation_param.moveXfix ,moveYtemp*pl_screen_operation_param.moveYfix);

			//使用完当前位移量，保存
			pl_screen_operation_param.axisXp = pl_screen_operation_param.axisX;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.axisY;

			sTouchMsgClickTick =GetPhoneLinkTickCount();
		}
		else if(action==0x01)//down
		{
			send_hid_mode_cotrol_data(1,0,x*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);

			//使用完当前位移量，保存
			pl_screen_operation_param.axisXp = pl_screen_operation_param.axisX;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.axisY;
		}
		else if(action ==0x00)//up
		{
			send_hid_mode_cotrol_data(0,0,x*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);
		}
	}
	else if(display_mode==PHONE_VERTICAL)//竖屏
	{
		if(action==0x02 )//move
		{
			//本次与上一次的坐标差，计算出本次x及y轴的位移总量
			moveXtemp=(pl_screen_operation_param.axisX-pl_screen_operation_param.axisXp)*pl_screen_operation_param.moveXfix;
			moveYtemp=(pl_screen_operation_param.axisY-pl_screen_operation_param.axisYp)*pl_screen_operation_param.moveYfix;
			INFO("Caolinfeng add moveXtemp=%d,moveYtemp=%d\n",moveXtemp,moveYtemp);

			send_hid_mode_cotrol_data(2,0,moveXtemp*pl_screen_operation_param.moveXfix ,moveYtemp*pl_screen_operation_param.moveYfix);

			//使用完当前位移量，保存
			pl_screen_operation_param.axisXp = pl_screen_operation_param.axisX;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.axisY;
		}
		else if(action==1)//down
		{
			send_hid_mode_cotrol_data(1,0,(x-pl_screen_operation_param.vaild_axis_x_start)*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);

			//使用完当前位移量，保存
			pl_screen_operation_param.axisXp = pl_screen_operation_param.axisX;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.axisY;

		}
		else if(action ==0)//up
		{
			send_hid_mode_cotrol_data(0,0,(x-pl_screen_operation_param.vaild_axis_x_start)*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);
		}
	} 
	goto touch_msg_process_finish;

	touch_msg_process_finish :
	pthread_mutex_unlock(&touch_deal_proctect_mutex);
	return 0;
}

 
//Deal with Double Touch Message From HMI
int proc_double_touch_info(int x,int y,unsigned char action,int x1,int y1,unsigned char action1)
{	
	//INFO("proc_double_touch_info:x=%d,y=%d,action=%d,x1=%d,y1=%d,action1=%d\n",x,y,action,x1,y1,action1);
	int moveXtemp =0;  //x位移总距离
	int moveYtemp =0;  //y位移总距离
	int moveX1temp =0;  //x位移总距离
	int moveY1temp =0;  //y位移总距离
	pthread_mutex_lock(&touch_deal_proctect_mutex);
	pl_screen_operation_param.axisX = x;
	pl_screen_operation_param.axisY = y;
	pl_screen_operation_param.axisX1 = x1;
	pl_screen_operation_param.axisY1 = y1;

	static unsigned int sDoubleTouchMsgClickTick =0;
	unsigned int separateTime =0;
	
	//INFO("recv display_mode:%d,pl_screen_operation_param.phone_width:%d,pl_screen_operation_param.phone_height:%d\n",display_mode, pl_screen_operation_param.phone_width, pl_screen_operation_param.phone_height);
	//INFO("recv display_mode:%d,pl_screen_operation_param.vaild_axis_x_end:%d,pl_screen_operation_param.vaild_axis_y_end:%d\n",display_mode, pl_screen_operation_param.vaild_axis_x_end,   pl_screen_operation_param.vaild_axis_y_end);
	//INFO("recv display_mode:%d,pl_screen_operation_param.moveXfix:%1f,pl_screen_operation_param.moveYfix:%1f\n",display_mode, pl_screen_operation_param.moveXfix, pl_screen_operation_param.moveYfix);
	//INFO("Caolinfeng recv action=%d,axisX=%d,axisY=%d,axisXp=%d, axisYp=%d\n",action,pl_screen_operation_param.axisX,pl_screen_operation_param.axisY,pl_screen_operation_param.axisXp,pl_screen_operation_param.axisYp);
	
	//Reject Invalid TouchMsg From HMI.
	if(x<pl_screen_operation_param.vaild_axis_x_start
		||x>pl_screen_operation_param.vaild_axis_x_end
		||y<pl_screen_operation_param.vaild_axis_y_start
		||y>pl_screen_operation_param.vaild_axis_y_end
		||x1<pl_screen_operation_param.vaild_axis_x_start
		||x1>pl_screen_operation_param.vaild_axis_x_end
		||y1<pl_screen_operation_param.vaild_axis_y_start
		||y1>pl_screen_operation_param.vaild_axis_y_end)
	{
		INFO("Invalid TouchMsg\n");
		goto touch_msg_process_finish;
	}

	//Horizontal Mode
	if(display_mode==PHONE_HORIZONTAL)//横屏
	{
		if(action==0x02 )//move
		{
			separateTime =GetPhoneLinkTickCount() -sDoubleTouchMsgClickTick;
			//INFO("mytest separateTime:%d\r\n",separateTime);
			if(separateTime <80)
			{
				//INFO("touchmsg less than system tick(80ms).\r\n");
				goto touch_msg_process_finish;
			}
			send_spp_mode_cotrol_data(2,0,x*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);
			send_spp_mode_cotrol_data(2,1,x1*pl_screen_operation_param.moveXfix ,y1*pl_screen_operation_param.moveYfix);

			sDoubleTouchMsgClickTick =GetPhoneLinkTickCount();
		}
		else if(action==0x01)//down
		{
			send_spp_mode_cotrol_data(1,0,x*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);
			send_spp_mode_cotrol_data(1,1,x1*pl_screen_operation_param.moveXfix ,y1*pl_screen_operation_param.moveYfix);
		}
		else if(action ==0x00)//up
		{
			send_spp_mode_cotrol_data(0,0,x*pl_screen_operation_param.moveXfix ,y*pl_screen_operation_param.moveYfix);
			send_spp_mode_cotrol_data(0,1,x1*pl_screen_operation_param.moveXfix ,y1*pl_screen_operation_param.moveYfix);
		}
	} 
	goto touch_msg_process_finish;

	touch_msg_process_finish :
	pthread_mutex_unlock(&touch_deal_proctect_mutex);
	return 0;
}

