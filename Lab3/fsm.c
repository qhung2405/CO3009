#include "fsm.h"
#include "software_timer.h"
#include "input_reading.h"
#include "led_display.h"
typedef enum {
    PHASE_RED1_GREEN2,
    PHASE_RED1_YELLOW2,
    PHASE_GREEN1_RED2,
    PHASE_YELLOW1_RED2
} TrafficPhase;

static int current_mode = MODE_1_NORMAL;
static TrafficPhase current_phase = PHASE_RED1_GREEN2;

static int red_time = 5;
static int yellow_time = 2;
static int green_time = 3;
static int temp_time = 1;
static int count_road1 = 5;
static int count_road2 = 3;

static void update_mode1_display_and_lights(void) {
    switch (current_phase) {
        case PHASE_RED1_GREEN2:
            setTraffic_Road1(LIGHT_RED);
            setTraffic_Road2(LIGHT_GREEN);
            break;
        case PHASE_RED1_YELLOW2:
            setTraffic_Road1(LIGHT_RED);
            setTraffic_Road2(LIGHT_YELLOW);
            break;
        case PHASE_GREEN1_RED2:
            setTraffic_Road1(LIGHT_GREEN);
            setTraffic_Road2(LIGHT_RED);
            break;
        case PHASE_YELLOW1_RED2:
            setTraffic_Road1(LIGHT_YELLOW);
            setTraffic_Road2(LIGHT_RED);
            break;
    }
    led_buffer[0] = count_road1 / 10;
    led_buffer[1] = count_road1 % 10;
    led_buffer[2] = count_road2 / 10;
    led_buffer[3] = count_road2 % 10;
}

static void start_mode1(void) {
    current_phase = PHASE_RED1_GREEN2;
    count_road1 = red_time;
    count_road2 = green_time;
    setTimer(TIMER_TRAFFIC, 1000);
    update_mode1_display_and_lights();
}

void fsm_init(void) {
    start_mode1();
    setTimer(TIMER_7SEG, 250);
}

void fsm_run(void) {
    if (is_button_pressed(0)) {
        current_mode++;
        if (current_mode > MODE_4_MOD_GREEN) {
            current_mode = MODE_1_NORMAL;
        }

        switch (current_mode) {
            case MODE_1_NORMAL:
                if (red_time < green_time + yellow_time) {
                    red_time = green_time + yellow_time;
                }
                start_mode1();
                break;

            case MODE_2_MOD_RED:
                turnOffAllLEDs();
                temp_time = red_time;
                setTimer(TIMER_BLINK, 250);
                break;

            case MODE_3_MOD_AMBER:
                turnOffAllLEDs();
                temp_time = yellow_time;
                setTimer(TIMER_BLINK, 250);
                break;

            case MODE_4_MOD_GREEN:
                turnOffAllLEDs();
                temp_time = green_time;
                setTimer(TIMER_BLINK, 250);
                break;
        }
    }

    if (current_mode == MODE_1_NORMAL) {
        if (isTimerExpired(TIMER_TRAFFIC)) {
            count_road1--;
            count_road2--;
            if (count_road2 <= 0 && current_phase == PHASE_RED1_GREEN2) {
                current_phase = PHASE_RED1_YELLOW2;
                count_road2 = yellow_time;
            } else if (count_road2 <= 0 && current_phase == PHASE_RED1_YELLOW2) {
                current_phase = PHASE_GREEN1_RED2;
                count_road1 = green_time;
                count_road2 = red_time;
            } else if (count_road1 <= 0 && current_phase == PHASE_GREEN1_RED2) {
                current_phase = PHASE_YELLOW1_RED2;
                count_road1 = yellow_time;
            } else if (count_road1 <= 0 && current_phase == PHASE_YELLOW1_RED2) {
                current_phase = PHASE_RED1_GREEN2;
                count_road1 = red_time;
                count_road2 = green_time;
            }

            update_mode1_display_and_lights();
            setTimer(TIMER_TRAFFIC, 1000);
        }
    } else {
        led_buffer[0] = 0;
        led_buffer[1] = current_mode;
        led_buffer[2] = temp_time / 10;
        led_buffer[3] = temp_time % 10;

        if (isTimerExpired(TIMER_BLINK)) {
            if (current_mode == MODE_2_MOD_RED) toggleLED_Red();
            else if (current_mode == MODE_3_MOD_AMBER) toggleLED_Yellow();
            else if (current_mode == MODE_4_MOD_GREEN) toggleLED_Green();
            setTimer(TIMER_BLINK, 250);
        }

        if (is_button_pressed(1)) {
            temp_time++;
            if (temp_time > 99) temp_time = 1;
            setTimer(TIMER_AUTO_INC, 500);
        }

        if (is_button_pressed_1s(1)) {
            if (isTimerExpired(TIMER_AUTO_INC)) {
                temp_time++;
                if (temp_time > 99) temp_time = 1;
                setTimer(TIMER_AUTO_INC, 500);
            }
        }

        if (is_button_pressed(2)) {
            if (current_mode == MODE_2_MOD_RED) {
                red_time = temp_time;
            } else if (current_mode == MODE_3_MOD_AMBER) {
                yellow_time = temp_time;
            } else if (current_mode == MODE_4_MOD_GREEN) {
                green_time = temp_time;
            }
        }
    }
}
