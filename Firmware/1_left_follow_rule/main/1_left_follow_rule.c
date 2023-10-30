#include "mazeblaze3.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"

float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

#define NO_OF_NODES 100
int store_path[NO_OF_NODES]={1,0};
int final_run[NO_OF_NODES] = {0};
int array_index = 1;
int direction = 1; // 1= North, 2= East, 3= South, 4= West.. assumed bot is moving in a north direction
int directionflag=0;
int counter;
int f=1;
int show_finalrun=1;
TaskHandle_t taskhandle1 = NULL;

/*FINAL TURN FLAGS*/
int left_final_check=0;
int right_final_check=0;
int final_left_turn_check=0;
int final_right_turn_check=0;
int take_final_right_turn=0;
int take_final_left_turn=0;
int take_straight=0;
int stop=0;
int x = 0;


#define BOOT_BUTTON GPIO_NUM_0   //boot button
int enable_boot=0;

/*This method involves tuning kp , ski ,kd physically*/
#define GOOD_DUTY_CYCLE 91
#define MIN_DUTY_CYCLE 61
#define MAX_DUTY_CYCLE 96

#define PWM 85
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
            left_final_check=0;
            right_final_check=0;
            final_left_turn_check=0;
            final_right_turn_check=0;
            take_final_right_turn=0;
            take_final_left_turn=0;
            take_straight=0;
            stop=0; 
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
            left_final_check=0;
            right_final_check=0;
            final_left_turn_check=0;
            final_right_turn_check=0;
            take_final_right_turn=0;
            take_final_left_turn=0;
            take_straight=0;
            stop=0; 
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

void final_straight_tune()
{
   for (int l=0; l<3; l++)
   {
    get_raw_lsa();
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);
    printf("FINAL STRAIGHT");
    vTaskDelay(10 / portTICK_PERIOD_MS);
   } 
}

void final_straight()
{
    get_raw_lsa();
    while (lsa_reading[0] == 1000 || lsa_reading[4] == 1000) //Left sensor==>0 Right sensor ==>4 WHITE=1000
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
   left_final_check=0;
   right_final_check=0;
   final_left_turn_check=0;
   final_right_turn_check=0;
   take_final_right_turn=0;
   take_final_left_turn=0;
   take_straight=0;
   stop=0;
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

    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void final_error_tuning()
{
    get_raw_lsa();
    calculate_error();
    calculate_correction();

    left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
    right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /*goes forward in this case*/
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);

    vTaskDelay(30 / portTICK_PERIOD_MS);
}

void check_only_right()
{
    get_raw_lsa();
    while (lsa_reading[4] == 1000 && lsa_reading[3] == 1000)
    {
        get_raw_lsa();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
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
            final_error_tuning();
        }
        else
        {
            Leftturn();
        }
    }

}

void final_right_turn()
{
     right_tune = 1;
        while (right_tune)
        {

            printf("rightcheckturn\n");
            get_raw_lsa();
            if (lsa_reading[4] == 1000 && lsa_reading[2] == 1000)
            {
                printf("rightcheckcontinue\n");
                final_error_tuning();
            }
            else
            {
                Rightturn();
            }
        }
}

void simplify_path()
{   
    printf("SIMPLIFY PATH");
    int size = sizeof(store_path) / sizeof(store_path[0]);
    int prev_index = 0;
    int prev_value = store_path[prev_index];

    for (int i = 1; i < size; i++)
    {
      if (abs(prev_value - store_path[i]) == 2)
        {
            if(store_path[prev_index] !=0 && store_path[i] !=0 ){
        
                while((abs(store_path[prev_index -1] - store_path[i+1])==2)&&(store_path[prev_index - 1] != 0)){

                    store_path[prev_index -1] = 0;
                    store_path[i+1] =0;

                    
                }
                store_path[prev_index] = 0;
                store_path[i] = 0;
                prev_index++;
                prev_value = store_path[prev_index];
            }
            else
            {
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
    
    for(int i =0;i<size;i++)
    {
        if(store_path[i]!=0)
        {
            final_run[x]=store_path[i];
            x ++;
        }
        else
        {
           continue;
        }

    }
    printf("Final Run: ");
    for(int i =0 ;i<x;i++)
    {
        printf("%d ",final_run[i]);
    }
}
                                
void final_maze_solving()
{
    get_raw_lsa();
    //LEFT
    printf("1: %d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);       
    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000))
    {
        // It detects PLUS NODE & Only Left Node
        printf("leftcheck1\n");
        left_final_check = 1;
    }
    // RIGHT   
    if ((lsa_reading[0] == 0) && (lsa_reading[3] == 1000) && (lsa_reading[2] == 1000) && (lsa_reading[4] == 1000))
    {
        printf("rightcheck1\n");
        right_final_check = 1;
    }
    
    if (left_final_check == 1 || right_final_check == 1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);//delay
        final_straight_tune();
        get_raw_lsa();
        
        printf("1: %d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);
        if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // checks left first
        {
            final_left_turn_check = 1;
            final_right_turn_check = 0;
            left_final_check = 0;
            right_final_check = 0;
            printf("leftcheck2\n");
        }
        else if (lsa_reading[0] == 0 && lsa_reading[3] == 1000 && lsa_reading[2] == 1000 && lsa_reading[4] == 1000)
        {
            final_right_turn_check = 1;
            final_left_turn_check = 0;
            left_final_check = 0;
            printf("right_turn\n");
            right_final_check = 1;
        }
        else
        {
            left_turn_check = 0;
            final_right_turn_check = 0;
            final_left_turn_check = 0;
            right_final_check = 0;
            printf("leftcheck1failed\n");
        }
    }

    if (final_left_turn_check == 1 || final_right_turn_check == 1)
    {
    //printf("%d ",final_run[i]);
    printf("final_left_right_turn_check\n");
    printf("\n final:%d:", final_run[f] );
    printf("\n final-1:%d:", final_run[f-1]);
    printf("\n f=%d", f);
    
    if (final_run[f] == 0)
        {
        printf("stop\n");
        stop=1;
        }
        else if ((final_run[f] - final_run[f - 1] == -1) || (final_run[f] - final_run[f - 1] == 3)) //LEFT
        {
            printf("take_final_left_turn\n");
            take_final_left_turn=1;
            f++;
        }
        else if ((final_run[f] - final_run[f - 1] == 1) || (final_run[f] - final_run[f - 1] == -3)) //RIGHT
        {
            printf("take_final_right_turn\n");
            take_final_right_turn=1;
            f++;
        }
        else if (final_run[f] - final_run[f - 1] == 0) //STRAIGHT
        {
            printf("take_straight2\n");
            take_straight=1;
            f++;
        } 

    }

    if(take_final_left_turn==1)
    {
        printf("take_final_right_turn2\n");
        final_left_turn();
    }
    else if(take_final_right_turn==1)
    {
        printf("take_final_left_turn2\n");
        final_right_turn();
    }
    else if(take_straight==1)
    {
        printf("take_straight_final\n");
        final_straight(); 
    }
    
    else if (stop==1)
    {
    printf("take_stop_final\n");
    while (1)
        {
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void final_traversal()
{
    printf("FINAL TRANSVERSAL");
    simplify_path();
    show_finalrun=1;
    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    while(1)
    { 

        if(show_finalrun==1)
        {
            printf("Final Run: ");
            for(int i =0 ;i<x;i++)
            {
            printf("%d ",final_run[i]);
            }
            show_finalrun=0;
        }

        final_maze_solving();    
        straight();

        
    } 
}

//FUNCTION TO RESUME CODE BY USING BOOT BUTTON

void boot_button()
{

   while(1)
   {
     int boot_button_state = gpio_get_level(BOOT_BUTTON); //1=OPEN 0=CLOSE
     vTaskDelay(10 / portTICK_PERIOD_MS);
     if(boot_button_state==1 && enable_boot==1)
     {
        set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
        set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
        printf("\n STATE1: %d", boot_button_state);
        vTaskDelay(10 / portTICK_PERIOD_MS);
     }

     else if(boot_button_state==0)
     {
       enable_boot=0;
     }
     
     if(boot_button_state==1 && enable_boot==0)
     {
       enable_boot=0;
       printf("\n STATE2: %d", boot_button_state);
       vTaskDelay(1000 / portTICK_PERIOD_MS);
       final_traversal();
     }
        // Control the LED based on the button state
     printf("\n STATE: %d", boot_button_state);

     
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
    printf("1: %d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);

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
        vTaskDelay(40 / portTICK_PERIOD_MS);//delay
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
    
    
    if (left_turn_check)
    { 
       vTaskDelay(10 / portTICK_PERIOD_MS);
       get_raw_lsa();
       counter=0;
       printf("%d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);
       while (lsa_reading[0] == 1000 && lsa_reading[1] == 1000)
        {
            get_raw_lsa();
            counter++;
            vTaskDelay(10 / portTICK_PERIOD_MS);
            printf("\n counter %d", counter);
            printf("%d %d %d %d %d\n", lsa_reading[0], lsa_reading[1], lsa_reading[2], lsa_reading[3], lsa_reading[4]);
            if (counter >= 20 && lsa_reading[0] == 1000 && lsa_reading[1] == 1000)
            {
                while(1)
                {
                    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
                    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    counter=0;
                    printf("counter %d", counter);
                    printf("STOP");
                    enable_boot=1;
                    boot_button();
                }
            }

        }
        printf("left turn flag on");
        left_turn=1;

    }
    
    if (right_turn)
    {
        printf("check only right\n");
        check_only_right();
    }
    
    else if (left_turn)
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
    
    get_raw_lsa();
    if ((only_right == 1) && (right_turn == 1))
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
        
        vTaskDelay(30 / portTICK_PERIOD_MS);
        get_raw_lsa();
        if ((right_check == 0) && (left_check == 0) && (right_turn == 0) && (left_turn == 0) && (lsa_reading[0] == 0) && (lsa_reading[1] == 0) && (lsa_reading[3] == 0) && (lsa_reading[2] == 0) && (lsa_reading[4] == 0))
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
    // Configuration of boot button    
    gpio_config_t button_conf;
    button_conf.pin_bit_mask = (1ULL << BOOT_BUTTON);
    button_conf.mode = GPIO_MODE_INPUT;
    button_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    button_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&button_conf);

    ESP_ERROR_CHECK(enable_lsa());
    ESP_ERROR_CHECK(enable_motor_driver());

    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, &taskhandle1);
}
