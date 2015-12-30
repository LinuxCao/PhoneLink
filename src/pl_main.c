 /************************************************************************
* File Name : pl_main.c
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description : phoneLink Main Process
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		     Description
* 1        V0.1     2015/01/26      CaoLinfeng 	    CMFB Project                 phoneLink Main Process
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
pthread_t bt_msg_process_thread_tid; 				//bt消息处理线程
pthread_t communication_mainapp_thread_tid;		//于HMI主控程序通信
pthread_t deal_with_hmi_data_thread_tid;			//hmi主控程序消息处理线程
pthread_t deal_with_touchmsg_thread_tid; 				//触摸数据处理线程ID
pthread_t communication_with_mainapp_touchmsg_thread_tid; 		//触摸数据接收线程ID

/************************************************************************
* Function Define Section
************************************************************************/
//phoneLink LOG Set
void PhoneLink_LogPrint_Set()
{
		printf("PhoneLink_LogPrint_Set\n");
		// PhoneLink LOG Set
		char *PhoneLink_level_string = getenv("PHONELINK_LOG_LEVEL");
		if(PhoneLink_level_string == NULL)
		{
			gPhoneLinkLogLevel = PHONELINK_LEVEL_INFO;
		} 
		else if(PhoneLink_level_string[0] >='0' &&PhoneLink_level_string[0] <='3')
		{
			gPhoneLinkLogLevel = PhoneLink_level_string[0]-'0';
		}
		else
		{
			gPhoneLinkLogLevel = PHONELINK_LEVEL_INFO;
		}
}

//Main Enter
int main (int argc, const char* argv[])
{
		INFO("phonelink server start\n");
		PhoneLink_LogPrint_Set();
		int ret =-1;
		static int num = 0;
		//初始化hid设备断开信号量
		ret = init_bt_sem();
		if (ret != 0) {
			ERROR("init_bt_sem init failed\n");
		}

		//PhoneLink Recive Data From Bluetooth
		if (pthread_create(&bt_msg_process_thread_tid, NULL, bt_msg_process_thread,
				NULL ) != 0)
		{
			WARN("pthread_create read thread error.\n");
			return -1;
		}
		pthread_detach(bt_msg_process_thread_tid);

		
		init_hmi_msg_sem();
		if (pthread_create(&communication_mainapp_thread_tid, NULL, communication_with_mainapp_thread,
				NULL ) != 0)
		{
			WARN("pthread_create communication_with_mainapp_thread error.\n");
			return -1;
		}
		pthread_detach(communication_mainapp_thread_tid);
		

		if (pthread_create(&deal_with_hmi_data_thread_tid, NULL, deal_with_hmi_data_thread,
				NULL ) != 0)
		{
			WARN("pthread_create deal_with_hmi_data_thread error.\n");
			return -1;
		}
		pthread_detach(deal_with_hmi_data_thread_tid);
		

		ret = init_phone_link();
		if(ret ==-2)		//can't listen blesocket
		{
			return -1;

		}
		

		init_dalink_protocol_ack_sem();
		
		init_touchmsg_sem();
		//触摸数据接收线程
		if (pthread_create(&communication_with_mainapp_touchmsg_thread_tid, NULL, communication_with_mainapp_touchmsg_thread,
				NULL ) != 0)
		{
			WARN("pthread_create communication_with_mainapp_touchmsg_thread error.\n");
			return -1;
		}
		pthread_detach(communication_with_mainapp_touchmsg_thread_tid);

		//触摸数据处理线程
		if (pthread_create(&deal_with_touchmsg_thread_tid, NULL, deal_with_touchmsg_thread,
				NULL ) != 0)
		{
			WARN("pthread_create deal_with_touchmsg_thread error.\n");
			return -1;
		}
		pthread_detach(deal_with_touchmsg_thread_tid);
		
		if (pthread_mutex_init(&spp_send_proctect_mutex, NULL ) != 0) {
			ERROR("spp_send_proctect_mutex create error!\n");
		}
		if (pthread_mutex_init(&touch_deal_proctect_mutex, NULL ) != 0) {
			ERROR("touch_deal_proctect_mutex create error!\n");
		}
		
		sendPhoneLinkStartMsg();

		while(1)
		{
			sleep(120);
			if(SppState_Connected==get_bt_spp_state())
			{
				pl_dalink_send_heartbeat_packet(BT_SPP_ANDROID);
			}
		}
}




