#include "stm32f3xx.h"
#include "pinAccess.h"

void serialSetup()
{
    // input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM6EN;
    // reset peripheral (mandatory!)
    RCC->APB1RSTR |= (RCC_APB1RSTR_USART2RST | RCC_APB1RSTR_TIM6RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART2RST | RCC_APB1RSTR_TIM6RST);

    pinMode(GPIOA, 2, OUTPUT);
    pinAlt(GPIOA, 2, 7);

    /**
     * With a frequency of 32MHz, to get a baud rate of 115200
     * we need to divide by ~278
     * 278 = 0x0116
     */
    USART2->BRR = 0x0116;
    USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

int main()
{
    serialSetup();

    USART2->TDR = 'C';

    while (1)
    {
    }
}
