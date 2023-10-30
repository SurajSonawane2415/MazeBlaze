#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <"bar_graph.h">
#define led1 12
#define led2 33
#define led3 25
#define led4 26
#define led5 27
#define led6 14
#define led7 13
#define led8 23
void get_led(uint8_t data,const int *pin_out)
{
for(int i=0;i<8;i++)
{
if ((data & 0x80) == 0x80)
            {
                // set gpio value to 1
                gpio_set_level((gpio_num_t)pin_out[i], 1);
            }
            else
            {
                // set gpio value to 0
                gpio_set_level((gpio_num_t)pin_out[i], 0);
            }
            data= data<<1;
            }
            }
void app_main(){
static const int pin_out[8] = {led1,led2,led3,led4,led5,led6,led7,led8};
uint8_t data;
uint64_t bit_mask = 0;
bit_mask = (1ULL<<led1) | (1ULL<<led2) | (1ULL<<led3) | (1ULL<<led4) | (1ULL<<led5) | (1ULL<<led6) | (1ULL<<led7) | (1ULL<<led8);
gpio_config_t io_config;
io_conf.pin_bit_mask = bit_mask;
io_conf.mode = GPIO_MODE_OUTPUT;
io_conf.pull_up_en =0;
io_conf.pull_down_en = 1;
io_conf.intr_type = GPIO_INTR_DISABLE;

esp_err_t err = gpio_config(&io_conf);
if(err == ESP_OK){
for(int a=0,a<5,a++){
get_led(0xFF,pin_out);
vTaskDelay(1000 / portTICK_RATE_MS);
get_led(0x00,pin_out);
vTaskDelay(1000 / portTICK_RATE_MS);
}
data = 0x01;
while(1)
{
if(data == 0x00){
data = 0x01;
}
get_led(data,pin_out);
data=data<<1;
vTaskDelay(1000 / portTICK_RATE_MS);
}
  else
    {
        ESP_LOGE("bar_graph.h", "error: %s", esp_err_to_name(err));
        return err;
      }
      }
      }
      


