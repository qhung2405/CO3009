#include "input_reading.h"

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

static uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0};
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0};
static uint8_t button_flag[NO_OF_BUTTONS] = {0};

static GPIO_TypeDef* BUTTON_PORTS[NO_OF_BUTTONS] = {BTN1_GPIO_Port, BTN2_GPIO_Port, BTN3_GPIO_Port};
static uint16_t BUTTON_PINS[NO_OF_BUTTONS] = {BTN1_Pin, BTN2_Pin, BTN3_Pin};

void button_reading(void) {
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(BUTTON_PORTS[i], BUTTON_PINS[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            if (buttonBuffer[i] != debounceButtonBuffer1[i]) {
                buttonBuffer[i] = debounceButtonBuffer1[i];
                if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                    button_flag[i] = 1;
                }
            }

            if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    flagForButtonPress1s[i] = 1;
                }
            } else {
                counterForButtonPress1s[i] = 0;
                flagForButtonPress1s[i] = 0;
            }
        }
    }
}

int is_button_pressed(int index) {
    if (index >= NO_OF_BUTTONS || index < 0) return 0;
    if (button_flag[index] == 1) {
        button_flag[index] = 0;
        return 1;
    }
    return 0;
}

int is_button_pressed_1s(int index) {
    if (index >= NO_OF_BUTTONS || index < 0) return 0;
    return (flagForButtonPress1s[index] == 1);
}
