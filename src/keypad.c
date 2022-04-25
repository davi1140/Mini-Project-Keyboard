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
	//set GPIOC outputs 4-7
	GPIOC -> MODER &= ~0x0000ff00;
	GPIOC -> MODER |=  0x00005500;
	GPIOC -> OTYPER |= 0x000000f0;
	//set GPIOC inputs 0-3
	GPIOC -> MODER &= ~0x000000ff;
	GPIOC -> PUPDR &= ~0x000000ff;
	GPIOC -> PUPDR |=  0x00000055;
}
*//
void show_keypad(int c) {
    int size = 0;		//Total size of characters being transmitted
    char value = '0';	//Single character transfer 'value'
    const char map;		//Character array
    for (;;size++) {
        value = get_keypress(); //Makes value equal to current button press character (i.e. A,B,C,D,0,1,2,3,etc.)
	    if(value == '#') {
                size--; 	//Subtracts one, to get correct size value for next for loop (Otherwise last value outputted is always garbage)
		for(int count = 0; count <= size; count++) {
		    GPIOC -> ODR = ((c & 7) << 4) | map(count);	//Shows all values on output (Think output is in GPIOC considering that no other port is mentioned)
		}
		count = 0; 	//Resets count, size and value (just in case they are carry overed by software gore)
		size = 0;
		value = '0';
		break;		//Exits for loop
	}
        map[size] = value;  	//Maps all values to constant character array
    }
    return;
}
//*
void init_tim7(void)
{
    //Sets timer 7 to 1 kHz
    RCC -> APB1ENR |= 1 << 5;
    TIM7 -> DIER |= 1<<0;
    TIM7 -> PSC = 1000-1;
    TIM7 -> ARR = 48 - 1;
    TIM7 -> CR1 |= 0x00000001;
    NVIC -> ISER[0] |= 1<<18;
}

void TIM7_IRQHandler(void) {
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
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap[4*c+i]);
    }
	//show_keypad(c);
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
        event = get_key_event();
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}
