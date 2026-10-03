// Behaviour: Stress Test: each tick there is an HRT start time
// Expected: Each tick there should be a HRT deadline and a HRT start

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

    ulTestTicksToRun = 10;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(0),  pdMS_TO_TICKS(1),  0  , (void *) 10 },
        { "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(1),  pdMS_TO_TICKS(2),  0  , (void *) 10 },
        { "Task_C", vTaskFunctionC, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(2),  pdMS_TO_TICKS(3),  0  , (void *) 10 },
        { "Task_D", vTaskFunctionD, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(3),  pdMS_TO_TICKS(4),  0  , (void *) 10 },
        { "Task_E", vTaskFunctionE, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(4),  pdMS_TO_TICKS(5),  0  , (void *) 10 },
        { "Task_F", vTaskFunctionF, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(5),  pdMS_TO_TICKS(6),  0  , (void *) 10 },
        { "Task_G", vTaskFunctionG, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(6),  pdMS_TO_TICKS(7),  0  , (void *) 10 }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(10), 1, 7, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionB(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionC(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionD(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
void vTaskFunctionE(void *pvParameters) { int t = (int)pvParameters; vTaskSimulateBehavior(t); vTimelineTaskComplete(); }
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