#include "led_display.h"

int led_buffer[4] = {0, 0, 0, 0};
static const uint8_t LED7_HEX[10] = {
    0x40, // 0: 0100 0000
    0x79, // 1: 0111 1001
    0x24, // 2: 0010 0100
    0x30, // 3: 0011 0000
    0x19, // 4: 0001 1001
    0x12, // 5: 0001 0010
    0x02, // 6: 0000 0010
    0x78, // 7: 0111 1000
    0x00, // 8: 0000 0000
    0x10  // 9: 0001 0000
};

static uint16_t SEG_PINS[7] = {A_Pin, B_Pin, C_Pin, D_Pin, E_Pin, F_Pin, G_Pin};

void display7SEG(int num) {
    if (num < 0 || num > 9) return;
    uint8_t code = LED7_HEX[num];
    for (int i = 0; i < 7; i++) {
        HAL_GPIO_WritePin(GPIOB, SEG_PINS[i], (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void update7SEG(int index) {
    HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);
    display7SEG(led_buffer[index]);
    switch (index) {
        case 0:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void setTraffic_Road1(LightColor color) {
    HAL_GPIO_WritePin(GPIOA, RED1_Pin | YELLOW1_Pin | GREEN1_Pin, GPIO_PIN_SET);
    switch (color) {
        case LIGHT_RED:
            HAL_GPIO_WritePin(RED1_GPIO_Port, RED1_Pin, GPIO_PIN_RESET);
            break;
        case LIGHT_YELLOW:
            HAL_GPIO_WritePin(YELLOW1_GPIO_Port, YELLOW1_Pin, GPIO_PIN_RESET);
            break;
        case LIGHT_GREEN:
            HAL_GPIO_WritePin(GREEN1_GPIO_Port, GREEN1_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void setTraffic_Road2(LightColor color) {
    HAL_GPIO_WritePin(GPIOA, RED2_Pin | YELLOW2_Pin | GREEN2_Pin, GPIO_PIN_SET);
    switch (color) {
        case LIGHT_RED:
            HAL_GPIO_WritePin(RED2_GPIO_Port, RED2_Pin, GPIO_PIN_RESET);
            break;
        case LIGHT_YELLOW:
            HAL_GPIO_WritePin(YELLOW2_GPIO_Port, YELLOW2_Pin, GPIO_PIN_RESET);
            break;
        case LIGHT_GREEN:
            HAL_GPIO_WritePin(GREEN2_GPIO_Port, GREEN2_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void toggleLED_Red(void) {
    HAL_GPIO_WritePin(GPIOA, YELLOW1_Pin | GREEN1_Pin | YELLOW2_Pin | GREEN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(GPIOA, RED1_Pin | RED2_Pin);
}

void toggleLED_Yellow(void) {
    HAL_GPIO_WritePin(GPIOA, RED1_Pin | GREEN1_Pin | RED2_Pin | GREEN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(GPIOA, YELLOW1_Pin | YELLOW2_Pin);
}

void toggleLED_Green(void) {
    HAL_GPIO_WritePin(GPIOA, RED1_Pin | YELLOW1_Pin | RED2_Pin | YELLOW2_Pin, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(GPIOA, GREEN1_Pin | GREEN2_Pin);
}

void turnOffAllLEDs(void) {
    HAL_GPIO_WritePin(GPIOA, RED1_Pin | YELLOW1_Pin | GREEN1_Pin | RED2_Pin | YELLOW2_Pin | GREEN2_Pin, GPIO_PIN_SET);
}
