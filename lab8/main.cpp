#include "stm32f3xx.h"
#include "pinAccess.h"
#include "tft.h"

#include "serial.h"

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
    TIM6->ARR = 2000 - 1;  // 100ms
    TIM6->CR1 = 1;

    // enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);
}

extern "C" void TIM6_DAC1_IRQHandler(void)
{
    char msg[] = "Hello, World!";

    Serial.println(msg);
    Serial.println(100);
    Serial.println(937103);
    Serial.println(1094197483);
    Serial.println(0);
    Serial.println(-1094197483);
    Serial.println(-487232425);

    // Acknowladge
    TIM6->SR &= ~TIM_SR_UIF;
}

int main()
{
    setup();
    Tft.setup();
    Serial.setup();

    pinMode(GPIOB, 0, OUTPUT);

    USART2->TDR = '\0';

    while (1)
    {

        Tft.setTextCursor(1, 1);
        Tft.eraseText(20);

        Tft.print("Available: ");
        Tft.println(Serial._buffer.size());

        digitalToggle(GPIOB, 0);

        for (volatile int x = 0; x < 10000000; x++)
            ;
    }
}
