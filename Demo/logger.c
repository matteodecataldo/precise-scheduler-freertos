#include "logger.h"
#include "task.h"
#include <stdio.h>




extern void * volatile pxCurrentTCB;

TraceEntry_t xTraceLog[LOG_CAPACITY];
uint32_t ulLogIndex = 0;

void vTraceTick(uint32_t xTickCount) {
   
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = (uint32_t) xTickCount;
        xTraceLog[ulLogIndex].xTask = (TaskHandle_t)pxCurrentTCB;
        xTraceLog[ulLogIndex].ucEventID = EVENT_TICK ; 
        ulLogIndex++;
    }else{
    	ulLogIndex = 0 ;
    }

}

void vTraceMajorFrameEnd(uint32_t xTickCount) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = NULL; // No specific task
        xTraceLog[ulLogIndex].ucEventID = EVENT_MAJOR_FRAME_END;
        ulLogIndex++;
    }
}

void vTraceMajorFrameStart(uint32_t xTickCount) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = NULL; // No specific task
        xTraceLog[ulLogIndex].ucEventID = EVENT_MAJOR_FRAME_START;
        ulLogIndex++;
    }
}

void vTraceSubframeEnd(uint32_t xTickCount) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_SUBFRAME_END;
        ulLogIndex++;
    }
}

void vTraceHRTStart(uint32_t xTickCount, void* pxTask) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = pxTask;
        xTraceLog[ulLogIndex].ucEventID = EVENT_HRT_START;
        ulLogIndex++;
    }
}

void vTraceSRTStartResume(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState) {

    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = pxTask;

        if( uxTaskState == tsTASK_STATE_RESET ){
            xTraceLog[ulLogIndex].ucEventID = EVENT_SRT_START;
        }
        else if( uxTaskState == tsTASK_STATE_PREEMPTED ){
            xTraceLog[ulLogIndex].ucEventID = EVENT_RESUME_SRT;
        }
        
        ulLogIndex++;

    }
}

void vTraceIdleStart(uint32_t xTickCount, void* pxTask) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = pxTask;
        xTraceLog[ulLogIndex].ucEventID = EVENT_IDLE;
        ulLogIndex++;
    }
}

void vTraceTaskSwitchOut(uint32_t xTickCount, void* pxTask, uint32_t uxTaskState, uint32_t type) {

    if (ulLogIndex < LOG_CAPACITY) {
        if( uxTaskState == tsTASK_STATE_PREEMPTED && type == tsTASK_TYPE_SRT){
            xTraceLog[ulLogIndex].ucEventID = SRT_PREEMPTED;
            xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
            xTraceLog[ulLogIndex].xTask = pxTask;
        }else if(uxTaskState == tsTASK_STATE_DISPATCHED) {
            xTraceLog[ulLogIndex].ucEventID = TASK_END;
            xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
            xTraceLog[ulLogIndex].xTask = pxTask;
        }
        ulLogIndex++;

    }
}

void vTraceHRTTerminated(uint32_t xTickCount, void* pxTask) {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = xTickCount;
        xTraceLog[ulLogIndex].xTask = pxTask;
        xTraceLog[ulLogIndex].ucEventID = EVENT_HRT_TERMINATED;
        ulLogIndex++;
    }
}

void vTraceErrorAlreadyInitialized() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_ALREADY_INITIALIZED;
        ulLogIndex++;
    }
}

void vTraceErrorNullConfig() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_NULL_CONFIG;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidMajorFrame() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_MAJOR_FRAME;
        ulLogIndex++;
    }
}

void vTraceWarningSubframeCountMismatch() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_WARNING_SUBFRAME_COUNT_MISMATCH;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidSubframe() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_SUBFRAME;
        ulLogIndex++;
    }
}

void vTraceWarningTaskCountMismatch() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_WARNING_TASK_COUNT_MISMATCH;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidTaskcount() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_TASKCOUNT;
        ulLogIndex++;
    }
}

void vTraceErrorNullTasklist() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_NULL_TASKLIST;
        ulLogIndex++;
    }
}

void vTraceErrorFrameAlignment() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_FRAME_ALIGNMENT;
        ulLogIndex++;
    }
}

void vTraceErrorNullTaskName() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_NULL_TASK_NAME;
        ulLogIndex++;
    }
}

void vTraceErrorNullTaskFunction() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_NULL_TASK_FUNCTION;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidTaskStackSize() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_TASK_STACK_SIZE;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidTaskType() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_TASK_TYPE;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidHrtSubframeId() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_HRT_SUBFRAMEID;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidHrtInterval() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_HRT_INTERVAL;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidHrtExceedsMajorframeBound() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_HRT_EXCEEDS_MAJORFRAME_BOUND;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidHrtExceedsSubframeBound() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_HRT_EXCEEDS_SUBFRAME_BOUND;
        ulLogIndex++;
    }
}

void vTraceErrorInvalidHrtOverlap() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_INVALID_HRT_OVERLAP;
        ulLogIndex++;
    }
}

void vTraceErrorNotInitialized() {
    if (ulLogIndex < LOG_CAPACITY) {
        xTraceLog[ulLogIndex].ulTimestamp = 0;
        xTraceLog[ulLogIndex].xTask = NULL;
        xTraceLog[ulLogIndex].ucEventID = EVENT_ERROR_NOT_INITIALIZED;
        ulLogIndex++;
    }
}