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

#ifndef __HAL_DEVICE_GIGADEVICE_GD32VF103C8_H__
#define __HAL_DEVICE_GIGADEVICE_GD32VF103C8_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             0

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
//#include "../common/__common.h"

#include "./Vendor/gd32vf103.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USBFS_IRQHandler

#define USB_OTG0_CONFIG                                                         \
    .ep_num = 8,                                                                \
    .irq = USBFS_IRQn,                                                          \
    .reg = 0x50000000,                                                          \
    .speed = USB_SPEED_FULL,                                                    \
	.dma_en = false,                                                            \
	.ulpi_en = false,                                                           \
	.utmi_en = false,                                                           \
	.vbus_en = false,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
