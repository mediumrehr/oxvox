#ifndef _OXVOX_H_
#define _OXVOX_H_

/* general settings */
#define _50KHZ_ // sample at 50kHz; comment to change sampling to 20kHz
#define _SHOW_DISPLAY_ // comment this line to disable oled display

/* 
 * Description:
 *   Initialize and configure PWM
 * Input: none
 */
static void pwm_init(void);

/* 
 * Description:
 *   Event handler for timer to update PWM value
 * Input:
 *   event_type: timer event type (not used)
 *   p_context: general purpose parameter to pass data (not used)
 */
static void wavetable_lookup_timer_handler(nrf_timer_event_t event_type, void* p_context);

/* 
 * Description:
 *   Initialize and set callback for buttons
 * Input: none
 */
static void init_bsp(void);

/* 
 * Description:
 *   Event handler for button press/release to change note and display screen
 * Input:
 *   evt: event type
 */
static void bsp_evt_handler(bsp_event_t evt);

/* 
 * Description:
 *   Initialize TWI (i2c) communication
 * Input: none
 */
static void twi_init(void);

/* 
 * Description:
 *   Event handler for TWI communication
 * Input:
 *   p_event: structure for a TWI event (not used)
 *   p_context: general purpose parameter to pass data (not used)
 */
static void twi_handler(nrf_drv_twi_evt_t const* p_event, void* p_context);

/* 
 * Description:
 *   Initialize ssd1306 OLED display
 * Input: none
 */
static void init_ssd1306(void);

/* 
 * Description:
 *   Send command to ssd1306
 * Input:
 *   command: command to be sent
 */
static void ssd1306_command(uint8_t command);

/* 
 * Description:
 *   Select screen to show on display
 * Input:
 *   screen: screen number to show
 */
static void display(uint8_t screen);

#endif // _OXVOX_H_
