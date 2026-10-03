#include "uart.h"

void UART_init( void )
{
    UART0_BAUDDIV = 16;
    UART0_CTRL = 1;
}

void UART_printf(const char *s) {
    while(*s != '\0') {
        UART0_DATA = (unsigned int)(*s);
        s++;
    }
}

void UART_print_int_99(uint32_t n) {
    uint32_t tens = 0;

    while (n >= 10) {
        n -= 10;
        tens++;
    }

    if (tens > 0) {
        UART0_DATA = (unsigned int)(tens + '0');
    }

    UART0_DATA =     (unsigned int)(n + '0');
}
