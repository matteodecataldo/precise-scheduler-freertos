// Behaviour: Passes a NULL pointer to the scheduler configuration
// Expected: vConfigureScheduler should trigger EVENT_ERROR_NULL_CONFIG

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 5000;

    if (vConfigureScheduler(NULL) == pdTRUE) {
        vTaskStartScheduler();
    }

    for(;;);
}
