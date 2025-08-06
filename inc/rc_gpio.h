/**
 * @file gpio.h
 * Class declaration for SPI helper files
 */

/**
 * Example of gpio.h class declaration for GPIO portability
 *
 * @defgroup Porting_GPIO Porting: GPIO
 * @{
 */
#ifndef RC24_GPIO_H
#define RC24_GPIO_H

#include "rc/gpio.h"
#include "rc/led.h"
#include <cstdio>

// Struct to encapsulate GPIO details
typedef enum GPIOPin {
    GPIO3_1,
    GPIO3_2,
    GPIO3_17,
    GPIO3_20,
    GPIO1_17,
    GPIO1_25,
    LED_GREEN,
    LED_RED
}rf24_gpio_pin_t;

#define LED 0

class GPIO
{
public:
    /* Constants */
    static const int DIRECTION_OUT = GPIOHANDLE_REQUEST_OUTPUT;
    static const int DIRECTION_IN = GPIOHANDLE_REQUEST_INPUT;

    static const int OUTPUT_HIGH = 1;
    static const int OUTPUT_LOW = 0;

    GPIO();

    /**
     * Similar to Arduino pinMode(pin,mode);
     * @param port
     * @param DDR
     */
    static void open(GPIOPin port, unsigned long DDR);

    /**
     *
     * @param port
     */
    static void close(GPIOPin port);

    /**
     * Similar to Arduino digitalRead(pin);
     * @param port
     */
    static int read(GPIOPin port);

    /**
     * Similar to Arduino digitalWrite(pin,state);
     * @param port
     * @param value
     */
    static void write(GPIOPin port, int value);
    virtual ~GPIO();
};


/**@}*/

#endif // RF24_BBB