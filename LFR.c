#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"

float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

#define NO_OF_NODES 100
int store_path[NO_OF_NODES]={1,0};
int final_run[]={};
int array_index = 1;
int direction = 1; // 1= North, 2= East, 3= South, 4= West.. assumed bot is moving in a north direction
int directionflag=0;
int counter;
int f=0;
TaskHandle_t taskhandle1 = NULL;

/*This method involves tuning kp , ski ,kd physically*/
#define GOOD_DUTY_CYCLE 88
#define MIN_DUTY_CYCLE 58
#define MAX_DUTY_CYCLE 93

#define PWM 82
int rotation1 = 0;
bool rotation2 = false;
int left_check = 0;
int right_check = 0;
int left_turn_check = 0;
int left_turn=0;
int right_turn = 0;
int left_tune = 0;
int only_right = 0;
int uturn_tune = 0;
int right_tune = 0;
int Check_End=0;

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
    direction = (direction - 1) % 4;
    if (direction == 0) direction = 4;
    directionflag = 1;
    if (directionflag) 
    {
        switch (direction) 
        {
            case 1:
                printf("\t North\n");
                store_path[array_index] = 1;
                break;
            case 2:
                printf("\t East\n");
                store_path[array_index] = 2;
                break;
            case 3:
                printf("\t South\n");
                store_path[array_index] = 3;
                break;
            case 4:
                printf("\t West\n");
                store_path[array_index] = 4;
                break;
        }
    }
    directionflag=0;
    array_index++;

    int left = 1;
    
    while (left)
    {

        get_raw_lsa();

        if (lsa_reading[2] == 0)
        {
            rotation1 = 1;
        }
        
        set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);

        if (lsa_reading[2] == 1000 && rotation1==1)
        {
            // ssvTaskDelay(40 / portTICK_PERIOD_MS);
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            vTaskDelay(20 / portTICK_PERIOD_MS);
            Check_End=0;
            left_tune = 0;
            rotation1 = 0;
            left_turn_check=0;
            rotation2 = 0;
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
    direction = (direction + 1) % 4;
    if (direction == 0) direction = 4;
    directionflag = 1;
    if (directionflag) 
    {
        switch (direction) 
        {
            case 1:
                printf("\t North\n");
                store_path[array_index] = 1;
                break;
            case 2:
                printf("\t East\n");
                store_path[array_index] = 2;
                break;
            case 3:
                printf("\t South\n");
                store_path[array_index] = 3;
                break;
            case 4:
                printf("\t West\n");
                store_path[array_index] = 4;
                break;
        }
    }
    directionflag=0;
    array_index++;

    printf("R T \n");
    int right = 1;

    while (right)
    {
        get_raw_lsa();

        if (lsa_reading[3] == 0)
        {
            rotation2 = true;
        }

        set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, PWM);
        printf("CONTINUE R T \n");
        if (lsa_reading[2] == 1000 && lsa_reading[3] == 1000 && rotation2)
        {
            printf("STOP R T \n");
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            rotation1 = 0;
            rotation2 = 0;
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
    direction = (direction - 2) % 4;
    if (direction == 0) direction = 4;
    if (direction == -1) direction = 3;
    directionflag = 1;
    if (directionflag) 
    {
        switch (direction) 
        {
            case 1:
                printf("\t North\n");
                store_path[array_index] = 1;
                break;
            case 2:
                printf("\t East\n");
                store_path[array_index] = 2;
                break;
            case 3:
                printf("\t South\n");
                store_path[array_index] = 3;
                break;
            case 4:
                printf("\t West\n");
                store_path[array_index] = 4;
                break;
        }
    }
    directionflag=0;
    array_index++;
    
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
            rotation2 = 0;
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

    vTaskDelay(150 / portTICK_PERIOD_MS);
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
        //STRAIGHT
        direction = direction;
        if (direction == 0) direction = 4;
        if (direction == -1) direction = 3;
        directionflag = 1;
        if (directionflag) 
        {
            switch (direction) 
            {
                case 1:
                    printf("\t North\n");
                    store_path[array_index] = 1;
                    break;
                case 2:
                    printf("\t East\n");
                    store_path[array_index] = 2;
                    break;
                case 3:
                    printf("\t South\n");
                    store_path[array_index] = 3;
                    break;
                case 4:
                    printf("\t West\n");
                    store_path[array_index] = 4;
                    break;
            }
        }
        directionflag=0;
        array_index++;
        only_right = false;
    }
}
void final_left_turn()
{ 
    left_tune = 1;
    while (left_tune)
    {

        printf("leftcheckturn\n");
        get_raw_lsa();
        vTaskDelay(10 / portTICK_PERIOD_MS);
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
void final_right_turn(){
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
void simplify_path()
{   
    int size = sizeof(store_path) / sizeof(store_path[0]);
    int prev_index = 0;
    int prev_value = store_path[prev_index];
    int x = 0;

    for (int i = 1; i < size; i++)
    {
      if (abs(prev_value - store_path[i]) == 2)
        {
            if(store_path[prev_index] !=0){
        
                while((abs(store_path[prev_index -1] - store_path[i+1])==2)&&(store_path[prev_index - 1] != 0)){

                    store_path[prev_index -1] = 0;
                    store_path[i+1] =0;

                    
                }
                store_path[prev_index] = 0;
                store_path[i] = 0;
                prev_index++;
                prev_value = store_path[prev_index];
            }
            else{
            prev_index++;
             prev_value = store_path[prev_index];
            }
          
        }
        else{
            prev_index++;
            prev_value = store_path[prev_index];
        }
    }
    printf("DRY RUN: ");
    for (int i = 0; i < array_index; i++)
    {
        printf("%d ", store_path[i]);
    }
    
    for(int i =0;i<size;i++){
              if(store_path[i]!=0){
                final_run[x]=store_path[i];
                x ++;
                 }
              else{
                continue;
              }

    }
    printf("Final Run: ");
    for(int i =0 ;i<x;i++){
        printf("%d ",final_run[i]);
}
}
void final_traversal()
{
    
    simplify_path();
    
    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
     
    while(1)
    { 
        get_raw_lsa();
        if (lsa_reading[0] == 1000 && lsa_reading[1] == 1000 && lsa_reading[2] == 1000)
        {
            left_check = 1;
        }
        if (lsa_reading[2] == 1000 && lsa_reading[3] == 1000 && lsa_reading[4] == 1000)
        {
            right_check = 1;
        }
        if (left_check == 1 || right_check == 1)
           {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            
            
            get_raw_lsa();
            if (lsa_reading[0] == 1000 && lsa_reading[1] == 1000 && lsa_reading[2] == 1000)
            {
                if ((final_run[f + 1] - final_run[f]) == 3 || final_run[f + 1] - final_run[f] == -1)
                {
                    final_left_turn();
                    f++;
                }
                else if ((final_run[f + 1] - final_run[f]) == 0)
                {
                    straight();
                    f++;
                }
                else if (final_run[f + 1] == 0)
                {
                    while (1)
                    {
                        set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
                        set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);

                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                }
            }
            get_raw_lsa();
            if (lsa_reading[0] == 0 && lsa_reading[2] == 1000 && lsa_reading[3] == 1000 && lsa_reading[4] == 1000)
            {
                if ((final_run[f + 1] - final_run[f]) == -3 || final_run[f + 1] - final_run[f] == 1)
                {
                    final_right_turn();
                    f++;
                }
                else if ((final_run[f + 1] - final_run[f]) == 0)
                {
                    straight();
                    f++;
                }
                else if (final_run[f + 1] == 0)
                {
                    while (1)
                    {
                        set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
                        set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);

                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                }
            }
            left_check =0;
            right_check =0;
        }
        
        straight();

        
    } 
}
void LFR()
{
    // lsa_readings();
    get_raw_lsa();
    left_check = 0;
    right_check= 0;
    left_turn_check = 0;
    right_turn = 0;
    left_turn=0;
    only_right = 0;
    //LEFT
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
        vTaskDelay(10 / portTICK_PERIOD_MS);//delay
        get_raw_lsa();
        
        if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // checks left first
        {
            left_turn_check = 1;
            left_check = 0;
            right_check = 0;
            printf("leftcheck2\n");
        }
        else if (lsa_reading[0] == 0 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[4] == 1000)
        {
            right_turn = 1;
            left_turn_check = 0;
            printf("right_turn\n");
            left_check = 0;
            right_check = 1;
        }
        else
        {
            left_turn_check = 0;
            right_turn = 0;
            left_check = 0;
            right_check = 0;
            printf("leftcheck1failed\n");
        }
    }

    if (right_turn)
    {
        printf("check only right\n");
        check_only_right();
    }
    
    
    if (left_turn_check)
    { 
       get_raw_lsa();
       vTaskDelay(10 / portTICK_PERIOD_MS);
       counter=0;
       while (lsa_reading[0] == 1000 && lsa_reading[1] == 1000 && lsa_reading[2] == 1000 && lsa_reading[3] == 1000 && lsa_reading[4] == 1000)
        {
            get_raw_lsa();
            counter++;
            vTaskDelay(10 / portTICK_PERIOD_MS);
            printf("\n counter %d", counter);
            printf("%d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);
            if (counter >= 20 && lsa_reading[0] == 1000 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[1] == 1000 && lsa_reading[4] == 1000)
                {
                    while(1){
                    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
                    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    counter=0;
                    printf("counter %d", counter);
                    printf("STOP");
                    final_traversal();
                    }
                }

        }
        left_turn=1;

    }

    get_raw_lsa();
    
    if (left_turn)
    {
        left_tune = 1;
        while (left_tune)
        {

            printf("leftcheckturn\n");
            get_raw_lsa();
            vTaskDelay(10 / portTICK_PERIOD_MS);
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

    else if ((only_right == 1) && (right_turn == 1))
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

    else if ((right_check == 0) && (left_check == 0) && (right_turn == 0) && (left_turn == 0) && (lsa_reading[0] == 0) && (lsa_reading[1] == 0) && (lsa_reading[3] == 0) && (lsa_reading[2] == 0) && (lsa_reading[4] == 0))
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
        printf("The current readings in ARRAY are : ");
        for (int i = 0; i < array_index; i++)
        {
            printf("%d ", store_path[i]);
        }
        printf("\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}



void app_main()
{
    ESP_ERROR_CHECK(enable_lsa());
    ESP_ERROR_CHECK(enable_motor_driver());

    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, &taskhandle1);
}
