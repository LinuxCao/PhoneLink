#ifndef _BTCSMSG_H_
#define _BTCSMSG_H_
#include<stdlib.h>

#define MSG_BUF_SIZE	2048
#define BT_MSG_DATA_LEN	512

#define CONTACTS_BUFF_SIZE 512
#define PAIREDDEV_BUFF_SIZE 392

#define BT_NAME_SIZE 100

typedef enum __BtVisibleConfigType{
	BIT_DISCOVERABLE = 0x01,
	BIT_CONNECTABLE = (0x01 << 1),
	BIT_PAIRINGCONTROL = (0x01 << 2),
}BtVisibleConfigType;

typedef enum __BtProfoileStatusType{
	BIT_HFP = 0x01,
	BIT_A2DP = (0x01 << 1),
	BIT_SPP = (0x01 << 2),
	BIT_HID = (0x01 << 3),
	BIT_AVRCP = (0x01 << 4),
	BIT_DUN = (0x01 << 5),
	BIT_OPP = (0x01 << 6),
}BtProfoileStatusType;

typedef enum
{
	BT_EVENT_NONE = 0x00,

//RSP
	BT_MSG_HID_STATE  = 0x0B,
	BT_MSG_HID_SEND_DATA_RES,
	BT_MSG_HID_REVICE_DATA,

	BT_MSG_SPP_STATE,
	BT_MSG_SPP_SEND_DATA_RES,
	BT_MSG_SPP_REVICE_DATA,

//	HardKey
	BT_MSG_HARD_KEY,
	//BT_MSG_RFCOMM_CONNECTED,
	//BT_MSG_RFCOMM_DISCONNECTED,

	BT_MSG_UPDATE_POWER_STATE  = 0x10000,// 1

	BT_MSG_DISCONNECT,						//0x10001
	BT_MSG_UPDATE_PAIR_LIST,				//0x10002

	BT_MSG_UPDATE_DEV_INFO,					//0x10003

	BT_MSG_AUDIO_TRANSFER_PHONE,			//0x10004
	BT_MSG_AUDIO_TRANSFER_HANDFREE,			//0x10005
	BT_MSG_AUDIO_TRANSFER_ERROR,			//0x10006

	BT_MSG_UPDATE_CONTACTS,					//0x10007
	BT_MSG_CONTACT_ONE,						//0x10008
	BT_MSG_UPDATE_CALL_LOG,					//0x10009
	BT_MSG_CALL_LOG_ONE,					//0x1000a
	BT_MSG_PB_DOWNLOAD_ERROR,				//0x1000b

	BT_MSG_HFP_SIGNAL,						//0x1000c
	BT_MSG_HFP_BATTCHG,						//0x1000d

	BT_MSG_HFP_OUTGOINGCALL,				//0x1000e
	BT_MSG_HFP_COMINGCALL,					//0x1000f
	BT_MSG_HFP_ACTIVE,						//0x10010
	BT_MSG_HFP_FREE,						//0x10011
	BT_MSG_HFP_CALLER_NUM,					//0x10012
	BT_MSG_HFP_CALLER_NAME,					//0x10013
	BT_MSG_HFP_DIAL_NUM_ERROR,				//0x10014

	BT_MSG_A2DP_PLAY,						//0x10015
	BT_MSG_A2DP_STOP,						//0x10016

	BT_MSG_PAIRING_REQUEST,					//0x10017
	BT_MSG_CODEC_POP_MUTE_ON,				//0x10018
	BT_MSG_CODEC_POP_MUTE_OFF,				//0x10019
	BT_MSG_MIC_MUTE,						//0x1001a
	BT_MSG_MIC_UNMUTE,						//0x1001b

	BT_MSG_UPDATE_BLUETOOTH_NAME,			//0x1001c
	BT_MSG_UPDATE_BLUETOOTH_PIN_CODE,		//0x1001d
	BT_MSG_SET_BLUETOOTH_NAME_ERROR,		//0x1001e
	BT_MSG_SET_BLUETOOTH_PIN_CODE_ERROR,	//0x1001f

	BT_MSG_DEV_MANU_FACTORER,				//0x10020

	BT_MSG_DOWNLOAD_PROGRESS,				//0x10021

	BT_MSG_CALL_LOG_DOWNLOAD_ERROR,			//0x10022
	BT_MSG_SSP_CONFIRMATION,				//0x10023
	BT_MSG_DEL_PHONEBOOK,					//0x10024
	BT_MSG_UPDATE_HFP_CONNECTING_DEVID,		//0x10025
	BT_MSG_UPDATE_HFP_CONNECT_FAILD_DEVID,	//0x10026
	BT_MSG_UPDATE_VISIBLE_CONFIG,			//0x10027
	BT_MSG_PROFILE_CONNECT,					//0x10028
	BT_MSG_PROFILE_DISCONNECT,				//0x10029
	BT_MSG_PAIR_SUCCESS,					//0x1002a
	BT_MSG_PAIR_FAIL,						//0x1002b
	BT_MSG_UPDATE_MUSIC_INFO,				//0x1002c
	BT_MSG_UPDATE_CALLER_INFO,				//0x1002d
	BT_MSG_VOICE_DIAL_ACTIVE,				//0x1002e
	BT_MSG_VOICE_DIAL_TIME_OUT,				//0x1002f
	BT_MSG_VOICE_DIAL_TERMINATED,			//0x10030
}BTEventType;

typedef enum
{
	HardKey_SWC_Vol_Up_Press = 0x01,
	HardKey_SWC_Vol_Down_Press = 0x02,
	HardKey_SWC_Previous_Press = 0x03,
	HardKey_SWC_Next_Press = 0x04,
	HardKey_SWC_Source_Press = 0x05,
	HardKey_Rot_Vol_Up = 0x10,
	HardKey_Rot_Vol_Down = 0x11,
	HardKey_PWR_PRESS = 0x12,
	HardKey_TUNEUP_PRESS = 0x13,
	HardKey_TUNEDOWN_PRESS = 0x14,
	HardKey_TUNE_PUSH_PRESS = 0x15,
	HardKey_CAMERA_PRESS = 0x16,
	HardKey_AUDIO_PRESS = 0x17,
	HardKey_CD_AUX_PRESS = 0x18,
	HardKey_FM_AM_PRESS = 0x19,
	HardKey_USB_PRESS = 0x1A,
	HardKey_PHONE_PRESS = 0x1B,
	HardKey_PRESET1_PRESS = 0x1C,
	HardKey_PRESET2_PRESS = 0x1D,
	HardKey_PRESET3_PRESS = 0x1E,
	HardKey_PRESET4_PRESS = 0x1F,
	HardKey_PRESET5_PRESS = 0x20,
	HardKey_PRESET6_PRESS = 0x21,
	HardKey_SCAN_PRESS = 0x30,
	HardKey_MENU_PRESS = 0x31,
	HardKey_BACK_PRESS = 0x32,
	HardKey_SEEKUP_PRESS = 0x33,
	HardKey_SEEKDOWN_PRESS = 0x34,
	HardKey_EJECT_PRESS = 0x40
}HardKeyValue;

typedef enum __BtPhonebookNumberType{
	BtNumPrefered = 0x01,
	BtNumHome,
	BtNumWork,
	BtNumCell,
	BtNumOthers,
	BtNumUnknown,
}BtPhonebookNumberType;

typedef struct _BTCSMSG
{
	int eEventType;
	int  nDataLen;
	unsigned char szData[BT_MSG_DATA_LEN];
}__attribute__ ((packed))BTCSMSG;

typedef enum
{
	BT_REQ_SUCCESS = 0X00,
	BT_REQ_ERROR,
	BT_REQ_SLC_NOT_ESTABLISHED
}BTCsReqRes;

typedef enum __BtCsSppState{
	SppState_ReadyNoInitialized = 0X00,
	SppState_Ready = 0X01,
	SppState_Connecting = 0X02,
	SppState_Connected = 0X03,
	SppState_ErrorSlcAlreadyEstablished = 0X92
}BtCsSppState;

typedef enum __BtCsHidState{
	HidState_ReadyNotConnect = 0X01,
	HidState_Connecting = 0X02,
	HidState_Connected = 0X03,
	HidState_ErrorSlcAlreadyEstablished = 0X92,
}BtCsHidState;

typedef enum __BtMicMuteState{
	BtMicStateMute = 0x00,
	BtMicStateUnMute = 0x01,
}BtMicMuteState;

typedef enum
{
	BT_HID_DEV_NONE = 0x00,
	BT_HID_DEV_MOUSE,
	BT_HID_DEV_KEYBOARD
}BTHidDevType;

typedef enum
{
	BT_HID_M_BUTTON_MOVE_NONE = 0x00,
	BT_HID_M_BUTTON_MOVE_UP,
	BT_HID_M_BUTTON_MOVE_DOWN = 0xff
}BTMiddleButtonMoveType;

typedef enum
{
	BT_HID_BUTTON_NONE_OR_UP = 0x00,
	BT_HID_BUTTON_L_DOWN,
	BT_HID_BUTTON_R_DOWN,
	BT_HID_BUTTON_M_DOWN
}BTButtonStatus;

typedef enum
{
	BTPBError_uknown = 0X00,
	BTPBError_PhoneBook_is_not_supported,
	BTPBError_invalid_param,
	BTPBError_wrong_state,
	BTPBError_Serial_flash_bad,
	BTPBError_Time_out,
	BTPBError_HFP_is_not_connected,
	BTPBError_HFP_disconnected_at_downLoading,
	BTPBError_PB_size_is_0,
	BTPBError_Busy,
}BTPBDownloadErrorType;

typedef struct _BTHidMouseData
{
	short xOffset;
	short yOffset;
	BTMiddleButtonMoveType eMiddleButtonMoveType;
	BTButtonStatus eButtonStatus;
}__attribute__ ((packed))BTHidMouseData;

typedef enum
{
	BT_HID_AUXILIARY_KEY_NONE = 0X00,
	BT_HID_AUXILIARY_KEY_LEFT_CONTROL = 0X01,
	BT_HID_AUXILIARY_KEY_LEFT_SHIFT = 0X02,
	BT_HID_AUXILIARY_KEY_LEFT_ALT = 0X04,
	BT_HID_AUXILIARY_KEY_LEFT_GUI = 0X08,
	BT_HID_AUXILIARY_KEY_RIGHT_CONTROL = 0X10,
	BT_HID_AUXILIARY_KEY_RIGHT_SHIFT = 0X20,
	BT_HID_AUXILIARY_KEY_RIGHT_ALT = 0X40,
	BT_HID_AUXILIARY_KEY_RIGHT_GUI = 0X80,
}BTHidAuxiliaryKeyType;

#ifndef __KEY_PHONE_LINK_CAO
typedef enum
{
	KEY_ESC = 0x29,
	KEY_HOMEPAGE = 0x223,
	KEY_BACK = 0x224,
	KEY_COMPOSE = 0x65,

	KEY_DOWN = 0x51,
	KEY_UP = 0x52,
	KEY_RIGHT = 0x4f,

	KEY_LEFT = 0x50,
	KEY_ENTER = 0x58,
	KEY_PLAYPAUSE = 0xcd,

	KEY_VOLUMEUP = 0xe9,
	KEY_VOLUMEDOWN = 0xea,
	KEY_PREVIOUSONG = 0xb6,

	KEY_NEXTSONG = 0xb5,
}BTHidPhoneKeyValue;
#else
typedef enum
{
	BT_KEY_ESC = 0x29,
	BT_KEY_HOMEPAGE = 0x223,
	BT_KEY_BACK = 0x224,
	BT_KEY_COMPOSE = 0x65,

	BT_KEY_DOWN = 0x51,
	BT_KEY_UP = 0x52,
	BT_KEY_RIGHT = 0x4f,

	BT_KEY_LEFT = 0x50,
	BT_KEY_ENTER = 0x58,
	BT_KEY_PLAYPAUSE = 0xcd,

	BT_KEY_VOLUMEUP = 0xe9,
	BT_KEY_VOLUMEDOWN = 0xea,
	BT_KEY_PREVIOUSONG = 0xb6,

	BT_KEY_NEXTSONG = 0xb5,
}BTHidPhoneKeyValue;
#endif

typedef struct _BTHidKeyBoardData
{
	BTHidAuxiliaryKeyType eAuxiliaryKeyType;
	unsigned int cKey;
}__attribute__ ((packed))BTHidKeyBoardData;

typedef enum __BtReqSetScanMode{
	Not_discoverable_and_not_connectable = 0x00,
	Discoverable_but_not_connectable,
	Not_discoverable_but_connectable,
	Discoverable_and_connectable
}BtReqSetScanMode;

typedef enum
{
	BT_POWER_OFF = 0X00,
	BT_IS_READY,
	BT_POWER_ON_INITING,
}BTPowerState;

typedef enum __BtIndHfpState{
	HfpStateNotInitialized = 0X00,
	HfpStateReady ,
	HfpStateConnecting ,
	HfpStateConnected ,
	HfpStateOutgoingCall ,
	HfpStateInComingCall ,
	HfpStateCallIsActive ,
	HfpStateThreeWayCallWaiting ,
	HfpStateThreeWayMultiCall ,
	HfpStateThreeWayCallOnHold
}BtIndHfpState;

typedef enum
{
	BT_A2DP_PLAY = 0X00,
	BT_A2DP_STOP,
}BTMusicState;

typedef enum __BtReqSetPairingControlMode{
	BtPairReject = 0X00,
	BtPairAccept,
	BtPairForwardPairingRequestToHost
}BtReqSetPairingControlMode;

typedef enum __BtReqSlcType{
	BtReqSlc_HFP = 0X01,
	BtReqSlc_A2DP_CT = 0x02,
	BtReqSlc_SPP = 0x04,
	BtReqSlc_A2DP_TG = 0x05,
	BtReqSlc_DUN = 0x07,
	BtReqSlc_FTP = 0x08,
	BtReqSlc_HFP_AG = 0x09,
	BtReqSlc_iAP = 0x0B,
	BtReqSlc_HID = 0x0C,
	BtReqSlc_ALL = 0xFF
}BtReqSlcType;

typedef enum __BtReqAudioTransferType{
	BtReqAudioTransfeHF2AG = 0X00,
	BtReqAudioTransfeAG2HF
}BtReqAudioTransfeType;

typedef enum __BtStorageType{
	BtSIM = 0x01,
	BtPhoneMemory,
	BtMissed,
	BtReceived ,
	BtDialed,
	BtContactsAll = 0xF0,
	BtCallLogAll = 0xF1,
	BtStorageAll = 0xFF  //BtMissed + BtReceived + BtDialed
}BtStorageType;
typedef BtStorageType storagetype_t;


#if 0
typedef enum __THREE_WAY_CALL_STATUS{
	THREE_WAY_HANGUP = -1,
	THREE_WAY_ACTIVE,
	THREE_WAY_HOLD,
	THREE_WAY_WAIT,
}three_way_call_status;
#else
typedef enum __THREE_WAY_CALL_STATUS{
	THREE_WAY_HANGUP= -1,
	THREE_WAY_ACTIVE,
	THREE_WAY_HELD,
	THREE_WAY_DIALING,	//(outgoing calls only) 
	THREE_WAY_ALERTING,	//(outgoing calls only) 
	THREE_WAY_INCOMING,	//(incoming calls only) 
	THREE_WAY_WAITING,	//(incoming calls only) 
}three_way_call_status;
#endif

typedef enum __WAITING_CALL_PROC_TYPE{
	HANG_UP_WAITING_CALL = 0,
	HANG_UP_CURRENT_AND_ACCEPT_WAITING_CALL,
	SWITCH_CALL,
	CONFERENCE_CALL,
}waiting_call_proc_type;

#define CONTACTS_GB2312NAME_SIZE 101
#define CONTACTS_NAME_SIZE 100
#define CONTACTS_NUM_SIZE 30

#define PAIREDDEV_NAME_SIZE 100
#define DEVICE_NAME_SIZE_MAX	33
#define DEV_MANU_FACTORER_STR_SZIE	DEVICE_NAME_SIZE_MAX
#define BT_PIN_CODE_SIZE		5

#define FIRST_LETTER_MAX	21

typedef struct _head_data {
        struct list_head *next, *prev;
}head_data_t;

typedef enum __BtParingType{
	BT_PAIRING_TYPE_REQUEST = 0X01,
	BT_PAIRING_TYPE_SSP_CONFIRMATION = 0X02,
}BtParingType;

//End
typedef struct _PAIRNUMREQ
{
	BtParingType eParingType;
	unsigned char BdAddr[6];//remote device
	unsigned char PairingMethod;
	unsigned int 	PairingCode;
	unsigned char DevName[PAIREDDEV_NAME_SIZE];
}PAIRNUMREQ;

typedef struct __btLocalDevInfo
{
	//unsigned char status;
	unsigned char BT_address[6];
	unsigned int major;
	unsigned int minor;
	unsigned int model_number;
	unsigned int ctrl_number;
	unsigned char pin_code_size;
	unsigned char pin_code[BT_PIN_CODE_SIZE];
	unsigned char device_name_size;
	char device_name[DEVICE_NAME_SIZE_MAX];
}BtLocalDevInfo;

typedef enum __BtConnectionStateType{
	BT_CONNECTION_OPERATE_TYPE_NONE = 0X00,
	BT_CONNECTION_STATE_CONNECT,
	BT_CONNECTION_STATE_DISCONNECT,
	BT_CONNECTION_OPERATE_CONNECT,
	BT_CONNECTION_OPERATE_DISCONNECT
}BtConnectionStateType;

typedef struct _ConnectionOperate
{
	BtConnectionStateType nConnectionState;
	unsigned int nConnectOperate;
	unsigned int nDisconnectOperate;
	unsigned int nLastOperate;
	unsigned int nConnectDevID;
}ConnectionOperate_t;

typedef struct _state_t
{
	BtLocalDevInfo local_dev_info;
	unsigned char szDevManuFactorer[DEV_MANU_FACTORER_STR_SZIE];
	BTMusicState eBtMusicState;
	BtIndHfpState eHfpState;
	int eVoiceDialState;
	BtMicMuteState eMicMuteState;
	BtReqAudioTransfeType audioTransfeMode;
	int nHfpConnectingDevId;
	int nHfpConnectFaildDevId;
	int nBtAutoBrowseState;
	int eA2dpState;
	int eAvrcpState;
	ConnectionOperate_t nextConnectionOperate;
	int nAudioSetStreamMode;
}state_t;


typedef struct _PAIRLIST
{
	unsigned char DeviceID;
	unsigned int status;
	unsigned char BdAddr[6];
	unsigned char DevName[PAIREDDEV_NAME_SIZE];
	int nSupportService;

	head_data_t hlist;
}PAIRLIST;
typedef PAIRLIST pairlist_t;

typedef struct _CONTACTSLIST
{
	BtStorageType StorageType;
	BtPhonebookNumberType NumberType;
	unsigned char gb2312_name_size;
	unsigned char gb2312_name[CONTACTS_GB2312NAME_SIZE];
	char utf8_contact_name[CONTACTS_NAME_SIZE];
	char first_letter[FIRST_LETTER_MAX];
	char characterTone[FIRST_LETTER_MAX];	//Neusoft/xuyk Addd
	unsigned char Number[CONTACTS_NUM_SIZE];
	BtStorageType PbType;
	//char szBuff[CONTACTS_BUFF_SIZE];
	int numberCount;
	BtPhonebookNumberType Number1Type;
	unsigned char Number1[CONTACTS_NUM_SIZE];
	BtPhonebookNumberType Number2Type;
	unsigned char Number2[CONTACTS_NUM_SIZE];
	BtPhonebookNumberType Number3Type;
	unsigned char Number3[CONTACTS_NUM_SIZE];

	head_data_t hlist;
}CONTACTSLIST;

typedef CONTACTSLIST contacts_t;

typedef struct _BT_MUSIC_INFO
{
	int nCurTime;
	int nTotalTime;
	char szMediaTitle[BT_NAME_SIZE];
	char szArtistName[BT_NAME_SIZE];
	char szAlbumName[BT_NAME_SIZE];
	
}BT_MUSIC_INFO;
typedef BT_MUSIC_INFO bt_music_info_t;

#define BT_CALLER_MAX 2
typedef struct _BT_CALLER_INFO
{
	int nCall1Status;
	char szNum[BT_NAME_SIZE];
	char szName[BT_NAME_SIZE];
}BT_CALLER_INFO;
typedef BT_CALLER_INFO bt_caller_info_t;

typedef enum __BtReqSspConfirmation{
	BtSspConfirmationReject = 0X00,
	BtSspConfirmationAccept
}BtReqSspConfirmation;

#endif
