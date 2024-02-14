#include "stm32f3xx.h"
#include "pinAccess.h"

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
    TIM6->ARR = 2000 - 1;  // 100ms
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

void serialPrintChar(char c)
{
    USART2->TDR = c;

    while (!(USART2->ISR & USART_ISR_TC))
        ;
}

void serialPrintString(char *s)
{
    while (*s != '\0')
        serialPrintChar(*s++);
}

void serialPrintInt(int n)
{
    int idx = 0, len = 0;
    char buffer[10], reversed[12]; // MAX_INT = 2,147,483,647 (10 chars) + sign char + \0

    if (n < 0)
    {
        reversed[idx++] = '-';
        n = -n; // Remove minus sign
    }

    do
    {
        uint8_t ones = n % 10;
        n /= 10;
        buffer[len++] = '0' + ones;
    } while (n != 0);

    for (int i = len - 1; i >= 0; i--)
    {
        reversed[idx++] = buffer[i];
    }
    reversed[idx] = 0; // Null terminator

    serialPrintString(reversed);
}

extern "C" void TIM6_DAC1_IRQHandler(void)
{
    char msg[] = "Hello, World!\n";
    serialPrintString(msg);
    serialPrintInt(100);
    serialPrintChar('\n');
    serialPrintInt(937103);
    serialPrintChar('\n');
    serialPrintInt(1094197483);
    serialPrintChar('\n');
    serialPrintInt(0);
    serialPrintChar('\n');
    serialPrintInt(-1094197483);
    serialPrintChar('\n');
    serialPrintInt(-487232425);
    serialPrintChar('\n');

    // Acknowladge
    TIM6->SR &= ~TIM_SR_UIF;
}

int main()
{
    serialSetup();

    while (1)
    {
    }
}
