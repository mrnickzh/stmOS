#include "config.h"
#if CONFIG_BUTTONS

#include "drivers/button.h"
#include "drivers/driver.h"
#include "scheduler/scheduler.h"

static button_config_t g_cfg;
static uint8_t g_initialized;

static int button_init_impl(const button_config_t *cfg)
{
    uint32_t port_num = ((uint32_t)cfg->port - GPIOA_BASE) / 0x400;
    BIT_SET(RCC->AHB1ENR, (1UL << port_num));
    
    for (int i = 0; i < NUM_BTNS; i++) {
        uint8_t pin = cfg->pins[i];
		BIT_CLEAR(cfg->port->MODER, (3UL << (pin * 2)));
		
		if (cfg->active_low) {
			BIT_CLEAR(cfg->port->PUPDR, (3UL << (pin * 2)));
			BIT_SET(cfg->port->PUPDR, (1UL << (pin * 2)));
		} else {
			BIT_CLEAR(cfg->port->PUPDR, (3UL << (pin * 2)));
			BIT_SET(cfg->port->PUPDR, (2UL << (pin * 2)));
		}
    }
    
    g_cfg = *cfg;
    g_initialized = 1;
    return 0;
}

int button_init(const button_config_t *cfg) {
    return button_init_impl(cfg);
}

uint32_t button_pressed_bitmap(void)
{
    if (!g_initialized) return 0;
    uint32_t raw = g_cfg.port->IDR;
    uint32_t mask = 0;
    for (int i = 0; i < NUM_BTNS; i++) { 
		// mask |= (1u << g_cfg.pins[i]);
		BIT_SET(mask, (1u << g_cfg.pins[i]));
	}
    
    if (g_cfg.active_low) { return (~raw) & mask; }
    else { return raw & mask; }
}

uint32_t is_button_pressed(int idx)
{
	if (!g_initialized) return 0;
	if (idx < 0 || idx >= NUM_BTNS) return 0;
	
	uint32_t raw = g_cfg.port->IDR;
	uint32_t mask = 0;
	BIT_SET(mask, (1u << g_cfg.pins[idx]));
	
    uint32_t pressed = g_cfg.active_low ? (~raw) & mask : raw & mask;
    return pressed ? 1 : 0;
}

static int button_driver_init(const void *cfg) {
    return button_init_impl((const button_config_t *)cfg);
}

const driver_t button_driver = {
    .name = "button",
    .init = button_driver_init,
};

#endif // CONFIG_BUTTONS