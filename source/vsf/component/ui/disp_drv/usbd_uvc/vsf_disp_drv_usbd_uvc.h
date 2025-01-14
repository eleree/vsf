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
#ifndef __VSF_DISP_DRV_USBD_UVC_H___
#define __VSF_DISP_DRV_USBD_UVC_H___

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_DISP_DRV_USBD_UVC == ENABLED

#if     defined(__VSF_DISP_USBD_UVC_CLASS_IMPLEMENT)
#   undef __VSF_DISP_USBD_UVC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(__VSF_DISP_USBD_UVC_CLASS_INHERIT)
#   undef __VSF_DISP_USBD_UVC_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_USE_EDA != ENABLED
#   error "need VSF_USBD_CFG_USE_EDA��
#endif

#if VSF_USE_USB_DEVICE != ENABLED || VSF_USE_USB_DEVICE_UVC != ENABLED
#   error "need VSF_USE_USB_DEVICE and VSF_USE_USB_DEVICE_UVC"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_disp_usbd_uvc_t)

def_simple_class(vsf_disp_usbd_uvc_t) {
    implement(vsf_disp_t);

    public_member(
        vsf_usbd_UVC_t  *uvc;
        uint16_t        frame_cnt;
    )
    private_member(
        vsf_eda_t       eda;
        uint8_t         *cur_buffer;
        uint8_t         header[2];
        uint16_t        line_cnt;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_disp_drv_t vsf_disp_drv_usbd_uvc;

/*============================ PROTOTYPES ====================================*/

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_DRV_USBD_UVC_H___
