#include "stm32f3xx.h"
#include "spi.h"
#include "MCP23S17.h"

void setup()
{
    ioExt.Init();

    for (int i = 0; i < 8; i++)
    {
        ioExt.pinMode(MCP23S17::Port::A, i, MCP23S17::PinMode::Output);
        ioExt.pinMode(MCP23S17::Port::B, i, MCP23S17::PinMode::Input_Pullup);
    }
}

int main()
{
    setup();
    int i = 0;
    while (1)
    {
        bool isUp = ioExt.digitalRead(MCP23S17::Port::B, 0);

        if (isUp)
            i++;
        else
            i--;

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
}
