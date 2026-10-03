// Behaviour: A task is provided without a function (NULL pointer)
// Expected: vConfigureScheduler should trigger EVENT_ERROR_NULL_TASK_FUNCTION

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 5000;

    TimelineTaskConfig_t timeline[] = {
        { "Task_A", NULL, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, pdMS_TO_TICKS(100),  pdMS_TO_TICKS(999),  0, NULL }
        };
    TimelineConfig_t cfg = { pdMS_TO_TICKS(5000), 1, 1, timeline };

    if (vConfigureScheduler(&cfg) == pdTRUE) {
        vTaskStartScheduler();
    }
    
    for(;;);
}