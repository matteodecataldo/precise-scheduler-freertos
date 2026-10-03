#include "FreeRTOS.h"
#include "task.h"

#include "uart.h"


#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

void vTaskFunction(void *pvParameters);
void vTaskSimulateBehavior( const TickType_t xTicksToSimulate );

#if (configUSE_RT_SCHEDULER == 1)
void vTaskFunctionA(void *pvParameters);
void vTaskFunctionB(void *pvParameters);
void vTaskFunctionC(void *pvParameters);
void vTaskFunctionD(void *pvParameters);
#endif /* #if ( configUSE_RT_SCHEDULER == 1) */

int main(int argc, char **argv){

	(void) argc;
	(void) argv;

    UART_init();

	#if (configUSE_RT_SCHEDULER == 1)
		TimelineTaskConfig_t timeline[] = {
        	{ "Task_A", vTaskFunctionA, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, 100, 999, 0, (void *) 1010},
        	{ "Task_B", vTaskFunctionB, configMINIMAL_STACK_SIZE, tsTASK_TYPE_HRT, 2000, 2980, 2, (void *) 700},
			{ "Task_C", vTaskFunctionC, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, 0, 0, 0, (void *) 1000},
			{ "Task_D", vTaskFunctionD, configMINIMAL_STACK_SIZE, tsTASK_TYPE_SRT, 0, 0, 0, (void *) 200}
    	};
		TimelineConfig_t cfg = { 5000, 5, configRT_TASK_COUNT, timeline };
		if ( vConfigureScheduler(&cfg) != pdTRUE ){
			UART_printf("Wrong Configuration\n");
			for( ; ; );
		}
		
	#else /* #if ( configUSE_RT_SCHEDULER == 1) */
	xTaskCreate(
		// Function which implements the task
		vTaskFunction,
		// Name of the task (debug purposes, not used by the kernel)
		"Task1",
		// Stack to allocate to the task
		configMINIMAL_STACK_SIZE,
		// Parameter passed to the task. Not needed for Hello World example
		NULL,
		// Priority assigned to the task
		mainTASK_PRIORITY,
		// Task handle. Not required
		NULL
	);
	#endif /* #if ( configUSE_RT_SCHEDULER == 1) */

	// vRunStaticTests();

	// Give control to the scheduler
	vTaskStartScheduler();

	// If everything ok should never reach here
    for( ; ; );
}

/* Task Function */
void vTaskFunction(void *pvParameters) {

	// Avoid warning about unused pvParameters
	(void) pvParameters;

    for (;;) {

        // Task code: print a message
        UART_printf("Hello, World!\n");

        // Delay for 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#if (configUSE_RT_SCHEDULER == 1)
void vTaskFunctionA(void *pvParameters){

	int ticks = ( int ) pvParameters;

	// Task code: print a message
    UART_printf("HRT - START - TaskA\n");
	vTaskSimulateBehavior(ticks);
	UART_printf("HRT - END - TaskA\n");
	vTimelineTaskComplete();
}

void vTaskFunctionB(void *pvParameters){

	int ticks = ( int ) pvParameters;

	// Task code: print a message
    UART_printf("HRT - START - TaskB\n");
	vTaskSimulateBehavior(ticks);
	UART_printf("HRT - END - TaskB\n");
	vTimelineTaskComplete();
}

void vTaskFunctionC(void *pvParameters){

	int ticks = ( int ) pvParameters;

	UART_printf("SRT - START - TaskC\n");
	vTaskSimulateBehavior(ticks);
	UART_printf("SRT - END - TaskC\n");
	
	vTimelineTaskComplete();
}

void vTaskFunctionD(void *pvParameters){

	int ticks = ( int ) pvParameters;

	UART_printf("SRT - START - TaskD\n");
	vTaskSimulateBehavior(ticks);
	UART_printf("SRT - END - TaskD\n");
	vTimelineTaskComplete();
}
void vTaskSimulateBehavior( const TickType_t xTicksToSimulate )
{
    TickType_t xCurrentTime = xTaskGetTimelineTickCount();
    TickType_t xTimeToWake = xCurrentTime + xTicksToSimulate;

    while( xCurrentTime < xTimeToWake )
    {
        xCurrentTime = xTaskGetTimelineTickCount();
    }
}
#endif /* #if ( configUSE_RT_SCHEDULER == 1) */