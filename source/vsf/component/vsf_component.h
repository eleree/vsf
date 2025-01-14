/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_COMPONENT_H__
#define __VSF_COMPONENT_H__

/*============================ INCLUDES ======================================*/
#include "./vsf_component_cfg.h"


#include "./av/vsfav.h"

#include "./input/vsf_input.h"
#include "./input/driver/hid/vsf_input_hid.h"
#include "./input/driver/ds4/vsf_input_ds4.h"

#include "./usb/driver/hcd/ohci/vsf_ohci.h"
#include "./usb/driver/otg/musb/fdrc/vsf_musb_fdrc_dcd.h"
#include "./usb/driver/otg/dwcotg/vsf_dwcotg_dcd.h"

#include "./usb/host/vsf_usbh.h"
#include "./usb/host/class/HUB/vsf_usbh_HUB.h"
#include "./usb/host/class/libusb/vsf_usbh_libusb.h"
#include "./usb/host/class/CDC/vsf_usbh_CDC.h"
#include "./usb/host/class/CDC/vsf_usbh_CDCECM.h"
#include "./usb/host/class/BTHCI/vsf_usbh_BTHCI.h"
#include "./usb/host/class/HID/vsf_usbh_HID.h"
#include "./usb/host/class/HID/vsf_usbh_ds4.h"

#include "./usb/device/vsf_usbd.h"
#include "./usb/device/class/CDC/vsf_usbd_CDC.h"
#include "./usb/device/class/CDC/vsf_usbd_CDCACM.h"
#include "./usb/device/class/HID/vsf_usbd_HID.h"
#include "./usb/device/class/UVC/vsf_usbd_UVC.h"

#include "./tcpip/netdrv/vsf_netdrv.h"

#include "./ui/disp_drv/vsf_disp_drv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_component_init(void);

#endif
/* EOF */
