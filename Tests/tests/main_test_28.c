// Behaviour: Passes HRTs not ordered by start time
// Expected: HRT inside pxTasks should be ordered by start time

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);
void vTaskFunctionB(void *pvParameters);
void vTaskFunctionC(void *pvParameters);
void vTaskFunctionD(void *pvParameters);
void vTaskFunctionE(void *pvParameters);
void vTaskFunctionF(void *pvParameters);
void vTaskFunctionG(void *pvParameters);
void vTaskSimulateBehavior( const TickType_t xTicksToSimulate );

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 400;

    TimelineTaskConfig_t timeline[] = {
        { "Task_D", vTaskFunctionD, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(160),  pdMS_TO_TICKS(180),  1  , NULL },
        { "Task_E", vTaskFunctionE, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(180),  pdMS_TO_TICKS(199),  1  , NULL },
        { "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(120),  pdMS_TO_TICKS(140),  1  , NULL },
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(100),  pdMS_TO_TICKS(120),  1  , NULL },
        { "Task_F", vTaskFunctionF, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS(0),    pdMS_TO_TICKS(0),    1  , (void *) 50 },
        { "Task_C", vTaskFunctionC, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(140),  pdMS_TO_TICKS(160),  1  , NULL },
        { "Task_G", vTaskFunctionG, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS(0),  pdMS_TO_TICKS(0),  1  , (void *) 50 },
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(200), 2, 7, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionB(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionC(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionD(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionE(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionF(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionG(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }


void vTaskSimulateBehavior( const TickType_t xTicksToSimulate )
{
    TickType_t xCurrentTime = xTaskGetTimelineTickCount();
    TickType_t xTimeToWake = xCurrentTime + xTicksToSimulate;

    while( xCurrentTime < xTimeToWake )
    {
        xCurrentTime = xTaskGetTimelineTickCount();
    }
}