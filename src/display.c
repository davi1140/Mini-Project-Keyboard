#include "stm32f0xx.h"
//useful information in lab 8
#include "display.h"

void init_spi1() {
    // PA5  SPI1_SCK AF-0
    // PA6  SPI1_MISO AF-0
    // PA7  SPI1_MOSI AF-0
    // PA15 SPI1_NSS  AF-0
	RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1 -> CR1 &= ~SPI_CR1_SPE;
	//This should set the baud rate high
	//data size 16 bits
	SPI1 -> CR1 |= SPI_CR1_MSTR | 0x0038;
	SPI1 -> CR2  = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_TXDMAEN | 0x0900;
	SPI1 -> CR1 |= SPI_CR1_SPE;

	//Do the pins
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER |= 0b10 << (2*5) | 0b10 << (2*6) | 0b10 << (2*7) | 0b10 << (2*15);


}

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void spi_cmd(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    SPI1->DR = data;
}

void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
}

void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}

void spi1_display1(const char *string) {
    spi_cmd(0x02);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}

void spi1_display2(const char *string) {
    spi_cmd(0xc0);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}

//This is the code you would use to get something to display
//void display_message() {
//	init_spi1();
//	spi1_init_oled();
//	spi1_display1("Poggers my");
//	spi1_display2("Child");
//}


