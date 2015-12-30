 /************************************************************************
* File Name : pl_bt_hid_fun.c
* Copyright :  HSAE Corporation, All Rights Reserved.
* Module Name : phoneLink
*
* Create Date : 2015/01/26
* Author/Corporation : CaoLinfeng/HSAE
*
*Abstract Description :  HID Command
*
----------------------------------------Revision History---------------------------------
* No     Version  	 Date      		Revised By		 Item		 		     Description
* 1        V0.1     2015/01/26      CaoLinfeng 	    CMFB Project         		 HID Command
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
* Function Define Section
************************************************************************/
//HID Reset
void bt_mouse_adjust()
{
	INFO("------bt_mouse_adjust------\n");
	BTHidMouseData data;
	data.xOffset = -4000;
	data.yOffset = -4000;
	data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
	data.eButtonStatus = BT_HID_BUTTON_L_DOWN;
	sendMouseData(&data);
	
	data.xOffset = -4000;
	data.yOffset = -4000;
	data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
	data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;
	sendMouseData(&data);
}


#if 0
/*
 * 鼠标校正，将鼠标移到（0，0）位置
 */
void bt_mouse_adjust()
{
	INFO("------bt_mouse_adjust------\n");
	BTHidMouseData data;
	data.xOffset = -4000;
	data.yOffset = -4000;
	data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
	data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;
	sendMouseData(&data);
	usleep(80000);
}

/*
 * 鼠标校正，将鼠标移到（0，0）位置
 */
void bt_mouse_adjust()
{
	INFO("------bt_mouse_adjust------\n");
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
		BTHidMouseData data;
		data.xOffset = 0x8ff;
		data.yOffset = 0x8ff;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;
		sendMouseData(&data);
		usleep(80000);
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}

/*
 * 鼠标移动函数
 */
void bt_mouse_move(short xoffset,short yoffset)
{
	INFO("------bt_mouse_move moveXtemp=%d,moveYtemp=%d----\n",xoffset,yoffset);
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
	BTHidMouseData data;

		data.xOffset = xoffset;
		data.yOffset = yoffset;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;

		sendMouseData(&data);
//		usleep(1000);
//	}
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}
/*
 * 鼠标托动函数（鼠标处于按下状态）
 */
void bt_mouse_press_move(short xoffset,short yoffset )
{
	INFO("------bt_mouse_press_move x=%d,y=%d----\n",xoffset,yoffset);
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
		BTHidMouseData data;
		data.xOffset = xoffset;
		data.yOffset = yoffset;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_L_DOWN;

		sendMouseData(&data);
		//usleep(1000);
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}
/*
 * 鼠标左键单击
 */
void bt_mouse_oneclick()
{
	DEBUG("------bt_mouse_oneclick---\n");
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
	BTHidMouseData data;
	data.xOffset = 0;
	data.yOffset = 0;
	data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
	data.eButtonStatus = BT_HID_BUTTON_L_DOWN;
	sendMouseData(&data);
	data.xOffset = 0;
	data.yOffset = 0;
	data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
	data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;

	sendMouseData(&data);
	//usleep(1000);
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}
/*
 * 鼠标左键按下
 */
void bt_mouse_press()
{
	INFO("------bt_mouse_press-------\n");
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
		BTHidMouseData data;
		data.xOffset = 0;
		data.yOffset = 0;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_L_DOWN;
		sendMouseData(&data);
		//usleep(1000);
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}

void bt_mouse_press_test(void)
{

		BTHidMouseData data;
		data.xOffset = 0;
		data.yOffset = 0;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_L_DOWN;
		sendMouseData(&data);
		//usleep(1000);

	return;
}
/*
 * 鼠标左键抬起
 */
void bt_mouse_release(void)
{
	INFO("------bt_mouse_release---\n");
	if(phone_current_mode == BT_CONNECTED_ANDROID_HID)
	{
		BTHidMouseData data;
		data.xOffset = 0;
		data.yOffset = 0;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;

		sendMouseData(&data);
		//usleep(1000);
	}
	else if(phone_current_mode == BT_CONNECT_IPOD_HID)
	{

	}
	return;
}

/*
 * 鼠标左键抬起
 */
void bt_mouse_release_test(void)
{

		BTHidMouseData data;
		data.xOffset = 0;
		data.yOffset = 0;
		data.eMiddleButtonMoveType = BT_HID_M_BUTTON_MOVE_NONE;
		data.eButtonStatus = BT_HID_BUTTON_NONE_OR_UP;

		sendMouseData(&data);
		//usleep(1000);

	return;
}
#endif
