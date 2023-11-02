#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "esp_log.h"

//LED Pins
#define LED1 12
#define LED2 33
#define LED3 25
#define LED4 26
#define LED5 27
#define LED6 14
#define LED7 13
#define LED8 23

// Function to set LEDs based on binary data
void set_leds_from_data(uint8_t data, const int *pin_out)
{
    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0x80) // Bit masking to check the most significant bit
        {
            gpio_set_level((gpio_num_t)pin_out[i], 1); // Set the GPIO pin to HIGH
        }
        else
        {
            gpio_set_level((gpio_num_t)pin_out[i], 0); // Set the GPIO pin to LOW
        }
        data = data << 1; // Shift the data to the left to process the next bit
    }
}

void app_main()
{
    uint8_t data;
    static const char *TAG = "led_task_code";
    uint64_t bit_mask = 0;
    static const int pin_out[8] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8};

    // Create a bitmask for the GPIO pins
    bit_mask = (1ULL << LED1) | (1ULL << LED2) | (1ULL << LED3) | (1ULL << LED4) |
               (1ULL << LED5) | (1ULL << LED6) | (1ULL << LED7) | (1ULL << LED8);

    // Configure the GPIO pins
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = bit_mask;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 1;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "GPIO config error: %s", esp_err_to_name(err));
        return;
    }

    // Flash all LEDs on and off for 5 iterations
    for (int j = 0; j < 5; j++)
    {
        data = 0xFF;
        set_leds_from_data(data, pin_out);
        vTaskDelay(pdMS_TO_TICKS(500));

        set_leds_from_data(0x00, pin_out);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    set_leds_from_data(0x00, pin_out);

    data = 0x01;

    while (1)
    {
        if (data == 0x00)
        {
            data = 0x01;
        }
        set_leds_from_data(data, pin_out);
        data = data << 1; // Shift the data to the left for circular LED pattern
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
