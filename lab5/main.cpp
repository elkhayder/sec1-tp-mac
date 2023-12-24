#include "stm32f3xx.h"
#include "pinAccess.h"
#include "exti.h"
#include "tft.h"

#define TRIG_ECHO GPIOA, 10

volatile uint32_t dt_us = 0;
volatile bool unavailable = false;

void setup()
{
    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST | RCC_APB1RSTR_TIM7RST;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM6RST | RCC_APB1RSTR_TIM7RST);

    // Configure timer
    TIM6->CNT = 0;
    TIM6->SR = 0;
    TIM6->PSC = 64000 - 1;
    TIM6->ARR = 100 - 1; // 100ms
    TIM6->CR1 |= TIM_CR1_CEN;

    // enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);

    // Configure timer
    TIM7->CNT = 0;
    TIM7->SR = 0;
    TIM7->PSC = 64 - 1;
    TIM7->ARR = 50000 - 1; // 50ms
    TIM7->CR1 |= TIM_CR1_CEN;

    attachInterrupt(TRIG_ECHO, CHANGE);
}

extern "C" void TIM6_DAC1_IRQHandler(void)
{
    pinMode(TRIG_ECHO, OUTPUT);
    EXTI->IMR &= ~EXTI_IMR_MR10;

    digitalWrite(TRIG_ECHO, 1);
    for (volatile int i = 0; i < 50; i++)
        ;
    digitalWrite(TRIG_ECHO, 0);

    pinMode(TRIG_ECHO, INPUT);
    EXTI->IMR |= EXTI_IMR_MR10;

    // Enable 7 interrupt
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_DAC2_IRQn);
    TIM7->CNT = 0;

    // acknowledge
    TIM6->SR &= ~TIM_SR_UIF;
}

extern "C" void TIM7_DAC2_IRQHandler(void)
{
    unavailable = true;
    TIM7->CNT = 0;

    // acknowledge
    TIM7->SR &= ~TIM_SR_UIF;
}

extern "C" void EXTI15_10_IRQHandler(void)
{
    TIM7->DIER &= ~TIM_DIER_UIE;
    NVIC_DisableIRQ(TIM7_DAC2_IRQn);
    unavailable = false;
    // static uint8_t isCounting = false; /// Maybe it should be removed?

    if (digitalRead(TRIG_ECHO) == 1) // Rizing edge
    {
        // if (!isCounting)
        // {
        TIM7->CNT = 0;
        //     isCounting = true;
        // }
    }
    else // Falling edge
    {
        // if (isCounting)
        // {
        dt_us = TIM7->CNT;
        //     isCounting = false;
        // }
    }

    EXTI->PR |= EXTI_PR_PR15; // it acknowledge
}

int main()
{
    setup();
    Tft.setup();

    while (1)
    {
        Tft.erase();

        if (unavailable)
        {
            Tft.setTextCursor(1, 1);
            Tft.println("Sensor unavailable");
        }
        else
        {
            uint32_t distance = dt_us / 58; // cm

            const int16_t MAX_HEIGHT = Tft.height() - 20,
                          MIN_HEIGHT = 4,
                          MAX_DISTANCE = 100,
                          MIN_DISTANCE = 2;

            float per = (float)distance / (float)(MAX_DISTANCE - MIN_DISTANCE);

            if (per > 1)
                per = 1;

            int16_t height = MIN_HEIGHT + (float)(MAX_HEIGHT - MIN_HEIGHT) * per;

            Tft.fillRect(
                Tft.width() / 2 - 25,
                MAX_HEIGHT - height,
                50,
                height,
                ST7735_BLUE * (1 - per) + ST7735_RED * per);

            Tft.setTextCursor(3, 80);
            Tft.print("Distance: ");
            Tft.print(distance);
            Tft.println(" cm");
        }

        for (volatile int i = 0; i < 2000000; i++)
            ;
    }
}
