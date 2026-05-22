#ifndef CONFIG_H
#define CONFIG_H

#define UART_PINS PA9

#define TP_SCL PA14
#define TP_SDA PA13

#define CAPS_LED_PIN PC13

#define OLED_SCL PB6
#define OLED_SDA PB7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define TOUCHPAD_MIN_X 1300
#define TOUCHPAD_MAX_X 5700
#define TOUCHPAD_MIN_Y 1000
#define TOUCHPAD_MAX_Y 4800

// from dts:
// touchpad-resolution-x = <2879>;
// touchpad-resolution-y = <1799>;
#define EXPECTED_RES_X 2879
#define EXPECTED_RES_Y 1799

#define BOARD_RESET NVIC_SystemReset

const uint8_t ROW_PINS[8]  = {PB1, PB10, PB12, PB13, PB15, PA11, PB5, PA15};
const uint8_t COL_PINS[16] = {PB0, PA7, PA6, PA5, PB8, PA3, PB4, PA2, PA0, PB11, PB14, PA12, PB9, PB3, PA8, PA1};

#endif
