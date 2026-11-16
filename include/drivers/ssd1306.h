/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include "drivers/i2c.h"

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8
#endif

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01  // Generic error.
} SSD1306_Error_t;

// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SSD1306_t;

typedef struct {
	const i2c_config_t *i2c_cfg;
    uint32_t addr;
    uint32_t width;
    uint32_t height;
} ssd1306_config_t;

int ssd1306_init(const ssd1306_config_t *cfg);

extern const driver_t i2c_driver;

// Procedure definitions
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
uint8_t ssd1306_GetDisplayOn();
void ssd1306_SetDisplayOn(const uint8_t on);
void ssd1306_SetContrast(const uint8_t value);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, uint32_t buff_size);
void ssd1306_Reset(void);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_SetContrast(const uint8_t value);

/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_SetDisplayOn(const uint8_t on);

/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t ssd1306_GetDisplayOn();

// Low-level procedures
void ssd1306_Reset(void);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, uint32_t buff_size);
SSD1306_Error_t ssd1306_FillBuffer(uint8_t* buf, uint32_t len);

#endif // SSD1306_H
