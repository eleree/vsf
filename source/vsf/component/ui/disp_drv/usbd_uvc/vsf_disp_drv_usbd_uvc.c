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

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_DISP_DRV_USBD_UVC == ENABLED

#define __VSF_DISP_USBD_UVC_CLASS_IMPLEMENT
#define __VSF_DISP_CLASS_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t vsf_disp_usbd_uvc_init(vsf_disp_t *pthis);
static vsf_err_t vsf_disp_usbd_uvc_refresh(vsf_disp_t *pthis, vsf_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_disp_drv_t vsf_disp_drv_usbd_uvc = {
    .init           = vsf_disp_usbd_uvc_init,
    .refresh        = vsf_disp_usbd_uvc_refresh,
};

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ IMPLEMENTATION ================================*/

static void vsf_disp_usbd_uvc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_disp_usbd_uvc_t *disp_uvc = container_of(eda, vsf_disp_usbd_uvc_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp_uvc->header[0] = 2;
        disp_uvc->header[1] = 0;
        disp_uvc->line_cnt = 0;
        disp_uvc->frame_cnt = 0;
        break;
    case VSF_EVT_MESSAGE:
        if (++disp_uvc->line_cnt >= disp_uvc->param.height) {
            disp_uvc->line_cnt = 0;
            disp_uvc->frame_cnt++;
            disp_uvc->header[1] ^= 1;
        }
        vsf_disp_on_ready(&disp_uvc->use_as__vsf_disp_t);
        break;
    case VSF_EVT_REFRESH:
        vsf_usbd_UVC_send_packet(disp_uvc->uvc, disp_uvc->cur_buffer,
                2 + disp_uvc->param.width * VSF_DISP_GET_PIXEL_SIZE(disp_uvc) / 8);
        break;
    }
}

static vsf_err_t vsf_disp_usbd_uvc_init(vsf_disp_t *pthis)
{
    vsf_disp_usbd_uvc_t *disp_uvc = (vsf_disp_usbd_uvc_t *)pthis;
    VSF_UI_ASSERT(disp_uvc != NULL);

    vsf_eda_set_evthandler(&disp_uvc->eda, vsf_disp_usbd_uvc_evthandler);
    return vsf_eda_init(&disp_uvc->eda, VSF_USBD_CFG_USE_EDA, false);
}

// disp_buff MUST be a line_buffer with 2-byte header
static vsf_err_t vsf_disp_usbd_uvc_refresh(vsf_disp_t *pthis, vsf_disp_area_t *area, void *disp_buff)
{
    vsf_disp_usbd_uvc_t *disp_uvc = (vsf_disp_usbd_uvc_t *)pthis;
    uint8_t *buffer = (uint8_t *)disp_buff - 2;
    VSF_UI_ASSERT(disp_uvc != NULL);

    if (    (0 == area->pos.x)
        &&  (disp_uvc->param.width == area->size.x)
        &&  (1 == area->size.y)) {

        buffer[0] = disp_uvc->header[0];
        buffer[1] = disp_uvc->header[1];
        disp_uvc->cur_buffer = buffer;
        vsf_eda_post_evt(&disp_uvc->eda, VSF_EVT_REFRESH);
    } else {
        vsf_trace(VSF_TRACE_ERROR, "non-line disp area [%d,%d], [%d,%d]\r\n",
                    area->pos.x, area->pos.y, area->size.x, area->size.y);
        ASSERT(false);
    }
    return VSF_ERR_NONE;
}

#endif

/* EOF */
