/*
 * OX VOX v1.0
 * Rob Rehr (@mediumrehr)
 * March 28, 2018
 */

#include <stdio.h>
#include <string.h>
#include "nrf_drv_pwm.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_delay.h"
#include "oxvox.h"
#include "tables.h"
#include "screens.h"

/* PWM instance */
static nrf_drv_pwm_t m_pwm = NRF_DRV_PWM_INSTANCE(0);
/* Timer instance */
const nrf_drv_timer_t WAVETABLE_LOOKUP_TIMER = NRF_DRV_TIMER_INSTANCE(1);
/* TWI instance */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* note to play */
// 1:C (261.6 Hz)
// 2:D (293.7 Hz)
// 3:E (329.6 Hz)
// 4:F (349.2 Hz)
// 5:G (392.0 Hz)
// 6:A (440.0 Hz)
// 7:B (493.4 Hz)
// 8:C (523.3 Hz)
static volatile uint8_t note = 0; // start silent
#ifdef _50KHZ_
    const uint8_t note_array_size[8] = {191, 170, 152, 143, 128, 114, 101, 96}; // 50kHz
#else
    const uint8_t note_array_size[8] = {76, 68, 60, 56, 52, 44, 40, 36}; // 20kHz
#endif

/* indicates if operation on TWI has ended */
static volatile bool m_xfer_done = false;

static volatile uint16_t tableIndex = 0;

/* set up PWM */
static nrf_pwm_values_individual_t m_pwm_seq_values;
static nrf_pwm_sequence_t const    m_pwm_seq = {
    .values.p_individual = &m_pwm_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_pwm_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static void pwm_init(void) {
    nrf_drv_pwm_config_t const m_pwm_config = {
        .output_pins = {
            AUDIO_OUT_PIN | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            NRF_DRV_PWM_PIN_NOT_USED, // channel 1
            NRF_DRV_PWM_PIN_NOT_USED, // channel 2
            NRF_DRV_PWM_PIN_NOT_USED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 255,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm, &m_pwm_config, NULL));

    m_pwm_seq_values.channel_0 = 0;
    m_pwm_seq_values.channel_1 = 0;
    m_pwm_seq_values.channel_2 = 0;
    m_pwm_seq_values.channel_3 = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm, &m_pwm_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

static void wavetable_lookup_timer_handler(nrf_timer_event_t event_type, void* p_context) {
    uint8_t channel = 0; // only using the first pwm channel
    uint16_t * p_channels = (uint16_t *)&m_pwm_seq_values;
    
    if (note == 0) {
        return;
    }
    
    if (tableIndex < note_array_size[note-1]) {
        tableIndex++;
    } else {
        tableIndex = 0;
    }
    
    // set the duty cycle for pwm from wavetable 
    switch(note) {
        case 1: // C
            #ifdef _50KHZ_
                p_channels[channel] = SinTableC_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 2: // D
            #ifdef _50KHZ_
                p_channels[channel] = SinTableD_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 3: // E
            #ifdef _50KHZ_
                p_channels[channel] = SinTableE_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 4: // F
            #ifdef _50KHZ_
                p_channels[channel] = SinTableF_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 5: // G
            #ifdef _50KHZ_
                p_channels[channel] = SinTableG_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 6: // A
            #ifdef _50KHZ_
                p_channels[channel] = SinTableA_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 7: // B
            #ifdef _50KHZ_
                p_channels[channel] = SinTableB_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        case 8: // C
            #ifdef _50KHZ_
                p_channels[channel] = SinTableC8_50k[tableIndex];
            #else
                p_channels[channel] = SinTableC8_20k[tableIndex];
            #endif
            break;
        
        default:
            break;
    }
}

static void init_bsp(void) {
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);

    APP_ERROR_CHECK(app_timer_init());
        APP_ERROR_CHECK(bsp_init(BSP_INIT_BUTTONS, bsp_evt_handler));
    APP_ERROR_CHECK(bsp_buttons_enable());
}

static void bsp_evt_handler(bsp_event_t evt) {
    switch (evt) {
        // Button 1 - play note 1
        case BSP_EVENT_KEY_0:
            if (bsp_button_is_pressed(0)) {
                display(1); // show C screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 1;
            } else {
                if (note == 1) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 2 - play note 2
        case BSP_EVENT_KEY_1:
            if (bsp_button_is_pressed(1)) {
                display(2); // show D screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 2;
            } else {
                if (note == 2) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                        
        // Button 3 - play note 3
        case BSP_EVENT_KEY_2:
            if (bsp_button_is_pressed(2)) {
                display(3); // show E screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 3;
            } else {
                if (note == 3) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 4 - play note 4
        case BSP_EVENT_KEY_3:
            if (bsp_button_is_pressed(3)) {
                display(4); // show F screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 4;
            } else {
                if (note == 4) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 5 - play note 5
        case BSP_EVENT_KEY_4:
            if (bsp_button_is_pressed(4)) {
                display(5); // show G screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 5;
            } else {
                if (note == 5) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 6 - play note 6
        case BSP_EVENT_KEY_5:
            if (bsp_button_is_pressed(5)) {
                display(AUDIO_ENABLE_PIN); // show A screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 6;
            } else {
                if (note == 6) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 7 - play note 7
        case BSP_EVENT_KEY_6:
            if (bsp_button_is_pressed(AUDIO_ENABLE_PIN)) {
                display(7); // show B screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 7;
            } else {
                if (note == 7) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;
                
        // Button 8 - play note 8
        case BSP_EVENT_KEY_7:
            if (bsp_button_is_pressed(7)) {
                display(1); // show C screen
                nrf_gpio_pin_clear(AUDIO_ENABLE_PIN); // enable audio out
                note = 8;
            } else {
                if (note == 8) {
                    display(0); // show main screen
                    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
                    note = 0;
                }
            }
            break;

        default:
            display(0); // show main screen
            nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
            note = 0;
            break;
    }
}

static void twi_init(void) {
    const nrf_drv_twi_config_t twi_config = {
       .scl                = OXVOX_SCL_PIN,
       .sda                = OXVOX_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL));

    nrf_drv_twi_enable(&m_twi);
}

static void twi_handler(nrf_drv_twi_evt_t const* p_event, void* p_context) {
    switch (p_event->type) {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX) {
//                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}

static void init_ssd1306(void) {   
    // Init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80U);                                 // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(SSD1306_LCDHEIGHT - 1);                 // 0x3F (64)

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0U);                                  // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    
    ssd1306_command(0x14U);                                 // we'll generate the high voltage from the 3.3v line internally! (neat!)
    
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00U);                                 // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1U);
    ssd1306_command(SSD1306_COMSCANDEC);
  
    // for 128x64
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12U);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0xCFU);
    
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    
    ssd1306_command(0xF1U);                                 // we'll generate the high voltage from the 3.3v line internally! (neat!)
    
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40U);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}

static void ssd1306_command(uint8_t command) {
    m_xfer_done = false;
    
    uint8_t control = 0x00U;   // Co = 0, D/C = 0
    uint8_t reg[2] = {control, command};
    APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, reg, sizeof(reg), false));

    while (m_xfer_done == false);
}

static void display(uint8_t screen) {
    #ifdef _SHOW_DISPLAY_
        ssd1306_command(SSD1306_COLUMNADDR);
        ssd1306_command(0); // Column start address (0 = reset)
        ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

        ssd1306_command(SSD1306_PAGEADDR);
        ssd1306_command(0); // Page start address (0 = reset)
        ssd1306_command(7); // Page end address
        
        for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
            m_xfer_done = false;
            
            // send a bunch of data in one transmission
            uint8_t regVal1 = 0x40;
            uint8_t reg[17] = {regVal1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            for (uint8_t x=0; x<16; x++) {
                if (screen == 0) { // OX VOX screen
                    reg[x+1] = buffer[i];
                } else if (screen == 1) { // C screen
                    reg[x+1] = c_buffer[i];
                } else if (screen == 2) { // D screen
                    reg[x+1] = d_buffer[i];
                } else if (screen == 3) { // E screen
                    reg[x+1] = e_buffer[i];
                } else if (screen == 4) { // F screen
                    reg[x+1] = f_buffer[i];
                } else if (screen == 5) { // G screen
                    reg[x+1] = g_buffer[i];
                } else if (screen == 6) { // A screen
                    reg[x+1] = a_buffer[i];
                } else if (screen == 7) { // B screen
                    reg[x+1] = b_buffer[i];
                }
                i++;
            }
            i--;
            
            APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, reg, 17, false));
            while (m_xfer_done == false);
        }
    #endif
}

int main(void) {    
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

    init_bsp();
    
    // audio output pin
    nrf_gpio_cfg_output(AUDIO_OUT_PIN);
    nrf_gpio_pin_clear(AUDIO_OUT_PIN); // start with low
    
    // amlifier enable pin
    nrf_gpio_cfg_output(AUDIO_ENABLE_PIN);
    nrf_gpio_pin_set(AUDIO_ENABLE_PIN); // disable audio out
    
    //Configure WAVETABLE_LOOKUP_TIMER for updating pwm value at sampling rate
    nrf_drv_timer_config_t wavetable_lookup_timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&WAVETABLE_LOOKUP_TIMER, &wavetable_lookup_timer_cfg, wavetable_lookup_timer_handler);
    APP_ERROR_CHECK(err_code);

    // set time(in miliseconds) between consecutive compare events
    #ifdef _50KHZ_
        uint32_t time_us = 20; // 50kHz
    #else
        uint32_t time_us = 50; // 20kHz
    #endif

    time_ticks = nrf_drv_timer_us_to_ticks(&WAVETABLE_LOOKUP_TIMER, time_us);
    nrf_drv_timer_extended_compare(&WAVETABLE_LOOKUP_TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    nrf_drv_timer_enable(&WAVETABLE_LOOKUP_TIMER);

    #ifdef _SHOW_DISPLAY_
        twi_init();
        init_ssd1306();
        display(0);
    #endif

    pwm_init();

    for (;;) {  
        // Wait for an event.
        do {
            __WFE();
        } while (m_xfer_done == false);

        // Clear the event register.
        __SEV();
        __WFE();
    }
}
