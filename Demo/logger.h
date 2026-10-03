#ifndef LOGGER_H
#define LOGGER_H

#include "FreeRTOS.h"
#include <stdint.h>

#define LOG_CAPACITY 5000

/* Operational Events */
#define EVENT_TICK                                       0
#define EVENT_MAJOR_FRAME_END                            1
#define EVENT_MAJOR_FRAME_START                          29
#define EVENT_SUBFRAME_END                               2
#define EVENT_HRT_START                                  3
#define EVENT_SRT_START                                  4
#define EVENT_RESUME_SRT                                 5
#define EVENT_IDLE                                       6
#define EVENT_HRT_TERMINATED                             7
#define SRT_PREEMPTED                                    8
#define TASK_END                                         9

/* Error and Warning Events */
#define EVENT_ERROR_ALREADY_INITIALIZED                  10
#define EVENT_ERROR_NULL_CONFIG                          11
#define EVENT_ERROR_INVALID_MAJOR_FRAME                  12
#define EVENT_WARNING_SUBFRAME_COUNT_MISMATCH            13
#define EVENT_ERROR_INVALID_SUBFRAME                     14
#define EVENT_WARNING_TASK_COUNT_MISMATCH                15
#define EVENT_ERROR_INVALID_TASKCOUNT                    16
#define EVENT_ERROR_NULL_TASKLIST                        17
#define EVENT_ERROR_FRAME_ALIGNMENT                      18
#define EVENT_ERROR_NULL_TASK_NAME                       19
#define EVENT_ERROR_NULL_TASK_FUNCTION                   20
#define EVENT_ERROR_INVALID_TASK_STACK_SIZE              21
#define EVENT_ERROR_INVALID_TASK_TYPE                    22
#define EVENT_ERROR_INVALID_HRT_SUBFRAMEID               23
#define EVENT_ERROR_INVALID_HRT_INTERVAL                 24
#define EVENT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND 25
#define EVENT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND   26
#define EVENT_ERROR_INVALID_HRT_OVERLAP                  27
#define EVENT_ERROR_NOT_INITIALIZED                      28


typedef struct {
    uint32_t ulTimestamp;
    void *xTask;
    uint8_t ucEventID;
} TraceEntry_t;

extern TraceEntry_t xTraceLog[LOG_CAPACITY];
extern uint32_t ulLogIndex;

/* Operational Trace Functions */
void vTraceTick(uint32_t xTickCount);
void vTraceMajorFrameEnd(uint32_t xTickCount);
void vTraceMajorFrameStart(uint32_t xTickCount);
void vTraceSubframeEnd(uint32_t xTickCount);
void vTraceHRTStart(uint32_t xTickCount, void* pxTask);
void vTraceSRTStartResume(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState);
void vTraceIdleStart(uint32_t xTickCount, void* pxTask);
void vTraceTaskSwitchOut(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState, uint32_t xType);
void vTraceHRTTerminated(uint32_t xTickCount, void* pxTask);

/* Error and Warning Trace Functions */
void vTraceErrorAlreadyInitialized(void);
void vTraceErrorNullConfig(void);
void vTraceErrorInvalidMajorFrame(void);
void vTraceWarningSubframeCountMismatch(void);
void vTraceErrorInvalidSubframe(void);
void vTraceWarningTaskCountMismatch(void);
void vTraceErrorInvalidTaskcount(void);
void vTraceErrorFrameAlignment(void);
void vTraceErrorNullTaskName(void);
void vTraceErrorNullTaskFunction(void);
void vTraceErrorInvalidTaskStackSize(void);
void vTraceErrorInvalidTaskType(void);
void vTraceErrorInvalidHrtSubframeId(void);
void vTraceErrorInvalidHrtInterval(void);
void vTraceErrorInvalidHrtExceedsMajorframeBound(void);
void vTraceErrorInvalidHrtExceedsSubframeBound(void);
void vTraceErrorNullTasklist(void);
void vTraceErrorInvalidHrtOverlap(void);
void vTraceErrorNotInitialized(void);


#endif /* LOGGER_H */