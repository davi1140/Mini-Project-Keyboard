#include "stm32f0xx.h"
//useful information in lab 6
#include "keypad.h"

uint8_t hist[16];
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;
const char keymap[] = "DCBA#9630852*741";
uint8_t col;

void init_keypad() {
	init_tim7();

	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC -> MODER &= ~0x0000ff00;
	GPIOC -> MODER |=  0x00005500;
	GPIOC -> OTYPER |= 0x000000f0;
	//now set the inputs 0-3
	GPIOC -> MODER &= ~0x000000ff;
	GPIOC -> PUPDR &= ~0x000000ff;
	GPIOC -> PUPDR |=  0x00000055;
}

void init_tim7(void)
{
    //enable the clock and trigger at 1khz
    RCC -> APB1ENR |= 1 << 5;
    TIM7 -> DIER |= 1<<0;
    TIM7 -> PSC = 1000-1;
    TIM7 -> ARR = 48 - 1;
    TIM7 -> CR1 |= 0x00000001;
    NVIC -> ISER[0] |= 1<<18;
}

void TIM7_IRQHandler(void) {
    // Remember to acknowledge the interrupt here!
    TIM7 -> SR &= ~0x00000001;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

void push_queue(int n) {
    queue[qin] = n;
    qin ^= 1;
}

char pop_queue() {
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    // We used to make students do this in assembly language.
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap[4*c+i]);
    }
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

char get_key_event(void) {
    for(;;) {
        asm volatile ("wfi" : :);   // wait for an interrupt
        if (queue[qout] != 0)
            break;
    }
    return pop_queue();
}

char get_keypress() {
    char event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        // ...but ignore if it's a release.
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}


