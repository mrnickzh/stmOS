#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "platform/STM32F411xE.h"
#include "drivers/driver.h"

typedef struct {
    struct I2C_t *instance;
    struct GPIO_t *scl_port;
    uint8_t scl_pin;
    uint8_t scl_af;
    struct GPIO_t *sda_port;
    uint8_t sda_pin;
    uint8_t sda_af;
    uint32_t speed_hz;
} i2c_config_t;

int i2c_init(const i2c_config_t *cfg);
int i2c_write(uint8_t addr, uint8_t reg, const uint8_t *buf, uint32_t len);

extern const driver_t i2c_driver;

#endif // I2C_H