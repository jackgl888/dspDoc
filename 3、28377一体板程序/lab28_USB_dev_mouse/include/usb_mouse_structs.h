//###########################################################################
//
// FILE:   usb_mouse_structs.h
//
// TITLE:  USB mouse device structure definitions.
//
//###########################################################################
// $TI Release: F2837xS Support Library v190 $
// $Release Date: Mon Feb  1 16:59:09 CST 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __USB_MOUSE_STRUCTS_H__
#define __USB_MOUSE_STRUCTS_H__

extern unsigned long MouseHandler(void *pvCBData,
                                  unsigned long ulEvent,
                                  unsigned long ulMsgData,
                                  void *pvMsgData);

extern const tUSBDHIDMouseDevice g_sMouseDevice;

#endif // __USB_MOUSE_STRUCTS_H__
