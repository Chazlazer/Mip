/*
 * Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * @file RF24_arch_config.h
 * General defines and includes for RF24/Linux
 */

/**
 * Example of RF24_arch_config.h for RF24 portability
 *
 * @defgroup Porting_General Porting: General
 * @{
 */

 #ifndef ARDUINO_WRAPPER_H
 #define ARDUINO_WRAPPER_H
 
 #include <stddef.h>
 #include "rc_gpio.h"
 #include "rc_compatibility.h"
 #include "rc_spi.h"
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>
 #include <sys/time.h>

 #define _BV(x) (1 << (x))
 #define _SPI   spi

//  typedef uint16_t prog_uint16_t;
//  typedef enum rf24_gpio_pin_t;
 #define PSTR(x)  (x)
 #define printf_P printf
 #define strlen_P strlen
 #define PROGMEM
 #define pgm_read_word(p) (*(const unsigned short*)(p))
 #define PRIPSTR          "%s"
 #define pgm_read_byte(p) (*(const unsigned char*)(p))
 #define pgm_read_ptr(p)  (*(void* const*)(p))

 #define RF24_PIN_INVALID 0xFF

 // Function, constant map as a result of migrating from Arduino
 #define LOW                      GPIO::OUTPUT_LOW
 #define HIGH                     GPIO::OUTPUT_HIGH
 #define INPUT                    GPIO::DIRECTION_IN
 #define OUTPUT                   GPIO::DIRECTION_OUT
 #define digitalWrite(pin, value) GPIO::write(pin, value)
 #define pinMode(pin, direction)  GPIO::open(pin, direction)
 #define delay(millisec)          __msleep(millisec)
 #define delayMicroseconds(usec)  __usleep(usec)
 #define millis()                 __millis()
 
 /**@}*/
 #endif // ARDUINO_WRAPPER_H