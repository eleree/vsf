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

#ifndef __VSF_OS_H__
#define __VSF_OS_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "./vsf_kernel_common.h"
#include "./vsf_evtq.h"
#include "./task/vsf_task.h"
/*============================ MACROS ========================================*/

typedef vsf_arch_prio_t vsf_sched_lock_status_t;

#if VSF_OS_CFG_PRIORITY_NUM > 1

#   define __vsf_sched_safe(...)                                                \
        {                                                                       \
            vsf_sched_lock_status_t lock_status = vsf_sched_lock();             \
            __VA_ARGS__;                                                        \
            vsf_sched_unlock(lock_status);                                      \
        }                                                                       \

#   define vsf_sched_safe()             code_region(&VSF_SCHED_SAFE_CODE_REGION)
#   define vsf_sched_safe_exit()        vsf_sched_unlock(lock_status)
#else
            
#   define VSF_SCHED_SAFE_CODE_REGION   DEFAULT_CODE_REGION_NONE
#   define vsf_sched_lock()             0
#   define vsf_sched_unlock(level)      
#   define vsf_sched_safe()             if (0)
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define __vsf_sched_safe(__CODE) __CODE
#   else
#       define __vsf_sched_safe(...)    __VA_ARGS__
#   endif
#   define vsf_sched_safe_exit()
#endif

#define __vsf_interrupt_safe            __SAFE_ATOM_CODE
#define vsf_interrupt_safe              SAFE_ATOM_CODE

#define vsf_protect_scheduler()         vsf_sched_lock()
#define vsf_unprotect_scheduler(__state)                                        \
            vsf_sched_unlock((vsf_sched_lock_status_t)(__state))
#define vsf_protect_sched()             vsf_protect_scheduler()
#define vsf_unprotect_sched             vsf_unprotect_scheduler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#ifdef __VSF_OS_CFG_EVTQ_LIST
declare_vsf_pool(vsf_evt_node_pool)
def_vsf_pool(vsf_evt_node_pool, vsf_evt_node_t)
#endif

struct vsf_kernel_resource_t {
#if __VSF_OS_SWI_NUM > 0
    struct {
        const vsf_arch_prio_t               *os_swi_priorities_ptr;
        uint16_t                            swi_priority_cnt;
    } arch;
#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    struct {
        vsf_evtq_t *queue_array;
#   if defined(__VSF_OS_CFG_EVTQ_ARRAY)
        vsf_evt_node_t                      **nodes;
        uint8_t                             node_bit_sz;
#endif
#   if defined(__VSF_OS_CFG_EVTQ_LIST)
        vsf_pool_block(vsf_evt_node_pool)   *nodes_buf_ptr;
        uint16_t                            node_cnt;
#   endif

        uint16_t                            queue_cnt;
    } evt_queue;

#endif
#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    struct {
        vsf_pool_block(vsf_eda_frame_pool)  *frame_buf_ptr;
        uint16_t                            frame_cnt;
    } frame_stack;
#endif
}ALIGN(4);

typedef struct vsf_kernel_resource_t vsf_kernel_resource_t;
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_OS_CFG_PRIORITY_NUM > 1
extern const code_region_t VSF_SCHED_SAFE_CODE_REGION;
#endif

/*============================ PROTOTYPES ====================================*/

//! vsf_kernel_os_start and vsf_kernel_os_run_priority are ONLY used when __vsf_main_entry is not used
extern void vsf_kernel_os_start(void);
extern void vsf_kernel_os_run_priority(vsf_prio_t priority);
#ifndef WEAK_VSF_KERNEL_ERR_REPORT
extern void vsf_kernel_err_report(enum vsf_kernel_error_t err);
#endif

#if VSF_OS_CFG_PRIORITY_NUM > 1
extern vsf_sched_lock_status_t vsf_sched_lock(void);
extern void vsf_sched_unlock(vsf_sched_lock_status_t origlevel);
#endif

#endif
#endif
