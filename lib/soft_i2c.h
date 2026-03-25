#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include "ch32v00X.h"

typedef struct {
    GPIO_TypeDef* sda_port;
    uint16_t sda_pin;
    GPIO_TypeDef* scl_port;
    uint16_t scl_pin;
} SoftI2_t;

void softi2c_init(SoftI2_t* i2c);
void softi2c_start(SoftI2_t* i2c);
void softi2c_stop(SoftI2_t* i2c);
uint8_t softi2c_writebyte(SoftI2_t* i2c, uint8_t data);

#endif