/**
 * 
 * 
 */
#ifndef PCA10040_OXVOX_H
#define PCA10040_OXVOX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"
	
/* i2c definitions */
#define TWI_INSTANCE_ID 0
#define SSD1306_ADDR    (0x78U >> 1) // aka 0x3C
#define OXVOX_SCL_PIN   25 // SCL signal pin
#define OXVOX_SDA_PIN   24 // SDA signal pin

/* oled display definitions */
// ssd1306 128x64
#define SSD1306_LCDWIDTH            128U
#define SSD1306_LCDHEIGHT           64U

#define SSD1306_SETCONTRAST         0x81U
#define SSD1306_DISPLAYALLON_RESUME 0xA4U
#define SSD1306_DISPLAYALLON        0xA5U
#define SSD1306_NORMALDISPLAY       0xA6U
#define SSD1306_INVERTDISPLAY       0xA7U
#define SSD1306_DISPLAYOFF          0xAEU
#define SSD1306_DISPLAYON           0xAFU

#define SSD1306_SETDISPLAYOFFSET    0xD3U
#define SSD1306_SETCOMPINS          0xDAU

#define SSD1306_SETVCOMDETECT       0xDBU

#define SSD1306_SETDISPLAYCLOCKDIV  0xD5U
#define SSD1306_SETPRECHARGE        0xD9U

#define SSD1306_SETMULTIPLEX        0xA8U

#define SSD1306_SETLOWCOLUMN        0x00U
#define SSD1306_SETHIGHCOLUMN       0x10U

#define SSD1306_SETSTARTLINE        0x40U

#define SSD1306_MEMORYMODE          0x20U
#define SSD1306_COLUMNADDR          0x21U
#define SSD1306_PAGEADDR            0x22U

#define SSD1306_COMSCANINC          0xC0U
#define SSD1306_COMSCANDEC          0xC8U

#define SSD1306_SEGREMAP            0xA0U

#define SSD1306_CHARGEPUMP          0x8DU

#define SSD1306_EXTERNALVCC         0x1U
#define SSD1306_SWITCHCAPVCC        0x2U

// scrolling defines
#define SSD1306_ACTIVATE_SCROLL                         0x2FU
#define SSD1306_DEACTIVATE_SCROLL                       0x2EU
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3U
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26U
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27U
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29U
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2AU

/* button definitions */
#define BUTTONS_NUMBER  8

// BSides buttons config
#define BUTTON_1            5
#define BUTTON_2            2
#define BUTTON_3            4
#define BUTTON_4            3
#define BUTTON_5            7
#define BUTTON_6            13
#define BUTTON_7            11
#define BUTTON_8            12
#define AUDIO_OUT_PIN       8
#define AUDIO_ENABLE_PIN    6
/*
// DEF CON buttons config
#define BUTTON_1            2
#define BUTTON_2            6
#define BUTTON_3            5
#define BUTTON_4            4
#define BUTTON_5            13
#define BUTTON_6            7
#define BUTTON_7            12
#define BUTTON_8            11
#define AUDIO_OUT_PIN       8
#define AUDIO_ENABLE_PIN    3
*/
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8 }

#define BSP_BUTTON_0    BUTTON_1
#define BSP_BUTTON_1    BUTTON_2
#define BSP_BUTTON_2    BUTTON_3
#define BSP_BUTTON_3    BUTTON_4
#define BSP_BUTTON_4    BUTTON_5
#define BSP_BUTTON_5    BUTTON_6
#define BSP_BUTTON_6    BUTTON_7
#define BSP_BUTTON_7    BUTTON_8

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#ifdef __cplusplus
}
#endif

#endif // PCA10040_OXVOX_H
