#include "config.h"
#include "platform/board.h"
#include "platform/STM32F411xE.h"

#if CONFIG_BUTTONS
#include "drivers/button.h"
const button_config_t g_btn_cfg = {
    .port       = GPIOA,
    .pins       = { 0, 1, 2, 3 },
    .active_low = 1,
};
#endif

#if CONFIG_I2C
#include "drivers/i2c.h"
const i2c_config_t g_i2c1_cfg = {
    .instance   = I2C1,
    .scl_port   = GPIOB,
    .scl_pin    = 6,
    .scl_af     = 4,
    .sda_port   = GPIOB,
    .sda_pin    = 7,
    .sda_af     = 4,
    .speed_hz   = 100000,
};
#endif

#if CONFIG_SSD1306
#include "drivers/ssd1306.h"
const ssd1306_config_t g_ssd1306_cfg = {
    .i2c_cfg    = &g_i2c1_cfg,
    .addr       = 0x3C,
    .width      = 128,
    .height     = 64,
};
#endif

int board_init(void)
{
#if CONFIG_I2C
    if (i2c_init(&g_i2c1_cfg) < 0) return -1;
#endif
#if CONFIG_SSD1306
    if (ssd1306_init(&g_ssd1306_cfg) < 0) return -1;
#endif
#if CONFIG_BUTTONS
    if (button_init(&g_btn_cfg) < 0) return -1;
#endif
    return 0;
}