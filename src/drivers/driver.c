#include "config.h"
#include "drivers/driver.h"
#include "utils/strutil.h"
#include "platform/Cortex-M4.h"

#if CONFIG_BUTTONS
extern const driver_t button_driver;
#endif
#if CONFIG_I2C
extern const driver_t i2c_driver;
#endif
#if CONFIG_OLED
extern const driver_t ssd1306_driver;
#endif

const driver_t *const g_drivers[] = {
#if CONFIG_I2C
    &i2c_driver,
#endif
#if CONFIG_OLED
    &ssd1306_driver,
#endif
#if CONFIG_BUTTONS
    &button_driver,
#endif
    NULL
};

const driver_t *driver_get(const char *name)
{
    for (int i = 0; g_drivers[i]; i++) {
        if (!strcomp(g_drivers[i]->name, name)) {
            return g_drivers[i];
		}
	}
    return NULL;
}