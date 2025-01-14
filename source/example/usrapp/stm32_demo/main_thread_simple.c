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
//#include "app_cfg.h"
#include "vsf.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_thread(user_thread_a_t)

def_vsf_thread(user_thread_a_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *psem;
    ));

declare_vsf_thread(user_thread_b_t)

def_vsf_thread(user_thread_b_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *psem;
    ));

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


void vsf_kernel_thread_simple_demo(void)
{    
    //! initialise semaphore
    vsf_sem_init(&user_sem, 0); 
    
    //! start the user task a
    {
        static NO_INIT user_thread_a_t __user_task_a;
        __user_task_a.param.psem = &user_sem;
        init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0);
    }
    
    //! start the user task b
    {
        static NO_INIT user_thread_b_t __user_task_b;
        __user_task_b.param.psem = &user_sem;
        init_vsf_thread(user_thread_b_t, &__user_task_b, vsf_prio_0);
    }
}

implement_vsf_thread(user_thread_a_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_delay_ms(1000);
        printf("post semaphore...     [%08x]\r\n", cnt++);
        vsf_sem_post(this.psem);            //!< post a semaphore
    }
}

implement_vsf_thread(user_thread_b_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_sem_pend(this.psem);            //! wait for semaphore forever
        printf("receive semaphore...  [%08x]\r\n", cnt++);
    }
}


void vsf_timer_test(vsf_callback_timer_t *timer)
{
			printf("\r\nvsf_timer_test \r\n");
			vsf_callback_timer_add_ms(timer,2000);
}

vsf_callback_timer_t timer;

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    
    vsf_kernel_thread_simple_demo();
    timer.on_timer = vsf_timer_test;
		vsf_callback_timer_add_ms(&timer,2000);
	
#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    while(1) {
        printf("hello world! \r\n");
			  vsf_delay_ms(1000);
        vsf_yield();
    }
#else
    return 0;
#endif
}

#endif