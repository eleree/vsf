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



#ifndef __VSF_INPUT_TOUCHSCREEN_H__
#define __VSF_INPUT_TOUCHSCREEN_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_INPUT_TOUCHSCREEN_SET(__EVENT, __ID, __IS_DOWN, __X, __Y)           \
            do {                                                                \
                (__EVENT)->id = ((__ID) | ((__IS_DOWN) << 8));                  \
                (__EVENT)->cur.valu32 = ((__X) | ((__Y) << 16));                \
            } while (0)

#define VSF_INPUT_TOUCHSCREEN_GET_ID(__EVENT)                                   \
            ((uint8_t)(((__EVENT)->id >> 0) & 0xFF))
#define VSF_INPUT_TOUCHSCREEN_IS_DOWN(__EVENT)                                  \
            (!!(((__EVENT)->id >> 8) & 0xFF))
#define VSF_INPUT_TOUCHSCREEN_GET_X(__EVENT)                                    \
            ((uint16_t)(((__EVENT)->cur.valu32 >> 0) & 0xFFFF))
#define VSF_INPUT_TOUCHSCREEN_GET_Y(__EVENT)                                    \
            ((uint16_t)(((__EVENT)->cur.valu32 >> 16) & 0xFFFF))

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_TOUCHSCREEN = VSF_INPUT_USER_TYPE,
};

struct vsf_touchscreen_evt_t {
    implement(vsf_input_evt_t)
};
typedef struct vsf_touchscreen_evt_t vsf_touchscreen_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */