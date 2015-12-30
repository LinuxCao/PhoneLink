 /************************************************************************
* File Name : pl_bluetooth_recive_thread.h
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
* Multi-Include-Prevent Section
************************************************************************/
#ifndef PL_BLUETOOTH_RECIVE_THREAD_H_
#define PL_BLUETOOTH_RECIVE_THREAD_H_

/************************************************************************
* Include File Section
************************************************************************/
#include "BtCSMsg.h"
#include "pl_phone_app_protocol.h"
#include "pl_hmi_communication_proc.h"

/************************************************************************
* Prototype Declare Section
************************************************************************/

//Get SmartPhone's Type
PHONE_TYPE get_phone_type();

//Set SmartPhone's Type
void  set_phone_type(PHONE_TYPE var);

//Get Bt Connect Status
PL_CONNECT_STATUS get_current_phone_connect_status();

// Save Bt Connect Status
void set_current_phone_connect_status(PL_CONNECT_STATUS status);

//Get SPP's Status
BtCsSppState get_bt_spp_state(void);

//Get HID's Status
BtCsHidState get_bt_hid_state(void);

//Get HFP's Status
BtIndHfpState get_bt_hfp_state(void);

//Initialize PhoneLink
 int init_phone_link(void);

//Initialize Bt's Semaphore
 int init_bt_sem();

//write data into the bt semaphore array
 int write_sem_data(BTCSMSG * pMsg);

//The bluetooth module callback function
int Bluetooth_spp_data_proc(BTCSMSG * pMsg);

//wait bt command
 int wait_bt_command(int timeout_ms,BTCSMSG * pMsg);

//wait bt response. 
//time_out=0:Infinite wait
 int wait_bt_response(int time_out) ;

//Update Bt State
 void confirm_current_connect_state();

//PhoneLink deal with bt  message from bt
 void *bt_msg_process_thread(void *arg);


#endif/*PL_BLUETOOTH_RECIVE_THREAD_H_*/

