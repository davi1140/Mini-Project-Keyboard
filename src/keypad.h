#ifndef __KEYPAD_H
#define __KEYPAD_H

void init_keypad();
void init_tim7(void);
void TIM7_IRQHandler(void);
void push_queue(int n);
char pop_queue();
void update_history(int c, int rows);
void drive_column(int c);
int read_rows();
char get_key_event(void);
char get_keypress();




#endif
