// Behaviour: vConfigureScheduler is not called
// Expected: vTaskStartScheduler should trigger EVENT_ERROR_NOT_INITIALIZED

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulTestTicksToRun;

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ulTestTicksToRun = 400;

    vTaskStartScheduler();
    
    for(;;);
}
