#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

#if (configUSE_RT_SCHEDULER == 1)

#if (configUSE_RT_HOOK == 1)
void vApplicationWrongConfigurationHook( UBaseType_t errCode )
{
    UART_printf("vApplicationWrongConfigurationHook(): Wrong Configuration\nERR_CODE: ");
    UART_print_int_99(errCode);
    UART_printf("\n");
    for( ; ; )
    {
    }
}
#endif /* configUSE_RT_HOOK */

#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void )
{
    /* If configUSE_MALLOC_FAILED_HOOK is set to 1 then this function will
    *  be called automatically if a call to pvPortMalloc() fails.  pvPortMalloc()
    *  is called automatically when a task, queue or semaphore is created. */
    taskDISABLE_INTERRUPTS();
    
    UART_printf( "Application Malloc Failed Hook called\n" );

    for( ; ; )
    {
    }
}
#endif /* configUSE_MALLOC_FAILED_HOOK */

//not used
#if (configUSE_IDLE_HOOK == 1)
void vApplicationIdleHook( void )
{

}
#endif /* configUSE_IDLE_HOOK */

//not used
#if (configUSE_TICK_HOOK == 1)
void vApplicationTickHook( void )
{

}
#endif /* configUSE_TICK_HOOK */

//not used
#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName )
{
    (void) pxTask;
    (void) pcTaskName;
}
#endif /* configCHECK_FOR_STACK_OVERFLOW */

#endif /* #if (configUSE_RT_SCHEDULER == 1) */