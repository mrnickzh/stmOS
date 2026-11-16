#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include "platform/STM32F411xE.h"
#include "drivers/driver.h"

#define BTN_LEFT      0
#define BTN_RIGHT     1
#define BTN_DOWN      2
#define BTN_OK        3

#define NUM_BTNS    4

typedef struct {
    struct GPIO_t *port;
    uint8_t pins[NUM_BTNS];
    uint8_t active_low;
} button_config_t;

int button_init(const button_config_t *cfg);
uint32_t button_pressed_bitmap(void);
uint32_t is_button_pressed(int idx);

extern const driver_t button_driver;

#endif // BUTTON_H