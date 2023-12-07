#include "stm32f3xx.h"
#include "pinAccess.h"
#include "adc.h"

#define Timer TIM6
#define Button GPIOB, 1
#define SEQ_LEN sizeof(BSRR_RotationSeq) / sizeof(*BSRR_RotationSeq)
#define ARR(f) 1000.0 / f - 1

#define FullRoundSteps 64 * 64

// Global variables, are they bad?
uint32_t RotationIndex = 0;
uint32_t BSRR_RotationSeq[8] = {}; // Will be calculated on setup

void setup()
{
    // Configure Button
    pinMode(Button, INPUT_PULLUP);

    // Setup Motor Output pins
    for (int i = 5; i <= 8; i++)
        pinMode(GPIOA, i, OUTPUT);

    ADCInit(); // Setup ADC

    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;

    // Configure timer
    Timer->CNT = 0;
    Timer->SR = 0;
    Timer->PSC = 64000 - 1;
    Timer->ARR = ARR(10); // Starting with 10Hz
    Timer->CR1 = 1;

    // Precalculate BSRR values]
    const uint8_t seq[] = {8, 0xC, 4, 6, 2, 3, 1, 9};

    for (int i = 0; i < 8; i++)
    {
        BSRR_RotationSeq[i] = seq[i] << 5           // 1's
                              | ~seq[i] << (16 + 5) // 0's
            ;
    }
}

void stepCCW()
{
    RotationIndex++;
    RotationIndex %= SEQ_LEN;

    GPIOA->BSRR = BSRR_RotationSeq[RotationIndex];
}

void stepCW()
{
    if (RotationIndex == 0)
    {
        RotationIndex = SEQ_LEN - 1;
    }
    else
    {
        RotationIndex--;
    }

    GPIOA->BSRR = BSRR_RotationSeq[RotationIndex];
}

enum class ButtonState
{
    Released,
    Pushing,
    Pushed,
    Releasing,
};

ButtonState getButtonState()
{
    static ButtonState state = ButtonState::Released;

    switch (state)
    {
    case ButtonState::Released:
        if (digitalRead(Button) == 0)
            state = ButtonState::Pushing;
        break;

    case ButtonState::Pushing:
        state = ButtonState::Pushed;
        break;

    case ButtonState::Pushed:
        if (digitalRead(Button) == 1)
            state = ButtonState::Releasing;
        break;

    case ButtonState::Releasing:
        state = ButtonState::Released;
        break;
    }

    return state;
}

enum class Direction
{
    Clockwise,
    CounterClockwise
};

int main(void)
{
    setup();

    Direction direction = Direction::Clockwise;
    int rotatedSteps = 0;

    while (1)
    {

        if (getButtonState() == ButtonState::Pushing)
        {
            // Reset remaining steps
            rotatedSteps = 0;

            // Invert rotation direction
            if (direction == Direction::Clockwise)
                direction = Direction::CounterClockwise;
            else
                direction = Direction::Clockwise;
        }

        if (Timer->SR & 1U)
        {
            Timer->SR = 0; // Reset timer overflow flag

            if (rotatedSteps < FullRoundSteps)
            {
                if (direction == Direction::Clockwise)
                    stepCW();
                else // direction == Direction::CounterClockwise
                    stepCCW();

                // Update rotation frequency
                Timer->ARR = ARR(10 + (500 - 10) * (float)ADCRead() / 4096.);

                rotatedSteps++;
            }
        }
    }

    return 0;
}
