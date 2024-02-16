#include "fifo.h"

char Fifo::read()
{
    uint8_t readIndex;

    if (_writeIndex >= _available)
    {
        readIndex = _writeIndex - _available;
    }
    else
    {
        readIndex = FIFO_SIZE - _available + _writeIndex;
    }

    _available--;
    return _buffer[readIndex];
}

void Fifo::push(char c)
{
    _available++; // Increment available

    _buffer[_writeIndex] = c;

    if (_writeIndex == FIFO_SIZE - 1)
        _writeIndex = 0;
    else
        _writeIndex++;
}

bool Fifo::isFull()
{
    return _available >= FIFO_SIZE;
}

bool Fifo::isEmpty()
{
    return _available == 0;
}

uint8_t Fifo::size()
{
    return _available;
}