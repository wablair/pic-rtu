#ifndef _lcd_h_
#define _lcd_h_

#define COMPILE_LCD_FUNCTIONS

#define LCD_REMEMBER_COMMAND				0x8D
#define LCD_READ_STATUS_BYTE				0xD0
#define LCD_CONFIG_INTERRUPT_PIN_AS_OPEN_COLLECTOR	0x2A
#define LCD_CONFIG_INTERRUPT_PIN_AS_PUSH_PULL_OUTPUT	0x22
#define LCD_SET_I2C_BASE_ADDRESS			0x91
#define LCD_CONFIG_KEYPAD_AND_IO			0xE0
#define LCD_CONFIG_GPIO1				0xE3
#define LCD_CONFIG_GPIO2				0xE4
#define LCD_CONFIG_GPIO3				0xE5
#define LCD_WRITE_STRING				0x80
#define LCD_SET_STARTUP_SCREEN				0x90
#define LCD_BACKLIGHT_ON				0x28
#define LCD_BACKLIGHT_OFF				0x20
#define LCD_SET_BRIGHTNESS				0x81
#define LCD_SET_MAX_BRIGHTNESS				0xA3
#define LCD_CURSOR_DIR_FORWARD				0x09
#define LCD_CURSOR_DIR_BACKWARD				0x01
#define LCD_CURSOR_BLINK_ON				0x18
#define LCD_CURSOR_BLINK_OFF				0x10
#define LCD_CURSOR_UNDERLINE_ON				0x19
#define LCD_CURSOR_UNDRELINE_OFF			0x12
#define LCD_DISPLAY_ON					0x1A
#define LCD_DISPLAY_OFF					0x12
#define LCD_DISPLAY_CONTRAST				0x82
#define LCD_MOVE_CURSOR_RIGHT				0x83
#define LCD_MOVE_CURSOR_LEFT				0x84
#define LCD_SHIFT_DISPLAY_RIGHT				0x85
#define LCD_SHIFT_DISPLAY_LEFT				0x86
#define LCD_SHIFT_DISPLAY_UP				0x87
#define LCD_SHIFT_DISPLAY_DOWN				0x88
#define LCD_SET_CURSOR_ADDRESS				0x89
#define LCD_SET_CURSOR_POSITION				0x8A
#define LCD_CLEAR_DISPLAY				0x8C
#define LCD_DEFINE_CUSTOM_CHAR				0x92
#define LCD_LOAD_CUSTOM_CHAR_SET			0x8E
#define LCD_DRAW_VERTICAL_BAR_GRAPH			0x93
#define LCD_DRAW_TALLVERT_BAR_GRAPH			0x94
#define LCD_WRITE_NUMBER_STRING				0x8F
#define LCD_SET_KEYPAD_DEBOUNCE				0xE1
#define LCD_SET_KEYPAD_REPEAT_DELAY			0xA0
#define LCD_SET_KEYPAD_REPEAT_RATE			0xA1
#define LCD_SET_KEYPAD_BUZZ_PERIOD			0xA2
#define LCD_READ_KEYPAD_DATA				0xD1
#define LCD_SET_OUT1_AND_OUT2				0xE2
#define LCD_SET_OUT1_ON					0x38

#define LCD_ABE_LOGO_Q1	{0x1f, 0x1f, 0x1f, 0x1e, 0x18, 0x10, 0x0, 0x0}
#define LCD_ABE_LOGO_Q2	{0x1f, 0x1f, 0x1f, 0x7, 0x1b, 0x1d, 0x1d, 0x1e}
#define LCD_ABE_LOGO_Q3 {0x0, 0x0, 0x1, 0x3, 0xf, 0x1f, 0x1f, 0x1f}
#define LCD_ABE_LOGO_Q4 {0xf, 0x17, 0x17, 0x1b, 0x1c, 0x1f, 0x1f, 0x1f}

/* 20 characters plus null terminator */ 
#define LCD_LINE_ONE {7, 4, 1, 7, 7, 7, 7, 7, 7, 'A', 'x', 'i', 'o', 'm', '-', \
    'B', 'l', 'a', 'i', 'r', 0}
#define LCD_LINE_TWO {7, 3, 2, 7, 7, 7, 7, 7, 7, 7, 7, 'P', 'I', 'C', '-', \
    'R', 'T', 'U', 7, 7, 0}


int	 lcd_init(void);
int	 lcd_print(unsigned char *);
int	 lcd_putch(unsigned char);
int	 lcd_putint(unsigned int);
int	 lcd_set(unsigned int);
void	 lcd_set_startup(void);
void	 lcd_clear(void);
void	 i2c_PutByte_Remember(unsigned char);
void	 lcd_puthex(unsigned char);

#endif /* _lcd_h_ */
