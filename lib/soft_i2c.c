#include "soft_i2c.h"
#include "time.h"

#define DELAY_US 5

void softi2c_init(SoftI2_t* i2c) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;

    GPIO_InitStructure.GPIO_Pin = i2c->sda_pin;
    GPIO_Init(i2c->sda_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = i2c->scl_pin;
    GPIO_Init(i2c->scl_port, &GPIO_InitStructure);

    // §³§ä§Ñ§ß §á§à§Ü§à§ð
    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_SET);
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_SET);
}

void softi2c_start(SoftI2_t* i2c) {
    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_SET);
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_SET);
    delay_us(DELAY_US);

    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_RESET);
    delay_us(DELAY_US);
    
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_RESET);
}

void softi2c_stop(SoftI2_t* i2c) {
    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_RESET);
    delay_us(DELAY_US);
    
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_SET);
    delay_us(DELAY_US);
    
    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_SET);
    delay_us(DELAY_US);
}

/*
 * 1 - §µ§ã§á?§ç (ACK §à§ä§â§Ú§Þ§Ñ§ß§Ú§Û, §Ñ§Ò§à §Þ§Ú §Ó§Ó§Ñ§Ø§Ñ?§Þ§à, §ë§à §Ó?§Õ§á§â§Ñ§Ó§Ü§Ñ §á§â§à§Û§Õ§Ö§ß§Ñ)
 * 0 - §±§à§Þ§Ú§Ü§Ý§Ñ (NACK)
 */
uint8_t softi2c_writebyte(SoftI2_t* i2c, uint8_t data) {
    for(uint8_t i = 0; i < 8; i++) {
        // §£§Ú§ã§ä§Ñ§Ó§Ý§ñ?§Þ§à §Ò?§ä (§á§à§é§Ú§ß§Ñ§ð§é§Ú §Ù? §ã§ä§Ñ§â§ê§à§Ô§à - MSB)
        GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, (data & 0x80) ? Bit_SET : Bit_RESET);
        data <<= 1;
        delay_us(DELAY_US);

        // §¤§Ö§ß§Ö§â§å?§Þ§à ?§Þ§á§å§Ý§î§ã §ä§Ñ§Ü§ä§å§Ó§Ñ§ß§ß§ñ (SCL)
        GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_SET);
        delay_us(DELAY_US);
        GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_RESET);
        delay_us(DELAY_US);
    }

    // §¶§Ñ§Ù§Ñ §à§ä§â§Ú§Þ§Ñ§ß§ß§ñ ACK
    GPIO_WriteBit(i2c->sda_port, i2c->sda_pin, Bit_SET);
    delay_us(DELAY_US);
    
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_SET);
    delay_us(DELAY_US);
    
    // §¹§Ú§ä§Ñ?§Þ§à §ã§ä§Ñ§ß SDA (0 - §á§â§Ú§ã§ä§â?§Û §Ó?§Õ§á§à§Ó?§Ó §å§ã§á?§ê§ß§à)
    uint8_t ack = GPIO_ReadInputDataBit(i2c->sda_port, i2c->sda_pin);
    
    GPIO_WriteBit(i2c->scl_port, i2c->scl_pin, Bit_RESET);
    delay_us(DELAY_US);

    return (ack == 0); // §±§à§Ó§Ö§â§ä§Ñ?§Þ§à 1, §ñ§Ü§ë§à §à§ä§â§Ú§Þ§Ñ§Ý§Ú ACK
}