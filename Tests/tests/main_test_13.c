// Behaviour: Sets a different value for configRT_SUBFRAME_COUNT (FreeRTOSConfig.h) and uxSubframeCount (TimelineConfig_t) if configSUPPORT_STATIC_ALLOCATION is set to 1
// Expected: vConfigureScheduler should trigger EVENT_WARNING_SUBFRAME_COUNT_MISMATCH

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 5000;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(100),  pdMS_TO_TICKS(999),  0, NULL }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(5000), 5, 1, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }