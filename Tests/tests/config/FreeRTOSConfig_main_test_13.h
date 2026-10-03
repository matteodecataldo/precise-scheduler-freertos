/*
 * main_test_13.c: 
 *      - #define configASSERT( x ) ((void)(x));
 *      - #define configSUPPORT_STATIC_ALLOCATION          1
 *      - #define configRT_TASK_COUNT         	         1
 *      - #define configRT_SUBFRAME_COUNT         	     2
 */


/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
* Application specific definitions.
*
* These definitions should be adjusted for your particular hardware and
* application requirements.
*
* THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
* FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
*
* See http://www.freertos.org/a00110.html
*----------------------------------------------------------*/
#define configUSE_RT_SCHEDULER 					 1
#define configUSE_PENDING_TICKS                  0
#define configUSE_RT_HOOK				 		 0
#define configRT_TASK_COUNT         	         1
#define configRT_SUBFRAME_COUNT         	     2
#define configTRACE_INSTRUCTION_COUNT            0

#define configUSE_TRACE_FACILITY                 0
#define configGENERATE_RUN_TIME_STATS            0

#define configUSE_PREEMPTION                     1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( ( unsigned long ) 25000000 )
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                 ( ( unsigned short ) 80 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) ( 60 * 1024 ) )
#define configMAX_TASK_NAME_LEN                  ( 12 )
#define configUSE_TRACE_FACILITY                 0
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  0
#define configUSE_CO_ROUTINES                    0
#define configUSE_MUTEXES                        0
#define configUSE_RECURSIVE_MUTEXES              0
#define configCHECK_FOR_STACK_OVERFLOW           0
#define configUSE_MALLOC_FAILED_HOOK             0
#define configUSE_QUEUE_SETS                     1
#define configUSE_COUNTING_SEMAPHORES            1

#define configMAX_PRIORITIES                     ( 9UL )
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )
#define configQUEUE_REGISTRY_SIZE                10
#define configSUPPORT_STATIC_ALLOCATION          1
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configKERNEL_PROVIDED_STATIC_MEMORY      1

/* Timer related defines. */
#define configUSE_TIMERS                         0
#define configTIMER_TASK_PRIORITY                ( configMAX_PRIORITIES - 4 )
#define configTIMER_QUEUE_LENGTH                 20
#define configTIMER_TASK_STACK_DEPTH             ( configMINIMAL_STACK_SIZE * 2 )

#define configUSE_TASK_NOTIFICATIONS             1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    3

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                  1
#define INCLUDE_uxTaskPriorityGet                 1
#define INCLUDE_vTaskDelete                       1
#define INCLUDE_vTaskCleanUpResources             0
#define INCLUDE_vTaskSuspend                      1
#define INCLUDE_vTaskDelayUntil                   1
#define INCLUDE_vTaskDelay                        1
#define INCLUDE_uxTaskGetStackHighWaterMark       1
#define INCLUDE_xTaskGetSchedulerState            1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle    1
#define INCLUDE_xTaskGetIdleTaskHandle            1
#define INCLUDE_xSemaphoreGetMutexHolder          1
#define INCLUDE_eTaskGetState                     1
#define INCLUDE_xTimerPendFunctionCall            1
#define INCLUDE_xTaskAbortDelay                   1
#define INCLUDE_xTaskGetHandle                    1

/* This demo makes use of one or more example stats formatting functions. These
 * format the raw data provided by the uxTaskGetSystemState() function in to human
 * readable ASCII form.  See the notes in the implementation of vTaskList() within
 * FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS      0

#define configKERNEL_INTERRUPT_PRIORITY           ( 255 )        /* All eight bits as QEMU doesn't model the priority bits. */

#ifndef __IASMARM__ /* Prevent C code being included in IAR asm files. */
	#define configASSERT( x ) ((void)(x));
#endif


/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY             ( 4 )

/* Use the Cortex-M3 optimised task selection rather than the generic C code
 * version. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION          1

/* The Win32 target is capable of running all the tests tasks at the same
 * time. */
#define configRUN_ADDITIONAL_TESTS                       1

/* The test that checks the trigger level on stream buffers requires an
 * allowable margin of error on slower processors (slower than the Win32
 * machine on which the test is developed). */
#define configSTREAM_BUFFER_TRIGGER_LEVEL_TEST_MARGIN    4

#define intqHIGHER_PRIORITY      ( configMAX_PRIORITIES - 5 )
#define bktPRIMARY_PRIORITY      ( configMAX_PRIORITIES - 3 )
#define bktSECONDARY_PRIORITY    ( configMAX_PRIORITIES - 4 )

#define configENABLE_BACKWARD_COMPATIBILITY 0

/* traces */
#define traceTICK(x)                     vTraceTick(x)
#define traceMAJOR_FRAME_END(x)          vTraceMajorFrameEnd(x)
#define traceMAJOR_FRAME_START(x)        vTraceMajorFrameStart(x)
#define traceSUBFRAME_END(x)             vTraceSubframeEnd(x)
#define traceHRT_START(x, y)             vTraceHRTStart(x, y)
#define traceSRT_START_RESUME(x, y, z)   vTraceSRTStartResume(x, y, z)
#define traceIDLE_START(x, y)            vTraceIdleStart(x, y)
#define traceTASK_SWITCH_OUT(x, y, z, w) vTraceTaskSwitchOut(x, y, z, w)
#define traceHRT_TERMINATED(x, y)        vTraceHRTTerminated(x, y)

#define traceERROR_ALREADY_INITIALIZED()                  vTraceErrorAlreadyInitialized()
#define traceERROR_NULL_CONFIG()                          vTraceErrorNullConfig()
#define traceERROR_INVALID_MAJOR_FRAME()                  vTraceErrorInvalidMajorFrame()
#define traceWARNING_SUBFRAME_COUNT_MISMATCH()            vTraceWarningSubframeCountMismatch()
#define traceERROR_INVALID_SUBFRAME()                     vTraceErrorInvalidSubframe()
#define traceWARNING_TASK_COUNT_MISMATCH()                vTraceWarningTaskCountMismatch()
#define traceERROR_INVALID_TASKCOUNT()                    vTraceErrorInvalidTaskcount()
#define traceERROR_FRAME_ALIGNMENT()                      vTraceErrorFrameAlignment()
#define traceERROR_NULL_TASK_NAME()                       vTraceErrorNullTaskName()
#define traceERROR_NULL_TASK_FUNCTION()                   vTraceErrorNullTaskFunction()
#define traceERROR_INVALID_TASK_STACK_SIZE()              vTraceErrorInvalidTaskStackSize()
#define traceERROR_INVALID_TASK_TYPE()                    vTraceErrorInvalidTaskType()
#define traceERROR_INVALID_HRT_SUBFRAMEID()               vTraceErrorInvalidHrtSubframeId()
#define traceERROR_INVALID_HRT_INTERVAL()                 vTraceErrorInvalidHrtInterval()
#define traceERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND() vTraceErrorInvalidHrtExceedsMajorframeBound()
#define traceERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND()   vTraceErrorInvalidHrtExceedsSubframeBound()
#define traceERROR_NULL_TASKLIST()                        vTraceErrorNullTasklist()
#define traceERROR_INVALID_HRT_OVERLAP()                  vTraceErrorInvalidHrtOverlap()
#define traceERROR_NOT_INITIALIZED()                      vTraceErrorNotInitialized()

#define traceINSTRUCTION_COUNT(x, y)                      vTraceInstructionCount( x,y)


#ifndef __ASSEMBLER__
    /* Operational Traces */
    extern void vTraceTick(uint32_t xTickCount);
    extern void vTraceMajorFrameEnd(uint32_t xTickCount);
    extern void vTraceMajorFrameStart(uint32_t xTickCount);
    extern void vTraceSubframeEnd(uint32_t xTickCount);
    extern void vTraceHRTStart(uint32_t xTickCount, void* pxTask);
    extern void vTraceSRTStartResume(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState);
    extern void vTraceIdleStart(uint32_t xTickCount, void* pxTask);
    extern void vTraceTaskSwitchOut(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState, uint32_t xType);
    extern void vTraceHRTTerminated(uint32_t xTickCount, void* pxTask);
    /* Error and Warning Traces */
    extern void vTraceErrorAlreadyInitialized(void);
    extern void vTraceErrorNullConfig(void);
    extern void vTraceErrorInvalidMajorFrame(void);
    extern void vTraceWarningSubframeCountMismatch(void);
    extern void vTraceErrorInvalidSubframe(void);
    extern void vTraceWarningTaskCountMismatch(void);
    extern void vTraceErrorInvalidTaskcount(void);
    extern void vTraceErrorFrameAlignment(void);
    extern void vTraceErrorNullTaskName(void);
    extern void vTraceErrorNullTaskFunction(void);
    extern void vTraceErrorInvalidTaskStackSize(void);
    extern void vTraceErrorInvalidTaskType(void);
    extern void vTraceErrorInvalidHrtSubframeId(void);
    extern void vTraceErrorInvalidHrtInterval(void);
    extern void vTraceErrorInvalidHrtExceedsMajorframeBound(void);
    extern void vTraceErrorInvalidHrtExceedsSubframeBound(void);
    extern void vTraceErrorNullTasklist(void);
    extern void vTraceErrorInvalidHrtOverlap(void);
    extern void vTraceErrorNotInitialized(void);
    /* Overhead */
    extern void vTraceInstructionCount(uint8_t flag, uint32_t count);
    extern uint32_t ulGetTraceTimestamp(void);
#endif /* __ASSEMBLER__ */

#endif /* FREERTOS_CONFIG_H */
