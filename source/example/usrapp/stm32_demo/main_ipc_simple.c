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

declare_grouped_evts(cpl_grouped_evts_t) 
	def_grouped_evts(cpl_grouped_evts_t)
	def_adapter (cpl_grouped_evts_t, cpl_a_evt),
	def_adapter (cpl_grouped_evts_t, cpl_b_evt),
	def_adapter (cpl_grouped_evts_t, cpl_c_evt),
end_def_grouped_evts(cpl_grouped_evts_t)

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
				uint32_t cnt;
				vsf_thread_t * thread_obj;
				vsf_trig_t * set_trig;
				vsf_trig_t * wait_trig;
				cpl_grouped_evts_t * set_group_evts;
				uint_fast32_t set_mask;
    ));

declare_vsf_thread(user_thread_b_t)

def_vsf_thread(user_thread_b_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *psem;
				uint32_t cnt;
				vsf_trig_t * wait_trig;
				cpl_grouped_evts_t * set_group_evts;
				uint_fast32_t set_mask;
    ));

declare_vsf_thread(user_thread_c_t)

def_vsf_thread(user_thread_c_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *psem;
				uint32_t cnt;
				vsf_trig_t * wait_trig;
				cpl_grouped_evts_t * set_group_evts;
				uint_fast32_t set_mask;
    ));
		
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#define AUTO true
#define MANUAL false
	
static NO_INIT vsf_sem_t user_sem;
static bool __flag = false;
static NO_INIT vsf_trig_t __trig_start;
enum{
VSF_EVT_START = VSF_EVT_USER + 1,
}vsf_user_evt_t;



static NO_INIT cpl_grouped_evts_t __cpl_grouped_evts;

static NO_INIT vsf_trig_t __trig_start_a; 
static NO_INIT vsf_trig_t __trig_start_b;
static NO_INIT vsf_trig_t __trig_start_c;

static NO_INIT vsf_trig_t __trig_cpl_a;
static NO_INIT vsf_trig_t __trig_cpl_b;
static NO_INIT vsf_trig_t __trig_cpl_c;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_kernel_post_evt_simple_demo(void)
{
	vsf_thread_t * target_thread;
	{
		static NO_INIT user_thread_b_t __user_task_b;
		__user_task_b.param.cnt = 0;
		target_thread = (vsf_thread_t*) & __user_task_b;
		init_vsf_thread(user_thread_b_t, &__user_task_b, vsf_prio_0);
	}
	
	{
		static NO_INIT user_thread_a_t __user_task_a;
		__user_task_a.param.cnt = 0;
		__user_task_a.thread_obj = target_thread;
		init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0);
	}
	
}
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

implement_grouped_evts(cpl_grouped_evts_t,
	add_sync_adapter( &__trig_cpl_a, cpl_a_evt_msk), 
	add_sync_adapter( &__trig_cpl_b, cpl_b_evt_msk),
	add_sync_adapter( &__trig_cpl_c, cpl_c_evt_msk),
)

void vsf_kernel_grouped_evts_simple_demo(void)
{
	vsf_trig_init(&__trig_start_a, RESET, AUTO); 
	vsf_trig_init(&__trig_start_b, RESET, AUTO);
	vsf_trig_init(&__trig_start_c, RESET, AUTO);
	vsf_trig_init(&__trig_cpl_a, RESET, MANUAL); 
	vsf_trig_init(&__trig_cpl_b, RESET, MANUAL);
	vsf_trig_init(&__trig_cpl_c, RESET, MANUAL);

	init_grouped_evts(cpl_grouped_evts_t, &__cpl_grouped_evts, MANUAL);

	/*start the user task a*/
	{
	static NO_INIT user_thread_a_t __user_task_a;
	__user_task_a.param.wait_trig = &__trig_start_a;
	__user_task_a.param.set_group_evts = &__cpl_grouped_evts;
	__user_task_a.param.set_mask = cpl_a_evt_msk;
	init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0); 
	} 
	/*start the user task b*/
	{
	static NO_INIT user_thread_b_t __user_task_b;
	__user_task_b.param.wait_trig = &__trig_start_b;
	__user_task_b.param.set_group_evts = &__cpl_grouped_evts; 
	__user_task_b.param.set_mask = cpl_b_evt_msk;
	init_vsf_thread(user_thread_b_t, &__user_task_b, vsf_prio_0);
	} 
	/*start the user task c*/
	{
	static NO_INIT user_thread_c_t __user_task_c;
	__user_task_c.param.wait_trig = &__trig_start_c;
	__user_task_c.param.set_group_evts = &__cpl_grouped_evts;
	__user_task_c.param.set_mask = cpl_c_evt_msk;
	init_vsf_thread(user_thread_c_t, &__user_task_c, vsf_prio_0);
	}
}

void vsf_kernel_trig_simple_demo(void)
{
	vsf_trig_init(&__trig_start, RESET, AUTO);
	{
		static NO_INIT user_thread_a_t __user_task_a;
		__user_task_a.param.set_trig = & __trig_start;
		init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0);
	}
	{
		static NO_INIT user_thread_b_t __user_task_b;
		__user_task_b.param.wait_trig = & __trig_start;
		init_vsf_thread(user_thread_b_t, &__user_task_b, vsf_prio_0);
	}
}

implement_vsf_thread(user_thread_a_t) 
{
	while(1) {
		vsf_trig_wait(this.wait_trig);
		printf("task_a start\r\n"); 
		vsf_delay_ms(rand()%1000); 
		set_grouped_evts(this.set_group_evts, this.set_mask); 
		printf("task_a stopped\r\n"); 
	}
}

implement_vsf_thread(user_thread_b_t) 
{
	while(1) {
		vsf_trig_wait(this.wait_trig);
		printf("task_b start\r\n");
		vsf_delay_ms(200);
		set_grouped_evts(this.set_group_evts, this.set_mask);
		printf("task_b stopped\r\n");
	}
}

implement_vsf_thread(user_thread_c_t)
{
	while(1) {
		vsf_trig_wait(this.wait_trig);
		printf("task_c start\r\n");
		vsf_delay_ms(100);
		set_grouped_evts(this.set_group_evts, this.set_mask);
		printf("task_c stopped\r\n");
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
            mem_sharable( using_grouped_evt; )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    srand(2);
		//vsf_kernel_post_evt_simple_demo();
		//vsf_kernel_trig_simple_demo();
    //vsf_kernel_thread_simple_demo();
    //timer.on_timer = vsf_timer_test;
		//vsf_callback_timer_add_ms(&timer,2000);
		vsf_kernel_grouped_evts_simple_demo();
	
	
#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    while(1) {
				vsf_trig_set(&__trig_start_a);
				vsf_trig_set(&__trig_start_b);
				vsf_trig_set(&__trig_start_c);
				printf("-----All tasks have been triggered-----\r\n");
        //printf("hello world! \r\n");
				wait_for_all(&__cpl_grouped_evts, all_evts_msk_of_cpl_grouped_evts_t) {
					reset_grouped_evts(&__cpl_grouped_evts,
					all_evts_msk_of_cpl_grouped_evts_t);					
					printf("----All tasks completed and sync-ed----\r\n\r\n");
				}
			  vsf_delay_ms(1000);
    }
#else
    return 0;
#endif
}

#endif