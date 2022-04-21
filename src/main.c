#include "stm32f0xx.h"
#include "display.h"
#include "keypad.h"

void display_message(char key) {
	init_spi1();
	spi1_init_oled();
	char* print;
	switch (key) {
	case ('1'):
			print = "Mode:";
			break;
	case ('2'):
			print = "Mode:";
			break;
	}
	spi1_display1(print);
	spi1_display2("Bruh");
}

int main(void)
{
	init_keypad();
	while(1 == 1) {
		char key = get_key_event();
		display_message(key);
	}
	for(;;);
}
