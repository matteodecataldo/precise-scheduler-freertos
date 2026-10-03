// Behaviour: Passes a NULL pointer to the task list configuration
// Expected: vConfigureScheduler should trigger EVENT_ERROR_INVALID_TASKCOUNT

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

void vTaskFunctionA(void *pvParameters);

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 5000;

    TimelineConfig_t cfg = { pdMS_TO_TICKS(5000), 1, 1, NULL };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}

void vTaskFunctionA(void *pvParameters) { (void) pvParameters; vTimelineTaskComplete(); }