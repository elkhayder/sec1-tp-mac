#ifndef __MCP23S17_H
#define __MCP23S17_H

#include "stm32f3xx.h"

extern "C" void EXTI9_5_IRQHandler(void);

class MCP23S17
{
public:
    typedef void (*InterruptCallback)(void);

    enum class Port : uint8_t
    {
        A = 0x00,
        B = 0x01,
    };

    enum class PinMode : uint8_t
    {
        Output = 0x00,
        Input = 0x01,
        Input_Pullup = 0x02,
    };

    enum class InterruptType
    {
        Rising,
        Falling,
        Both
    };

private:
    enum class reg : uint8_t
    {
        IODIRA = 0x00,
        IODIRB = 0x01,
        IPOLA = 0x02,
        IPOLB = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA = 0x06,
        DEFVALB = 0x07,
        INTCONA = 0x08,
        INTCONB = 0x09,
        IOCON1 = 0x0A,
        IOCON2 = 0x0B,
        GPPUA = 0x0C,
        GPPUB = 0x0D,
        INTFA = 0x0E,
        INTFB = 0x0F,
        INTCAPA = 0x10,
        INTCAPB = 0x11,
        IOA = 0x12,
        IOB = 0x13,
        OLATA = 0x14,
        OLATB = 0x15,
    };

public:
    MCP23S17() {}
    void Init();

    void pinMode(Port p, uint8_t idx, PinMode mode);
    void digitalWrite(Port p, uint8_t idx, bool value);
    bool digitalRead(Port p, uint8_t idx);
    uint8_t readBits(Port p);

    void onInterrupt(void);

    void attachInterrupt(Port p, uint8_t idx, InterruptType type, InterruptCallback callback);

private:
    void _writeRegister(reg r, uint8_t val);
    uint8_t _readRegister(reg r);
    void _setBit(reg r, uint8_t idx);
    void _clearBit(reg r, uint8_t idx);

    reg _getRelativeRegister(reg base, Port p);
    uint8_t _getInterruptHandlerIndex(Port p, uint8_t idx);

    // friend void EXTI1_IRQHandler(void);

private:
    struct Interrupt
    {
        bool enabled = false;
        InterruptType type;
        InterruptCallback callback = nullptr;
    } _interrupts[16]; // 16 PINS (2 ports, 8 pins each)
};

extern MCP23S17 ioExt;

#endif // __MCP23S17_H