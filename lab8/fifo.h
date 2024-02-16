#ifndef __FIFO_H
#define __FIFO_H

#ifndef FIFO_SIZE
#define FIFO_SIZE 100
#endif

#if FIFO_SIZE > 255
#error "FIFO_SIZE Max value is 255"
#endif

typedef unsigned char uint8_t;

class Fifo
{
public:
    char read();
    void push(char c);

    bool isFull();
    bool isEmpty();

    uint8_t size();

private:
    volatile char _buffer[FIFO_SIZE];
    volatile uint8_t _available = 0, _writeIndex = 0; // TODO : Check if these values are auto initialized
};

#endif // __FIFO_H