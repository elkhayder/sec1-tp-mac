#include "stm32f3xx.h"
#include "pinAccess.h"

#define CH0 GPIOB, 7
#define CH1 GPIOF, 0
#define CH2 GPIOF, 1

#define ButtonRight GPIOB, 6
#define ButtonLeft GPIOB, 1

int setLed(int id)
{
    if (id > 5) // Verify input
        return -1;

    switch (id)
    {
    case 0:
        // CH0
        pinMode(CH0, OUTPUT);
        digitalWrite(CH0, 1);
        // CH1
        pinMode(CH1, OUTPUT);
        digitalWrite(CH1, 0);
        // CH2
        pinMode(CH2, INPUT);
        break;

    case 1:
        // CH0
        pinMode(CH0, OUTPUT);
        digitalWrite(CH0, 0);
        // CH1
        pinMode(CH1, OUTPUT);
        digitalWrite(CH1, 1);
        // CH2
        pinMode(CH2, INPUT);
        break;

    case 2:
        // CH0
        pinMode(CH0, INPUT);
        // CH1
        pinMode(CH1, OUTPUT);
        digitalWrite(CH1, 1);
        // CH2
        pinMode(CH2, OUTPUT);
        digitalWrite(CH2, 0);
        break;

    case 3:
        // CH0
        pinMode(CH0, INPUT);
        // CH1
        pinMode(CH1, OUTPUT);
        digitalWrite(CH1, 0);
        // CH2
        pinMode(CH2, OUTPUT);
        digitalWrite(CH2, 1);
        break;

    case 4:
        // CH0
        pinMode(CH0, OUTPUT);
        digitalWrite(CH0, 1);
        // CH1
        pinMode(CH1, INPUT);
        // CH2
        pinMode(CH2, OUTPUT);
        digitalWrite(CH2, 0);
        break;

    case 5:
        // CH0
        pinMode(CH0, OUTPUT);
        digitalWrite(CH0, 0);
        // CH1
        pinMode(CH1, INPUT);
        // CH2
        pinMode(CH2, OUTPUT);
        digitalWrite(CH2, 1);
        break;
    }

    return 0;
}

void wait()
{
    volatile int i = 0;

    for (i = 0; i < 1000; i++)
        ;
}

void charlieplexing(uint8_t mask)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if (mask & (1 << i))
        {
            setLed(i);
            wait();
        }
    }
}

class Button
{
    enum State
    {
        RELEASED,
        PUSHING,
        PUSHED,
        RELEASING
    };

public:
    Button(GPIO_TypeDef *port, unsigned char pin) : _port(port),
                                                    _pin(pin)
    {
        pinMode(_port, _pin, INPUT_PULLUP);
    }

    void Update()
    {
        int value = digitalRead(_port, _pin);

        switch (_state)
        {
        case RELEASED:
            if (value == 0)
            {
                _state = PUSHING;
            }
            break;

        case PUSHING:
            _state = PUSHED;
            break;

        case PUSHED:
            if (value == 1)
            {
                _state = RELEASING;
            }
            break;

        case RELEASING:
            _state = RELEASED;
            break;

        default:
            break;
        }
    }

    int JustClicked()
    {
        return _state == PUSHING;
    }

private:
    GPIO_TypeDef *_port;
    unsigned char _pin;

    State _state = State::RELEASED;
};

int main(void)
{
    Button rightButton(ButtonRight), leftButton(ButtonLeft);

    uint8_t shift = 0;

    while (1)
    {
        rightButton.Update();
        leftButton.Update();

        if (rightButton.JustClicked())
        {
            if (shift == 5)
            {
                shift = 0;
            }
            else
            {
                shift++;
            }
        }
        else if (leftButton.JustClicked())
        {
            if (shift == 0)
            {
                shift = 5;
            }
            else
            {
                shift--;
            }
        }

        charlieplexing(0xFF & ~(1 << shift));
    }
}