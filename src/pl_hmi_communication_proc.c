 /************************************************************************
* File Name : pl_hmi_communication_proc.c
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description : The Process between PhoneLinnk And HMI
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		        Description
* 1        V0.1     2015/01/26      CaoLinfeng 	   CMFB Project              The Process between PhoneLinnk And HMI
*
************************************************************************/

/************************************************************************
* Include File Section
************************************************************************/
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/un.h> 
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>


#include "BtCSMsg.h"
#include "BtClient.h"
#include "pl_bt_hid_fun.h"
#include "pl_bluetooth_recive_thread.h"
#include "pl_phone_app_protocol.h"
#include "pl_touch_info_proc.h"
#include "pl_hmi_communication_proc.h"
#include "pl_main.h"
#include "hid_keys.h"
#include "hiddef.h"

/************************************************************************
* Static Variable Define Section
************************************************************************/
static int connect_mainapp_fd;
static int connect_mainapp_touchmsg_fd;
static int phonelink_touchmsg_read_point ;
static int phonelink_touchmsg_write_point;
static int phonelink_hmi_msg_read_point ;
static int phonelink_hmi_msg_write_point;
static sem_t phonelink_touchmsg_sem; 					//用于等待触摸屏数据
static sem_t phonelink_hmi_msg_sem; 					//用于等待HMI数据
static PL_TOUCH_INFO phonelink_socket_touchmsg[MAX_TOUCH_MSG_SIZE] ;
static PL_MsgInfo_t phonelink_socket_hmi_msg[MAX_HMI_MSG_SIZE] ;
static PL_Miracast_CONNECT_STATUS pl_miracast_status=Miracast_UNCONNECT;
static DALINK_STATUS phonelink_dalink_status=DALINK_BACKGROUND;
static DALINK_MAIN_MENU_STATUS phonelink_dalink_mainmenu_status=DALINK_MAIN_MENU_BACKGROUND;
static HMI_SEND_PHONE_DISPLAY_MODE phonelink_dalink_screen_direction_status=HMI_SEND_PHONE_SCREEN_DEFAULT;
static PHONE_SCREEN_ON_OFF_STATUS phone_screen_state=PHONE_SCREEN_DEFAULT;						


/************************************************************************
* Function Define Section
************************************************************************/

//Get Miracast's Status
PL_Miracast_CONNECT_STATUS get_miracast_status()
{
	return pl_miracast_status;
}

//Set Miracast's Status
void set_miracast_status(PL_Miracast_CONNECT_STATUS status)
{
	pl_miracast_status = status;
}

//Get DaLink Status 
DALINK_STATUS get_dalink_status()
{
	return phonelink_dalink_status;
}

//Set DaLink Status
void set_dalink_status(DALINK_STATUS status)
{
	phonelink_dalink_status = status;
}

//Get DaLinkMainMenu's Status
DALINK_MAIN_MENU_STATUS get_dalink_mainmenu_status()
{
	return phonelink_dalink_mainmenu_status;
}

//Set DaLinkMainMenu's Status
void set_dalink_mainmenu_status(DALINK_MAIN_MENU_STATUS status)
{
	phonelink_dalink_mainmenu_status = status;
}

//Get Screen Direction Status
HMI_SEND_PHONE_DISPLAY_MODE get_screen_direction_status()
{
	return phonelink_dalink_screen_direction_status;
}

//Set Screen Direction Status
void set_screen_direction_status(HMI_SEND_PHONE_DISPLAY_MODE status)
{
	phonelink_dalink_screen_direction_status = status;
}

//Get Screen Status: ON or OFF
PHONE_SCREEN_ON_OFF_STATUS get_screen_state()
{
	return phone_screen_state;
}

//Set Screen Status: ON or OFF
void set_screen_state(PHONE_SCREEN_ON_OFF_STATUS state)
{
	 phone_screen_state = state;
}

//PhoneLink send data to HMI with socket
int send_data_to_mainapp(char *buf ,int size)
{
	int ret =-1;
	if(Miracast_UNCONNECT==get_miracast_status())
	{
		INFO("Miracast_UNCONNECT:send_data_to_mainapp fail\n");
		return -1;
	}
	ret =write(connect_mainapp_fd,buf,size);
	if(ret <0)
	{
		ERROR("send_data_to_mainapp error \n");
		return -1;
	}
	else
	{
		DEBUG("send_data_to_mainapp success ,and buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x,buf[4]=%x,buf[5]=%x,buf[6]=%x,buf[7]=%x,buf[8]=%x,size =%d\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],size);
		return ret;
	}
}

//PhoneLink send start to HMI
int sendPhoneLinkStartMsg()
{
	int ret =-1;
	INFO("sendPhoneLinkStartMsg \n");
	PL_MsgInfo_t msg;
	msg.MsgType = WM_PLINK_START;
	memset(msg.MsgBuf,0,256);
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send bt's connect status to HMI
int send_connect_stautus(PL_CONNECT_STATUS status)
{
	INFO("send_connect_stautus status=%d\n",status);
	int ret =-1;
	PL_MsgInfo_t msg;
	memset(&msg,0,sizeof(PL_MsgInfo_t));
	msg.MsgType = WM_PLINK_CONNECT_STATUS;
	*(msg.MsgBuf)= status;
	//*(msg.MsgBuf+4) = display_mode;
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));

	return ret ;
}

//PhoneLink send DaLink's status to HMI
int sendDalinkStatus(DALINK_STATUS status)
{
	int ret =-1;
	PL_MsgInfo_t msg;
	INFO("sendDalinkStatus status =%d\n",status);
	msg.MsgType = WM_PLINK_DALINK_STATUS;
	memset(msg.MsgBuf,0,256);
	*(msg.MsgBuf)= status;
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send DaLinkMainMenu's status to HMI
int sendDalinkMainmenuStatus(DALINK_MAIN_MENU_STATUS status)
{
	int ret =-1;
	PL_MsgInfo_t msg;
	INFO("sendDalinkmainMENU status =%d\n",status);
	msg.MsgType = WM_PLINK_DALINK_MAINMENU_STATUS;
	memset(msg.MsgBuf,0,256);
	*(msg.MsgBuf)= status;
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send siri's status to HMI
int sendSiriStatus(char status)
{
	int ret =-1;
	PL_MsgInfo_t msg;
	INFO("sendSiriStatus status =%d\n",status);
	msg.MsgType = WM_PLINK_PHONE_SIRI_STATUS;
	memset(msg.MsgBuf,0,256);
	*(msg.MsgBuf)= status;
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send smartphone's screen direction status to HMI
int send_screen_direction_status(char direction)
{
	int ret =-1;
	INFO("send_screen_direction_status direction =%d\n",direction);
	PL_MsgInfo_t msg;
	msg.MsgType = WM_PLINK_SCREEN_DIRECTION;
	memset(msg.MsgBuf,0,256);
	msg.MsgBuf[0] = direction;
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send Park Status's requestion  to HMI
int sendParkStateReq()
{
	int ret =-1;
	PL_MsgInfo_t msg;
	msg.MsgType = WM_PLINK_PARK_REQ;
	memset(msg.MsgBuf,0,256);

	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//PhoneLink send smartphone's system time  to HMI
int sendPhoneGpsTime(PL_IPHONE_TIME *phone_time)
{
	int ret = -1;
	INFO("sendPhoneGpsTime time =%d,%d ,%d,%d\n",phone_time->UTC_Highest_Byte,phone_time->UTC_Secondary_High_Byte,phone_time->UTC_Middle_Byte,phone_time->UTC_Lowest_Byte);
	PL_MsgInfo_t msg;
	msg.MsgType = WM_PLINK_PHONE_TIME;
	memset(msg.MsgBuf,0,256);
	memcpy(msg.MsgBuf,phone_time,sizeof(PL_IPHONE_TIME));
	ret = send_data_to_mainapp((char*)&msg,sizeof(msg));
	return ret;
}

//Initialize HMI message's Semaphore
int init_hmi_msg_sem()
{
 	INFO("init_hmi_msg_sem\n");
	int ret = -1;
	ret = sem_init(&phonelink_hmi_msg_sem, 0, 0);
	if (ret != 0) {
		ERROR("phonelink_hmi_msg_sem init failed\n");
	}
	return ret;
}

//write data into the hmi message semaphore array
int write_hmimsg_sem_data(PL_MsgInfo_t * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("write_hmimsg_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(phonelink_socket_hmi_msg+phonelink_hmi_msg_write_point,pMsg,sizeof(PL_MsgInfo_t));
	 	if(phonelink_hmi_msg_write_point>=MAX_HMI_MSG_SIZE-1)
		{
	 		phonelink_hmi_msg_write_point=0;
		}
		else
		{
			phonelink_hmi_msg_write_point+=1;
		}
		sem_post(&phonelink_hmi_msg_sem);
		DEBUG("phonelink_hmi_msg_write_point=%d\n",phonelink_hmi_msg_write_point);
		return 0;
}

//Read data from the hmi message  semaphore array
int read_hmimsg_sem_data(PL_MsgInfo_t * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("read_hmimsg_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(pMsg,phonelink_socket_hmi_msg+phonelink_hmi_msg_read_point,sizeof(PL_MsgInfo_t));
		if(phonelink_hmi_msg_read_point>=MAX_HMI_MSG_SIZE-1)
		{
			phonelink_hmi_msg_read_point=0;
		}
		else
		{
			phonelink_hmi_msg_read_point+=1;
		}
		DEBUG("phonelink_hmi_msg_read_point=%d\n",phonelink_hmi_msg_read_point);
		return 0;
}

//PhoneLink deal with hmi  message from hmi
void *deal_with_hmi_data_thread(void *arg)
{
       INFO("deal_with_hmi_data_thread start\n");
	PL_MsgInfo_t hmi_msg;
	int ret = -1;
	int i = 0;
	BT_SPP_PHONE_TYPE phonetype;
	while(1)
	{
		sem_wait(&phonelink_hmi_msg_sem);
		read_hmimsg_sem_data(&hmi_msg);

		DEBUG("socket_recvdata_deal pMsgInfo->MsgType=%d\n",hmi_msg.MsgType);
		DEBUG("socket_recvdata_deal pMsgInfo->MsgBuf=%d %d %d %d %d\n",hmi_msg.MsgBuf[0],hmi_msg.MsgBuf[1],hmi_msg.MsgBuf[2],hmi_msg.MsgBuf[3],hmi_msg.MsgBuf[4]);
		switch(hmi_msg.MsgType)
		{
			case WM_PLINK_PARK_PLAY:							//park信号，车辆行车
				INFO("socket_recvdata_deal WM_PLINK_PARK_PLAY \n");
				park_state = PARK_STATE_PLAY;
				pl_dalink_send_parkinfo_packet(BT_SPP_ANDROID,park_state);
				break;
			case WM_PLINK_PARK_STOP:							//park 行车停车
				INFO("socket_recvdata_deal WM_PLINK_PARK_STOP \n");
				 park_state = PARK_STATE_STOP;
				 pl_dalink_send_parkinfo_packet(BT_SPP_ANDROID,park_state);
				 break;
			case WM_PLINK_START_DALINK:
				INFO("socket_recvdata_deal WM_PLINK_START_DALINK \n");
				start_android_dalink_app();
				break;
			case WM_PLINK_CONNECT_STATUS_REQ:
			{
				
				INFO("socket_recvdata_deal WM_PLINK_CONNECT_STATUS_REQ\n");
				switch(get_current_phone_connect_status())
				{
					case BT_CONNECTED:
					{
						send_connect_stautus(BT_CONNECTED);//HFP HID SPP=Ture
						break;
					}
					case BT_UNCONNECT:
					{
						send_connect_stautus(BT_UNCONNECT);
						break;
					}
					case BT_CONNECTED_HFP_HID:
					{
						send_connect_stautus(BT_CONNECTED_HFP_HID);//SPP=False
						break;
					}
					case BT_CONNECTED_HFP_SPP:
					{
						send_connect_stautus(BT_CONNECTED_HFP_SPP);//HID=False
						break;
					}

					case BT_CONNECTED_HID_SPP:
					{
						send_connect_stautus(BT_CONNECTED_HID_SPP);//HFP=False
						break;
					}
					default:
					break;
				}

				break;
			}
			case WM_PLINK_DALINK_STATUS_REQ:
				INFO("socket_recvdata_deal WM_PLINK_DALINK_STATUS_REQ \n");
				//sendDalinkStatus(get_dalink_status());
				pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_DALINK_APP_STATE);
			break;
			
			case WM_PLINK_DALINK_MAINMENU_STATUS_REQ:
				INFO("socket_recvdata_deal WM_PLINK_DALINK_MAINMENU_STATUS_REQ \n");
				//sendDalinkMainmenuStatus(get_dalink_mainmenu_status());
				pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_DALINK_MAINMENU_STATUS_STATE);
			break;
			
			case WM_PLINK_PHONE_TIME_REQ:
				INFO("socket_recvdata_deal WM_PLINK_PHONE_TIME_REQ \n");
				pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_SYSTEM_TIME);
			break;
			
			case WM_PLINK_PHONE_SIRI_REQ:
				INFO("socket_recvdata_deal WM_PLINK_PHONE_SIRI_REQ\n");
				pl_dalink_send_siri_packet(BT_SPP_ANDROID,OPEN_SIRI);
			break;

			case WM_PLINK_SCREEN_DIRECTION_REQ:
				INFO("socket_recvdata_deal WM_PLINK_SCREEN_DIRECTION_REQ recive\n");
				pl_dalink_send_request_phonestate_packet(BT_SPP_ANDROID,BT_SPP_MESSAGE_PHONE_DIRECTION);
			break;

			case WM_PLINK_SCREEN_DIRECTION_SET:
			break;
			
			case WM_PLINK_HARDKEY_HOME_DOWN:
			{
				INFO("socket_recvdata_deal WM_PLINK_HARDKEY_HOME_DOWN\n");
				//key press
				BTHidKeyBoardData data;
				data.eAuxiliaryKeyType = BT_HID_AUXILIARY_KEY_NONE;
				data.cKey = HID_ATTR_HOME;
				sendSpecificKeyBoardData(&data);
				usleep(500*1000);
				//key release
				data.eAuxiliaryKeyType = BT_HID_AUXILIARY_KEY_NONE;
				data.cKey = 0;
				sendSpecificKeyBoardData(&data);
				break;
			}
			case WM_PLINK_HARDKEY_BACK_DOWN:
			{
				INFO("socket_recvdata_deal WM_PLINK_HARDKEY_BACK_DOWN\n");
				//key press
				BTHidKeyBoardData data;
				data.eAuxiliaryKeyType = BT_HID_AUXILIARY_KEY_NONE;
				data.cKey = HID_ATTR_BACK;
				sendSpecificKeyBoardData(&data);
				usleep(500*1000);
				//key release
				data.eAuxiliaryKeyType = BT_HID_AUXILIARY_KEY_NONE;
				data.cKey = 0;
				sendSpecificKeyBoardData(&data);
				break;
			}
			case WM_PLINK_MIRACAST_CONNECT:
			{
				INFO("socket_recvdata_deal WM_PLINK_MIRACAST_CONNECT\n");
				set_miracast_status(Miracast_CONNECTED);
				break;
			}
			case WM_PLINK_MIRACAST_DISCONNECT:
			{
				INFO("socket_recvdata_deal WM_PLINK_MIRACAST_DISCONNECT\n");
				set_miracast_status(Miracast_UNCONNECT);
				break;
			}

			default:
			break;
		}
	}
	return 0;
}

//Socket Connect Process
int socket_conn(const char * name)
{
    int fd;
	int ret;

	static struct sockaddr_un srv_addr;

	//creat unix socket
	fd=socket(PF_UNIX,SOCK_STREAM,0);
	if(fd<0){
		perror("cannot create communication socket");
		return -1;
	}
	srv_addr.sun_family=AF_UNIX;
	strcpy(srv_addr.sun_path,name);
	INFO("--------------Start connect------------\n");
	//connect server
	ret=connect(fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
	if(ret==-1){
		perror("cannot connect to the server");
		close(fd);
		return -1;
	}
	INFO("-------------- connect ok----------\n");
	return fd;
}

//The socket process between PhoneLinnk And HMI
void *communication_with_mainapp_thread(void *arg)
{
       INFO("communication_with_mainapp_thread start\n");
	int ret;
	char rec_buf[4096];
	int i =0 ;
	PL_MsgInfo_t msg ;
	while(1)
	{
		connect_mainapp:connect_mainapp_fd = socket_conn(PHONELINK_SOCKET_PATH);
		if(connect_mainapp_fd <0)
		{
			sleep(1);
			continue;
		}
		INFO("communication_with_mainapp_thread connect success\n");
		while(1)
		{
			//recv from server
			memset(rec_buf,0,4096);
			//INFO("communication_with_mainapp_thread recv start\n");
			ret = recv(connect_mainapp_fd,rec_buf,sizeof(rec_buf),0);
			if(ret <=0)		//表示主机连接断开
			{
				WARN("socket connect may disconnect,need reconnect\n");
				goto connect_mainapp;
			}
			INFO("communication_with_mainapp_thread recv datalen =%d,rec_buf[0]=%d,rec_buf[1]=%d,rec_buf[2]=%d,rec_buf[3]=%d,rec_buf[4]=%d\n",
				ret,rec_buf[0],rec_buf[1],rec_buf[2],rec_buf[3],rec_buf[4]);

			while(i+sizeof(PL_MsgInfo_t) <= ret)
			{
				memcpy(&msg,rec_buf+i,sizeof(PL_MsgInfo_t));
				write_hmimsg_sem_data(&msg);
				i+=sizeof(PL_MsgInfo_t);
			}
			i=0;
		}
	}
	return 0;
}

//Initialize hmi touch message's Semaphore
int init_touchmsg_sem()
{
	INFO("init_touchmsg_sem\n");
	int ret = -1;
	ret = sem_init(&phonelink_touchmsg_sem, 0, 0);
	if (ret != 0) {
		ERROR("phonelink_touchmsg_sem init failed\n");
	}
	return ret;
}

//write data into the hmi touch message semaphore array
int write_touchmsg_sem_data(PL_TOUCH_INFO * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("write_touchmsg_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(phonelink_socket_touchmsg+phonelink_touchmsg_write_point,pMsg,sizeof(PL_TOUCH_INFO));
		if(phonelink_touchmsg_write_point>=MAX_TOUCH_MSG_SIZE-1)
		{
			phonelink_touchmsg_write_point=0;
		}
		else
		{
			phonelink_touchmsg_write_point+=1;
		}
		sem_post(&phonelink_touchmsg_sem);
		DEBUG("phonelink_touchmsg_write_point=%d\n",phonelink_touchmsg_write_point);
		return 0;
}

//Read data from the hmi touch message  semaphore array
int read_touchmsg_sem_data(PL_TOUCH_INFO * pMsg)
{
		if(pMsg ==NULL)
		{
			ERROR("read_touchmsg_sem_data pMsg==NULL!\n");
			return -1;
		}
	 	memcpy(pMsg,phonelink_socket_touchmsg+phonelink_touchmsg_read_point,sizeof(PL_TOUCH_INFO));
		if(phonelink_touchmsg_read_point>=MAX_TOUCH_MSG_SIZE-1)
		{
			phonelink_touchmsg_read_point=0;
		}
		else
		{
			phonelink_touchmsg_read_point+=1;
		}
		DEBUG("phonelink_touchmsg_read_point=%d\n",phonelink_touchmsg_read_point);
		return 0;
}

//The socket process between PhoneLinnk And HMI touch message
void *communication_with_mainapp_touchmsg_thread(void *arg)
{
	INFO("communication_with_mainapp_touchmsg_thread\n");
	int ret;
	PL_TOUCH_INFO touchmsg;;
	INFO("communication_with_mainapp_touchmsg_thread start\n");

	while(1)
	{
		connect_mainapp_touchmsg_socket:connect_mainapp_touchmsg_fd = socket_conn(PHONELINK_SOCKET_PATH_TOUCH);
		phonelink_touchmsg_read_point =0;
		phonelink_touchmsg_write_point =0;
		memset(phonelink_socket_touchmsg,0,sizeof(PL_TOUCH_INFO)*MAX_TOUCH_MSG_SIZE);
		if(connect_mainapp_touchmsg_fd <0)
		{
			sleep(5);
			continue;
		}
		INFO("communication_with_mainapp_touchmsg_thread connect success\n");
		while(1)
		{
			//recv from server
			memset(&touchmsg,0,sizeof(PL_TOUCH_INFO));
			ret = recv(connect_mainapp_touchmsg_fd,&touchmsg,sizeof(PL_TOUCH_INFO),0);
			if(ret <=0)		//表示主机连接断开
			{
				WARN("socket connect may disconnect,need reconnect\n");
				goto connect_mainapp_touchmsg_socket;
			}
			printf("touch msg recv touchmsg.serialNum=%d,x=%d,y=%d,flag=%d,x1=%d,y1=%d,flag1=%d\n",
				touchmsg.serialNum,touchmsg.x,touchmsg.y,touchmsg.flag,touchmsg.x1,touchmsg.y1,touchmsg.flag1);
			write_touchmsg_sem_data(&touchmsg);
		}
	}
	return 0;
}

//PhoneLink deal with hmi touch message from hmi
void *deal_with_touchmsg_thread(void *arg)
{
	INFO("deal_with_touchmsg_thread\n");
	PL_TOUCH_INFO touchmsg;
	while(1)
	{
		sem_wait(&phonelink_touchmsg_sem);
		read_touchmsg_sem_data(&touchmsg);
		if(touchmsg.serialNum==1)
		{
			proc_double_touch_info(touchmsg.x,touchmsg.y,touchmsg.flag,touchmsg.x1,touchmsg.y1,touchmsg.flag1);
		}	
		else if(touchmsg.serialNum==0)
		{
			proc_touch_info(touchmsg.x,touchmsg.y,touchmsg.flag);
		}
	}
	return 0;
}

