
// Behaviour: SRT runs in gap between two HRT tasks
// Expected: SRT run in the expected gap

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

    ulTestTicksToRun = 500;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS( 0), pdMS_TO_TICKS(29), 0, (void *) pdMS_TO_TICKS(25) },
        { "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(60), pdMS_TO_TICKS(99), 0, (void *) pdMS_TO_TICKS(30) },
        { "Task_C", vTaskFunctionC, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS( 0), pdMS_TO_TICKS( 0), 0, (void *) pdMS_TO_TICKS(20) },
        { "Task_D", vTaskFunctionD, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, pdMS_TO_TICKS( 0), pdMS_TO_TICKS( 0), 0, (void *) pdMS_TO_TICKS(10) }
    };
    TimelineConfig_t cfg = { 500, 5, 4, timeline };

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