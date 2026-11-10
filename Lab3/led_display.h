#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "main.h"

typedef enum {
    LIGHT_OFF,
    LIGHT_RED,
    LIGHT_YELLOW,
    LIGHT_GREEN
} LightColor;

extern int led_buffer[4];

void display7SEG(int num);
void update7SEG(int index);

void setTraffic_Road1(LightColor color);
void setTraffic_Road2(LightColor color);

void toggleLED_Red(void);
void toggleLED_Yellow(void);
void toggleLED_Green(void);
void turnOffAllLEDs(void);

#endif /* INC_LED_DISPLAY_H_ */
