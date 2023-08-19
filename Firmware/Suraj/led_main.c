#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
 
#define LED1 12
#define LED2 33
#define LED3 25
#define LED4 26
#define LED5 27
#define LED6 14
#define LED7 13
#define LED8 23
 
 
void app_main()
{
	//GPIO CONFIGURATION AND BITMASKING 
 
	bit_mask = (1ULL << LED1) | (1ULL << LED2) | (1ULL << LED5) | (1ULL << LED6) | (1ULL << LED7) | (1ULL << LED8);
 
	gpio_config_t io_conf;
	// bit mask for the pins, each bit maps to a GPIO
	io_conf.pin_bit_mask = bit_mask;
	// set gpio mode to input
	io_conf.mode = GPIO_MODE_OUTPUT;
	// enable pull up resistors
	io_conf.pull_up_en = 0;
	// disable pull down resistors
	io_conf.pull_down_en = 1;
	// disable gpio interrupts
	io_conf.intr_type = GPIO_INTR_DISABLE;
 
}
