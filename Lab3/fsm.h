#ifndef INC_FSM_H_
#define INC_FSM_H_

#define MODE_1_NORMAL     1
#define MODE_2_MOD_RED    2
#define MODE_3_MOD_AMBER  3
#define MODE_4_MOD_GREEN  4

void fsm_init(void);
void fsm_run(void);

#endif /* INC_FSM_H_ */
