// Behaviour: HRT task in last subframe replays each major frames
// Expected: at least 3 HRT_START events in window [4000, 4500)

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);
void vTaskFunctionB(void *pvParameters);
void vTaskFunctionC(void *pvParameters);
void vTaskFunctionD(void *pvParameters);
void vTaskSimulateBehavior( const TickType_t xTicksToSimulate );

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 1500;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS( 10), pdMS_TO_TICKS( 99), 0, (void *) pdMS_TO_TICKS(50) },
        { "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(400), pdMS_TO_TICKS(500), 4, (void *) pdMS_TO_TICKS(100) },
        { "Task_C", vTaskFunctionC, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS(  0), pdMS_TO_TICKS(  0), 0, (void *) pdMS_TO_TICKS(30) },
        { "Task_D", vTaskFunctionD, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS(  0), pdMS_TO_TICKS(  0), 0, (void *) pdMS_TO_TICKS(20) }
    };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(500), 5, 4, timeline };

    if (vConfigureScheduler(&cfg) != pdTRUE) {
        for(;;);
    }

    vTaskStartScheduler();
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionB(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionC(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionD(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }

void vTaskSimulateBehavior( const TickType_t xTicksToSimulate )
{
    TickType_t xCurrentTime = xTaskGetTimelineTickCount();
    TickType_t xTimeToWake = xCurrentTime + xTicksToSimulate;

    while( xCurrentTime < xTimeToWake )
    {
        xCurrentTime = xTaskGetTimelineTickCount();
    }
}