#ifndef __SERIAL_H
#define __SERIAL_H

#include "fifo.h"

extern "C" void USART2_IRQHandler(void);

/*
 * Generate print and println function overloads for each type
 * Actual handler should be named _print{Suffix}
 */
#define GEN_PRINT(type, Suffix) \
    void print(type v)          \
    {                           \
        _print##Suffix(v);      \
    }                           \
                                \
    void println(type v)        \
    {                           \
        print(v);               \
        print('\n');            \
    }

class _serial
{
public:
    void setup();

    GEN_PRINT(char, Char)
    GEN_PRINT(char *, String)
    GEN_PRINT(int, Int)

private:
    /* Print handlers */
    void _printChar(char c);
    void _printString(char *s);
    void _printInt(int n);

    void _sendNextByte(void);

    friend int main(); // TODO : Remove
    friend void USART2_IRQHandler();

private:
    Fifo _buffer;
};

extern _serial Serial;

#endif // __SERIAL_H