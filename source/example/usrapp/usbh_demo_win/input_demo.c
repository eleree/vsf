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

#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void input_demo_trace_hid(vsf_hid_event_t *hid_evt)
{
    if (hid_evt->id != 0) {
        uint_fast16_t generic_usage, usage_page, usage_id;

        generic_usage = HID_GET_GENERIC_USAGE(hid_evt->id);
        usage_page = HID_GET_USAGE_PAGE(hid_evt->id);
        usage_id = HID_GET_USAGE_ID(hid_evt->id);

        vsf_trace(VSF_TRACE_DEBUG, "hid(%d): page=%d, id=%d, pre=%d, cur=%d" VSF_TRACE_CFG_LINEEND,
            generic_usage, usage_page, usage_id, hid_evt->pre, hid_evt->cur);
    }
}

static void input_demo_trace_gamepad(vsf_gamepad_evt_t *gamepad_evt)
{
    if (gamepad_evt->id != GAMEPAD_ID_DUMMY) {
        vsf_trace(VSF_TRACE_DEBUG, "gamepad(%d): cur=%d, pre=%d" VSF_TRACE_CFG_LINEEND,
            (vsf_gamepad_id_t)gamepad_evt->id, gamepad_evt->cur, gamepad_evt->pre);
    }
}

static void input_demo_trace_touchscreen(vsf_touchscreen_evt_t* ts_evt)
{
    vsf_trace(VSF_TRACE_DEBUG, "touchscreen(%d): %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
        VSF_INPUT_TOUCHSCREEN_GET_ID(ts_evt),
        VSF_INPUT_TOUCHSCREEN_IS_DOWN(ts_evt) ? "down" : "up",
        VSF_INPUT_TOUCHSCREEN_GET_X(ts_evt),
        VSF_INPUT_TOUCHSCREEN_GET_Y(ts_evt));
}

static void input_demo_trace_keyboard(vsf_keyboard_evt_t* kb_evt)
{
    vsf_trace(VSF_TRACE_DEBUG, "keyboard: %d %s" VSF_TRACE_CFG_LINEEND,
        VSF_INPUT_KEYBOARD_GET_KEYCODE(kb_evt),
        VSF_INPUT_KEYBOARD_IS_DOWN(kb_evt) ? "down" : "up");
}

static void input_demo_trace_sensor(vsf_sensor_evt_t *sensor_evt)
{
    int16_t gyro_pitch = 0, gyro_yaw = 0, gyro_roll = 0, acc_x = 0, acc_y = 0, acc_z = 0;
    uint_fast32_t value_cnt = 0, pos = 0;
    uint8_t *buffer = sensor_evt->data;
    vsf_sensor_item_info_t *info;

    for (uint_fast8_t i = 0; i < sensor_evt->desc.item_num; i++) {
        info = &sensor_evt->desc.item_info[i];

        switch (info->id) {
        case SENSOR_ID_GYRO:
            switch (info->subid) {
            case SENSOR_SUBID_PITCH:
                ASSERT(info->bitlen == 16);
                gyro_pitch = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            case SENSOR_SUBID_YAW:
                ASSERT(info->bitlen == 16);
                gyro_yaw = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            case SENSOR_SUBID_ROLL:
                ASSERT(info->bitlen == 16);
                gyro_roll = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            }
            break;
        case SENSOR_ID_ACC:
            switch (info->subid) {
            case SENSOR_SUBID_X:
                ASSERT(info->bitlen == 16);
                acc_x = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            case SENSOR_SUBID_Y:
                ASSERT(info->bitlen == 16);
                acc_y = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            case SENSOR_SUBID_Z:
                ASSERT(info->bitlen == 16);
                acc_z = vsf_input_buf_get_value(buffer, pos, info->bitlen);
                value_cnt++;
                break;
            }
            break;
        }
        pos += info->bitlen;
    }

    if (value_cnt >= 6) {
        vsf_trace(VSF_TRACE_DEBUG, "6-asix: acc_x=%d, acc_y=%d, acc_z=%d, gyro_pitch=%d, gyro_yaw=%d, gyro_roll=%d" VSF_TRACE_CFG_LINEEND,
            (int)acc_x, (int)acc_y, (int)acc_z, (int)gyro_pitch, (int)gyro_yaw, (int)gyro_roll);
    }
}

void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *event)
{
    switch (type) {
#if VSF_USE_INPUT_HID == ENABLED
    case VSF_INPUT_TYPE_HID:
        input_demo_trace_hid((vsf_hid_event_t *)event);
        break;
#endif
    case VSF_INPUT_TYPE_SENSOR:
        input_demo_trace_sensor((vsf_sensor_evt_t *)event);
        break;
    case VSF_INPUT_TYPE_TOUCHSCREEN:
        input_demo_trace_touchscreen((vsf_touchscreen_evt_t*)event);
        break;
#if VSF_USE_INPUT_DS4 == ENABLED
    case VSF_INPUT_TYPE_DS4:
        input_demo_trace_gamepad((vsf_gamepad_evt_t *)event);
        break;
#endif
    case VSF_INPUT_TYPE_KEYBOARD:
        input_demo_trace_keyboard((vsf_keyboard_evt_t*)event);
        break;
    default:
        break;
    }
}
