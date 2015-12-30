 /************************************************************************
* File Name : pl_hmi_communication_proc.h
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
* Multi-Include-Prevent Section
************************************************************************/
#ifndef	PL_HMI_COMMUNICATION_PROC_H_
#define	PL_HMI_COMMUNICATION_PROC_H_

/************************************************************************
* Include File Section
************************************************************************/

/************************************************************************
* Macro Define Section
************************************************************************/
#define MAX_TOUCH_MSG_SIZE 100
#define MAX_HMI_MSG_SIZE 10

#define PHONELINK_SOCKET_PATH			"/var/tmp/phoneLink"
#define PHONELINK_SOCKET_PATH_TOUCH	"/var/tmp/phoneLink_touch"

/************************************************************************
* Enum Define Section
************************************************************************/
typedef enum 
{
		WM_PLINK_UNKNOWN = 0x00,
		WM_PLINK_START=0x01,
		WM_PLINK_PARK_PLAY=0x02,
		WM_PLINK_PARK_STOP=0x03,
		WM_PLINK_PARK_REQ=0x04,
		WM_PLINK_TOUCH_MSG=0x05,
		WM_PLINK_CONNECT_STATUS_REQ=0x06,
		WM_PLINK_CONNECT_STATUS=0x07,
		WM_PLINK_ENTER_DALINK=0x08,
		WM_PLINK_LEAVE_DALINK=0x09,
		WM_PLINK_MIRACAST_CONNECT=0x10,
		WM_PLINK_MIRACAST_DISCONNECT=0x11,
		WM_PLINK_MIRACAST_REQ=0x12,
		WM_PLINK_SCREEN_DIRECTION=0x13,
		WM_PLINK_SCREEN_DIRECTION_REQ=0x14,
		WM_PLINK_SCREEN_DIRECTION_SET=0x15,
		WM_PLINK_HARDKEY_PWR_ON=0x16,
		WM_PLINK_HARDKEY_PWR_OFF=0x17,
		WM_PLINK_HARDKEY_HOME_DOWN=0x18,
		WM_PLINK_HARDKEY_HOME_UP=0x19,
		WM_PLINK_HARDKEY_MENU_DOWN=0x1A,
		WM_PLINK_HARDKEY_MENU_UP =0x1B,
		WM_PLINK_HARDKEY_BACK_DOWN=0x1C,
		WM_PLINK_HARDKEY_BACK_UP=0x1D,
		WM_PLINK_HARDKEY_SWC_SOURCE_DOWN=0x1E,
		WM_PLINK_HARDKEY_SWC_SOURCE_UP=0x1F,
		WM_PLINK_HARDKEY_SWC_PREV_DOWN=0x20,
		WM_PLINK_HARDKEY_SWC_PREV_UP=0x21,
		WM_PLINK_HARDKEY_SWC_PREV_REWIND=0x22,
		WM_PLINK_HARDKEY_SWC_NEXT_DOWN=0x23,
		WM_PLINK_HARDKEY_SWC_NEXT_UP=0x24,
		WM_PLINK_HARDKEY_SWC_NEXT_FORWARD =0x25,
		WM_PLINK_HARDKEY_SWC_VOICE_ADD=0x26,
		WM_PLINK_HARDKEY_SWC_VOICE_SUB=0x27,
		WM_PLINK_HARDKEY_SWC_ENTER_DOW=0x28,
		WM_PLINK_HARDKEY_SWC_ENTER_UP=0x29,
		WM_PLINK_HARDKEY_SWC_BACK_DOWN=0x2A,
		WM_PLINK_HARDKEY_SWC_BACK_UP=0x2B,
		WM_PLINK_HARDKEY_SWC_ANSWER_DOWN=0x2C,
		WM_PLINK_HARDKEY_SWC_ANSWER_UP=0x2D,
		WM_PLINK_HARDKEY_SWC_ANSWER_DOWN_LONG=0x2E,
		WM_PLINK_HARDKEY_SWC_ANSWER_UP_LONG=0x2F,
		WM_PLINK_HARDKEY_SWC_DECLINE_DOWN=0x30,
		WM_PLINK_HARDKEY_SWC_DECLINE_UP=0x31,
		WM_PLINK_DALINK_STATUS =0x32,
		WM_PLINK_DALINK_STATUS_REQ =0x33,
		WM_PLINK_START_DALINK =0x34,
		WM_PLINK_PHONE_TIME_REQ =0x35,
		WM_PLINK_PHONE_TIME = 0x36,
		WM_PLINK_PHONE_SIRI_REQ = 0x37,
		WM_PLINK_PHONE_SIRI_STATUS =0x38,
		WM_PLINK_DALINK_MAINMENU_STATUS = 0x39,  
		WM_PLINK_DALINK_MAINMENU_STATUS_REQ = 0x3A,
}PL_MsgType_t;

//DALINK Status
typedef enum
{
	DALINK_FORGROUND=0,			//Forground
	DALINK_BACKGROUND=1,			//Background
	DALINK_STATUS_UNKNOW =0XFF,
}DALINK_STATUS;

//DALINK Main Menu Status
typedef enum
{
	DALINK_MAIN_MENU_FORGROUND=0,
	DALINK_MAIN_MENU_BACKGROUND=1,
	DALINK_MAIN_MENU_STATUS_UNKNOW =0XFF,
}DALINK_MAIN_MENU_STATUS;

//BT status
typedef enum
{
	BT_UNCONNECT=	0x00,					
	BT_CONNECTED_HFP_HID = 0x01, //HFP HID=Ture, SPP=False				
	BT_CONNECTED_HFP_SPP = 0x02, //HFP SPP=Ture, HID=False
	BT_CONNECTED_HID_SPP = 0x03, //SPP HID=Ture, HFP=False
	BT_CONNECTED=0x04,		     //HFP HID SPP=Ture
}PL_CONNECT_STATUS;

//Miracast status
typedef enum
{
	Miracast_UNCONNECT=0X00,//Miracast Fail				
	Miracast_CONNECTED= 0X01,//Miracast OK				
	Miracast_NULL = 0X02,//HFP HID SPP OK				
}PL_Miracast_CONNECT_STATUS;

//Smartphone Screen's Status.
typedef  enum
{
	PHONE_SCREEN_ON= 0X00, 
	PHONE_SCREEN_OFF= 0X01, 
	PHONE_SCREEN_DEFAULT=0x02,
} PHONE_SCREEN_ON_OFF_STATUS;

//Open Siri
typedef enum
{
	OPEN_SIRI_FALSE =0X00,	
	OPEN_SIRI_SUCCESS =0X01,				
	OPEN_SIRI_NULL   = 0X02,		
}PL_OPEN_PHONE_SIRI_STATUS;

//Close Siri
typedef enum
{
	CLOSE_SIRI_FALSE =0X00,	
	CLOSE_SIRI_SUCCESS =0X01,				
	CLOSE_SIRI_NULL   = 0X02,		
}PL_CLOSE_PHONE_SIRI_STATUS;

// Siri Status
typedef enum
{
	OPEN_SIRI =0X00,	
	CLOSE_SIRI =0X01,				
	SIRI_NULL   = 0X02,		
}PL_OPEN_OR_CLOSE_PHONE_SIRI;

//Direction:horizontal or vertical
typedef enum
{
	HMI_SEND_PHONE_HORIZONTAL_CMD=0x00,							
	HMI_SEND_PHONE_VERTICAL_CMD     =0x01,
	HMI_SEND_PHONE_SCREEN_DEFAULT =0x02,
} HMI_SEND_PHONE_DISPLAY_MODE;


/************************************************************************
* Struct Define Section
************************************************************************/
typedef struct 
{
	PL_MsgType_t MsgType;
	unsigned char MsgBuf[256];
}__attribute__ ((packed))PL_MsgInfo_t;

//Phone Time
typedef struct
{
	unsigned char UTC_Highest_Byte;
	unsigned char UTC_Secondary_High_Byte;
	unsigned char UTC_Middle_Byte;
	unsigned char UTC_Lowest_Byte;
}PL_IPHONE_TIME;

//Touch Msg
typedef struct
{
	unsigned short x;					//x
	unsigned short y;					//y
	unsigned char  flag;				//0:UP 1:DOWN
	unsigned short x1;					//x1
	unsigned short y1;					//y1
	unsigned char  flag1;				//0:UP 1:DOWN
	unsigned char serialNum ;		      //0:one point 1:many point
}PL_TOUCH_INFO;

/************************************************************************
* Prototype Declare Section
************************************************************************/
//Get Miracast's Status
PL_Miracast_CONNECT_STATUS get_miracast_status();

//Set Miracast's Status
void set_miracast_status(PL_Miracast_CONNECT_STATUS status);

//Get DaLink Status 
DALINK_STATUS get_dalink_status();

//Set DaLink Status
void set_dalink_status(DALINK_STATUS status);

//Get DaLinkMainMenu's Status
DALINK_MAIN_MENU_STATUS get_dalink_mainmenu_status();

//Set DaLinkMainMenu's Status
void set_dalink_mainmenu_status(DALINK_MAIN_MENU_STATUS status);

//Get Screen Direction Status
HMI_SEND_PHONE_DISPLAY_MODE get_screen_direction_status();

//Set Screen Direction Status
void set_screen_direction_status(HMI_SEND_PHONE_DISPLAY_MODE status);

//Get Screen Status: ON or OFF
PHONE_SCREEN_ON_OFF_STATUS get_screen_state();

//Set Screen Status: ON or OFF
void set_screen_state(PHONE_SCREEN_ON_OFF_STATUS state);

//PhoneLink send data to HMI with socket
int send_data_to_mainapp(char *buf ,int size);

//PhoneLink send start to HMI
int sendPhoneLinkStartMsg();

//PhoneLink send bt's connect status to HMI
int send_connect_stautus(PL_CONNECT_STATUS status);

//PhoneLink send DaLink's status to HMI
int sendDalinkStatus(DALINK_STATUS status);

//PhoneLink send DaLinkMainMenu's status to HMI
int sendDalinkMainmenuStatus(DALINK_MAIN_MENU_STATUS status);

//PhoneLink send siri's status to HMI
int sendSiriStatus(char status);

//PhoneLink send smartphone's screen direction status to HMI
int send_screen_direction_status(char direction);

//PhoneLink send Park Status's requestion  to HMI
int sendParkStateReq();

//PhoneLink send smartphone's system time  to HMI
int sendPhoneGpsTime(PL_IPHONE_TIME *phone_time);

//Initialize HMI message's Semaphore
int init_hmi_msg_sem();

//write data into the hmi message semaphore array
int write_hmimsg_sem_data(PL_MsgInfo_t * pMsg);

//Read data from the hmi message  semaphore array
int read_hmimsg_sem_data(PL_MsgInfo_t * pMsg);

//PhoneLink deal with hmi  message from hmi
void *deal_with_hmi_data_thread(void *arg);

//Socket Connect Process
int socket_conn(const char * name);

//The socket process between PhoneLinnk And HMI
void *communication_with_mainapp_thread(void *arg);

//Initialize hmi touch message's Semaphore
int init_touchmsg_sem();

//write data into the hmi touch message semaphore array
int write_touchmsg_sem_data(PL_TOUCH_INFO * pMsg);

//Read data from the hmi touch message  semaphore array
int read_touchmsg_sem_data(PL_TOUCH_INFO * pMsg);

//The socket process between PhoneLinnk And HMI touch message
void *communication_with_mainapp_touchmsg_thread(void *arg);

//PhoneLink deal with hmi touch message from hmi
void *deal_with_touchmsg_thread(void *arg);

#endif/*PL_HMI_COMMUNICATION_PROC_H_*/

