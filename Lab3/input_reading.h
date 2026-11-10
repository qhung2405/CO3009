#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"

#define NO_OF_BUTTONS 3
#define DURATION_FOR_AUTO_INCREASING 100 // 100 * 10ms = 1s

#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

void button_reading(void);
int is_button_pressed(int index);
int is_button_pressed_1s(int index);

#endif /* INC_INPUT_READING_H_ */
