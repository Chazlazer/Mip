/**
 * @file spi.h
 * Class declaration for SPI helper files
 */

/**
 * Example of spi.h class declaration for SPI portability
 *
 * @defgroup Porting_SPI Porting: SPI
 * @{
 */
#ifndef RC24_SPI_H
#define RC24_SPI_H

#include <stdint.h>
#include <cstddef>
#include "rc/spi.h"

#define SPI_BUS_1 1
#define SPI_SS1_1 0
#define SPI_SS1_2 1

class SPI
{

public:
    /**
     * SPI constructor
     */
    SPI();

    /**
     * Start SPI
     */
    void begin(int busNo, int slaveNo, int _mode, uint32_t freq);

    /**
     * Transfer a single byte
     * @param tx_ Byte to send
     * @return Data returned via spi
     */
    uint8_t transfer(uint8_t tx);

    /**
     * Transfer a buffer of data
     * @param txBuf Transmit buffer
     * @param rxBuf Receive buffer
     * @param len Length of the data
     */
    void transfernb(uint8_t* txBuf, uint8_t* rxBuf, uint32_t len);

    /**
     * Transfer a buffer of data without an rx buffer
     * @param buf Pointer to a buffer of data
     * @param len Length of the data
     */
    void transfern(uint8_t* buf, uint32_t len);
    // exclude this line from the docs to prevent warnings docs generators
    virtual ~SPI();

private:
    /** SPI bus */
    int bus;
    /** SPI Mode set */
    uint8_t mode;
    /** SPI word size */
    uint8_t bits;
    /** SPI speed */
    uint32_t speed;
    /**SPI slave */
    int slave;
};

/**@}*/

#endif // RF24_BBB