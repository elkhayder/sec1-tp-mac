#include "stm32f3xx.h"
#include "sw.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "tft.h"
// #include <math.h>

#define Iterations 100

#pragma GCC diagnostic ignored "-Wunused-variable"

int16_t sin(int step)
{

    const int16_t sinTab[256] = {
        0x0000, 0x00c9, 0x0192, 0x025b, 0x0324, 0x03ed, 0x04b6, 0x057f,
        0x0647, 0x0710, 0x07d9, 0x08a2, 0x096a, 0x0a33, 0x0afb, 0x0bc3,
        0x0c8b, 0x0d53, 0x0e1b, 0x0ee3, 0x0fab, 0x1072, 0x1139, 0x1201,
        0x12c8, 0x138e, 0x1455, 0x151b, 0x15e2, 0x16a8, 0x176d, 0x1833,
        0x18f8, 0x19bd, 0x1a82, 0x1b47, 0x1c0b, 0x1ccf, 0x1d93, 0x1e56,
        0x1f19, 0x1fdc, 0x209f, 0x2161, 0x2223, 0x22e5, 0x23a6, 0x2467,
        0x2528, 0x25e8, 0x26a8, 0x2767, 0x2826, 0x28e5, 0x29a3, 0x2a61,
        0x2b1f, 0x2bdc, 0x2c98, 0x2d55, 0x2e11, 0x2ecc, 0x2f87, 0x3041,
        0x30fb, 0x31b5, 0x326e, 0x3326, 0x33de, 0x3496, 0x354d, 0x3604,
        0x36ba, 0x376f, 0x3824, 0x38d8, 0x398c, 0x3a40, 0x3af2, 0x3ba5,
        0x3c56, 0x3d07, 0x3db8, 0x3e68, 0x3f17, 0x3fc5, 0x4073, 0x4121,
        0x41ce, 0x427a, 0x4325, 0x43d0, 0x447a, 0x4524, 0x45cd, 0x4675,
        0x471c, 0x47c3, 0x4869, 0x490f, 0x49b4, 0x4a58, 0x4afb, 0x4b9e,
        0x4c3f, 0x4ce1, 0x4d81, 0x4e21, 0x4ebf, 0x4f5e, 0x4ffb, 0x5097,
        0x5133, 0x51ce, 0x5269, 0x5302, 0x539b, 0x5433, 0x54ca, 0x5560,
        0x55f5, 0x568a, 0x571d, 0x57b0, 0x5842, 0x58d4, 0x5964, 0x59f3,
        0x5a82, 0x5b10, 0x5b9d, 0x5c29, 0x5cb4, 0x5d3e, 0x5dc7, 0x5e50,
        0x5ed7, 0x5f5e, 0x5fe3, 0x6068, 0x60ec, 0x616f, 0x61f1, 0x6271,
        0x62f2, 0x6371, 0x63ef, 0x646c, 0x64e8, 0x6563, 0x65dd, 0x6657,
        0x66cf, 0x6746, 0x67bd, 0x6832, 0x68a6, 0x6919, 0x698c, 0x69fd,
        0x6a6d, 0x6adc, 0x6b4a, 0x6bb8, 0x6c24, 0x6c8f, 0x6cf9, 0x6d62,
        0x6dca, 0x6e30, 0x6e96, 0x6efb, 0x6f5f, 0x6fc1, 0x7023, 0x7083,
        0x70e2, 0x7141, 0x719e, 0x71fa, 0x7255, 0x72af, 0x7307, 0x735f,
        0x73b5, 0x740b, 0x745f, 0x74b2, 0x7504, 0x7555, 0x75a5, 0x75f4,
        0x7641, 0x768e, 0x76d9, 0x7723, 0x776c, 0x77b4, 0x77fa, 0x7840,
        0x7884, 0x78c7, 0x7909, 0x794a, 0x798a, 0x79c8, 0x7a05, 0x7a42,
        0x7a7d, 0x7ab6, 0x7aef, 0x7b26, 0x7b5d, 0x7b92, 0x7bc5, 0x7bf8,
        0x7c29, 0x7c5a, 0x7c89, 0x7cb7, 0x7ce3, 0x7d0f, 0x7d39, 0x7d62,
        0x7d8a, 0x7db0, 0x7dd6, 0x7dfa, 0x7e1d, 0x7e3f, 0x7e5f, 0x7e7f,
        0x7e9d, 0x7eba, 0x7ed5, 0x7ef0, 0x7f09, 0x7f21, 0x7f38, 0x7f4d,
        0x7f62, 0x7f75, 0x7f87, 0x7f97, 0x7fa7, 0x7fb5, 0x7fc2, 0x7fce,
        0x7fd8, 0x7fe1, 0x7fe9, 0x7ff0, 0x7ff6, 0x7ffa, 0x7ffd, 0x7fff};

    step %= 1024;

    int16_t idx = step % 256;

    if (step < 256)
    {
        return sinTab[idx];
    }
    else if (step < 512)
    {
        return sin(255 - idx);
    }
    else if (step < 768)
    {
        return -sin(idx);
    }
    else
    {
        return -sin(255 - idx);
    }
}

int16_t cos(int step)
{
    return sin(step + 256);
}

/** affichage d'une valeur
 *  au format virgule fixe 10.6
 */
void affVF10_6(int16_t val)
{
    // seul endroit avec des 'float' car c'est du debug!!
    Tft.print(((float)(val)) / 64);
}

/** affichage d'une valeur
 *  au format virgule fixe 10.6
 */
void affVF1_15(int32_t val)
{
    // seul endroit avec des 'float' car c'est du debug!!
    Tft.print(((float)(val)) / 32768);
}

void setup()
{
    SWInit();
    Tft.setup();
}

int main(void)
{
    volatile int BoilerplateBenchmark;
    setup();

    Tft.setTextCursor(5, 0);

    // SWReset();
    // for (int i = 0; i < Iterations; i++)
    //     ;
    // BoilerplateBenchmark = SWGet();

    Tft.setCursor(0, 10);
    // Tft.print("Base time (Empty loop): ");
    // Tft.print(BoilerplateBenchmark);
    // Tft.println("uS");

    int steps[] = {0, 128, 512, 768, 1024, 64, 123, 830, 380};

    for (unsigned int i = 0; i < sizeof(steps) / sizeof(*steps); i++)
    {
        int idx = steps[i];
        float angle = (float)idx * 90 / 256.0;
        int16_t sinX = sin(idx);
        int16_t cosX = cos(idx);

        Tft.print(angle);
        Tft.print(" => ");
        affVF1_15(sinX);
        Tft.print(" ");
        affVF1_15(cosX);
        Tft.println();
    }

    /**
     * Fixed point arithmetic
     */

    // int16_t a = 0x00be, b = 0x1037;

    // Tft.print("a = ");
    // affVF10_6(a);
    // Tft.print(", b = ");
    // affVF10_6(b);
    // Tft.println();

    // /**
    //  * Benchmark
    //  */

    // {
    //     volatile int16_t c;
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         c = a + b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Add (10,6): ");
    //     affVF10_6(c);
    //     Tft.print("\r\nDuration: ");
    //     Tft.println(duration / Iterations);
    // }

    // {
    //     volatile int16_t c;
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         c = a - b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Sub (10,6): ");
    //     affVF10_6(c);
    //     Tft.print("\r\nDuration: ");
    //     Tft.println(duration / Iterations);
    // }

    // {
    //     volatile int32_t c;
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         c = (int32_t)a * (int32_t)b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Mul (10,6): ");
    //     affVF10_6(c >> 6);
    //     Tft.print("\r\nDuration: ");
    //     Tft.println(duration / Iterations);
    // }

    // volatile int a = 10, b = 20;

    // // Addition
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile int c = a + b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     // Tft.print("Addition: ");
    //     // affVF10_6((float)duration /Iterations);
    //     // Tft.println("uS");
    // }

    // // Subtraction
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile int c = a - b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Subtraction: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Multiplication
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile int c = a * b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Multiplication: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Division
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile int c = a / b;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Division: ");
    //     Tft.println(duration / Iterations);
    // }

    // volatile float d = 14242.242, e = -43841.314;

    // // Float addition
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = d + e;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float add: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Float subtraction
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = d - e;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float sub: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Float mul
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = d * e;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float mul: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Float div
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = d / e;
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float div: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Float sqrt
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = sqrt(e);
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float sqrt: ");
    //     Tft.println(duration / Iterations);
    // }

    // // Float cos
    // {
    //     SWReset();
    //     for (int i = 0; i < Iterations; i++)
    //     {
    //         volatile float c = cos(d);
    //     }

    //     uint32_t duration = SWGet() - BoilerplateBenchmark;
    //     Tft.print("Float cos: ");
    //     Tft.println(duration / Iterations);
    // }

    /* Infinite loop */
    while (1)
    {
    }
}
