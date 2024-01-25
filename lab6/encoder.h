#ifndef __ENCODER_H
#define __ENCODER_H

void encoderInit(void);

int encoderValue(void);

int encoderValueInSteps(void);

extern "C" void EXTI0_IRQHandler(void);

#endif // __ENCODER_H