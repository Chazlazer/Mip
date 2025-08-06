#include "rc_gpio.h"
#include "iostream"

#ifdef __cplusplus
extern "C" {
#endif

GPIO::GPIO()
{
}

void GPIO::open(GPIOPin port, unsigned long DDR)
{
    int chip, pin;

    // Map GPIOPin enum to chip and pin values
    switch(port) {
        case GPIO3_1: chip = 3; pin = 1; break;
        case GPIO3_2: chip = 3; pin = 2; break;
        case GPIO3_17: chip = 3; pin = 17; break;
        case GPIO3_20: chip = 3; pin = 20; break;
        case GPIO1_17: chip = 1; pin = 17; break;
        case GPIO1_25: chip = 1; pin = 25; break;
        default: return;  // Invalid GPIO
    }
    rc_gpio_init(chip, pin, DDR);
}

void GPIO::close(GPIOPin port)
{
    int chip, pin;
    rc_led_t led;

    switch(port) {
        case GPIO3_1: chip = 3; pin = 1; break;
        case GPIO3_2: chip = 3; pin = 2; break;
        case GPIO3_17: chip = 3; pin = 17; break;
        case GPIO3_20: chip = 3; pin = 20; break;
        case GPIO1_17: chip = 1; pin = 17; break;
        case GPIO1_25: chip = 1; pin = 25; break;
        case LED_GREEN: chip = LED; led = RC_LED_GREEN; break;
        case LED_RED: chip = LED; led = RC_LED_RED; break;
        default: return;
    }
    if(chip == LED){
        rc_led_cleanup();
    }
    else{
        rc_gpio_cleanup(chip, pin);
    }
}

int GPIO::read(GPIOPin port)
{
    int chip, pin;
    rc_led_t led;

    switch(port) {
        case GPIO3_1: chip = 3; pin = 1; break;
        case GPIO3_2: chip = 3; pin = 2; break;
        case GPIO3_17: chip = 3; pin = 17; break;
        case GPIO3_20: chip = 3; pin = 20; break;
        case GPIO1_17: chip = 1; pin = 17; break;
        case GPIO1_25: chip = 1; pin = 25; break;
        case LED_GREEN: chip = LED; led = RC_LED_GREEN; break;
        case LED_RED: chip = LED; led = RC_LED_RED; break;
        default: return -1;
    }
    if(chip == LED){
        return rc_led_get(led);
    }
    else{
        return rc_gpio_get_value(chip, pin);
    }
}

void GPIO::write(GPIOPin port, int value)
{
    int chip, pin;
    rc_led_t led;

    switch(port) {
        case GPIO3_1: chip = 3; pin = 1; break;
        case GPIO3_2: chip = 3; pin = 2; break;
        case GPIO3_17: chip = 3; pin = 17; break;
        case GPIO3_20: chip = 3; pin = 20; break;
        case GPIO1_17: chip = 1; pin = 17; break;
        case GPIO1_25: chip = 1; pin = 25; break;
        case LED_GREEN: chip = LED; led = RC_LED_GREEN; break;
        case LED_RED: chip = LED; led = RC_LED_RED; break;
        default: return;
    }
    if(chip == LED){
        rc_led_set(led,value);
    }
    else{
        rc_gpio_set_value(chip, pin, value);
    }
}

GPIO::~GPIO()
{
}

#ifdef __cplusplus
}
#endif