 /************************************************************************
* File Name : pl_bluetooth_recive_thread.c
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description : PhoneLink Recive Data From Bluetooth
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		     Description
* 1        V0.1     2015/01/26      CaoLinfeng 	    CMFB Project          PhoneLink Recive Data From Bluetooth
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
sem_t phone_state_from_bt_wait_sem; 					//用于等待手机连接状态、分辨率等一系列参数
BTCSMSG phonestate_sem_data[10];						//蓝牙信号量发送参数
static int sem_data_readpoint =0;
static int sem_data_writepoint =0;

static PHONE_TYPE g_bt_phone_type=BT_CONNECT_ANDROID;									//当前蓝牙连接的手机系统类型
static PL_CONNECT_STATUS g_phone_connected_status=BT_UNCONNECT;			//当前手机连接状态
static BtCsSppState bt_spp_state = SppState_ReadyNoInitialized;//蓝牙spp连接状态
static BtCsHidState bt_hid_state = HidState_ReadyNotConnect;	//蓝牙hid连接状态
static BtIndHfpState bt_hfp_state = HfpStateNotInitialized;			//蓝牙HFP状态
static int phone_connect_init_complete =0;						//手机连接初始化完成



/************************************************************************
* Function Define Section
************************************************************************/

//Get SmartPhone's Type
PHONE_TYPE get_phone_type()
{
	return g_bt_phone_type;
}

//Set SmartPhone's Type
void  set_phone_type(PHONE_TYPE var)
{
	g_bt_phone_type=var;
}

//Get Bt Connect Status
PL_CONNECT_STATUS get_current_phone_connect_status()
{
	return g_phone_connected_status;
}
// Save Bt Connect Status
void set_current_phone_connect_status(PL_CONNECT_STATUS status)
{
	g_phone_connected_status=status;
}

//Get SPP's Status
BtCsSppState get_bt_spp_state(void)
{
	return bt_spp_state;
}

//Get HID's Status
BtCsHidState get_bt_hid_state(void)
{
	return bt_hid_state;
}

//Get HFP's Status
BtIndHfpState get_bt_hfp_state(void)
{
	return bt_hfp_state;
}

//Initialize PhoneLink
 int init_phone_link(void)
 {
  	INFO("init_phone_link start\n");

 	int ret =-1;
 	if (init_Bt_client() < 0)
 	{
 		WARN("\033[1;31m init_Bt_client error!\033[m\n");
 		return -2;
 	}
	RegistBtDataPro(Bluetooth_spp_data_proc);

	
 	pl_screen_operation_param.screenWidth = VEHICLE_LCD_WIDTH;
 	pl_screen_operation_param.screenHeight = VEHICLE_LCD_HEIGHT;
 	g_bt_phone_type =BT_CONNECT_ANDROID;			//默认连接为android手机
 	phone_current_mode = BT_CONNECTED_ANDROID_HID;// 默认HID回控
 	park_state = PARK_STATE_STOP;
	
 	set_dalink_status(DALINK_BACKGROUND);
 	set_dalink_mainmenu_status(DALINK_MAIN_MENU_BACKGROUND);
	set_screen_state(PHONE_SCREEN_ON);
 	usleep(100000);
 	phone_connect_init_complete = 1;				//不管是否连接成功，都应该启动检测线程
 	usleep(1000);
 	INFO("sendHIdQueryState\n");
 	sendHidQueryState();
 	usleep(10000);
 	INFO("sendSPPQueryState\n");
 	sendSppQueryState();
 	usleep(100000);

 	return ret;
 }

//Initialize Bt's Semaphore
 int init_bt_sem()
 {
 	INFO("init_bt_sem\n");
	int ret = -1;
	ret = sem_init(&phone_state_from_bt_wait_sem, 0, 0);
	if (ret != 0) {
		ERROR("phone_state_from_bt_wait_sem init failed\n");
	}
	return ret;
 }


//write data into the bt semaphore array
 int write_sem_data(BTCSMSG * pMsg)
 {
		if(pMsg ==NULL)
		{
			ERROR("write_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(phonestate_sem_data+sem_data_writepoint,pMsg,pMsg->nDataLen +8);
		if(sem_data_writepoint>=9)
		{
			sem_data_writepoint=0;
		}
		else
		{
			sem_data_writepoint+=1;
		}
		DEBUG("sem_data_writepoint=%d\n",sem_data_writepoint);
		return 0;
 }

//The bluetooth module callback function
int Bluetooth_spp_data_proc(BTCSMSG * pMsg) 
{
	DEBUG("eEventType:<%02x> nDataLen:<%02x> <%02x>\n",pMsg->eEventType,pMsg->nDataLen,*(pMsg->szData));
	//INFO("Bluetooth_spp_data_proc\n");
	switch (pMsg->eEventType)
	{
	case BT_MSG_SPP_STATE:
	case BT_MSG_HID_STATE:
	case BT_MSG_UPDATE_PAIR_LIST:
	case BT_MSG_DEV_MANU_FACTORER:
		write_sem_data(pMsg);
		sem_post(&phone_state_from_bt_wait_sem);
		break;
	case BT_MSG_SPP_REVICE_DATA:
		if(g_bt_phone_type != BT_CONNECT_IPHONE)
		{
			write_sem_data(pMsg);
			sem_post(&phone_state_from_bt_wait_sem);
		}
		break;
	case BT_MSG_HARD_KEY:
		write_sem_data(pMsg);
		sem_post(&phone_state_from_bt_wait_sem);
		break;
	case BT_MSG_VOICE_DIAL_ACTIVE:
		write_sem_data(pMsg);
		sem_post(&phone_state_from_bt_wait_sem);
		break;
	case BT_MSG_VOICE_DIAL_TERMINATED:
		write_sem_data(pMsg);
		sem_post(&phone_state_from_bt_wait_sem);
		break;
	default:
		break;
	}
	return 0;
}

//wait bt command
 int wait_bt_command(int timeout_ms,BTCSMSG * pMsg)
 {
 	int res =0;
 	int timeout = timeout_ms;

 	res = wait_bt_response(timeout);
 	DEBUG("wait_bt_response ret=%d\n",res);
 	if(res >=0)
 	{
		memcpy(pMsg,phonestate_sem_data+sem_data_readpoint,8+(phonestate_sem_data+sem_data_readpoint)->nDataLen);
		if(sem_data_readpoint>=9)
		{
			sem_data_readpoint=0;
		}
		else
		{
			sem_data_readpoint+=1;
		}
		DEBUG("sem_data_readpoint=%d\n",sem_data_readpoint);
 	}
 	return  res;
 }

//wait bt response. 
//time_out=0:Infinite wait
 int wait_bt_response(int time_out) 
 {
 	struct timespec ts;
 	struct timeval tt;
 	int ret;

 	gettimeofday(&tt, NULL );

 	if (time_out == 0)
 	{
 		ret = sem_wait(&phone_state_from_bt_wait_sem);
 		return ret;
 	} else
 	{
 		int second = time_out / 1000;
 		int nsecond = time_out % 1000 * 1000 * 1000 + tt.tv_usec * 1000;
 		ts.tv_sec = tt.tv_sec + second;

 		if (nsecond > 999999999)
 		{
 			ts.tv_nsec = nsecond - 1000000000;
 			ts.tv_sec += 1;
 		} else
 		{
 			ts.tv_nsec = nsecond;
 		}

 		ret = sem_timedwait(&phone_state_from_bt_wait_sem, &ts);
 		return ret;
 	}

 }

//Update Bt State
 void confirm_current_connect_state()
 {
	//INFO("bt_spp_state=%d,bt_hfp_state=%d,bt_hid_state=%d\n",bt_spp_state,bt_hfp_state,bt_hid_state);
	if(bt_spp_state != SppState_Connected && bt_hfp_state != HfpStateConnected  && bt_hid_state != HidState_Connected)
	{
		set_current_phone_connect_status(BT_UNCONNECT);
		send_connect_stautus(BT_UNCONNECT);//HFP HID SPP=False

	}
	else  if(bt_spp_state != SppState_Connected && bt_hfp_state == HfpStateConnected && bt_hid_state == HidState_Connected)
	{
		set_current_phone_connect_status(BT_CONNECTED_HFP_HID);
		send_connect_stautus(BT_CONNECTED_HFP_HID);//SPP=False
	}
	else  if(bt_spp_state == SppState_Connected && bt_hfp_state == HfpStateConnected && bt_hid_state != HidState_Connected)
	{
		set_current_phone_connect_status(BT_CONNECTED_HFP_SPP);
		send_connect_stautus(BT_CONNECTED_HFP_SPP);//HID=False

	}
	else  if(bt_spp_state == SppState_Connected && bt_hfp_state != HfpStateConnected && bt_hid_state == HidState_Connected)
	{
		set_current_phone_connect_status(BT_CONNECTED_HID_SPP);
		send_connect_stautus(BT_CONNECTED_HID_SPP);//HFP=False
	}
	else  if(bt_spp_state == SppState_Connected && bt_hfp_state == HfpStateConnected && bt_hid_state == HidState_Connected)
	{
		set_current_phone_connect_status(BT_CONNECTED);
		send_connect_stautus(BT_CONNECTED);//SPP HID HFP=Ture

	}
 }

//PhoneLink deal with bt  message from bt
 void *bt_msg_process_thread(void *arg)
 {
       INFO("bt_msg_process_thread start\n");
 	BTCSMSG btMsg;
 	int ret =0;
 	BtCsSppState bt_spp_state_last = SppState_ReadyNoInitialized;
 	BtCsHidState bt_hid_state_last = HidState_ReadyNotConnect;
 	BtIndHfpState bt_hfp_state_last = HfpStateNotInitialized;
 	while(1)
 	{
 		if(phone_connect_init_complete ==1)
 		{
 			while(1)
 			{
 				ret = wait_bt_command(0,&btMsg);
 				if(ret == 0)
 				{
 					switch(btMsg.eEventType)
 					{
 						case BT_MSG_UPDATE_PAIR_LIST:
						{
							INFO("RECV BT_MSG_UPDATE_PAIR_LIST\n");
							//更新HFP状态
							bt_hfp_state_last = bt_hfp_state;
							if(1 == check_Connect(BIT_HFP))
							{
								INFO("detect HFP connect\n");

								bt_hfp_state = HfpStateConnected;
								if(bt_hfp_state != bt_hfp_state_last)
								{
									confirm_current_connect_state();
								}
							}
							else
							{	
								INFO("detect HFP disconnect\n");
								bt_hfp_state = HfpStateNotInitialized;
								confirm_current_connect_state();
							}

							//更新HID状态
							bt_hid_state_last = bt_hid_state;
							if(1 == check_Connect(BIT_HID))
							{
								INFO("detect HID connect\n");

								bt_hid_state = HidState_Connected;
								if(bt_hid_state != bt_hid_state_last)
								{
									confirm_current_connect_state();
								}
							}
							else
							{	
								INFO("detect HID disconnect\n");
								bt_hid_state = HidState_ReadyNotConnect;
								confirm_current_connect_state();
							}

							//更新SPP状态
							bt_spp_state_last = bt_spp_state;
							if(1 == check_Connect(BIT_SPP))
							{
								INFO("detect SPP connect\n");

								bt_spp_state = SppState_Connected;
								if(bt_spp_state != bt_spp_state_last)
								{
									confirm_current_connect_state();
								}
							}
							else
							{	
								INFO("detect SPP disconnect\n");
								bt_spp_state = SppState_ReadyNoInitialized;
								confirm_current_connect_state();
							}
							break;
						}
 						case BT_MSG_SPP_REVICE_DATA:
						{
							INFO("RECV BT_MSG_SPP_REVICE_DATA\n");
							phone_app_msg_deal(btMsg.nDataLen,(char *)btMsg.szData);
							break;
						}
						default:
 							break;
 					}

 				}
 			}
 		}
 		else
 		{
 			usleep(100000);
 		}
 	}
 	return 0;
 }


