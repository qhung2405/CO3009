#include "software_timer.h"

#define NUM_TIMERS 4
static int timer_counter[NUM_TIMERS] = {0};
static int timer_flag[NUM_TIMERS] = {0};

void setTimer(int index, int duration) {
    if (index >= 0 && index < NUM_TIMERS) {
        timer_counter[index] = duration / TIMER_CYCLE;
        if (duration > 0 && timer_counter[index] == 0) {
            timer_counter[index] = 1;
        }
        timer_flag[index] = 0;
    }
}

int isTimerExpired(int index) {
    if (index >= 0 && index < NUM_TIMERS) {
        if (timer_flag[index] == 1) {
            timer_flag[index] = 0;
            return 1;
        }
    }
    return 0;
}

void timerRun(void) {
    for (int i = 0; i < NUM_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}
