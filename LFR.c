#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"

float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

TaskHandle_t taskhandle1 = NULL;

/*This method involves tuning kp , ki ,kd physically*/
#define GOOD_DUTY_CYCLE 89
#define MIN_DUTY_CYCLE 59
#define MAX_DUTY_CYCLE 94

#define PWM 80
bool rotation1 = false;
bool rotation2 = false;
int left_check = 0;
int right_check = 0;
int left_turn = 0;
int right_turn = 0;
int left_tune = 0;
int only_right = 0;
int uturn_tune = 0;
int right_tune = 0;

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
            // ssvTaskDelay(40 / portTICK_PERIOD_MS);
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            left_tune = 0;
            ll = 0;
            left = 0;
            left_check = 0;
            right_check = 0;
            left_turn = 0;
            right_turn = 0;
            printf("E\n");
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Rightturn()
{
    printf("R T \n");
    int right = 1;

    while (right)
    {
        get_raw_lsa();

        if (lsa_reading[3] == 0)
        {
            rr = true;
        }

        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, PWM);
        printf("CONTINUE R T \n");
        if (lsa_reading[2] == 1000 && lsa_reading[3] == 1000 && rotation2)
        {
            printf("STOP R T \n");
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            rotation2 = 0;
            rotation1=0;
            only_right = 0;
            right_tune = 0;
            left_check = 0;
            right_check = 0;
            left_turn = 0;
            right_turn = 0;
            right = 0;
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Uturn()
{
    while (lsa_reading[2] == 0)
    {
        get_raw_lsa();

        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, PWM);
        printf("U T \n");
        if (lsa_reading[2] == 1000)
        {
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            printf("U T STOP \n");
            rotation1 = 0;
            rr=0;
            only_right = 0;
            uturn_tune=0;

            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void straight()
{
    get_raw_lsa();
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);

    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void straight_tuning()
{
    get_raw_lsa();
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);

    vTaskDelay(80 / portTICK_PERIOD_MS);
}

void errordelay()
{
    get_raw_lsa();
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);

    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void check_only_right()
{
    while (lsa_reading[4] == 1000 && lsa_reading[3] == 1000)
    {
        get_raw_lsa();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelay(30 / portTICK_PERIOD_MS);
    get_raw_lsa();

    if ((lsa_reading[0] == 0 && lsa_reading[1] == 0 && lsa_reading[3] == 0 && lsa_reading[2] == 0 && lsa_reading[4] == 0))
    {
        only_right = true;
    }
    else if (lsa_reading[2] == 1000 && (lsa_reading[1] == 1000 || lsa_reading[3] == 1000))
    {
        only_right = false;
    }
}

void LFR()
{
    // lsa_readings();
    get_raw_lsa();
    left_check = 0;
    right_check = 0;
    left_turn = 0;
    right_turn = 0;
    only_right = 0;
    // LEFT
    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000))
    {
        // It detects PLUS NODE & Only Left Node
        printf("leftcheck1\n");
        left_check = 1;
    }
    // RIGHT
    if ((lsa_reading[0] == 0) && (lsa_reading[3] == 1000) && (lsa_reading[2] == 1000) && (lsa_reading[4] == 1000))
    {
        printf("rightcheck1\n");
        right_check = 1;
    }

    if (left_check == 1 || right_check == 1)
    {
        vTaskDelay(20 / portTICK_PERIOD_MS);//delay
        get_raw_lsa();
        
        if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // checks left first
        {
            left_turn = 1;
            left_check = 0;
            right_check = 0;
            printf("leftcheck2\n");
        }
        else if (lsa_reading[0] == 0 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[4] == 1000)
        {
            right_turn = 1;
            left_check = 0;
            right_check = 1;
        }
        else
        {
            left_turn = 0;
            right_turn = 0;
            left_check = 0;
            right_check = 0;
            printf("leftcheck1failed\n");
        }
    }

    else if (right_turn)
    {
        printf("check only right\n");
        check_only_right();
    }
    
    get_raw_lsa();
    
    if (left_turn)
    {
        left_tune = 1;
        while (left_tune)
        {

            printf("leftcheckturn\n");
            get_raw_lsa();
            if (lsa_reading[0] == 1000 && lsa_reading[2] == 1000)
            {
                printf("leftcheckcontinue\n");
                straight_tuning();
            }
            else
            {
                Leftturn();
            }
        }
    }

    else if (only_right == 1 && right_turn == 1)
    {
        right_tune = 1;
        while (right_tune)
        {

            printf("rightcheckturn\n");
            get_raw_lsa();
            if (lsa_reading[4] == 1000 && lsa_reading[2] == 1000)
            {
                printf("rightcheckcontinue\n");
                straight_tuning();
            }
            else
            {
                Rightturn();
            }
        }
    }

    else if ((right_check == 0) && (right_turn == 0) && (left_turn == 0) && lsa_reading[0] == 0 && lsa_reading[1] == 0 && lsa_reading[3] == 0 && lsa_reading[2] == 0 && lsa_reading[4] == 0)
    {
        uturn_tune = 1;
        while (uturn_tune)
        {

            printf("Ucheckturn\n");
            get_raw_lsa();
            if (lsa_reading[2] == 1000)
            {
                printf("Ucheckcontinue\n");
                straight_tuning();
            }
            else
            {
                Uturn();
            }
        }
    }
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
