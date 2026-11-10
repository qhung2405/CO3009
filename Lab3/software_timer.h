#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#define TIMER_CYCLE 10

#define TIMER_TRAFFIC   0
#define TIMER_7SEG      1
#define TIMER_BLINK     2
#define TIMER_AUTO_INC  3

void setTimer(int index, int duration);
int isTimerExpired(int index);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
