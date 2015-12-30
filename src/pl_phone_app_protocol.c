 /************************************************************************
* File Name : pl_phone_app_protocol.c
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description :  The Process Between PhoneLink And Android's DaLink
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		     Description
* 1        V0.1     2015/01/26      CaoLinfeng 	    CMFB Project       The Process Between PhoneLink And Android's DaLink
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
static unsigned short tranID =0;				
static sem_t dalink_ack_sem ;						
BT_SPP_DALINK_ACK m_ack_data[BT_SPP_ACK_MAX_PACKETS_LEN] ;
static int ack_data_read_point = 0;
static int ack_data_write_point = 0;
static PL_IPHONE_TIME phone_gps_time;
static int phone_protocol_recive_flag ;
static int heart_beat_flag = 0 ;
char dalink_spp_send_buf[255];

/************************************************************************
* Function Define Section
************************************************************************/
//Reset HeartBeat Flag
void reset_heart_beat_flag()
{
	heart_beat_flag =0;
}

//Set HeartBeat Flag
void set_heart_beat_flag()
{
	heart_beat_flag =1 ;
}

//Get HeartBeat Flag
int get_heart_beat_flag()
{
	return heart_beat_flag;
}

//Reset SmartPhone Protocol Recive Flag
int reset_phone_protocol_recive_flag()
{
	phone_protocol_recive_flag =0;
	return 0;
}

//Get SmartPhone Protocol Recive Flag
int  get_phone_protocol_recive_flag()
{
	return phone_protocol_recive_flag;
}

//Wakeup SmartPhone
int check_screen_state_and_wakeup()
{
	DEBUG("check_screen_state_and_wakeup phone_screen_state =%d \n",get_screen_state());
	if(get_screen_state() == 1)
	{
		set_screen_state(PHONE_SCREEN_ON);
	}
	return 0;
}


//Initialize DaLink Protocol ACK Semapore
int init_dalink_protocol_ack_sem()
{
	INFO("init_dalink_protocol_ack_sem\n");
	int ret = -1;
	ret = sem_init(&dalink_ack_sem, 0, 0);
	if (ret != 0) {
		ERROR("dalink_ack_sem init failed\n");
	}
	return ret;
}

//PhoneLink Send SPP Data To Smartphone's DaLink
int protected_spp_send_data(BT_SPP_PHONE_TYPE phone_type,unsigned char cDataLen ,unsigned char *pData)
{
	char  cmd_id = 0 ;
	unsigned short tran_id = 0 ;
	char d_flag = 0;
	int ret =0 ;
	int i =0 ;
	if(pData == NULL || cDataLen < 10)
	{
		WARN("data send to android error :pData =0x%x ,cDataLen=%d \n",(int )pData,cDataLen);
		return -1;
	}
	pthread_mutex_lock(&spp_send_proctect_mutex);

	d_flag = *(pData+BT_SPP_MESSAGE_DFLAG_INDEX);
	cmd_id = *(pData+BT_SPP_MESSAGE_CMD_TYPE_INDEX);
	tran_id = (short)(*(pData+BT_SPP_MESSAGE_TRAN_ID_HIGH_INDEX)<<8) |(short)(*(pData+BT_SPP_MESSAGE_TRAN_ID_LOW_INDEX)) ;
	resend_func:
	i++;
	if(i <= 3 )
	{

		sendSppData(cDataLen,pData);

		if(d_flag ==0x01)
		{
			ret = wait_dalink_ack(1000,tran_id,cmd_id);
			if(ret !=0)
			{
				goto resend_func;
			}
		}
	}
	else
	{
		WARN("protected_android_spp_send_data retry 3 times and have no ack \n");
		pthread_mutex_unlock(&spp_send_proctect_mutex);
		return -2 ;
	}
	pthread_mutex_unlock(&spp_send_proctect_mutex);
	return 0;
}

#if 0
int protected_iphone_spp_send_data(int ipod_handle,EA_PROTOCOL_INDEX protocol_index,int data_len ,char * data)
{
	int ret = -1;
	pthread_mutex_lock(&spp_send_proctect_mutex);
	int i= 0;
    DEBUG("%s protected_iphone_spp_send_data protocol_index =%d,len =%d :",__TIME__,protocol_index,data_len);
    for(i=0;i< data_len;i++)
     {
    	DEBUG("0X%2X ",data[i]);
     }
    DEBUG("\n");
	ret  = ipod_send_data_to_app(ipod_handle,protocol_index,data_len,data);
	pthread_mutex_unlock(&spp_send_proctect_mutex);
	return ret;
}

int  identification_android_connect()
{
	BTCSMSG bt_Msg;

	int identification_flag = -1;
	int ret = 0;
	//获取分辨率
	getResolution:INFO("获取分辨率\n");

	pl_dalink_send_request_phoneinfo_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_INFO_RESOLUTION);
	ret = wait_bt_command(200,&bt_Msg);
	if(ret ==0 && bt_Msg.eEventType == BT_MSG_SPP_REVICE_DATA)
	{
		identification_flag =0;
		INFO("----------------identification_flag =0 set----------- \n");
		phone_app_msg_deal(bt_Msg.nDataLen ,(char *)bt_Msg.szData);
	}
	else if(ret ==0 && bt_Msg.eEventType != BT_MSG_SPP_REVICE_DATA)
	{
		goto getResolution;
	}
	else
	{
		INFO("获取分辨率超时 \n");
	}
	usleep(100000);
	getOrientation :INFO("获取横竖屏\n");
	//获取横竖屏
	pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_DIRECTION);
	ret =wait_bt_command(200,&bt_Msg);
	if(ret ==0 && bt_Msg.eEventType == BT_MSG_SPP_REVICE_DATA)
	{
		phone_app_msg_deal(bt_Msg.nDataLen ,(char *)bt_Msg.szData);
	}
	else if(ret ==0 && bt_Msg.eEventType != BT_MSG_SPP_REVICE_DATA)
	{
		goto getOrientation;
	}
	else
	{
		INFO("获取横竖屏超时 \n");
	}
	usleep(100000);
	getConnectmode:INFO("获取连接方式\n");
	//获取连接方式
	pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_CONTRIL_MODE);
	ret = wait_bt_command(200,&bt_Msg);
	if(ret ==0 && bt_Msg.eEventType == BT_MSG_SPP_REVICE_DATA)
	{
		phone_app_msg_deal(bt_Msg.nDataLen ,(char *)bt_Msg.szData);
		if(phone_current_mode != BT_CONNECTED_ANDROID_SPP && phone_current_mode != BT_CONNECTED_ANDROID_HID)
		{
			goto getConnectmode;
		}
	}
	else if(ret ==0 && bt_Msg.eEventType != BT_MSG_SPP_REVICE_DATA)
	{
		goto getConnectmode;
	}
	else
	{
		INFO("获取连接方式超时 \n");
	}
	usleep(100000);

	getMHLConnectState:INFO("获取MHL状态\n");
	//获取连接方式
#ifdef _NEW_SPP_PROTOCOL_
	pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_HDMI_STATE);

#endif
	ret = wait_bt_command(200,&bt_Msg);
	if(ret ==0 && bt_Msg.eEventType == BT_MSG_SPP_REVICE_DATA)
	{
		phone_app_msg_deal(bt_Msg.nDataLen ,(char *)bt_Msg.szData);
	}
	else if(ret ==0 && bt_Msg.eEventType != BT_MSG_SPP_REVICE_DATA)
	{
		goto getMHLConnectState;
	}
	else
	{
		INFO("获取MHL连接状态方式超时 \n");
	}
	usleep(10000);
	return identification_flag;
}


//
int write_ea_sem_data(EA_DATA_RECEIVED * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("write_ea_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(ea_data_rev_arrary+ea_data_rev_arrary_write_point,pMsg,sizeof(EA_DATA_RECEIVED));
		if(ea_data_rev_arrary_write_point>=MAX_EA_MSG_CATCH_PARCKET_LEN-1)
		{
			ea_data_rev_arrary_write_point=0;
		}
		else
		{
			ea_data_rev_arrary_write_point+=1;
		}
		sem_post(&ea_data_recive_sem);
		DEBUG("ea_data_rev_arrary_write_point=%d\n",ea_data_rev_arrary_write_point);
		return 0;
}

//
int read_ea_sem_data(EA_DATA_RECEIVED * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("read_ea_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(pMsg,ea_data_rev_arrary+ea_data_rev_arrary_read_point,sizeof(EA_DATA_RECEIVED));
		if(ea_data_rev_arrary_read_point>=MAX_EA_MSG_CATCH_PARCKET_LEN-1)
		{
			ea_data_rev_arrary_read_point=0;
		}
		else
		{
			ea_data_rev_arrary_read_point+=1;
		}
		DEBUG("ea_data_rev_arrary_read_point=%d\n",ea_data_rev_arrary_read_point);
		return 0;
}

//
void iphone_ea_data_proc(EA_PROTOCOL_INDEX protocol_index,int len ,char * buf)
{
	EA_DATA_RECEIVED ea_data_rev;
	ea_data_rev.protocol_index = protocol_index;
	ea_data_rev.len = len;
	if(len >255)
	{
		WARN("iphone_ea_data_proc receive data too long .and len = %d\n",len);
		return ;
	}
	memcpy(ea_data_rev.buf,buf,len);
	write_ea_sem_data(&ea_data_rev);

	return ;
}

//
void *ea_msg_process_thread(void *arg)
{
	INFO("ea_msg_process_thread start\n");
	int ret =-1;
	EA_DATA_RECEIVED ea_data_rev;
	ea_data_rev_arrary_read_point =0;
	ea_data_rev_arrary_write_point =0;
	//初始化EA数据接收信号量
	ret = sem_init(&ea_data_recive_sem, 0, 0);

	if (ret != 0) {
		ERROR("ea_data_recive_sem init failed\n");
	}
	INFO("ea_msg_process_thread start!\n");
	while(1)
	{
		ret = sem_wait(&ea_data_recive_sem);
		if(ret == 0 )
		{
			read_ea_sem_data(&ea_data_rev);
			if(ea_data_rev.protocol_index == HSAE_PROTOCOL_INDEX)
			{
				phone_app_msg_deal(ea_data_rev.len,ea_data_rev.buf);
			}
			else if(ea_data_rev.protocol_index == AUTO_NAVI_PROTOCOL_INDEX)
			{
				//autonavi_recive_data_process(ea_data_rev.buf,ea_data_rev.len);
			}

		}
	}
	sem_destroy(&ea_data_recive_sem);
	return 0;
}

//PhoneLink Send Start DaLink CMD To SmartPhone
int pl_enter_dalink_mainmenu()
{
	int ret = -1;
	char data  = BT_SPP_MESSAGE_PHONE_START_DALINK ;
	INFO("pl_enter_dalink_mainmenu \n");
 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_ENTER_IPHONE_MAINMENU,tranID++,D_FLAG_NEED_ACK,&data,1,dalink_spp_send_buf);
 	if(get_phone_type() == BT_CONNECT_IPHONE)
 	{
 		protected_spp_send_data(BT_SPP_IPHONE,ret,(unsigned char *)dalink_spp_send_buf);

 		usleep(20000);
 	}
 	return ret ;
}

int pl_iphone_reset_ea_comm()
{
	int ret = -1;
	char data  = BT_SPP_MESSAGE_PHONE_RESET_EA_COMM ;
	INFO("pl_iphone_reset_ea_comm \n");
 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_ENTER_IPHONE_MAINMENU,tranID++,D_FLAG_NEED_ACK,&data,1,dalink_spp_send_buf);
 	if(get_phone_type() == BT_CONNECT_IPHONE)
 	{
 		protected_spp_send_data(BT_SPP_IPHONE,ret,(unsigned char *)dalink_spp_send_buf);

 		usleep(20000);
 	}
 	return ret ;
}

int dalink_spp_phone_dirction_data_dispose(char dirction)
{
	INFO("dalink_spp_phone_dirction_data_dispose  enter\n");
	phone_protocol_recive_flag |= SPP_MSG_SCREEN_DIRECTION_BIT;
	if(get_phone_type() != BT_CONNECT_IPHONE)
	{
		if(dirction == 1 || dirction==3)//horizontal mode
		{
			display_mode = PHONE_HORIZONTAL;
			pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
			pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
			pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
			pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

			pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
			pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;

			INFO("手机当前横屏\n");
			DEBUG("phone_height =%d,phone_width=%d,vaild_axis_x_start=%d,vaild_axis_x_end=%d,vaild_axis_y_start=%d,vaild_axis_y_end=%d\n",
														pl_screen_operation_param.phone_height,pl_screen_operation_param.phone_width,
														pl_screen_operation_param.vaild_axis_x_start,pl_screen_operation_param.vaild_axis_x_end,
														pl_screen_operation_param.vaild_axis_y_start,pl_screen_operation_param.vaild_axis_y_end);
			DEBUG("moveXfix=%f,moveYfix=%f\n",pl_screen_operation_param.moveXfix,pl_screen_operation_param.moveYfix);
			if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
			{
				bt_mouse_adjust();
				pl_screen_operation_param.axisXp = pl_screen_operation_param.vaild_axis_x_end;
				pl_screen_operation_param.axisYp = pl_screen_operation_param.vaild_axis_y_end;
				pl_screen_operation_param.axisX = pl_screen_operation_param.vaild_axis_x_end;
				pl_screen_operation_param.axisY = pl_screen_operation_param.vaild_axis_y_end;
			}
		}
		else//vertical mode
		{
			display_mode = PHONE_VERTICAL;
			if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
			{
				pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/(float)pl_screen_operation_param.screenHeight;							//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
				pl_screen_operation_param.moveYfix = pl_screen_operation_param.moveXfix;
				pl_screen_operation_param.vaild_axis_x_start =(pl_screen_operation_param.screenWidth -((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix))/2 ;		//屏幕的有效操作范围--x轴起始
				pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.vaild_axis_x_start+((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix);		//屏幕的有效操作范围--x轴结束
				pl_screen_operation_param.vaild_axis_y_start =0;							//屏幕的有效操作范围--y轴起始
				pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;				//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_
				bt_mouse_adjust();
				pl_screen_operation_param.axisXp = pl_screen_operation_param.vaild_axis_x_end;
				pl_screen_operation_param.axisYp = pl_screen_operation_param.vaild_axis_y_end;
				pl_screen_operation_param.axisX = pl_screen_operation_param.vaild_axis_x_end;
				pl_screen_operation_param.axisY = pl_screen_operation_param.vaild_axis_y_end;
			}
			else if(phone_current_mode == BT_CONNECTED_ANDROID_SPP)
			{
				pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
				pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
				pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
				pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

				pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
				pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;
			}
			INFO("手机当前竖屏\n");
		}
	}
	else
	{
		if(dirction==0 || dirction==2)		//竖屏
		{
			display_mode = PHONE_VERTICAL;
			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				if(pl_screen_operation_param.phone_width == 1136)			//iphone5
				{
					send_adjust_msg(100,IPHONE5_VERTICAL);
				}
				else								//iphone4s
				{
					send_adjust_msg(100,IPHONE4S_VERTICAL);
				}
			}
			else
			{
				if(pl_screen_operation_param.phone_width == 1136)			//iphone5
				{
					init_touch_info(0,IPHONE5_VERTICAL);
				}
				else								//iphone4s
				{
					init_touch_info(0,IPHONE4S_VERTICAL);
				}
			}

		}
		else if(dirction==1 || dirction==3)		//横屏
		{
			display_mode = PHONE_HORIZONTAL;
			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				if(pl_screen_operation_param.phone_width == 1136)
				{
					send_adjust_msg(100,IPHONE5_HORIZONTAL);
				}
				else
				{
					send_adjust_msg(100,IPHONE4S_HORIZONTAL);
				}
			}
			else
			{
				if(pl_screen_operation_param.phone_width == 1136)
				{
					init_touch_info(0,IPHONE5_HORIZONTAL);
				}
				else
				{
					init_touch_info(0,IPHONE4S_HORIZONTAL);
				}
			}

		}
	}
	return 0;
}

//PhoneLink Deal With SmartPhone's Screen Direction From DaLink
int dalink_spp_phone_control_mode_dispose(char control_mode)
{
	phone_protocol_recive_flag |= SPP_MSG_CONTROL_MODE_BIT;
	if(get_phone_type() != BT_CONNECT_IPHONE)
	{
		//modify 2014.02.19 防止在用户切换操作模式，然后MHL换手机，会闪一下连接正常
		//phone_link_video_table_id = MHL_720P;
		//send_connect_stautus(PHONE_CONNECTED,phone_link_video_table_id);

		if(control_mode == 1 && is_android_phone_support_spp_control())		//switch spp
		{
				INFO("当前处于android SPP控制模式\n");
				phone_current_mode = BT_CONNECTED_ANDROID_SPP;
				//sendPhoneType(BT_CONNECT_ANDROID);
				pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
				pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
				pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
				pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

				pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
				pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;

		}
		else //switch hid
		{
			INFO("当前处于android HID控制模式\n");
			phone_current_mode = BT_CONNECTED_ANDROID_HID;
			//sendPhoneType(BT_CONNECT_ANDROID);
			if(display_mode == PHONE_HORIZONTAL)
			{
				pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
				pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
				pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
				pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

				pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
				pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;
			}
			else
			{
				pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/(float)pl_screen_operation_param.screenHeight;							//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
				pl_screen_operation_param.moveYfix = pl_screen_operation_param.moveXfix;
				pl_screen_operation_param.vaild_axis_x_start =(pl_screen_operation_param.screenWidth -((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix))/2 ;		//屏幕的有效操作范围--x轴起始
				pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.vaild_axis_x_start+((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix);		//屏幕的有效操作范围--x轴结束
				pl_screen_operation_param.vaild_axis_y_start =0;							//屏幕的有效操作范围--y轴起始
				pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;				//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_
			}
			//bt_mouse_adjust();								//防止按电源键黑屏后hid唤醒屏幕
			pl_screen_operation_param.axisXp = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.vaild_axis_y_end;
			pl_screen_operation_param.axisX = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisY = pl_screen_operation_param.vaild_axis_y_end;
		}
	}
	else
	{
		if(control_mode == 0)			//HID模式
		{
			INFO("当前处于iphone HID控制模式\n");
			phone_current_mode = BT_CONNECT_IPOD_HID;
			//sendPhoneType(BT_CONNECT_IPHONE);
//			//////////////////////此处由于iphone没有发送app 退出/////////////////////////////////
//			if(iphone_auto_navi_start_flag == 0)
//			{
//				phonelink_dalink_status =DALINK_BACKGROUND;
//				sendDalinkStatus(phonelink_dalink_status);
//			}
//			////////////////////////////////////////////////////////

			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				//i_init_assistivetouch_func(0);

				//i_uninit_keyoboard_func(0);
				if(pl_screen_operation_param.phone_width == 1136)			//iphone5
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						send_adjust_msg(100,IPHONE5_HORIZONTAL);
					}
					else
					{
						send_adjust_msg(100,IPHONE5_VERTICAL);
					}
				}
				else								//iphone4s
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						send_adjust_msg(100,IPHONE4S_HORIZONTAL);
					}
					else
					{
						send_adjust_msg(100,IPHONE4S_VERTICAL);
					}
				}
			}
			else
			{
				if(pl_screen_operation_param.phone_width == 1136)			//iphone5
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						init_touch_info(0,IPHONE5_HORIZONTAL);
					}
					else
					{
						init_touch_info(0,IPHONE5_VERTICAL);
					}
				}
				else								//iphone4s
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						init_touch_info(0,IPHONE4S_HORIZONTAL);
					}
					else
					{
						init_touch_info(0,IPHONE4S_VERTICAL);
					}
				}
			}
		}
		else if(control_mode == 1)		//SPP模式
		{
			INFO("当前处于iphone SPP 控制模式\n");
			phone_current_mode = BT_CONNECT_IPOD_SPP;
			//sendPhoneType(BT_CONNECT_IPHONE);
			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				//i_close_assistive_func(0);
				//i_init_keyoboard_func(0);
			}


		}
	}
	return 0;
}
int dalink_spp_phone_dalink_app_state_dispose(char state)
{
	phone_protocol_recive_flag |= SPP_MSG_DALINK_STATUS_BIT;
	INFO("dalink_spp_phone_dalink_app_state_dispose state =%d\n",state);
	if(get_phone_type() == BT_CONNECT_IPHONE)
	{

		phonelink_dalink_status = state;

		if(state == 0)		//前台
		{
			set_screen_state(PHONE_SCREEN_ON);
			//sendPhoneScreenState(PHONE_SCREEN_ON);			//add by jichunfan 2014.08.22 send to HMI APP
			iphone_auto_navi_start_flag = 0;
			iphone_current_app_status = IPHONE_DALINK_ACTIVITY;
			display_mode = PHONE_HORIZONTAL;
			INFO("APP is reliable! iphone DALINK在 前台 \n");
			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				//i_close_assistive_func(0);
				//i_init_keyoboard_func(0);
			}
			//sendPhoneType(BT_CONNECT_IPHONE);
			sendDalinkStatus(DALINK_FORGROUND);
			usleep(1000);

		}
		else if(state == 1 )	//后台
		{
			iphone_current_app_status = IPHONE_OTHER_APP_ACTIVITY;
			INFO("APP is not reliable!iphone DALINK在 后台 \n");
			display_mode = PHONE_VERTICAL;
			phone_current_mode = BT_CONNECT_IPOD_HID;
			//sendPhoneType(BT_CONNECT_IPHONE);
			//if(iphone_auto_navi_start_flag == 0)
			{
				phonelink_dalink_status =DALINK_BACKGROUND;
				sendDalinkStatus(phonelink_dalink_status);
				phonelink_dalink_mainmenu_status = DALINK_MAIN_MENU_BACKGROUND;
				sendDalinkMainmenuStatus(phonelink_dalink_mainmenu_status);
			}

			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				//i_init_assistivetouch_func(0);
				//i_uninit_keyoboard_func(0);
			}

			if(get_iphone_control_mode() == IPHONE_ASSISTIVE_TOUCH_MDOE)
			{
				if(pl_screen_operation_param.phone_width == 1136)			//iphone5
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						send_adjust_msg(500,IPHONE5_HORIZONTAL);
					}
					else
					{
						send_adjust_msg(500,IPHONE5_VERTICAL);
					}
				}
				else
				{
					if(display_mode == PHONE_HORIZONTAL)
					{
						send_adjust_msg(500,IPHONE4S_HORIZONTAL);
					}
					else
					{
						send_adjust_msg(500,IPHONE4S_VERTICAL);
					}
				}
			}
			else
			{
				if(pl_screen_operation_param.phone_width == 1136)
				{
					init_touch_info(0,IPHONE5_VERTICAL);
				}
				else
				{
					init_touch_info(0,IPHONE4S_VERTICAL);
				}
			}
		}
		//收到dalink前后台状态和MHL状态后发送连接成功消息modify 2014.06.04 by jichunfan
		if((get_current_phone_connect_status() == PHONE_CONNECTED_SUCCESS) &&
				(phone_protocol_recive_flag&SPP_MSG_MHL_STATUS_BIT))
		{
			//send_connect_stautus(PHONE_CONNECTED,phone_link_video_table_id);
			send_connect_stautus(PHONE_CONNECTED);
		}
	}
	return 0;
}
int dalink_spp_phone_hdmi_state_dispose(char state)
{
	phone_protocol_recive_flag |= SPP_MSG_MHL_STATUS_BIT;
	if(state == 0)
	{
		INFO("HML已断开\n");
	}
	else
	{
		INFO("HML已连接\n");
	}


	if(state == 0)
	{
		phone_link_video_table_id = HDMI_NO_PHONE_INSERT;
	}
	else
	{
		if(get_phone_type() != BT_CONNECT_IPHONE)
		{
			phone_link_video_table_id = MHL_720P;
			//sendPhoneType(BT_CONNECT_ANDROID);

		}
		else
		{
			phone_link_video_table_id = HDMI_1080P;
			//sendPhoneType(BT_CONNECT_IPHONE);
		}
	}
	//收到dalink前后台状态（或可信任状态）、并且蓝牙协议栈链接Ok，发送连接成功消息modify 2014.06.04 by jichunfan
	if(get_current_phone_connect_status() == PHONE_CONNECTED_SUCCESS &&
			((phone_protocol_recive_flag&SPP_MSG_RELIABLE_STATUS_BIT)||
			  (phone_protocol_recive_flag&SPP_MSG_DALINK_STATUS_BIT)
			))
	{
		if(get_phone_type() == BT_CONNECT_ANDROID)
		{
			//sendDalinkMainmenuStatus(phonelink_dalink_mainmenu_status);
			//sendDalinkStatus(phonelink_dalink_status);
		}
		//send_connect_stautus(PHONE_CONNECTED,phone_link_video_table_id);
		send_connect_stautus(PHONE_CONNECTED);
	}

	return 0;
}

int dalink_spp_phone_reliable_state_dispose(char state)
{
	phone_protocol_recive_flag |= SPP_MSG_RELIABLE_STATUS_BIT;
	phonelink_dalink_status = state;
	if(state == 0)
	{
		INFO("APP is reliable!可信任 app\n");
	}
	else
	{
		INFO("APP is not reliable!不可信任 app\n");

	}
	//sendDalinkStatus(phonelink_dalink_status);
	if((get_current_phone_connect_status() == PHONE_CONNECTED_SUCCESS)&&
			(phone_protocol_recive_flag&SPP_MSG_MHL_STATUS_BIT))
	{
		//send_connect_stautus(PHONE_CONNECTED,phone_link_video_table_id);
		//send_connect_stautus(PHONE_CONNECTED);
	}
	return 0;
}
#endif

//PhoneLink Deal With SPP Data From SmartPhone's DaLink 
int phone_app_msg_deal(int len ,char * buf)
{

	int i =0;

	int ret = -1;
	char * packet_header;
	DEBUG("%s recive spp data len = %d,and buf :",__TIME__,len);
	for(i=0;i<len;i++)
	{
		DEBUG("0X%X ",buf[i]);
	}
	DEBUG("\n");
	i = 0;
	while(i< len )
	{
		ret = dalink_check_packet_integrity(buf+i,len-i,&packet_header);
		if(ret >=0)
		{
			//INFO("SPP revice one frame \n");
			i+=ret;
			dalink_spp_packet_deal(packet_header,len);

		}
		else
		{
			INFO("SPP recice data check is not a ringht frame\n");
			return 0;
		}
	}

	return 0;
}




//PhoneLink Deal With SmartPhone's Resolution From DaLink
int dalink_spp_resolution_data_dispose(char * buf )
{
	int ratioWidth =0;
	int ratioHeight =0;
	phone_protocol_recive_flag |= SPP_MSG_SCREEN_RESOLUTION_BIT;
	ratioWidth = ((int )buf[0])<<8 |(int )buf[1];
	ratioHeight= ((int )buf[2])<<8 |(int )buf[3];
	INFO("dalink_spp_resolution_data_dispose ratioWidth=%d,ratioHeight=%d\n",ratioWidth,ratioHeight);
	
	if(ratioHeight > ratioWidth) //vertical
	{
		display_mode = PHONE_VERTICAL;
		pl_screen_operation_param.phone_height = ratioWidth;
		pl_screen_operation_param.phone_width = ratioHeight;
		pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/(float)pl_screen_operation_param.screenHeight;							//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
		pl_screen_operation_param.moveYfix = pl_screen_operation_param.moveXfix;
		pl_screen_operation_param.vaild_axis_x_start =(pl_screen_operation_param.screenWidth -((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix))/2 ;		//屏幕的有效操作范围--x轴起始
		pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.vaild_axis_x_start+((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix);		//屏幕的有效操作范围--x轴结束
		pl_screen_operation_param.vaild_axis_y_start =0;							//屏幕的有效操作范围--y轴起始
		pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;				//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_
	}
	else if(ratioWidth>ratioHeight)//horizontal 
	{
		display_mode = PHONE_HORIZONTAL;
		pl_screen_operation_param.phone_height = ratioHeight;
		pl_screen_operation_param.phone_width = ratioWidth;
		pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
		pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
		pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
		pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_
		pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width / (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
		pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height /(float)pl_screen_operation_param.screenHeight;
	}
	INFO("recv display_mode:%d,pl_screen_operation_param.phone_width:%d,pl_screen_operation_param.phone_height:%d\n",display_mode, pl_screen_operation_param.phone_width, pl_screen_operation_param.phone_height);
	INFO("recv display_mode:%d,pl_screen_operation_param.vaild_axis_x_end:%d,pl_screen_operation_param.vaild_axis_y_end:%d\n",display_mode, pl_screen_operation_param.vaild_axis_x_end,   pl_screen_operation_param.vaild_axis_y_end);
	INFO("recv display_mode:%d,pl_screen_operation_param.moveXfix:%1f,pl_screen_operation_param.moveYfix:%1f\n",display_mode, pl_screen_operation_param.moveXfix, pl_screen_operation_param.moveYfix);
	return 0;
}

//PhoneLink Deal With SmartPhone's Screen Direction From DaLink
int dalink_spp_phone_dirction_data_dispose(char dirction)
{
	INFO("dalink_spp_phone_dirction_data_dispose dirction=%d\n",dirction);
	phone_protocol_recive_flag |= SPP_MSG_SCREEN_DIRECTION_BIT;

	if(dirction == 1 || dirction==3)//horizontal mode
	{
		INFO("Current Phone is Horizontal Sreen\n");
		display_mode = PHONE_HORIZONTAL;
		pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
		pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
		pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
		pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

		pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
		pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;

		DEBUG("phone_height =%d,phone_width=%d,vaild_axis_x_start=%d,vaild_axis_x_end=%d,vaild_axis_y_start=%d,vaild_axis_y_end=%d\n",
													pl_screen_operation_param.phone_height,pl_screen_operation_param.phone_width,
													pl_screen_operation_param.vaild_axis_x_start,pl_screen_operation_param.vaild_axis_x_end,
													pl_screen_operation_param.vaild_axis_y_start,pl_screen_operation_param.vaild_axis_y_end);
		DEBUG("moveXfix=%f,moveYfix=%f\n",pl_screen_operation_param.moveXfix,pl_screen_operation_param.moveYfix);
		if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
		{
			bt_mouse_adjust();
			pl_screen_operation_param.axisXp = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.vaild_axis_y_end;
			pl_screen_operation_param.axisX = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisY = pl_screen_operation_param.vaild_axis_y_end;
		}
		
		if(get_screen_direction_status()!=HMI_SEND_PHONE_HORIZONTAL_CMD)
		{
			send_screen_direction_status(HMI_SEND_PHONE_HORIZONTAL_CMD);	
			set_screen_direction_status(HMI_SEND_PHONE_HORIZONTAL_CMD);
		}
	}
	else//vertical mode
	{
		INFO("Current Phone is Vertical Screen\n");
		display_mode = PHONE_VERTICAL;
		if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
		{
			pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/(float)pl_screen_operation_param.screenHeight;							//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
			pl_screen_operation_param.moveYfix = pl_screen_operation_param.moveXfix;
			pl_screen_operation_param.vaild_axis_x_start =(pl_screen_operation_param.screenWidth -((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix))/2 ;		//屏幕的有效操作范围--x轴起始
			pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.vaild_axis_x_start+((float)pl_screen_operation_param.phone_height/pl_screen_operation_param.moveXfix);		//屏幕的有效操作范围--x轴结束
			pl_screen_operation_param.vaild_axis_y_start =0;							//屏幕的有效操作范围--y轴起始
			pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;				//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_
			bt_mouse_adjust();
			pl_screen_operation_param.axisXp = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisYp = pl_screen_operation_param.vaild_axis_y_end;
			pl_screen_operation_param.axisX = pl_screen_operation_param.vaild_axis_x_end;
			pl_screen_operation_param.axisY = pl_screen_operation_param.vaild_axis_y_end;
		}
		else if(phone_current_mode == BT_CONNECTED_ANDROID_SPP)
		{
			pl_screen_operation_param.vaild_axis_x_start =0;					//屏幕的有效操作范围--x轴起始
			pl_screen_operation_param.vaild_axis_x_end = pl_screen_operation_param.screenWidth;			//屏幕的有效操作范围--x轴结束
			pl_screen_operation_param.vaild_axis_y_start =0;					//屏幕的有效操作范围--y轴起始
			pl_screen_operation_param.vaild_axis_y_end =pl_screen_operation_param.screenHeight ;		//屏幕的有效操作范围--y轴结束#define TOUCH_INFO_PROC_H_

			pl_screen_operation_param.moveXfix = (float)pl_screen_operation_param.phone_width/ (float)pl_screen_operation_param.screenWidth;		//此处参数需要根据手机分辨率而定,iphone4s=2  960/480 =2
			pl_screen_operation_param.moveYfix = (float)pl_screen_operation_param.phone_height/(float)pl_screen_operation_param.screenHeight;
		}
		if(get_screen_direction_status()!=HMI_SEND_PHONE_VERTICAL_CMD)
		{
			send_screen_direction_status(HMI_SEND_PHONE_VERTICAL_CMD);	
			set_screen_direction_status(HMI_SEND_PHONE_VERTICAL_CMD);
		}
	}
	
	
	INFO("recv display_mode:%d,pl_screen_operation_param.phone_width:%d,pl_screen_operation_param.phone_height:%d\n",display_mode, pl_screen_operation_param.phone_width, pl_screen_operation_param.phone_height);
	INFO("recv display_mode:%d,pl_screen_operation_param.vaild_axis_x_start:%d,pl_screen_operation_param.vaild_axis_x_end:%d\n",display_mode, pl_screen_operation_param.vaild_axis_x_start,   pl_screen_operation_param.vaild_axis_x_end);
	INFO("recv display_mode:%d,pl_screen_operation_param.vaild_axis_y_start:%d,pl_screen_operation_param.vaild_axis_y_end:%d\n",display_mode, pl_screen_operation_param.vaild_axis_y_start,   pl_screen_operation_param.vaild_axis_y_end);
	INFO("recv display_mode:%d,pl_screen_operation_param.moveXfix:%1f,pl_screen_operation_param.moveYfix:%1f\n",display_mode, pl_screen_operation_param.moveXfix, pl_screen_operation_param.moveYfix);

	return 0;
}

//PhoneLink Deal With DaLink Status From DaLink
int dalink_spp_phone_dalink_app_state_dispose(char state)
{
	INFO("dalink_spp_phone_dalink_app_state_dispose state =%d\n",state);
	phone_protocol_recive_flag |= SPP_MSG_DALINK_STATUS_BIT;
	if(state == 0) //DaLink forground
	{
		display_mode = PHONE_HORIZONTAL;
		if(DALINK_FORGROUND!=get_dalink_status())
		{
			sendDalinkStatus(DALINK_FORGROUND);
			set_dalink_status(DALINK_FORGROUND);
			usleep(1000);
		}
	}
       else if(state == 1) //DaLink Background
	{
		display_mode = PHONE_VERTICAL;
		if(DALINK_BACKGROUND!=get_dalink_status())
		{
			sendDalinkStatus(DALINK_BACKGROUND);
			set_dalink_status(DALINK_BACKGROUND);
			usleep(1000);
		}
	
	}
	return 0;
}

//PhoneLink Deal With DaLinkMainMenu Status From DaLink
int dalink_spp_mainmenu_state_dispose(char state)
{
	INFO("dalink_spp_mainmenu_state_dispose state =%d\n",state);
	phone_protocol_recive_flag |= SPP_MSG_DALINK_MAIN_MENU_STATUS_BIT;
	if(state == 0)
	{
		if(DALINK_MAIN_MENU_FORGROUND!=get_dalink_mainmenu_status())
		{
			sendDalinkMainmenuStatus(DALINK_MAIN_MENU_FORGROUND);
			set_dalink_mainmenu_status(DALINK_MAIN_MENU_FORGROUND);
		}

	}
	else
	{
		if(DALINK_MAIN_MENU_BACKGROUND!=get_dalink_mainmenu_status())
		{
			sendDalinkMainmenuStatus(DALINK_MAIN_MENU_BACKGROUND);
			set_dalink_mainmenu_status(DALINK_MAIN_MENU_BACKGROUND);
		}
	}
	return 0;
}

//PhoneLink Deal With Park's Requestion From DaLink
int dalink_spp_phone_request_park_state()
{
	INFO("sendParkQueryState\n");
	sendParkStateReq();
	return 0;
}

//Siri's Status Feedback
int dalink_spp_phone_siri_feedback(char open_or_close_status,char success_or_false )
{
	switch(open_or_close_status)
	{
		case OPEN_SIRI:
		{
			if(OPEN_SIRI_FALSE==success_or_false)
			{
				
			}
			else if(OPEN_SIRI_SUCCESS==success_or_false)
			{

			}

		break;
		}
		case CLOSE_SIRI:
		{
			if(OPEN_SIRI_FALSE==success_or_false)
			{

			}
			else if(OPEN_SIRI_SUCCESS==success_or_false)
			{

			}

		break;
		}
	}
}

//PhoneLink Send Siri CMD To Smartphone's DaLink
int dalink_spp_phone_siri_cmd(char open_or_close)
{
	switch(open_or_close)
	{
		case OPEN_SIRI:
		{
			pl_dalink_send_siri_feedback_packet(BT_SPP_ANDROID,OPEN_SIRI,OPEN_SIRI_SUCCESS);
			break;
		}
		case CLOSE_SIRI:
		{
			pl_dalink_send_siri_feedback_packet(BT_SPP_ANDROID,CLOSE_SIRI,CLOSE_SIRI_SUCCESS);
			break;
		}

	}
}

//PhoneLink Deal With SPP Data From DaLink
int dalink_spp_packet_deal(char * packet_header,int len)
{

	if(len <packet_header[BT_SPP_MESSAGE_LENGTH_INDEX]+ BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD)
	{
		WARN("dalink_spp_packet_deal recive packet len is not right ,recive len=%d,and except is %d\n",len,packet_header[BT_SPP_MESSAGE_LENGTH_INDEX]+ BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD);
		return -1;
	}
	unsigned short ack_tranid = ((short )packet_header[BT_SPP_MESSAGE_TRAN_ID_HIGH_INDEX]<<8) |((short )packet_header[BT_SPP_MESSAGE_TRAN_ID_LOW_INDEX]);
	unsigned char d_flag = packet_header[BT_SPP_MESSAGE_DFLAG_INDEX];
	unsigned char cmd = packet_header[BT_SPP_MESSAGE_CMD_TYPE_INDEX];
	BT_SPP_PHONE_TYPE phone_type;
	if(d_flag == 1)
	{
		pl_dalink_send_ack_packet(BT_SPP_ANDROID, ack_tranid , cmd);
	}
	switch(packet_header[BT_SPP_MESSAGE_CMD_TYPE_INDEX])
	{
		case BT_SPP_MESSAGE_DL_HU_ACK:
			INFO("recive ack  msg \n");
			post_bt_spp_ack_wait_sem(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX],ack_tranid);
			break;
		case BT_SPP_MESSAGE_DL_HU_HEARTBEAT:
			INFO("recive heart beat msg \n");
			set_heart_beat_flag();
			break;
		case BT_SPP_MESSAGE_DL_HU_PHONEINFO:
			INFO("Recv BT_SPP_MESSAGE_DL_HU_PHONEINFO\n");
			switch(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX])
			{
				case BT_SPP_MESSAGE_PHONE_INFO_RESOLUTION:
					//default send horizontal mode's resolution
					INFO("recive BT_SPP_MESSAGE_PHONE_INFO_RESOLUTION \n");
					dalink_spp_resolution_data_dispose(packet_header+BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1);
					break;
				default:
					break;

			}
			break;
		case BT_SPP_MESSAGE_DL_HU_PHONESTATE:
			INFO("Recv BT_SPP_MESSAGE_DL_HU_PHONESTATE\n");
			INFO("recive phonestate msg ,and type =%d \n",packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX]);
			switch(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX])
			{
				case BT_SPP_MESSAGE_PHONE_DIRECTION:
					dalink_spp_phone_dirction_data_dispose(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1]);
					break;
				case BT_SPP_MESSAGE_PHONE_DALINK_APP_STATE:
					dalink_spp_phone_dalink_app_state_dispose(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1]);
					break;
				case BT_SPP_MESSAGE_DALINK_MAINMENU_STATUS_STATE:
					dalink_spp_mainmenu_state_dispose(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1]);
					break;
				case BT_SPP_MESSAGE_PHONE_SYSTEM_TIME:
					memcpy(&phone_gps_time,packet_header+BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1,sizeof(PL_IPHONE_TIME));
					sendPhoneGpsTime(&phone_gps_time);
					break;
				default:
					break;
			}
			break;
		case BT_SPP_MESSAGE_DL_HU_REQUESTPARKSTATE:
			INFO("recive request park state msg \n");
			dalink_spp_phone_request_park_state();
			break;
		case BT_SPP_MESSAGE_DL_HU_START_ANDROID_APP:
			//INFO("Recv BT_SPP_MESSAGE_DL_HU_START_ANDROID_APP:APP Name=%d\n",packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX] );
			break;
		case BT_SPP_MESSAGE_DL_HU_SIRI:
		{
			INFO("Recv BT_SPP_MESSAGE_DL_HU_SIRI:siri Type=%d,siri Cmd=%d,siri Status=%d\n",
			packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX],
			packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1],
			packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+2]);
			switch(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX])
			{
				case BT_SPP_MESSAGE_SIRI_FEEDBACK:
				{
					dalink_spp_phone_siri_feedback(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1],
						packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+2]);
					break;
				}
				case BT_SPP_MESSAGE_SIRI_CMD:
				{
					dalink_spp_phone_siri_cmd(packet_header[BT_SPP_MESSAGE_PLAYLOAD_START_INDEX+1]);
					break;
				}

			}
			break;
		}
		default:
			break;
	}
	return 0;
}

//Checkout Every SPP Frame From DaLink
int dalink_check_packet_integrity(char * source_buf,char len, char ** packet_header_point)
{
	int  i = 0 ;
	DEBUG("recive spp data len = %d,and buf :",len);
	for(i=0;i<len;i++)
	{
		DEBUG("0X%X ",source_buf[i]);
	}
	DEBUG("\n");
	i = 0;
	while(i <len )
	{
		DEBUG("dalink_check_packet_integrity i=%d  \n",i);
		if(source_buf [i] == BT_SPP_MESSAGE_HEADER_HIGH && source_buf [i+1] == BT_SPP_MESSAGE_HEADER_LOW )
		{
			if(source_buf[i+BT_SPP_MESSAGE_LENGTH_INDEX]+BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD > len )		
			{
				INFO("check packet len is not right\n ");
				i +=2;
				continue;
			}
			else
			{
				DEBUG("check head  is  right and lengh is right\n ");
				if(source_buf [source_buf[i+BT_SPP_MESSAGE_LENGTH_INDEX]+8] == 0x0D && source_buf [source_buf[i+BT_SPP_MESSAGE_LENGTH_INDEX]+9] == 0X0A)
				{
					*packet_header_point = source_buf;		
					return source_buf[i+BT_SPP_MESSAGE_LENGTH_INDEX]+BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD;
				}
				else
				{
					WARN("check tail  is not  right \n ");
					i +=2;
					continue;
				}
			}
		}
		else
		{
			WARN("check head  is not right\n ");
			i++ ;
			continue;
		}
	}
	return -1;
}

//Compute  SPP Frame's CheckSum
char dalink_bluetooth_SPP_checkSum(char *pArray, int len)
{
	int i;
	char checksum;
	checksum = pArray[0];

	for(i=1; i< len; i++)
	{
		checksum ^= pArray[i];
	}

	return checksum;

}

//PhoneLink Create SPP Frame Struct
int dalink_create_spp_protocol_packet(char type, short tranID,char D_flag,char *pArray, int len,char * desbuf)
{
	if(desbuf == NULL || len >250)
	{
		WARN("create_spp_protocol_packet desbuf =null or len >250 len=%d\n",len);
		return -1 ;
	}
	desbuf[BT_SPP_MESSAGE_HAED_LOW_INDEX]	 = BT_SPP_MESSAGE_HEADER_HIGH;
	desbuf[BT_SPP_MESSAGE_HAED_HIGH_INDEX] =	BT_SPP_MESSAGE_HEADER_LOW;
	desbuf[BT_SPP_MESSAGE_CMD_TYPE_INDEX]	 =	type;
	desbuf[BT_SPP_MESSAGE_DFLAG_INDEX] =   D_flag;
	desbuf[BT_SPP_MESSAGE_TRAN_ID_HIGH_INDEX]	= (char)(tranID>>8);
	desbuf[BT_SPP_MESSAGE_TRAN_ID_LOW_INDEX]	= (char)tranID;
	desbuf[BT_SPP_MESSAGE_LENGTH_INDEX] = len;
	memcpy(desbuf+7,pArray,len);
	desbuf[8+len] =   0x0D;
	desbuf[9+len] =   0x0A;
	desbuf[7+len] = dalink_bluetooth_SPP_checkSum(desbuf,len+7);

	return BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD+len;
}

//PhoneLink Send SPP Data To Smartphone's DaLink
int bt_spp_send_data_to_dalink(BT_SPP_PHONE_TYPE phone_type,unsigned char cDataLen ,unsigned char *pData)
{
	int ret = -1;
	ret = protected_spp_send_data(phone_type,cDataLen,pData );
	return ret ;
}

//PhoneLink Send Start DaLink CMD To SmartPhone
int start_android_dalink_app()
{
	int ret = -1;
	char data  = BT_SPP_MESSAGE_PHONE_START_DALINK ;
	INFO("start_android_dalink_app \n");
	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_START_ANDROID_APP,tranID++,D_FLAG_NEED_ACK,&data,1,dalink_spp_send_buf);
	protected_spp_send_data(BT_SPP_ANDROID,ret,(unsigned char *)dalink_spp_send_buf);
	usleep(20000);
	return ret ;
}

//Create ACK SPP Packet
int create_ack_packet(short ack_tranid ,char cmd,char *desbuf )
{
	int ret = -1;
	char data = cmd ;
	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_DL_HU_ACK,ack_tranid,D_FLAG_NONEED_ACK,&data,1,desbuf);
	return ret ;
}

//PhoneLink Send ACK To DaLink
int pl_dalink_send_ack_packet(BT_SPP_PHONE_TYPE phone_type,short ack_tranid ,char cmd)
{
	int ret =-1;
	INFO("pl_dalink_send_ack_packet,ack_tranid=%d,cmd=%d\n ",ack_tranid,cmd);
	ret = create_ack_packet(ack_tranid,cmd,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Heartbeat SPP Packet
int create_heartbeat_packet(char *desbuf)
{
	int ret = -1;
	char data = 0 ;
	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_DL_HU_HEARTBEAT,tranID++,D_FLAG_NONEED_ACK,&data,1,desbuf);
	return ret;
}

//PhoneLink Send Heartbeat To DaLink
int pl_dalink_send_heartbeat_packet(BT_SPP_PHONE_TYPE phone_type)
{
	int ret =-1;
	INFO("pl_dalink_send_heartbeat_packet\n ");
	ret = create_heartbeat_packet(dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create TouchMsg SPP Packet
int create_touchscreen_packet(unsigned short x, unsigned short y ,char touch_flag ,char multi_serial,char *desbuf)
{
	int ret = -1;
	char data[6] = {0} ;
	data[0]  = (x>>8) &0xff ;
	data[1]  = x &0xff ;
	data[2]  = (y>>8) &0xff ;
	data[3]  = y &0xff ;
	data[4] = touch_flag;
	data[5] = multi_serial;
 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_TOUCH,tranID++,D_FLAG_NONEED_ACK,data,6,desbuf);
	return ret;
}

//PhoneLink Send TouchMsg To DaLink
int pl_dalink_send_touchscreen_packet(BT_SPP_PHONE_TYPE phone_type,unsigned short x, unsigned short y ,char touch_flag ,char multi_serial)
{
	int ret =-1;
	ret = create_touchscreen_packet(x,y,touch_flag,multi_serial,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}
//PhoneLink Send SPP TouchMsg To DaLink
void send_spp_mode_cotrol_data(int _actionIn, int _serialNum, int _axisX, int _axisY)
{
	INFO("send_spp_mode_cotrol_data _serialNum=%d,_actionIn=%d,_axisX=%d,_axisY=%d\n",_serialNum,_actionIn,_axisX,_axisY);
	int ret =0 ;
	ret = create_touchscreen_packet(_axisX,_axisY,_actionIn,_serialNum,dalink_spp_send_buf);
	protected_spp_send_data(BT_SPP_ANDROID,ret,(unsigned char *)dalink_spp_send_buf);
	usleep(20000);
}

//PhoneLink Send HID TouchMsg To SmartPhone
void send_hid_mode_cotrol_data(int _actionIn, int _serialNum, int _axisX, int _axisY)
{
    	INFO("send_hid_mode_cotrol_data _actionIn=%d,_axisX=%d,_axisY=%d\n",_actionIn,_axisX,_axisY);
	BTHidMouseData data;
	memset(&data,0,sizeof(BTHidMouseData));
	switch(_actionIn)
	{
		case  TOUCH_MOVE:
		{
			INFO("TOUCH_MOVE\n");
			data.xOffset=_axisX;
			data.yOffset=_axisY;
			data.eButtonStatus=BT_HID_BUTTON_L_DOWN;
			data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
			sendMouseData(&data);
			break;
		}
		case TOUCH_DOWN:
		{
			INFO("TOUCH_DOWN\n");
			data.xOffset=_axisX;
			data.yOffset=_axisY;
			data.eButtonStatus=BT_HID_BUTTON_L_DOWN;
			data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
			sendMouseData(&data);
			break;
		}
		case TOUCH_UP:
		{
			INFO("TOUCH_UP\n");
			data.xOffset=_axisX;
			data.yOffset=_axisY;
			data.eButtonStatus=BT_HID_BUTTON_NONE_OR_UP;
			data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
			sendMouseData(&data);
			// ��λ
			bt_mouse_adjust();
			break;
		}
	}
	return ;
}

//Create Key Message SPP Packet
int create_key_info_packet(char key_value ,char key_flag ,char * desbuf)
{
	int ret = -1;
	char data[2] = {0} ;
	data[0]  = key_value ;
	data[1]  = key_flag;

 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_KEY,tranID++,D_FLAG_NONEED_ACK,data,2,desbuf);
	return ret;
}

//PhoneLink Send Key  Message To DaLink
int pl_dalink_send_key_info_packet(BT_SPP_PHONE_TYPE phone_type,char key_value ,char key_flag)
{
	int ret =-1;
	ret = create_key_info_packet(key_value,key_flag,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Request SmartPhone's information  SPP Packet
int create_request_phoneinfo_packet(char phone_info_class ,char * desbuf)
{
	int ret = -1;
	char data  = phone_info_class ;

 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_REQUESTPHONEINFO,tranID++,D_FLAG_NONEED_ACK,&data,1,desbuf);
	return ret;
}

//PhoneLink Send SmartPhone's information To DaLink
int pl_dalink_send_request_phoneinfo_packet(BT_SPP_PHONE_TYPE phone_type,char phone_info_class)
{
	int ret =-1;
	ret = create_request_phoneinfo_packet(phone_info_class,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Request SmartPhone's Status  SPP Packet
int create_request_phonestate_packet(char phone_state_class ,char * desbuf)
{
	int ret = -1;
	char data  = phone_state_class ;

 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_REQUSETPHONESTATE,tranID++,D_FLAG_NEED_ACK,&data,1,desbuf);
	return ret;
}

//PhoneLink Send SmartPhone's Status To DaLink
int pl_dalink_send_request_phonestate_packet(BT_SPP_PHONE_TYPE phone_type,char phone_state_class)
{
	int ret =-1;
	ret = create_request_phonestate_packet(phone_state_class,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Request Park Information  SPP Packet
int create_parkinfo_packet(char park_state ,char * desbuf)
{
	int ret = -1;
	char data  = park_state ;

 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_PARKSTATE,tranID++,D_FLAG_NEED_ACK,&data,1,desbuf);
	return ret;
}

//PhoneLink Send Park Information To DaLink
int pl_dalink_send_parkinfo_packet(BT_SPP_PHONE_TYPE phone_type,char park_state)
{
	int ret =-1;
	ret = create_parkinfo_packet(park_state,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Siri  Status SPP Packet
int create_siri_packet(char siri_status,char *desbuf)
{
	int ret = -1;
	char data[2] = {0} ;
	data[0]  = BT_SPP_MESSAGE_SIRI_CMD;
	data[1]  = siri_status ;
 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_SIRI,tranID++,D_FLAG_NEED_ACK,data,2,desbuf);
	return ret;
}

//PhoneLink Send Siri Status To DaLink
int pl_dalink_send_siri_packet(BT_SPP_PHONE_TYPE phone_type,char siri_status)
{
	INFO("pl_dalink_send_siri_packet:siri_status=%d\n",siri_status);
	int ret =-1;
	ret = create_siri_packet(siri_status,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Create Siri  Cmd SPP Packet
int create_siri_feedback_packet(char siri_cmd,char siri_status,char *desbuf)
{
	int ret = -1;
	char data[3] = {0} ;
	data[0] = BT_SPP_MESSAGE_SIRI_FEEDBACK;
	data[1]  = siri_cmd;
	data[2]  = siri_status ;
 	ret  = dalink_create_spp_protocol_packet(BT_SPP_MESSAGE_HU_DL_SIRI,tranID++,D_FLAG_NEED_ACK,data,3,desbuf);
	return ret;
}

//PhoneLink Send Siri Cmd To DaLink
int pl_dalink_send_siri_feedback_packet(BT_SPP_PHONE_TYPE phone_type,char siri_cmd,char siri_status)
{
	INFO("pl_dalink_send_siri_feedback_packet:siri_cmd=%d,siri_status=%d\n",siri_cmd,siri_status);
	int ret =-1;
	ret = create_siri_feedback_packet(siri_cmd,siri_status,dalink_spp_send_buf);
	if(ret <=0)
	{
		return -1;
	}
	else
	{
		ret = bt_spp_send_data_to_dalink(phone_type,ret,(unsigned char *)dalink_spp_send_buf );
		return ret ;
	}
}

//Wait DaLink's Ack Response
int wait_dalink_ack_response(int time_out) 
{
	struct timespec ts;
	struct timeval tt;
	int ret;

	gettimeofday(&tt, NULL );

	if (time_out == 0) {
		ret = sem_wait(&dalink_ack_sem);
		return ret;
	} else {
		int second = time_out / 1000;
		int nsecond = time_out % 1000 * 1000 * 1000 + tt.tv_usec * 1000;
		ts.tv_sec = tt.tv_sec + second;

		if (nsecond > 999999999) {
			ts.tv_nsec = nsecond - 1000000000;
			ts.tv_sec += 1;
		} else {
			ts.tv_nsec = nsecond;
		} ;


		ret = sem_timedwait(&dalink_ack_sem, &ts);
		return ret;
	}

}

//Get  ACK From DaLink
BT_SPP_DALINK_ACK * get_dalink_ack_data(void) 
{
	BT_SPP_DALINK_ACK *pData;
	INFO("get_dalink_ack_data ack_data_read_point =%d,ack_data_write_point=%d\n",ack_data_read_point,ack_data_write_point);
	if(ack_data_read_point!=ack_data_write_point)
	{
		pData = &m_ack_data[ack_data_read_point];
		ack_data_read_point++;
		if(ack_data_read_point>=BT_SPP_ACK_MAX_PACKETS_LEN)
		{
			ack_data_read_point =0;
		}
	}
	else
	{
		ERROR("get_response_data error,readpoint=writepoint:%d\n",ack_data_read_point);
		return &m_ack_data[ack_data_read_point];
	}
	return pData;

}

//Post DaLink's ACK Semapore
int post_bt_spp_ack_wait_sem(unsigned char command_id,unsigned short trans_id)
{
	m_ack_data[ack_data_write_point].cmd_id = command_id;
	m_ack_data[ack_data_write_point].tran_id = trans_id;

	ack_data_write_point++;
	if(ack_data_write_point>=BT_SPP_ACK_MAX_PACKETS_LEN)
	{
		ack_data_write_point =0;
	}
	sem_post(&dalink_ack_sem);
	DEBUG("sem_post dalink_ack_sem \n");
	return 0;
}

//Wait DaLink's ACK
int wait_dalink_ack(int time_ms,short tran_id ,char cmd_id)
{
	int timeout = time_ms;
	BT_SPP_DALINK_ACK * prec_temp = NULL;
	int res =0 ;
	wait_ack_retry :res = wait_dalink_ack_response(timeout);
	if (res != -1)
	{
		prec_temp = get_dalink_ack_data();
		if(prec_temp == NULL)
		{
			WARN("get_dalink_ack_data return null \n");
			return -1;
		}
		if(prec_temp->cmd_id != cmd_id)
		{
			WARN("wait_dalink_ack cmd_id mismatch\n");
			timeout =40;
			goto wait_ack_retry;
		}
		else if(prec_temp->tran_id != tran_id)
		{
			WARN("wait_dalink_ack tran_id mismatch\n");
			timeout =40;
			goto wait_ack_retry;
		}
		else
		{
			INFO("wait_dalink_ack success cmd_id =%d ,tran_id =%d \n", cmd_id, tran_id);
			return 0 ;
		}
	}
	else
	{
		WARN("wait_dalink_ack timeout\n");
		return -1;
	}

}
