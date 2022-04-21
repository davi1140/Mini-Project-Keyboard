#ifndef __DISPLAY_H
#define __DISPLAY_H

void nano_wait(unsigned int n);
void spi_cmd(unsigned int data);
void spi_data(unsigned int data);
void spi1_init_oled();
void spi1_display1(const char *string);
void spi1_display2(const char *string);
void init_spi1();




#endif
