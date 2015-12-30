 /************************************************************************
* File Name : pl_phone_app_protocol.h
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
* Multi-Include-Prevent Section
************************************************************************/
#ifndef PL_PHONE_APP_PROTOCOL_H_
#define PL_PHONE_APP_PROTOCOL_H_

/************************************************************************
* Include File Section
************************************************************************/


/************************************************************************
* Macro Define Section
************************************************************************/
pthread_mutex_t spp_send_proctect_mutex;				
pthread_mutex_t touch_deal_proctect_mutex;	

#define BT_SPP_ACK_MAX_PACKETS_LEN 20

#define VEHICLE_LCD_WIDTH   800										
#define VEHICLE_LCD_HEIGHT  480											

//SPP Frame Struct Define
#define BT_SPP_MESSAGE_HAED_LOW_INDEX	 				0X00
#define BT_SPP_MESSAGE_HAED_HIGH_INDEX 				0X01
#define BT_SPP_MESSAGE_CMD_TYPE_INDEX 					0X02
#define BT_SPP_MESSAGE_DFLAG_INDEX 					0X03
#define BT_SPP_MESSAGE_TRAN_ID_HIGH_INDEX 				0X04
#define BT_SPP_MESSAGE_TRAN_ID_LOW_INDEX 			0X05
#define BT_SPP_MESSAGE_LENGTH_INDEX 					0X06
#define BT_SPP_MESSAGE_PLAYLOAD_START_INDEX 			0X07
#define BT_SPP_MESSAGE_PACKET_LEN_EXCEPT_PLAYLOAD    0X0A

//Description: DA<->DALINK
#define BT_SPP_MESSAGE_HEADER_HIGH   					0X44
#define BT_SPP_MESSAGE_HEADER_LOW    					0X4C
#define BT_SPP_MESSAGE_DL_HU_ACK						0X2E
#define BT_SPP_MESSAGE_DL_HU_HEARTBEAT		        	0x2F

//Description : DA->DALINK
#define BT_SPP_MESSAGE_HU_DL_GPS							0x34
#define BT_SPP_MESSAGE_HU_DL_REQUESTPHONEINFO			0x36
#define BT_SPP_MESSAGE_HU_DL_REQUSETPHONESTATE		0x38
#define BT_SPP_MESSAGE_HU_DL_PARKSTATE					0x3A
#define BT_SPP_MESSAGE_HU_DL_MUSICCMD					0x3C
#define BT_SPP_MESSAGE_HU_DL_START_ANDROID_APP			0X40
#define BT_SPP_MESSAGE_HU_DL_ENTER_IPHONE_MAINMENU	0X40
#define BT_SPP_MESSAGE_HU_DL_TOUCH        					0xA0
#define BT_SPP_MESSAGE_HU_DL_KEY							0xA2
#define BT_SPP_MESSAGE_HU_DL_SIRI							0xA8

//Description: DALINK->DA
#define BT_SPP_MESSAGE_DL_HU_REMOTECONTROLLER			0x33
#define BT_SPP_MESSAGE_DL_HU_PHONEINFO					0x37
#define BT_SPP_MESSAGE_DL_HU_PHONESTATE					0x39
#define BT_SPP_MESSAGE_DL_HU_REQUESTPARKSTATE			0X3B
#define BT_SPP_MESSAGE_DL_HU_MUSICINFO					0x3D
#define BT_SPP_MESSAGE_DL_HU_TBT							0x3F
#define BT_SPP_MESSAGE_DL_HU_START_ANDROID_APP			0X41
#define BT_SPP_MESSAGE_DL_HU_SIRI							0xA9

//Phone Info
#define BT_SPP_MESSAGE_PHONE_INFO_RESOLUTION        	0X00			
#define BT_SPP_MESSAGE_PHONE_INFO_TYPE         			0X01			

//Start APP
#define BT_SPP_MESSAGE_PHONE_START_DALINK 				0X00
#define BT_SPP_MESSAGE_PHONE_START_NAVI 				0X01
#define BT_SPP_MESSAGE_PHONE_RESET_EA_COMM 			0X03

//Phone State
#define BT_SPP_MESSAGE_PHONE_DIRECTION					0X00			
#define BT_SPP_MESSAGE_PHONE_CONTRIL_MODE				0X01			
#define BT_SPP_MESSAGE_PHONE_DALINK_APP_STATE			0X02			
#define BT_SPP_MESSAGE_PHONE_NAVIGATION_APP_STATE		0X03			
#define BT_SPP_MESSAGE_PHONE_HDMI_STATE					0X04		
#define BT_SPP_MESSAGE_PHONE_APP_RELIABLE_STATE		0X05			
#define BT_SPP_MESSAGE_DALINK_MAINMENU_STATUS_STATE	0X06			
#define BT_SPP_MESSAGE_PHONE_SCREEN_STATUS_STATE		0X07			
#define BT_SPP_MESSAGE_PHONE_GPS_TIME					0X08
#define BT_SPP_MESSAGE_PHONE_LOCK_STATE					0x09
#define BT_SPP_MESSAGE_PHONE_NETWORK_TIME				0X0A
#define BT_SPP_MESSAGE_PHONE_SYSTEM_TIME				0X0B

//Phone Siri 
#define BT_SPP_MESSAGE_SIRI_CMD        						0X00			
#define BT_SPP_MESSAGE_SIRI_FEEDBACK         				0X01		


/************************************************************************
* Enum Define Section
************************************************************************/
//ACK Flag
typedef enum
{
	D_FLAG_NONEED_ACK =0,
	D_FLAG_NEED_ACK,
}D_FALG_ENUM;

//SmartPhone's Type
typedef enum
{
	BT_SPP_ANDROID,
	BT_SPP_IPHONE,
}BT_SPP_PHONE_TYPE;

//Status Flag
typedef enum
{
	SPP_MSG_DALINK_STATUS_BIT 			  =0X01,				
	SPP_MSG_RELIABLE_STATUS_BIT 			  =0X01<<1,			
	SPP_MSG_DALINK_MAIN_MENU_STATUS_BIT=0X01<<2,			
	SPP_MSG_SCREEN_DIRECTION_BIT		  =0X01<<3,			
	SPP_MSG_SCREEN_RESOLUTION_BIT		   =0X01<<4,			
	SPP_MSG_CONTROL_MODE_BIT 			   =0X01<<5,			
}PHONE_PROTOCOL_MSG_FLAG;		

//Touch Donw or Up or Move Status
typedef enum
{
	TOUCH_UP=0x00,		//Up
	TOUCH_DOWN=0x01,	//Down
	TOUCH_MOVE =0x02,  //Move
}TOUCH_STATUS;

//�ֻ������ϱ�
typedef enum
{
	BT_CONNECT_NULL_PHONE =0,
	BT_CONNECT_ANDROID =1,
	BT_CONNECT_IPHONE =2 ,
}PHONE_TYPE;

typedef enum {
	BT_CONNECT_NONE,									//û���κ��ֻ�
	BT_CONNECTED_ANDROID_SPP	,						//������android�豸��ͨ��SPP��ʽ���Ϳ�������
	BT_CONNECTED_ANDROID_HID							//������android�豸��ͨ��HID��ʽ���Ϳ�������
} PHONE_CONNECT_MODE;
PHONE_CONNECT_MODE phone_current_mode ;			//��ǰ�ֻ����ӵ�״̬

//�ֻ�������״̬
typedef enum {
	PHONE_HORIZONTAL,	//����
	PHONE_VERTICAL,//����
} PHONE_DISPLAY_MODE;
PHONE_DISPLAY_MODE display_mode;					//�ֻ�������ز���


//פ��״̬
typedef enum
{
	PARK_STATE_STOP = 0x00,		//פ��״̬
	PARK_STATE_PLAY = 0x01,		//�г�״̬
}PARK_STATE;
PARK_STATE park_state;

/************************************************************************
* Struct Define Section
************************************************************************/
//DaLink' ACK Struct
typedef struct 
{
	short tran_id ;        							
	char cmd_id;									
} BT_SPP_DALINK_ACK;

//��Ļ���ز���
typedef struct _TOUCH_OPERATION_PARAM
{
	// �ֻ��ֱ���
	int phone_height ;
	int phone_width ;
	int screenWidth;				//������Ļ���
	int screenHeight;				//������Ļ�߶�
	int vaild_axis_x_start;		//��Ļ����Ч������Χ--x����ʼ
	int vaild_axis_x_end ;		//��Ļ����Ч������Χ--x�����
	int vaild_axis_y_start;		//��Ļ����Ч������Χ--y����ʼ
	int vaild_axis_y_end ;		//��Ļ����Ч������Χ--y�����#define TOUCH_INFO_PROC_H_
	int axisX;   					//��ǰx����
	int axisY;   					//��ǰy����
	int axisXp;  					//��һ��x����
	int axisYp;  					//��һ��y����
	int axisX1;   					//��ǰx1����
	int axisY1;   					//��ǰy1����
	int axisX1p;  					//��һ��x1����
	int axisY1p;  					//��һ��y1����
	double moveXfix;  			//�ֱ�������ϵ��
	double moveYfix;  			//�ֱ�������ϵ��
	float iphonemoveXfix;		//iphone����ƶ�ϵ������ϵ����Ҫ���ݷֱ���ϵ���͹���ٶ�һ�������
	float iphonemoveYfix;
}TOUCH_OPERATION_PARAM;
TOUCH_OPERATION_PARAM pl_screen_operation_param;


/************************************************************************
* Prototype Declare Section
************************************************************************/
//Reset HeartBeat Flag
void reset_heart_beat_flag();

//Set HeartBeat Flag
void set_heart_beat_flag();

//Get HeartBeat Flag
int get_heart_beat_flag();

//Reset SmartPhone Protocol Recive Flag
int reset_phone_protocol_recive_flag();

//Get SmartPhone Protocol Recive Flag
int  get_phone_protocol_recive_flag();

//Wakeup SmartPhone
int check_screen_state_and_wakeup();

//Initialize DaLink Protocol ACK Semapore
int init_dalink_protocol_ack_sem();

//PhoneLink Send SPP Data To Smartphone's DaLink
int protected_spp_send_data(BT_SPP_PHONE_TYPE phone_type,unsigned char cDataLen ,unsigned char *pData);

//PhoneLink Deal With SPP Data From SmartPhone's DaLink 
int phone_app_msg_deal(int len ,char * buf);

//PhoneLink Deal With SmartPhone's Resolution From DaLink
int dalink_spp_resolution_data_dispose(char * buf );


//PhoneLink Deal With SmartPhone's Screen Direction From DaLink
int dalink_spp_phone_dirction_data_dispose(char dirction);

//PhoneLink Deal With DaLink Status From DaLink
int dalink_spp_phone_dalink_app_state_dispose(char state);

//PhoneLink Deal With DaLinkMainMenu Status From DaLink
int dalink_spp_mainmenu_state_dispose(char state);

//PhoneLink Deal With Park's Requestion From DaLink
int dalink_spp_phone_request_park_state();

//Siri's Status Feedback
int dalink_spp_phone_siri_feedback(char open_or_close_status,char success_or_false );

//PhoneLink Send Siri CMD To Smartphone's DaLink
int dalink_spp_phone_siri_cmd(char open_or_close);

//PhoneLink Deal With SPP Data From DaLink
int dalink_spp_packet_deal(char * packet_header,int len);

//Checkout Every SPP Frame From DaLink
int dalink_check_packet_integrity(char * source_buf,char len, char ** packet_header_point);

//Compute  SPP Frame's CheckSum
char dalink_bluetooth_SPP_checkSum(char *pArray, int len);

//PhoneLink Create SPP Frame Struct
int dalink_create_spp_protocol_packet(char type, short tranID,char D_flag,char *pArray, int len,char * desbuf);

//PhoneLink Send SPP Data To Smartphone's DaLink
int bt_spp_send_data_to_dalink(BT_SPP_PHONE_TYPE phone_type,unsigned char cDataLen ,unsigned char *pData);

//PhoneLink Send Start DaLink CMD To SmartPhone
int start_android_dalink_app();

//Create ACK SPP Packet
int create_ack_packet(short ack_tranid ,char cmd,char *desbuf );

//PhoneLink Send ACK To DaLink
int pl_dalink_send_ack_packet(BT_SPP_PHONE_TYPE phone_type,short ack_tranid ,char cmd);

//Create Heartbeat SPP Packet
int create_heartbeat_packet(char *desbuf);

//PhoneLink Send Heartbeat To DaLink
int pl_dalink_send_heartbeat_packet(BT_SPP_PHONE_TYPE phone_type);

//Create TouchMsg SPP Packet
int create_touchscreen_packet(unsigned short x, unsigned short y ,char touch_flag ,char multi_serial,char *desbuf);

//PhoneLink Send TouchMsg To DaLink
int pl_dalink_send_touchscreen_packet(BT_SPP_PHONE_TYPE phone_type,unsigned short x, unsigned short y ,char touch_flag ,char multi_serial);

//PhoneLink Send SPP TouchMsg To DaLink
void send_spp_mode_cotrol_data(int _actionIn, int _serialNum, int _axisX, int _axisY);

//PhoneLink Send HID TouchMsg To SmartPhone
void send_hid_mode_cotrol_data(int _actionIn, int _serialNum, int _axisX, int _axisY);

//Create Key Message SPP Packet
int create_key_info_packet(char key_value ,char key_flag ,char * desbuf);

//PhoneLink Send Key  Message To DaLink
int pl_dalink_send_key_info_packet(BT_SPP_PHONE_TYPE phone_type,char key_value ,char key_flag);

//Create Request SmartPhone's information  SPP Packet
int create_request_phoneinfo_packet(char phone_info_class ,char * desbuf);

//PhoneLink Send SmartPhone's information To DaLink
int pl_dalink_send_request_phoneinfo_packet(BT_SPP_PHONE_TYPE phone_type,char phone_info_class);

//Create Request SmartPhone's Status  SPP Packet
int create_request_phonestate_packet(char phone_state_class ,char * desbuf);

//PhoneLink Send SmartPhone's Status To DaLink
int pl_dalink_send_request_phonestate_packet(BT_SPP_PHONE_TYPE phone_type,char phone_state_class);

//Create Request Park Information  SPP Packet
int create_parkinfo_packet(char park_state ,char * desbuf);

//PhoneLink Send Park Information To DaLink
int pl_dalink_send_parkinfo_packet(BT_SPP_PHONE_TYPE phone_type,char park_state);

//Create Siri  Status SPP Packet
int create_siri_packet(char siri_status,char *desbuf);

//PhoneLink Send Siri Status To DaLink
int pl_dalink_send_siri_packet(BT_SPP_PHONE_TYPE phone_type,char siri_status);

//Create Siri  Cmd SPP Packet
int create_siri_feedback_packet(char siri_cmd,char siri_status,char *desbuf);

//PhoneLink Send Siri Cmd To DaLink
int pl_dalink_send_siri_feedback_packet(BT_SPP_PHONE_TYPE phone_type,char siri_cmd,char siri_status);

//Wait DaLink's Ack Response
int wait_dalink_ack_response(int time_out) ;

//Get  ACK From DaLink
BT_SPP_DALINK_ACK * get_dalink_ack_data(void);

//Post DaLink's ACK Semapore
int post_bt_spp_ack_wait_sem(unsigned char command_id,unsigned short trans_id);

//Wait DaLink's ACK
int wait_dalink_ack(int time_ms,short tran_id ,char cmd_id);

#endif/*PL_PHONE_APP_PROTOCOL_H_*/
