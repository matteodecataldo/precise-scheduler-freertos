// Behaviour: Task type is neither HRT nor SRT ( e.g., IDLE )
// Expected: vConfigureScheduler should trigger EVENT_ERROR_INVALID_TASK_TYPE

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
        { "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_IDLE, pdMS_TO_TICKS(100),  pdMS_TO_TICKS(999),  0, NULL }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(5000), 1, 1, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }