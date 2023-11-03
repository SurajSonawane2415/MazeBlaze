# Firmware task
In this task, we write an ESP-IDF code to blink an LED without using the "sra_board.h" header file, which is used in the ["Wall-E 1_led_blink"](https://github.com/SRA-VJTI/Wall-E/tree/master/1_led_blink) code.

## Terms used
### GPIO Configuration
GPIO (General Purpose Input/Output) pins on microcontrollers are often configured to either input or output mode. When a pin is configured as an output, it can be used to control external devices like LEDs, motors, or sensors. Configuration typically involves specifying the pin, its mode, and any additional settings like pull-up or pull-down resistors.
**Code Snippet for GPIO Configuration**:
```c
// Configuration for GPIO pin 12 as an output with a pull-down resistor
gpio_config_t io_conf;
io_conf.pin_bit_mask = (1ULL << GPIO_NUM_12);  // Pin bitmask
io_conf.mode = GPIO_MODE_OUTPUT;  // Set the pin as an output
io_conf.pull_up_en = 0;  // Disable pull-up resistor
io_conf.pull_down_en = 1;  // Enable pull-down resistor
io_conf.intr_type = GPIO_INTR_DISABLE;  // Disable interrupt handling

esp_err_t err = gpio_config(&io_conf);
if (err != ESP_OK) {
   // Handle configuration error
}
```
In this snippet, `gpio_config_t` is a structure used to configure the GPIO pins. The `io_conf` structure specifies the pin bitmask, mode (output in this case), pull-up and pull-down settings, and interrupt handling. If the configuration fails, an error can be handled accordingly.

### Bit Masking
Bit masking is a technique used to manipulate and extract specific bits within a binary number. It's often used in microcontroller programming to set or clear individual bits in a register, which can control various hardware components.
For example, bit masking is use to set a specific bit to 1 while keeping all other bits unchanged. In C, the | (OR) operator for setting bits and the & (AND) operator to clear bits.
**Code Snippet for Bit Masking**:
```c
// Setting bit 2 to 1, assuming a 3-bit binary number
uint8_t data = 0b00000100;  // Binary representation
uint8_t bitmask = 0b00000100;  // Mask with a 1 in the desired bit
data |= bitmask;  // Setting the bit to 1

// Clearing bit 3 to 0
uint8_t data2 = 0b00001000;
uint8_t bitmask2 = 0b11110111;  // Mask with 0 in the desired bit
data2 &= bitmask2;  // Clearing the bit to 0
```
These snippets demonstrate bit masking for manipulating individual bits in binary data and configuring a GPIO pin for output with specific settings.

## Output:
![task (1)](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/6bc06711-1db2-4ed9-9822-a266b4067918)

## Refernces
- [SRA Embedded system study group](https://sravjti.in/embedded-systems-study-group/week5/week5.html) refernce for bit masking.
- [ESP-32 GPIO configuration guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html) GPIO configuration refernce.
