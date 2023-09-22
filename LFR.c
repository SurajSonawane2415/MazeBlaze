#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"


float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

TaskHandle_t taskhandle1 = NULL;

// MOTOR A0 ---> Right
// MOTOR A1 ---> Left

/*This method involves tuning kp , ki ,kd physically*/
#define GOOD_DUTY_CYCLE 80
#define MIN_DUTY_CYCLE 50
#define MAX_DUTY_CYCLE 85

#define PWM 80
/*variables which help to decide which turns to take*/

// all booleans
bool ll = false;

void calculate_error()
{

    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0;
    float pos = 0;

    for (int i = 0; i < 5; i++)
    {
        if (lsa_reading[i] > BLACK_PATCH)
        {
            all_black_flag = 0;
        }
        weighted_sum += (float)(weights[i]) * (lsa_reading[i]);
        sum = sum + lsa_reading[i];
    }

    if (sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = weighted_sum / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if (all_black_flag == 1) // If all black then we check for previous error to assign current error.
    {
        if (prev_error > 0)
        {
            error = 3;
        }
        else
        {
            error = -3;
        }
    }
    else
    {
        error = pos;
    }
}
// end of function

void calculate_correction()
{
    error = error * 10;              // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error; // used for calcuating kd
    cumulative_error += error;       // used for calculating ki

    cumulative_error = bound(cumulative_error, -30, 30);               // bounding cumulative_error to avoid the issue of cumulative_error being very large
    correction = kp * error + ki * cumulative_error + kd * difference; // defined in http_server.c

    prev_error = error; // update error
}
// end of function

float bound(float val, float min, float max) // To bound a certain value in range MAX to MIN
{
    if (val > max)
        val = max;
    else if (val < min)
        val = min;
    return val;
}
// end of function

void Leftturn()
{
    printf("L T \n");
    int left = 1;

    while (left)
    {

        get_raw_lsa();
        printf("y\n");
        
        if (lsa_reading[1] == 0)
        {
            ll = true;
        }

        set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);

        if (lsa_reading[1] == 1000 && lsa_reading[2] == 1000 && ll)
        {
            vTaskDelay(40 / portTICK_PERIOD_MS);
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            ll = 0;
            left = 0;
            break;
            printf("E\n");
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void straight()
{
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void LFR()
{
    //lsa_readings();
    get_raw_lsa();
    int left_flag=0;
    /*if ((prev_readings[0]<Transition_Value && line_sensor_readings.adc_reading[0]>Transition_Value) || (prev_readings[1]>Transition_Value && line_sensor_readings.adc_reading[1]<Transition_Value) || (prev_readings[2]>Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value) || (prev_readings[3]>Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value) || (prev_readings[4]<Transition_Value && line_sensor_readings.adc_reading[4]>Transition_Value)) //checking for transitions (BLACK TO WHITE FOR SENSOR 1&5) (WHITE TO BLACK for sensor 2,3,4)*/
    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // LEFT
    {
        //It detects PLUS NODE & Only Left Node
        left_flag=1;
        //Leftturn();         
    }
    
    if (left_flag == 1) // checks left first
        {
            int counter = 0;
            while (lsa_reading[0] == 1000 && lsa_reading[1] == 1000)
            {
                get_raw_lsa();
                vTaskDelay(10 / portTICK_PERIOD_MS);
                counter++;
                printf("%d\n", counter);
                if (counter >= 14 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[1] == 1000)
                {
                  Leftturn();
                }
            }
            
        }
    
    /*if(counter>=15 && line_sensor_readings.adc_reading[0]>Transition_Value)
    {
      Leftturn();
      counter=0;
    }*/
 /*   if (Right_flag)
    { 
      printf("checking is on"); 
      check_only_right();
    }*/

    
    // if(Right_flag)
#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"


float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

TaskHandle_t taskhandle1 = NULL;

// MOTOR A0 ---> Right
// MOTOR A1 ---> Left

/*This method involves tuning kp , ki ,kd physically*/
#define GOOD_DUTY_CYCLE 80
#define MIN_DUTY_CYCLE 50
#define MAX_DUTY_CYCLE 85

#define PWM 80
/*variables which help to decide which turns to take*/

// all booleans
bool ll = false;

void calculate_error()
{

    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0;
    float pos = 0;

    for (int i = 0; i < 5; i++)
    {
        if (lsa_reading[i] > BLACK_PATCH)
        {
            all_black_flag = 0;
        }
        weighted_sum += (float)(weights[i]) * (lsa_reading[i]);
        sum = sum + lsa_reading[i];
    }

    if (sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = weighted_sum / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if (all_black_flag == 1) // If all black then we check for previous error to assign current error.
    {
        if (prev_error > 0)
        {
            error = 3;
        }
        else
        {
            error = -3;
        }
    }
    else
    {
        error = pos;
    }
}
// end of function

void calculate_correction()
{
    error = error * 10;              // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error; // used for calcuating kd
    cumulative_error += error;       // used for calculating ki

    cumulative_error = bound(cumulative_error, -30, 30);               // bounding cumulative_error to avoid the issue of cumulative_error being very large
    correction = kp * error + ki * cumulative_error + kd * difference; // defined in http_server.c

    prev_error = error; // update error
}
// end of function

float bound(float val, float min, float max) // To bound a certain value in range MAX to MIN
{
    if (val > max)
        val = max;
    else if (val < min)
        val = min;
    return val;
}
// end of function

void Leftturn()
{
    printf("L T \n");
    int left = 1;

    while (left)
    {

        get_raw_lsa();
        printf("y\n");
        
        if (lsa_reading[1] == 0)
        {
            ll = true;
        }

        set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);

        if (lsa_reading[1] == 1000 && lsa_reading[2] == 1000 && ll)
        {
            vTaskDelay(40 / portTICK_PERIOD_MS);
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            ll = 0;
            left = 0;
            break;
            printf("E\n");
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void straight()
{
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void LFR()
{
    //lsa_readings();
    get_raw_lsa();
    int left_flag=0;
    /*if ((prev_readings[0]<Transition_Value && line_sensor_readings.adc_reading[0]>Transition_Value) || (prev_readings[1]>Transition_Value && line_sensor_readings.adc_reading[1]<Transition_Value) || (prev_readings[2]>Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value) || (prev_readings[3]>Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value) || (prev_readings[4]<Transition_Value && line_sensor_readings.adc_reading[4]>Transition_Value)) //checking for transitions (BLACK TO WHITE FOR SENSOR 1&5) (WHITE TO BLACK for sensor 2,3,4)*/
    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // LEFT
    {
        //It detects PLUS NODE & Only Left Node
        left_flag=1;
        //Leftturn();         
    }
    
    if (left_flag == 1) // checks left first
        {
            int counter = 0;
            while (lsa_reading[0] == 1000 && lsa_reading[1] == 1000)
            {
                get_raw_lsa();
                vTaskDelay(10 / portTICK_PERIOD_MS);
                counter++;
                printf("%d\n", counter);
                if (counter >= 14 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[1] == 1000)
                {
                  Leftturn();
                }
            }
            
        }
    
    /*if(counter>=15 && line_sensor_readings.adc_reading[0]>Transition_Value)
    {
      Leftturn();
      counter=0;
    }*/
 /*   if (Right_flag)
    { 
      printf("checking is on"); 
      check_only_right();
    }*/

    
    // if(Right_flag)
    // {
    //     if(line_sensor_readings.adc_reading[0]<Transition_Value && line_sensor_readings.adc_reading[1]<Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value && line_sensor_readings.adc_reading[4]<Transition_Value)
    //     {
    //         //IF ONLY RIGHT NODE IS DETECT
    //         Rightturn();
    //         Right_flag= 0;
    //         printf("turn on");

    //     }

    //     if(line_sensor_readings.adc_reading[0]<Transition_Value && line_sensor_readings.adc_reading[1]>Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value && line_sensor_readings.adc_reading[4]<Transition_Value)
    //     {
    //         //IF STRAIGHT + RIGHT NODE DETECT THEN SIMPLY Right_flag becomes zero and bot dont take any turn.
    //         Right_flag=0;
    //         printf("turn off");
        
    //     }
    
    // }

}



void line_follow_task(void *arg)
{

    while (1)
    { 
        LFR();
        straight();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    ESP_ERROR_CHECK(enable_lsa());
    ESP_ERROR_CHECK(enable_motor_driver());

    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, &taskhandle1);
}#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"


float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

TaskHandle_t taskhandle1 = NULL;

// MOTOR A0 ---> Right
// MOTOR A1 ---> Left

/*This method involves tuning kp , ki ,kd physically*/
#define GOOD_DUTY_CYCLE 80
#define MIN_DUTY_CYCLE 50
#define MAX_DUTY_CYCLE 85

#define PWM 80
/*variables which help to decide which turns to take*/

// all booleans
bool ll = false;

void calculate_error()
{

    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0;
    float pos = 0;

    for (int i = 0; i < 5; i++)
    {
        if (lsa_reading[i] > BLACK_PATCH)
        {
            all_black_flag = 0;
        }
        weighted_sum += (float)(weights[i]) * (lsa_reading[i]);
        sum = sum + lsa_reading[i];
    }

    if (sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = weighted_sum / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if (all_black_flag == 1) // If all black then we check for previous error to assign current error.
    {
        if (prev_error > 0)
        {
            error = 3;
        }
        else
        {
            error = -3;
        }
    }
    else
    {
        error = pos;
    }
}
// end of function

void calculate_correction()
{
    error = error * 10;              // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error; // used for calcuating kd
    cumulative_error += error;       // used for calculating ki

    cumulative_error = bound(cumulative_error, -30, 30);               // bounding cumulative_error to avoid the issue of cumulative_error being very large
    correction = kp * error + ki * cumulative_error + kd * difference; // defined in http_server.c

    prev_error = error; // update error
}
// end of function

float bound(float val, float min, float max) // To bound a certain value in range MAX to MIN
{
    if (val > max)
        val = max;
    else if (val < min)
        val = min;
    return val;
}
// end of function

void Leftturn()
{
    printf("L T \n");
    int left = 1;

    while (left)
    {

        get_raw_lsa();
        printf("y\n");
        
        if (lsa_reading[1] == 0)
        {
            ll = true;
        }

        set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);

        if (lsa_reading[1] == 1000 && lsa_reading[2] == 1000 && ll)
        {
            vTaskDelay(40 / portTICK_PERIOD_MS);
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            ll = 0;
            left = 0;
            break;
            printf("E\n");
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void straight()
{
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void LFR()
{
    //lsa_readings();
    get_raw_lsa();
    int left_flag=0;
    /*if ((prev_readings[0]<Transition_Value && line_sensor_readings.adc_reading[0]>Transition_Value) || (prev_readings[1]>Transition_Value && line_sensor_readings.adc_reading[1]<Transition_Value) || (prev_readings[2]>Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value) || (prev_readings[3]>Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value) || (prev_readings[4]<Transition_Value && line_sensor_readings.adc_reading[4]>Transition_Value)) //checking for transitions (BLACK TO WHITE FOR SENSOR 1&5) (WHITE TO BLACK for sensor 2,3,4)*/
    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // LEFT
    {
        //It detects PLUS NODE & Only Left Node
        left_flag=1;
        //Leftturn();         
    }
    
    if (left_flag == 1) // checks left first
        {
            int counter = 0;
            while (lsa_reading[0] == 1000 && lsa_reading[1] == 1000)
            {
                get_raw_lsa();
                vTaskDelay(10 / portTICK_PERIOD_MS);
                counter++;
                printf("%d\n", counter);
                if (counter >= 14 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[1] == 1000)
                {
                  Leftturn();
                }
            }
            
        }
    
    /*if(counter>=15 && line_sensor_readings.adc_reading[0]>Transition_Value)
    {
      Leftturn();
      counter=0;
    }*/
 /*   if (Right_flag)
    { 
      printf("checking is on"); 
      check_only_right();
    }*/

    
    // if(Right_flag)
    // {
    //     if(line_sensor_readings.adc_reading[0]<Transition_Value && line_sensor_readings.adc_reading[1]<Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value && line_sensor_readings.adc_reading[4]<Transition_Value)
    //     {
    //         //IF ONLY RIGHT NODE IS DETECT
    //         Rightturn();
    //         Right_flag= 0;
    //         printf("turn on");

    //     }

    //     if(line_sensor_readings.adc_reading[0]<Transition_Value && line_sensor_readings.adc_reading[1]>Transition_Value && line_sensor_readings.adc_reading[2]<Transition_Value && line_sensor_readings.adc_reading[3]<Transition_Value && line_sensor_readings.adc_reading[4]<Transition_Value)
    //     {
    //         //IF STRAIGHT + RIGHT NODE DETECT THEN SIMPLY Right_flag becomes zero and bot dont take any turn.
    //         Right_flag=0;
    //         printf("turn off");
        
    //     }
    
    // }

}



void line_follow_task(void *arg)
{

    while (1)
    { 
        LFR();
        straight();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    ESP_ERROR_CHECK(enable_lsa());
    ESP_ERROR_CHECK(enable_motor_driver());

    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, &taskhandle1);
}
