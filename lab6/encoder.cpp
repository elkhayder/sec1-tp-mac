#include "encoder.h"
#include "pinAccess.h"
#include "exti.h"

static int _encoderValue = 0;

void encoderInit(void)
{
    pinMode(GPIOA, 0, INPUT_PULLUP);
    pinMode(GPIOA, 1, INPUT_PULLUP);

    attachInterrupt(GPIOA, 0, FALLING);
}

int encoderValue(void)
{
    return _encoderValue;
}

int encoderValueInSteps(void)
{
    return (_encoderValue * 1000) / 90;
}

// Encoder
extern "C" void EXTI0_IRQHandler(void)
{
    if (digitalRead(GPIOA, 1) == 0)
    { // Increment
        if (_encoderValue < 45)
            _encoderValue++;
    }
    else
    {
        // Decrement
        if (_encoderValue > 0)
            _encoderValue--;
    }

    EXTI->PR |= EXTI_PR_PR0; // it acknowledge
}