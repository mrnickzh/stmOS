#include "config.h"
#if CONFIG_I2C

#include "drivers/i2c.h"
#include "drivers/driver.h"

static i2c_config_t g_cfg;
static uint8_t g_initialized;

static void i2c_gpio_config(struct GPIO_t *port, uint8_t pin, uint8_t af)
{
    // 1. MODER = AF (10)
	BIT_CLEAR(port->MODER, GPIO_MODER_MASK(pin));
	BIT_SET(port->MODER, GPIO_MODER_AF(pin));

    // 2. OTYPER = open-drain (1) — ОБЯЗАТЕЛЬНО для I2C
	BIT_SET(port->OTYPER, GPIO_OTYPER_OPEN_DRAIN(pin));

    // 3. OSPEEDR = very high (11) — для 400 кГц
	BIT_CLEAR(port->OSPEEDR, GPIO_MODER_MASK(pin));
	BIT_SET(port->OSPEEDR, GPIO_OSPEEDR_VERY_HIGH(pin));

    // 4. PUPDR = pull-up (01) — если нет внешних 4.7 кΩ
	BIT_CLEAR(port->PUPDR, GPIO_MODER_MASK(pin));
	BIT_SET(port->PUPDR, GPIO_PUPDR_PULL_UP(pin));

    // AFRL 0-7, AFRH 8-15
    if (pin < 8) {
		BIT_CLEAR(port->AFRL, GPIO_AF_MASK(pin));
		BIT_SET(port->AFRL, ((uint32_t)af << (pin * 4)));
    } else {
		BIT_CLEAR(port->AFRH, GPIO_AF_MASK(pin));
		BIT_SET(port->AFRH, ((uint32_t)af << ((pin - 8) * 4)));
    }
}

static int i2c_init_impl(const i2c_config_t *cfg)
{
	g_cfg = *cfg;
	
    struct I2C_t *I2Cx = cfg->instance;
	
	BIT_CLEAR(I2Cx->CR1, I2C_CR1_SWRST);
	BIT_SET(I2Cx->CR1, I2C_CR1_SWRST);

    // 1. Clock для GPIO (оба порта — могут быть разные)
    uint32_t scl_port_num = ((uint32_t)cfg->scl_port - GPIOA_BASE) / 0x400;
    uint32_t sda_port_num = ((uint32_t)cfg->sda_port - GPIOA_BASE) / 0x400;
    BIT_SET(RCC->AHB1ENR, 1UL << scl_port_num);
    if (sda_port_num != scl_port_num) {
        BIT_SET(RCC->AHB1ENR, 1UL << sda_port_num);
	}

    // 2. Clock для I2C — switch (надёжно)
    if      (I2Cx == I2C1) BIT_SET(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
    else if (I2Cx == I2C2) BIT_SET(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);
    else if (I2Cx == I2C3) BIT_SET(RCC->APB1ENR, RCC_APB1ENR_I2C3EN);
    else return -1;

    // 3. GPIO config (SCL + SDA)
    i2c_gpio_config(cfg->scl_port, cfg->scl_pin, cfg->scl_af);
    i2c_gpio_config(cfg->sda_port, cfg->sda_pin, cfg->sda_af);

    // 4. I2C: disable перед конфигом
    // I2Cx->CR1 &= ~I2C_CR1_PE;
	BIT_CLEAR(I2Cx->CR1, I2C_CR1_PE);

    // 5. CR2.FREQ = PCLK1 в МГц
    //    У вас APB1 prescaler = 1 → PCLK1 = SYSCLK = 25 МГц
    uint32_t pclk1_mhz = SYSCLK_FREQ / 1000000UL;    // 25
    I2Cx->CR2 = (pclk1_mhz & 0x3F);

    // 6. CCR + TRISE
    if (cfg->speed_hz <= 100000UL) {
        // Standard mode (100 kHz)
        // CCR = PCLK1 / (2 * SCL)
        I2Cx->CCR = SYSCLK_FREQ / (2 * cfg->speed_hz);

        // TRISE = PCLK1_MHz + 1 (макс. rise time 1000 нс)
        I2Cx->TRISE = pclk1_mhz + 1;
    } else {
        // Fast mode (400 kHz), DUTY = 2:1 (бит DUTY = 0)
        // CCR = PCLK1 / (3 * SCL)
        uint32_t ccr = SYSCLK_FREQ / (3 * cfg->speed_hz);
        I2Cx->CCR = I2C_CCR_FS | (ccr & 0xFFF);

        // TRISE = PCLK1_MHz * 0.3 + 1 (макс. rise time 300 нс)
        I2Cx->TRISE = ((pclk1_mhz * 300UL) / 1000UL) + 1;
    }

    // 7. Enable: ACK + PE
    I2Cx->CR1 = I2C_CR1_ACK | I2C_CR1_PE;

    return 0;
}

int i2c_write(uint8_t addr, uint8_t reg, const uint8_t *buf, uint32_t len)
{
    struct I2C_t *I2Cx = g_cfg.instance;

    // start transmit
	BIT_SET(I2Cx->CR1, I2C_CR1_START);
	while (!BIT_CHECK(I2Cx->SR1, I2C_SR1_SB)) {};

    // addr + W (bit 0 = 0)
    I2Cx->DR = (addr << 1) | 0;
	while (!BIT_CHECK(I2Cx->SR1, I2C_SR1_ADDR)) {};

    // clear addr
    (void)I2Cx->SR1;
    (void)I2Cx->SR2;
	
	while (!BIT_CHECK(I2Cx->SR1, I2C_SR1_TXE)) {};
    I2Cx->DR = reg;

    // write
    for (uint32_t i = 0; i < len; i++) {
		while (!BIT_CHECK(I2Cx->SR1, I2C_SR1_TXE)) {};
        I2Cx->DR = buf[i];
    }

    // everything sent?
	while (!BIT_CHECK(I2Cx->SR1, I2C_SR1_BTF)) {};
    I2Cx->CR1 |= I2C_CR1_STOP;

    return 0;
}

int i2c_init(const i2c_config_t *cfg) {
    return i2c_init_impl(cfg);
}

static int i2c_driver_init(const void *cfg) {
    return i2c_init_impl((const i2c_config_t *)cfg);
}

const driver_t i2c_driver = {
    .name = "i2c",
    .init = i2c_driver_init,
};

#endif // CONFIG_I2C