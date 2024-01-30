#include "MCP23S17.h"
#include "spi.h"

void MCP23S17::Init()
{
    setupSPI();
}

// write to a MCP register , using spi .
void MCP23S17::_writeRegister(reg r, uint8_t val)
{
    beginTransaction();

    transfer8(0x40); // 0b01000000
    transfer8((uint8_t)r);
    transfer8(val);

    endTransaction();
}
// read a MCP register , using spi .
uint8_t MCP23S17::_readRegister(reg r)
{
    beginTransaction();

    transfer8(0x41); // 0b01000001
    transfer8((uint8_t)r);
    uint8_t val = transfer8(0x00); // Get output

    endTransaction();

    return val;
}

void MCP23S17::_setBit(reg r, uint8_t idx)
{
    uint8_t state = _readRegister(r);
    _writeRegister(r, state | (0x01 << idx));
}

void MCP23S17::_clearBit(reg r, uint8_t idx)
{
    uint8_t state = _readRegister(r);
    _writeRegister(r, state & ~(0x01 << idx));
}

void MCP23S17::digitalWrite(Port p, uint8_t idx, bool value)
{
    uint8_t portRegister = (uint8_t)reg::IOA + (uint8_t)p;

    if (value)
        _setBit((reg)portRegister, idx);
    else
        _clearBit((reg)portRegister, idx);
}

uint8_t MCP23S17::readBits(Port p)
{
    uint8_t portRegister = (uint8_t)reg::IOA + (uint8_t)p;

    return _readRegister((reg)portRegister);
}

bool MCP23S17::digitalRead(Port p, uint8_t idx)
{
    return readBits(p) & (0x01 << idx);
}

void MCP23S17::pinMode(Port p, uint8_t idx, PinMode mode)
{
    uint8_t portIoRegister = (uint8_t)reg::IODIRA + (uint8_t)p;
    uint8_t portPullupRegister = (uint8_t)reg::GPPUA + (uint8_t)p;

    switch (mode)
    {
    case PinMode::Output:
        _clearBit((reg)portIoRegister, idx);
        break;

    case PinMode::Input_Pullup:
        _setBit((reg)portPullupRegister, idx);
        // Allowing fallthrough to set the pin as input
        [[fallthrough]];
    case PinMode::Input:
        _setBit((reg)portIoRegister, idx);
        break;
    }
}

MCP23S17 ioExt;