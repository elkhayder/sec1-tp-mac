#include "stm32f3xx.h"
#include "pinAccess.h"

extern "C" void TIM6_DAC1_IRQHandler(void)
{
    static char c = 'a';

    USART2->TDR = c;

    if (c == 'z')
    {
        c = 'a';
    }
    else
    {
        c++;
    }

    // Acknowladge
    TIM6->SR &= ~TIM_SR_UIF;
}

void serialSetup()
{
    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM6EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= (RCC_APB1RSTR_USART2RST | RCC_APB1RSTR_TIM6RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART2RST | RCC_APB1RSTR_TIM6RST);

    // Configure timer
    TIM6->CNT = 0;
    TIM6->SR = 0;
    TIM6->PSC = 64000 - 1; // 1ms
    TIM6->ARR = 100 - 1;   // 100ms
    TIM6->CR1 = 1;

    /*  */
    pinMode(GPIOA, 2, OUTPUT);
    pinAlt(GPIOA, 2, 7);

    /**
     * With a frequency of 32MHz, to get a baud rate of 115200
     * we need to divide by ~278
     * 278 = 0x0116
     */
    USART2->BRR = 0x0116;
    USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    // enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);
}

int main()
{
    serialSetup();

    while (1)
    {
    }
}
