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

#ifndef __VSF_INPUT_KEYBOARD_H__
#define __VSF_INPUT_KEYBOARD_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_INPUT_KEYBOARD_SET(__EVENT, __KEYCODE,  __IS_DOWN)                  \
            do {                                                                \
                (__EVENT)->id = ((__KEYCODE) | ((__IS_DOWN) << 16));            \
            } while (0)

#define VSF_INPUT_KEYBOARD_GET_KEYCODE(__EVENT)                                 \
            ((uint16_t)(((__EVENT)->id >> 0) & 0xFFFF))
#define VSF_INPUT_KEYBOARD_IS_DOWN(__EVENT)                                     \
            (!!((__EVENT)->id >> 16))

/*============================ TYPES =========================================*/

// TODO: add keycode

enum {
    VSF_INPUT_TYPE_KEYBOARD = VSF_INPUT_USER_TYPE,
};

struct vsf_keyboard_evt_t {
    implement(vsf_input_evt_t)
};
typedef struct vsf_keyboard_evt_t vsf_keyboard_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
