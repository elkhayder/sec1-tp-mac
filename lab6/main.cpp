#include "stm32f3xx.h"
#include "tft.h"

#include "encoder.h"
#include "servo.h"

#include "pinAccess.h"
#include "exti.h"

void setup()
{
    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;

    // Configure timer
    TIM6->CNT = 0;
    TIM6->SR = 0;
    TIM6->PSC = 64000 - 1; // 1ms
    TIM6->ARR = 10 - 1;    // 50ms
    TIM6->CR1 = 1;

    // enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);

    pinMode(GPIOB, 1, INPUT_PULLUP);
    attachInterrupt(GPIOB, 1, FALLING);
}

enum class Direction
{
    Clockwise,
    CounterClockwise,
};

enum class Mode
{
    Manual,
    Transition,
    Scan,
};

Mode _mode = Mode::Scan;
Direction _dir = Direction::Clockwise;

int _steps = 0;

extern "C" void EXTI1_IRQHandler(void)
{
    switch (_mode)
    {
    case Mode::Scan:
    case Mode::Transition:
        _mode = Mode::Transition;
        break;

    case Mode::Manual:
        _steps = encoderValueInSteps();
        _mode = Mode::Scan;
        break;
    }

    EXTI->PR |= EXTI_PR_PR1; // it acknowledge
}

extern "C" void TIM6_DAC1_IRQHandler(void)
{
    if (_mode == Mode::Scan || _mode == Mode::Transition)
    {
        if (_steps <= 0)
            _dir = Direction::Clockwise;
        else if (_steps >= 500)
            _dir = Direction::CounterClockwise;

        if (_dir == Direction::Clockwise)
            _steps++;
        else // CounterClockwise
            _steps--;

        if (_mode == Mode::Transition && _steps == encoderValueInSteps())
        {
            _mode = Mode::Manual;
        }
    }

    if (_mode == Mode::Manual)
    {
        servoSet(encoderValueInSteps());
    }
    else
    {
        servoSet(_steps);
    }

    // Acknowladge
    TIM6->SR &= ~TIM_SR_UIF;
}

int main()
{
    servoInit();
    encoderInit();
    setup();
    Tft.setup();

    Mode _prevMode = _mode;
    int _prevEncoder = encoderValue();
    int _prevSteps = _steps;

    Tft.print("Mode: ");
    Tft.println(_mode == Mode::Manual ? "Manual    " : _mode == Mode::Transition ? "Transition"
                                                                                 : "Scan      ");

    Tft.print("Read: ");
    Tft.print(encoderValue());
    Tft.print(" ");
    Tft.println(encoderValueInSteps());

    Tft.print("Scan: ");
    Tft.println(_steps);

    while (1)
    {
        // Tft.erase();

        if (_prevMode != _mode)
        {
            Tft.setTextCursor(6, 0);
            Tft.eraseText(11);
            Tft.println(_mode == Mode::Manual ? "Manual    " : _mode == Mode::Transition ? "Transition"
                                                                                         : "Scan      ");
            _prevMode = _mode;
        }

        if (_prevEncoder != encoderValue())
        {
            Tft.setTextCursor(6, 1);
            Tft.eraseText(6);

            if (encoderValue() < 10)
                Tft.print("0");
            Tft.print(encoderValue());

            Tft.print(" ");

            if (encoderValueInSteps() < 100)
                Tft.print("0");
            if (encoderValueInSteps() < 10)
                Tft.print("0");
            Tft.println(encoderValueInSteps());

            _prevEncoder = encoderValue();
        }

        if (_prevSteps != _steps)
        {
            Tft.setTextCursor(6, 2);
            Tft.eraseText(3);

            if (_steps < 100)
                Tft.print("0");
            if (_steps < 10)
                Tft.print("0");
            Tft.println(_steps);

            _prevSteps = _steps;
        }

        for (volatile int i = 0; i < 1000; i++)
            ;
    }
}
