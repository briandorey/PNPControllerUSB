/************************************************************************
	usb_config.c

	WFF USB Generic HID Demonstration 3
    usbGenericHidCommunication reference firmware 3_0_0_0
    Copyright (C) 2011 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com

************************************************************************/

#ifndef USBCONFIG_H
#define USBCONFIG_H

// Definitions
#define USB_EP0_BUFF_SIZE		8	// Valid Options are 8, 16, 32, or 64 bytes.								
#define USB_MAX_NUM_INT     	1
#define USB_MAX_EP_NUMBER	    1


// USB device descriptor
#define USB_USER_DEVICE_DESCRIPTOR &device_dsc
#define USB_USER_DEVICE_DESCRIPTOR_INCLUDE extern ROM USB_DEVICE_DESCRIPTOR device_dsc

// Configuration descriptors
#define USB_USER_CONFIG_DESCRIPTOR USB_CD_Ptr
#define USB_USER_CONFIG_DESCRIPTOR_INCLUDE extern ROM BYTE *ROM USB_CD_Ptr[]

// Set the USB ping pong mode
#define USB_PING_PONG_MODE USB_PING_PONG__FULL_PING_PONG

// Uncomment either USB_POLLING or USB_INTERRUPT to set how the USB
// device is updated
//#define USB_POLLING
#define USB_INTERRUPT

// Set the USB pullup option for usb_device.h
#define USB_PULLUP_OPTION USB_PULLUP_ENABLE
//#define USB_PULLUP_OPTION USB_PULLUP_DISABLED

// Use the internal USB transceiver module
#define USB_TRANSCEIVER_OPTION USB_INTERNAL_TRANSCEIVER

// Set either full-speed or low-speed USB device mode
#define USB_SPEED_OPTION USB_FULL_SPEED
//#define USB_SPEED_OPTION USB_LOW_SPEED

// Option to enable auto-arming of the status stage of control transfers
#define USB_ENABLE_STATUS_STAGE_TIMEOUTS

// Section 9.2.6 of the USB 2.0 specifications state that:
// Control transfers with no data stage must complete within 50ms of the start of the control transfer.
// Control transfers with (IN) data stage must complete within 50ms of sending the last IN data packet in fullfilment of the data stage.
// Control transfers with (OUT) data stage have no specific status stage timing (but must not exceed 5 seconds)
#define USB_STATUS_STAGE_TIMEOUT     (BYTE)45

#define USB_SUPPORT_DEVICE

#define USB_NUM_STRING_DESCRIPTORS 3

// Enable/disable event handlers
//#define USB_INTERRUPT_LEGACY_CALLBACKS
#define USB_ENABLE_ALL_HANDLERS
//#define USB_ENABLE_SUSPEND_HANDLER
//#define USB_ENABLE_WAKEUP_FROM_SUSPEND_HANDLER
//#define USB_ENABLE_SOF_HANDLER
//#define USB_ENABLE_ERROR_HANDLER
//#define USB_ENABLE_OTHER_REQUEST_HANDLER
//#define USB_ENABLE_SET_DESCRIPTOR_HANDLER
//#define USB_ENABLE_INIT_EP_HANDLER
//#define USB_ENABLE_EP0_DATA_HANDLER
//#define USB_ENABLE_TRANSFER_COMPLETE_HANDLER

// Set device type to USB HID
#define USB_USE_HID

// HID endpoint allocation
#define HID_INTF_ID             0x00
#define HID_EP 					1
#define HID_INT_OUT_EP_SIZE     3
#define HID_INT_IN_EP_SIZE      3
#define HID_NUM_OF_DSC          1
#define HID_RPT01_SIZE          28

#endif
