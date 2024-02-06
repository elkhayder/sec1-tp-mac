#include "stm32f3xx.h"
#include "spi.h"
#include "MCP23S17.h"

enum class Mode
{
    Chaser,
    BlinkingAll,
    BlinkingOdd,
    BlinkingEven
};

volatile Mode mode = Mode::Chaser;

void SetModeChaser() { mode = Mode::Chaser; }
void SetModeBlinkingAll() { mode = Mode::BlinkingAll; }
void SetModeBlinkingOdd() { mode = Mode::BlinkingOdd; }
void SetModeBlinkingEven() { mode = Mode::BlinkingEven; }

void ModeChaser()
{
    static int i = 0;

    i++;

    if (i > 7)
        i = 0;
    if (i < 0)
        i = 7;

    for (int j = 0; j < 8; j++)
    {
        ioExt.digitalWrite(MCP23S17::Port::A, j, i == j);
    }

    for (volatile int x = 0; x < 500000; x++)
        ;
}

void ModeBlinking()
{
    static bool ledsAreOn = false;
    ledsAreOn = !ledsAreOn;

    uint8_t val;

    switch (mode)
    {
    case Mode::BlinkingAll:
        val = 0xFF;
        break;
    case Mode::BlinkingEven:
        val = 0xAA;
        break;
    case Mode::BlinkingOdd:
        val = 0x55;
        break;
    default:
        break;
    }

    for (int i = 0; i < 8; i++)
    {
        if (!ledsAreOn)
            ioExt.digitalWrite(MCP23S17::Port::A, i, 0);
        else
            ioExt.digitalWrite(MCP23S17::Port::A, i, val & 1 << i);
    }

    for (volatile int x = 0; x < 1000000; x++)
        ;
}

void setup()
{
    ioExt.Init();

    for (int i = 0; i < 8; i++)
    {
        ioExt.pinMode(MCP23S17::Port::A, i, MCP23S17::PinMode::Output);
        ioExt.pinMode(MCP23S17::Port::B, i, MCP23S17::PinMode::Input_Pullup);
    }

    ioExt.attachInterrupt(MCP23S17::Port::B, 4, MCP23S17::InterruptType::Rising, SetModeChaser);
    ioExt.attachInterrupt(MCP23S17::Port::B, 5, MCP23S17::InterruptType::Rising, SetModeBlinkingAll);
    ioExt.attachInterrupt(MCP23S17::Port::B, 6, MCP23S17::InterruptType::Rising, SetModeBlinkingOdd);
    ioExt.attachInterrupt(MCP23S17::Port::B, 7, MCP23S17::InterruptType::Rising, SetModeBlinkingEven);
}

int main()
{
    setup();
    while (1)
    {
        switch (mode)
        {
        case Mode::Chaser:
            ModeChaser();
            break;

        default:
            ModeBlinking();
            break;
        }
    }
}
