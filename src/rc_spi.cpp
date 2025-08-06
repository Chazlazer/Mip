#include "rc_spi.h"

SPI::SPI(){

}

void SPI::begin(int busNo, int slaveNo, int _mode,uint32_t freq){
    bus = busNo;
    slave = slaveNo;
    mode = _mode;
    rc_spi_init_auto_slave(busNo, slave,mode, freq);
}

uint8_t SPI::transfer(uint8_t tx){
    uint8_t rxBuf = 0x0;
    rc_spi_transfer(bus, slave, &tx,1, &rxBuf);
    return rxBuf;
}

void SPI::transfernb(uint8_t* txBuf, uint8_t* rxBuf, uint32_t len){

    rc_spi_transfer(bus, slave, txBuf, len, rxBuf);
}

void SPI::transfern(uint8_t* buf, uint32_t len)
{
    transfernb(buf, buf, len);
}

SPI::~SPI(){
    
}