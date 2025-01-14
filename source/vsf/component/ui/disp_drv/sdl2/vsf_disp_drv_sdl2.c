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

#if VSF_USE_UI == ENABLED && VSF_USE_DISP_DRV_SDL2 == ENABLED

#define __VSF_DISP_SDL2_CLASS_IMPLEMENT
#define __VSF_DISP_CLASS_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/

#if     defined(WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN)                           \
    &&  defined(WEAK_VSF_INPUT_ON_TOUCHSCREEN)
WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN)                               \
    &&  defined(WEAK_VSF_INPUT_ON_GAMEPAD)
WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_KEYBOARD_EXTERN)                              \
    &&  defined(WEAK_VSF_INPUT_ON_KEYBOARD)
WEAK_VSF_INPUT_ON_KEYBOARD_EXTERN
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vsf_disp_sdl2_t {
    vsf_arch_irq_thread_t init_thread;
    bool is_init_called;
    bool is_inited;
};
typedef struct __vsf_disp_sdl2_t __vsf_disp_sdl2_t;

/*============================ LOCAL VARIABLES ===============================*/

static __vsf_disp_sdl2_t __vsf_disp_sdl2 = {
    .is_init_called = false,
    .is_inited      = false,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t vsf_disp_sdl2_init(vsf_disp_t *pthis);
static vsf_err_t vsf_disp_sdl2_refresh(vsf_disp_t *pthis, vsf_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_disp_drv_t vsf_disp_drv_sdl2 = {
    .init       = vsf_disp_sdl2_init,
    .refresh    = vsf_disp_sdl2_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_disp_sdl2_common_init(void)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
}

static void __vsf_disp_sdl2_common_fini(void)
{
    SDL_Quit();
}

static Uint32 __vsf_disp_sdl2_get_format(vsf_disp_sdl2_t *disp_sdl2)
{
    switch (disp_sdl2->param.color) {
    default:
    case VSF_DISP_COLOR_RGB565:     return SDL_PIXELFORMAT_RGB565;
    case VSF_DISP_COLOR_ARGB8888:   return SDL_PIXELFORMAT_ARGB8888;
    }
}

static uint_fast8_t __vsf_disp_sdl2_get_pixel_size(vsf_disp_sdl2_t *disp_sdl2)
{
    return VSF_DISP_GET_PIXEL_SIZE(disp_sdl2);
}

static void __vsf_disp_sdl2_screen_init(vsf_disp_sdl2_t *disp_sdl2)
{
    disp_sdl2->window = SDL_CreateWindow("Screen",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            disp_sdl2->param.width * disp_sdl2->amplifier,
                            disp_sdl2->param.height * disp_sdl2->amplifier,
                            0);
    disp_sdl2->renderer = SDL_CreateRenderer(disp_sdl2->window, -1, 0);
    disp_sdl2->texture = SDL_CreateTexture(disp_sdl2->renderer,
                            __vsf_disp_sdl2_get_format(disp_sdl2),
                            SDL_TEXTUREACCESS_STATIC,
                            disp_sdl2->param.width, disp_sdl2->param.height);
    SDL_SetTextureBlendMode(disp_sdl2->texture, SDL_BLENDMODE_BLEND);
}

static void __vsf_disp_sdl2_screen_update(vsf_disp_sdl2_t *disp_sdl2)
{
    SDL_Rect rect = {
       .x = disp_sdl2->area.pos.x,
       .y = disp_sdl2->area.pos.y,
       .w = disp_sdl2->area.size.x,
       .h = disp_sdl2->area.size.y,
    };
    SDL_UpdateTexture(disp_sdl2->texture, &rect, disp_sdl2->disp_buff,
        disp_sdl2->area.size.x * __vsf_disp_sdl2_get_pixel_size(disp_sdl2) / 8);
    SDL_RenderClear(disp_sdl2->renderer);
    SDL_RenderCopy(disp_sdl2->renderer, disp_sdl2->texture, NULL, NULL);
    SDL_RenderPresent(disp_sdl2->renderer);
}

static void __vsf_disp_sdl2_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;

    __vsf_arch_irq_set_background(irq_thread);
        __vsf_disp_sdl2_common_init();
        __vsf_disp_sdl2.is_inited = true;
    __vsf_arch_irq_fini(irq_thread);
}

static void __vsf_disp_sdl2_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_disp_sdl2_t *disp_sdl2 = container_of(irq_thread, vsf_disp_sdl2_t, thread);

    SDL_Event event;
    union {
        implement(vsf_input_evt_t)
        vsf_touchscreen_evt_t   ts_evt;
        vsf_gamepad_evt_t       gamepad_evt;
        vsf_keyboard_evt_t      keyboard_evt;
    } evt;
    vsf_input_type_t evt_type;

    uint_fast16_t x = 0, y = 0;
    bool is_down = false, is_to_update = false;

    while (!__vsf_disp_sdl2.is_inited) {
        Sleep(100);
    }

    __vsf_arch_irq_set_background(irq_thread);
        __vsf_disp_sdl2_screen_init(disp_sdl2);

    while (1) {
        if (disp_sdl2->disp_buff != NULL) {
            __vsf_disp_sdl2_screen_update(disp_sdl2);
            disp_sdl2->disp_buff = NULL;

            __vsf_arch_irq_start(irq_thread);
                vsf_disp_on_ready(&disp_sdl2->use_as__vsf_disp_t);
            __vsf_arch_irq_end(irq_thread, false);
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEBUTTONUP:
                is_down = false;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;
            case SDL_MOUSEBUTTONDOWN:
                is_down = true;
                x = event.motion.x / disp_sdl2->amplifier;
                y = event.motion.y / disp_sdl2->amplifier;
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;
            case SDL_MOUSEMOTION:
                x = event.motion.x / disp_sdl2->amplifier;
                y = event.motion.y / disp_sdl2->amplifier;
                is_to_update = is_down;
                evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!event.key.repeat) {
                    is_to_update = true;
                    evt_type = VSF_INPUT_TYPE_KEYBOARD;
                    // TODO: remap event.key.sym to keycode in VSF
                    VSF_INPUT_KEYBOARD_SET(&evt.keyboard_evt, event.key.keysym.sym, SDL_KEYDOWN == event.type);
                }
                break;

            case SDL_CONTROLLERDEVICEADDED:
                SDL_GameControllerOpen(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                break;
            case SDL_CONTROLLERAXISMOTION:
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_GAMEPAD;

                evt.gamepad_evt.is_signed = true;
                evt.gamepad_evt.bitlen = 16;
                evt.gamepad_evt.cur.val16 = event.caxis.value;

                switch (event.caxis.axis) {
                case SDL_CONTROLLER_AXIS_LEFTX:             evt.gamepad_evt.id = GAMEPAD_ID_LX;         break;
                case SDL_CONTROLLER_AXIS_LEFTY:             evt.gamepad_evt.id = GAMEPAD_ID_LY;         break;
                case SDL_CONTROLLER_AXIS_RIGHTX:            evt.gamepad_evt.id = GAMEPAD_ID_RX;         break;
                case SDL_CONTROLLER_AXIS_RIGHTY:            evt.gamepad_evt.id = GAMEPAD_ID_RY;         break;
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:       evt.gamepad_evt.id = GAMEPAD_ID_LT;         break;
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:      evt.gamepad_evt.id = GAMEPAD_ID_RT;         break;
                default:                                    is_to_update = false;                       break;
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                is_to_update = true;
                evt_type = VSF_INPUT_TYPE_GAMEPAD;

                evt.gamepad_evt.is_signed = false;
                evt.gamepad_evt.bitlen = 1;
                if (SDL_CONTROLLERBUTTONDOWN == event.type) {
                    evt.gamepad_evt.cur.bit = 1;
                    evt.gamepad_evt.pre.bit = 0;
                } else {
                    evt.gamepad_evt.cur.bit = 0;
                    evt.gamepad_evt.pre.bit = 1;
                }

                switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_A:               evt.gamepad_evt.id = GAMEPAD_ID_R_DOWN;     break;
                case SDL_CONTROLLER_BUTTON_B:               evt.gamepad_evt.id = GAMEPAD_ID_R_RIGHT;    break;
                case SDL_CONTROLLER_BUTTON_X:               evt.gamepad_evt.id = GAMEPAD_ID_R_LEFT;     break;
                case SDL_CONTROLLER_BUTTON_Y:               evt.gamepad_evt.id = GAMEPAD_ID_R_UP;       break;
                case SDL_CONTROLLER_BUTTON_BACK:            evt.gamepad_evt.id = GAMEPAD_ID_MENU_LEFT;  break;
                case SDL_CONTROLLER_BUTTON_GUIDE:           evt.gamepad_evt.id = GAMEPAD_ID_MENU_MAIN;  break;
                case SDL_CONTROLLER_BUTTON_START:           evt.gamepad_evt.id = GAMEPAD_ID_MENU_RIGHT; break;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:       evt.gamepad_evt.id = GAMEPAD_ID_LS;         break;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:      evt.gamepad_evt.id = GAMEPAD_ID_RS;         break;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:    evt.gamepad_evt.id = GAMEPAD_ID_LB;         break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:   evt.gamepad_evt.id = GAMEPAD_ID_RB;         break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:         evt.gamepad_evt.id = GAMEPAD_ID_L_UP;       break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:       evt.gamepad_evt.id = GAMEPAD_ID_L_DOWN;     break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:       evt.gamepad_evt.id = GAMEPAD_ID_L_LEFT;     break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:      evt.gamepad_evt.id = GAMEPAD_ID_L_RIGHT;    break;
                default:                                    is_to_update = false;                       break;
                }
                break;
            }

            if (is_to_update) {
                is_to_update = false;
                evt.dev = disp_sdl2;

                __vsf_arch_irq_start(irq_thread);
                    switch (evt_type) {
                    case VSF_INPUT_TYPE_TOUCHSCREEN:
                        VSF_INPUT_TOUCHSCREEN_SET(&evt.ts_evt, 0, is_down, x, y);
#ifndef WEAK_VSF_INPUT_ON_TOUCHSCREEN
                        vsf_input_on_touchscreen(&evt.ts_evt);
#else
                        WEAK_VSF_INPUT_ON_TOUCHSCREEN(&evt.ts_evt);
#endif
                        break;
                    case VSF_INPUT_TYPE_GAMEPAD:
#ifndef WEAK_VSF_INPUT_ON_GAMEPAD
                        vsf_input_on_gamepad(&evt.gamepad_evt);
#else
                        WEAK_VSF_INPUT_ON_GAMEPAD(&evt.gamepad_evt);
#endif
                        break;
                    case VSF_INPUT_TYPE_KEYBOARD:
#ifndef WEAK_VSF_INPUT_ON_KEYBOARD
                        vsf_input_on_keyboard(&evt.keyboard_evt);
#else
                        WEAK_VSF_INPUT_ON_KEYBOARD( &evt.keyboard_evt);
#endif
                        break;
                    }
                __vsf_arch_irq_end(irq_thread, false);
            }
        }
        Sleep(30);
    }
}






static vsf_err_t vsf_disp_sdl2_init(vsf_disp_t *pthis)
{
    vsf_disp_sdl2_t *disp_sdl2 = (vsf_disp_sdl2_t *)pthis;
    VSF_UI_ASSERT(disp_sdl2 != NULL);

    if (!__vsf_disp_sdl2.is_init_called) {
        __vsf_disp_sdl2.is_init_called = true;
        __vsf_disp_sdl2.init_thread.name = "disp_sdl2_init";
        __vsf_arch_irq_init(&__vsf_disp_sdl2.init_thread, __vsf_disp_sdl2_init_thread, vsf_arch_prio_0, true);
    }

    disp_sdl2->thread.name = "disp_sdl2";
    __vsf_arch_irq_init(&disp_sdl2->thread, __vsf_disp_sdl2_thread, vsf_arch_prio_0, true);
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_disp_sdl2_refresh(vsf_disp_t *pthis, vsf_disp_area_t *area, void *disp_buff)
{
    vsf_disp_sdl2_t *disp_sdl2 = (vsf_disp_sdl2_t *)pthis;
    VSF_UI_ASSERT(disp_sdl2 != NULL);

    if (disp_sdl2->disp_buff != NULL) {
        VSF_UI_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    disp_sdl2->area = *area;
    disp_sdl2->disp_buff = disp_buff;
    return VSF_ERR_NONE;
}

#endif

/* EOF */
