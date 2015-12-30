#ifndef _BTDATA_H_
#define _BTDATA_H_

#include "BtCSMsg.h"


#ifndef HMI_DEBUG
//#ifndef LINUX_EVN
#define LINUX_EVN
//#endif
#endif


typedef int   BOOL;
#define TRUE  1
#define FALSE 0

typedef int (*BtDataProCallBack)(BTCSMSG *);
typedef struct _BT_INFO
{
	//unsigned char *curr_mac;
	int (*fGetCurrCallNum)(char *pBuff,int nBuffLen);
	int (*fGetCurrCallName)(char *pBuff,int nBuffLen);
	int (*fGetSingnalStrength)();
	int (*fGetBattLevel)();
	int (*fGetHfpState)();
	int (*fGetAudioState)();

	int (*fGetPairdDevCount)();
	int (*fGetContactsCount)(BtStorageType);
	int (*fCurConnect)();
	int (*fGetPowerState)();
	BOOL (*fIsBrowsed)(BtStorageType);
	int (*fGetBrowseing)();

	void (*fBtTurnOn)();/**/
	void (*fBtTurnOff)();/**/

	//request
	void (*fLocalDevInfo) ();/**/
	void (*fScanMode) (BtReqSetScanMode);/**/
	void (*fPairingControl) (BtReqSetPairingControlMode);/**/
	void (*fSetPinCode) (unsigned char *);/**/
	void (*fGetPairedList) ();/**/
	void (*fEstablishSlc) (unsigned char,BtReqSlcType,unsigned char);/**/
	int (*fDelPaired) (unsigned char);/**/
	void (*fDisconnectSlc) (BtReqSlcType);/**/
	void (*fCheckLink) ();/**/
	void (*fAnswerCall) ();/**/
	void (*fTerminateCall)();/**/
	void (*fRejectCall)();/**/
	void (*fCall) (char*);/**/
	void (*fRedial) ();/**/
	void (*fAudioTransfer) (BtReqAudioTransfeType);/**/
	void (*fGetCurrCalls) ();/**/
	void (*fDtmf) (unsigned char);/**/

	void (*fPbBrowse)(BtStorageType,unsigned short,unsigned short);/**/
	void (*fPbDownloadCancel)();

	void (*fPlay) ();
	void (*fPause) ();
	void (*fStop) ();
	void (*fForward) ();
	void (*fBackward) ();
	void (*fFastForward) (int);
	void (*fBackForward) (int);
	void (*fGetPlayStatus) ();

	void (*fAnswerCallPrivate) ();
	void (*fMicMute) (BtMicMuteState);
	void (*fSetLocalDevName) (char *);
	int (*fGetState)(state_t *);
	int (*fGetBtMusicState)();
	int (*fGetBtMicMuteState)();	
	int (*fGetPinCode)(char *pBuff,int nBuffLen);
	int (*fGetLocalDevName)(char *pBuff,int nBuffLen);
	int (*fGetMacAddr)(char *pBuff,int nBuffLen);

	int (*fSyncBtPowerState) (unsigned char);/**/
	int (*fFactorySetting) ();/**/
	void (*fDelPhonebook) (BtStorageType storageType,unsigned short start,unsigned short end);
	void (*fSyncState) ();/**/

	int (*fCurConnectInfo)(PAIRLIST *pBtPairedDev);
	int (*fGetHfpConnectingDevId)();
	void (*fSetVisibleConfig)(int);

	int (*fPbReBrowse)(BtStorageType);/**/
	int (*fSendVol2BtModule)(int);
	int (*fGetHfpConnectFaildDevId)();
	int (*fVoiceDial)();
	int (*fHangupVoiceDial)();
	int (*fGetMusicInfo)(bt_music_info_t *);
	int (*fGetCallerInfo)(int, bt_caller_info_t *);
	int (*fCallWaiting)(int);
	int (*fSetAutoDownloadPB)(int);
	int (*fGetAutoDownloadPB)(int *);
	int (*fAudioSetStreamMode)(int);
	int (*fAudioGetStreamMode)();

	//indicator
}BT_INFO;

extern BT_INFO BtModuleInfo;

#ifdef LINUX_EVN

void RegistBtDataPro(BtDataProCallBack n_bt_data_pro);

int init_Bt_client();
int uinit_Bt_client();

void sendMouseData(BTHidMouseData *pData);
void sendKeyBoardData(BTHidKeyBoardData *pData);
void sendSpecificKeyBoardData(BTHidKeyBoardData *pData);
void sendPhoneKey(unsigned short cKey ,int key_status);
void sendSppData(unsigned char cDataLen ,unsigned char *pData);
void sendHidQueryState();
void sendSppQueryState();

void sendHidConnect();
void sendSppConnect();
void sendRfcommConnect();
void sendMHLMode(unsigned char VideoType,unsigned char Mode);

void s2cHardKey(unsigned char key);
int readBtState(char *section,char *key, int *nBtState);
int readBtPairedDevByIndex(PAIRLIST *pBtPairedDev, int index);
int getContactInfo(CONTACTSLIST *pContact, int nContactNum);
int getContactInfoByIndex(CONTACTSLIST *pContact, int nIndex ,BtStorageType StorageType);
int getBtVersion();
void sendPairConfirmation(BtReqSspConfirmation respond);

int check_Connect(BtProfoileStatusType nType);
int getPairedDevCount();
void sendSetDebug(unsigned int nDebug);
int getPinCode(char *pBuff,int nBuffLen);
int getLocalDevName(char *pBuff,int nBuffLen);
int getMacAddr(char *pBuff,int nBuffLen);
int getDevManuFactorer(char *pBuff,int nBuffLen);
int sendSyncBtPowerState(unsigned char state);
int bt_factorySetting();

int readBtPairedDevByDevId(PAIRLIST *pBtPairedDev, int devId);

int check_support_service(PAIRLIST BtPairedDev, BtProfoileStatusType nType);
#endif


#endif
