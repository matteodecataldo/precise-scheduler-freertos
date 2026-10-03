// Behaviour: vConfigureScheduler is called twice
// Expected: vConfigureScheduler should trigger EVENT_ERROR_ALREADY_INITIALIZED

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 400;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(100),  pdMS_TO_TICKS(120),  1  , NULL }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(200), 2, 1, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vConfigureScheduler(&cfg);
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }
