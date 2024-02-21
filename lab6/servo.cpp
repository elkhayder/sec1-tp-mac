#include "servo.h"
#include "pinAccess.h"

#define PB0 GPIOB, 0

void servoInit()
{
    pinMode(PB0, OUTPUT);
    pinAlt(PB0, 2);

    // Input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // Reset peripheral (mandatory!)
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

    // Configure timer
    TIM3->CNT = 0;         // Reset
    TIM3->SR = 0;          // Reset
    TIM3->PSC = 64 - 1;    // 1MHz
    TIM3->ARR = 20000 - 1; // 20ms

    // 3- PWM configuration
    TIM3->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;     // channel 3 as output
    TIM3->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; // output PWM mode 1
    TIM3->CCMR2 |= TIM_CCMR2_OC3PE;         // Pre-load register TIM3_CCR3
    TIM3->CR1 &= ~TIM_CR1_CMS_Msk;          // mode 1 // edge aligned mode
    TIM3->CR1 |= TIM_CR1_CEN;               // enable
    TIM3->CCER |= TIM_CCER_CC3E;            //
    TIM3->CCR3 = 1000 - 1;                  // Start at 1ms (0 deg)
}

void servoSet(int setPoint)
{
    if (setPoint < 0)
        setPoint = 0;
    else if (setPoint > 1000)
        setPoint = 1000;

    TIM3->CCR3 = 1000 + setPoint - 1; // Start at 1ms
}