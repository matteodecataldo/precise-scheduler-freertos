// Behaviour: Two HRT tasks are scheduled with overlapping execution windows in the same subframe
// Expected: vConfigureScheduler should trigger EVENT_ERROR_INVALID_HRT_OVERLAP

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);
void vTaskFunctionB(void *pvParameters);

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 5000;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(1100),  pdMS_TO_TICKS(1400),  1  , NULL },
        { "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(1300),  pdMS_TO_TICKS(1700),  1  , NULL }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(5000), 5, 2, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
void vTaskFunctionB(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }