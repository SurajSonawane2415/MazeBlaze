#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "sra_board.h"
#include "tuning_http_server.h"

#define MODE NORMAL_MODE
#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define bound_LSA_LOW 0
#define bound_LSA_HIGH 1000
#define BLACK_BOUNDARY 500
// Boundary value to distinguish between black and white readings

/*
 * weights given to respective line sensor
 */
const int weights[5] = {-5, -3, 1, 3, 5};
int current_value[5];
int previous_value[5];
int pwm = 40;

/*
 * Motor value boundts
 */
int optimum_duty_cycle = 59;
int lower_duty_cycle = 46;
int higher_duty_cycle = 71;
float left_duty_cycle = 0, right_duty_cycle = 0;

/*
 * Line Following PID Variables
 */
float error = 0, prev_error = 0, difference, cumulative_error, correction;

/*
 * Union containing line sensor readings
 */
line_sensor_array line_sensor_readings;

void calculate_correction()
{
    error = error * 10; // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error;
    cumulative_error += error;

    cumulative_error = bound(cumulative_error, -30, 30);

    correction = read_pid_const().kp * error + read_pid_const().ki * cumulative_error + read_pid_const().kd * difference;
    prev_error = error;
}

void calculate_error()
{
    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0;
    float pos = 0;
    int k = 0;

    for (int i = 0; i < 5; i++)
    {
        if (line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY)
        {
            all_black_flag = 0;
        }
        if (line_sensor_readings.adc_reading[i] > 700)
        {
            k = 1;
        }
        if (line_sensor_readings.adc_reading[i] < 700)
        {
            k = 0;
        }
        weighted_sum += (float)(weights[i]) * k;
        sum = sum + k;
    }

    if (sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = (weighted_sum - 1) / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if (all_black_flag == 1) // If all black then we check for previous error to assign current error.
    {
        if (prev_error > 0)
        {
            error = 2.5;
        }
        else
        {
            error = -2.5;
        }
    }
    else
    {
        error = pos;
    }
}
void lsa_readings()
{
    line_sensor_readings = read_line_sensor();
    for (int i = 0; i < 5; i++)
    {
        line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
        line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
        line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
    }

    int j = 0;
    for (j = 0; j < 5; j++)
    {
        current_value[j] = line_sensor_readings.adc_reading[j];
    }
    for (int k = 0; k < 5; k++)
    {
        previous_value[j] = current_value[j];
    }
}

void left_turn()
{
    //  int left = 1;
    // int  flag_1 = 1;
    //  while(left){
    //  if(flag_1){
    //     set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, pwm);
    //     set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, pwm);
    //     vTaskDelay(5000/portTICK_RATE_MS);
    //     }
    /*if(current_value[1]>BLACK_BOUNDARY && current_value[2]>BLACK_BOUNDARY && current_value[3]>BLACK_BOUNDARY){

                set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
                set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
                left = 0;
                flag_1 = 0;

                }*/
    ;
    while (1)
    {
        
        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, pwm);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, pwm);
    }
}

void right_turn()
{
    int right = 1;
    int flag_2 = 1;
    while (right)
    {
        if (flag_2)
        {
            set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, pwm);
            set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, pwm);
        }
        if (current_value[1] > BLACK_BOUNDARY && current_value[2] > BLACK_BOUNDARY && current_value[3] > BLACK_BOUNDARY)
        {

            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            right = 0;
            flag_2 = 0;
        }
    }
}
void U_turn()
{
    bool u_turn = true;
    bool flag_3 = true;
    while (u_turn)
    {
        if (flag_3)
        {
            set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, pwm);
            set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, pwm);
        }
        if (current_value[1] > BLACK_BOUNDARY && current_value[2] > BLACK_BOUNDARY && current_value[3] > BLACK_BOUNDARY)
        {

            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
        }
    }
}
void LFR()
{printf("left turn \n");
    if ((current_value[0] > BLACK_BOUNDARY && current_value[4] > BLACK_BOUNDARY) && (previous_value[0] < BLACK_BOUNDARY || previous_value[4] < BLACK_BOUNDARY || (previous_value[0] < BLACK_BOUNDARY && previous_value[4] < BLACK_BOUNDARY)))
    {
    printf("a \n");
        left_turn();
        // ESP_LOGI("left turn plus node");
    } // detect plus node
    if ((current_value[0] > BLACK_BOUNDARY && current_value[4] < BLACK_BOUNDARY) && (previous_value[0] < BLACK_BOUNDARY || previous_value[4] > BLACK_BOUNDARY || (previous_value[0] < BLACK_BOUNDARY && previous_value[4] < BLACK_BOUNDARY)))
    {
        printf("b \n");
        left_turn();
        // ESP_LOGI("left turn only left turn node");  // detect only left turn
    }
    if ((current_value[0] < BLACK_BOUNDARY && current_value[4] > BLACK_BOUNDARY) && (previous_value[0] > BLACK_BOUNDARY || previous_value[4] < BLACK_BOUNDARY || (previous_value[0] < 500 && previous_value[4] < BLACK_BOUNDARY)))
    {
        printf("c \n");
        right_turn();
        // ESP_LOGI("right turn only right turn node"); // detect only right turn
    }
    if ((current_value[0] < BLACK_BOUNDARY && current_value[1] < BLACK_BOUNDARY && current_value[2] < BLACK_BOUNDARY && current_value[3] < BLACK_BOUNDARY && current_value[4] < BLACK_BOUNDARY) && (previous_value[0] > BLACK_BOUNDARY || previous_value[4] > BLACK_BOUNDARY))
    {
        printf("d \n");
        U_turn();
        // ESP_LOGI("u turn only dead end node");// detects dead end
    }
}

void line_follow_task(void *arg)
{
    ESP_ERROR_CHECK(enable_motor_driver(a, NORMAL_MODE));
    ESP_ERROR_CHECK(enable_line_sensor());
    ESP_ERROR_CHECK(enable_bar_graph());
#ifdef CONFIG_ENABLE_OLED
    // Initialising the OLED
    ESP_ERROR_CHECK(init_oled());
    vTaskDelay(100);

    // Clearing the screen
    lv_obj_clean(lv_scr_act());

#endif

    while (true)
    {
        line_sensor_readings = read_line_sensor();
        for (int i = 0; i < 5; i++)
        {
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
            line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
        }
        lsa_readings();
        LFR();

        calculate_error();
        calculate_correction();

        left_duty_cycle = bound((optimum_duty_cycle + correction), lower_duty_cycle, higher_duty_cycle);
        right_duty_cycle = bound((optimum_duty_cycle - correction), lower_duty_cycle, higher_duty_cycle);

        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, left_duty_cycle);
        set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, right_duty_cycle);

        // ESP_LOGI("debug","left_duty_cycle:  %f    ::  right_duty_cycle :  %f  :: error :  %f  correction  :  %f  \n",left_duty_cycle, right_duty_cycle, error, correction);
        // ESP_LOGI("debug", "KP: %f ::  KI: %f  :: KD: %f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
#ifdef CONFIG_ENABLE_OLED
        // Diplaying kp, ki, kd values on OLED
        if (read_pid_const().val_changed)
        {
            display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
            reset_val_changed_pid_const();
        }
#endif

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, NULL);
    start_tuning_http_server();
}
