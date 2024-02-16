#include "serial.h"

#include "stm32f3xx.h"
#include "pinAccess.h"

extern "C" void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_TXE) // Transmission complet
    {
        if (Serial._buffer.isEmpty())
            USART2->RQR |= USART_RQR_TXFRQ;
        else
            Serial._sendNextByte();
    }
}

void _serial::setup()
{
    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;

    /* Serial output pin */
    pinMode(GPIOA, 2, OUTPUT); // PA2
    pinAlt(GPIOA, 2, 7);       // USART2_TX
    // /* Serial input pin */
    // pinMode(GPIOA, 3, INPUT); // PA3
    // pinAlt(GPIOA, 3, 7);      // USART2_RX

    /**
     * With a frequency of 32MHz, to get a baud rate of 115200
     * we need to divide by ~278
     * 278 = 0x0116
     */
    USART2->BRR = 0x0116;
    USART2->CR3 = 0;
    USART2->CR2 = 0;
    USART2->CR1 =      // USART_CR1_RE |    //  RX Enable
        USART_CR1_TE | // TX Enable
        USART_CR1_UE | // UART Enable
        // USART_CR1_TCIE | // TX Complet Interrupt
        USART_CR1_TXEIE | // TX Empty Interrupt
        0;

    NVIC_EnableIRQ(USART2_IRQn);
}

void _serial::_sendNextByte(void)
{
    if (!_buffer.isEmpty())
        USART2->TDR = _buffer.read();
}

void _serial::_printChar(char c)
{
    while (_buffer.isFull()) // Wait for fifo to empty
        ;

    _buffer.push(c);
}

void _serial::_printString(char *s)
{
    while (*s != '\0')
        _printChar(*s++);
}

void _serial::_printInt(int n)
{
    int idx = 0, len = 0;
    char buffer[10], reversed[12]; // MAX_INT = 2,147,483,647 (10 chars) + sign char + \0

    if (n < 0)
    {
        reversed[idx++] = '-';
        n = -n; // Remove minus sign
    }

    do
    {
        uint8_t ones = n % 10;
        n /= 10;
        buffer[len++] = '0' + ones;
    } while (n != 0);

    for (int i = len - 1; i >= 0; i--)
    {
        reversed[idx++] = buffer[i];
    }
    reversed[idx] = 0; // Null terminator

    _printString(reversed);
}

_serial Serial;