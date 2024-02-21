#include "MCP23S17.h"

#include "pinAccess.h"
#include "exti.h"
#include "spi.h"

void MCP23S17::Init()
{
    setupSPI(); // Setup SPI

    for (uint16_t i = 0; i < sizeof(_interrupts) / sizeof(*_interrupts); i++)
    {
        _interrupts[i].enabled = false;
        _interrupts[i].callback = nullptr;
    }

    // Configure EXTI (PA9)
    ::pinMode(GPIOA, 9, INPUT_PULLUP);
    ::attachInterrupt(GPIOA, 9, FALLING);
}

extern "C" void EXTI9_5_IRQHandler(void)
{
    ioExt.onInterrupt();

    EXTI->PR |= EXTI_PR_PR9; // acknowledge
}

void MCP23S17::onInterrupt(void)
{
    uint16_t interruptFlags = _readRegister(MCP23S17::reg::INTFB) << 8 | _readRegister(MCP23S17::reg::INTFA);
    uint16_t state = _readRegister(reg::INTCAPB) << 8 | _readRegister(reg::INTCAPA);

    for (int i = 0; i < 16; i++)
    {
        uint16_t bit_Msk = 1 << i;
        auto intr = &_interrupts[i];

        // If there is not interupt flag, or the interupt is not enabled
        if (!(interruptFlags & bit_Msk) || !intr->enabled)
            continue;

        // Any edge
        if (intr->type == InterruptType::Both)
            intr->callback();
        // Falling Edge
        else if (intr->type == InterruptType::Falling && !(state & bit_Msk))
            intr->callback();
        // Rising Edge
        else if (intr->type == InterruptType::Rising && (state & bit_Msk))
            intr->callback();
    }
}

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
    reg portRegister = _getRelativeRegister(reg::IOA, p);

    if (value)
        _setBit(portRegister, idx);
    else
        _clearBit(portRegister, idx);
}

uint8_t MCP23S17::readBits(Port p)
{
    return _readRegister(_getRelativeRegister(reg::IOA, p));
}

bool MCP23S17::digitalRead(Port p, uint8_t idx)
{
    return readBits(p) & (0x01 << idx);
}

void MCP23S17::pinMode(Port p, uint8_t idx, PinMode mode)
{
    reg portIoRegister = _getRelativeRegister(reg::IODIRA, p);
    reg portPullupRegister = _getRelativeRegister(reg::GPPUA, p);

    switch (mode)
    {
    case PinMode::Output:
        _clearBit(portIoRegister, idx);
        break;

    case PinMode::Input_Pullup:
        _setBit(portPullupRegister, idx);
        // Allowing fallthrough to set the pin as input
        [[fallthrough]];
    case PinMode::Input:
        _setBit(portIoRegister, idx);
        break;
    }
}

MCP23S17::reg MCP23S17::_getRelativeRegister(reg base, Port p)
{
    return static_cast<reg>(static_cast<uint8_t>(base) + static_cast<uint8_t>(p));
}

uint8_t MCP23S17::_getInterruptHandlerIndex(Port p, uint8_t idx)
{
    return (uint8_t)p * 8 + idx;
}

void MCP23S17::attachInterrupt(Port p, uint8_t idx, InterruptType type, InterruptCallback callback)
{

    // if (type == InterruptType::Both)
    // Pin value is compared against the previous pin value
    _clearBit(_getRelativeRegister(reg::INTCONA, p), idx);
    // else
    //     // Pin value is compared against the associated bit in the DEFVAL register
    //     _setBit(_getRelativeRegister(reg::INTCONA, p), idx);

    // // Set DEFVAL
    // if (type == InterruptType::Falling)
    //     // Set to 1 to detect when value changes to 0
    //     _setBit(_getRelativeRegister(reg::DEFVALA, p), idx);
    // else if (type == InterruptType::Rising)
    //     // Set to 0 to detect when value changes to 1
    //     _clearBit(_getRelativeRegister(reg::DEFVALA, p), idx);

    // Enable Interrupt
    _setBit(_getRelativeRegister(reg::GPINTENA, p), idx);

    _interrupts[_getInterruptHandlerIndex(p, idx)].type = type;
    _interrupts[_getInterruptHandlerIndex(p, idx)].enabled = true;
    _interrupts[_getInterruptHandlerIndex(p, idx)].callback = callback;
}

MCP23S17 ioExt;