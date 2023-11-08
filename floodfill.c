#include <stdio.h>
#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"

//FLOODFILL

int x=6, y=6;//STRATING POSITION
int orient=0, cell = 0;
int x0, x1, x2, y2, x3, y3;
char turning;
int que[100];
int H=8, W=8; //Height and width of our maze
#define H 8
#define W 8
int perform_floodfill=0;
int straight=0;

int l1 = 0;
int l2 = 0;
int r1 = 0;
int r2 = 0;
int s1 = 0;
int s2 = 0;
int u1 = 0;
int u2 = 0;

int cells_checking = 1;

float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

#define SIZE 8 

   //Flood 2D array of our maze at unknown walls/nodes condition
    int floodfill_array[H][W] = {
    {2, 1, 2, 3, 4, 5, 6, 7},
    {1, 0, 1, 2, 3, 4, 5, 6},
    {2, 1, 2, 3, 4, 5, 6, 7},
    {3, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 4, 5, 6, 7, 8, 9},
    {5, 4, 5, 6, 7, 8, 7, 10},  // Updated value at [5][6]
    {6, 5, 6, 7, 8, 9, 10, 11},
    {7, 6, 7, 8, 9, 10, 11, 12}
};


    int nodes[H][W] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
    }; //2D array to store node-types initaited with zero

    //x,y =current position
    // Accessing elements in the matrix for demonstration:
//MAZE TRAVEL
#define GOOD_DUTY_CYCLE 91
#define MIN_DUTY_CYCLE 61
#define MAX_DUTY_CYCLE 96

TaskHandle_t taskhandle1 = NULL;

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
            straight = 0;
            perform_floodfill=0;
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
            straight = 0;
            perform_floodfill=0;
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

void floodfill()
{
  
  while(1)
  {
    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
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
            rotation2 = 0;
            only_right = 0;
            uturn_tune=0;

            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void line_follow()
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

void go_straight()
{
    
    get_raw_lsa();
    while((lsa_reading[0] == 1000 || lsa_reading[4] == 1000))
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
}
    
int direction(int orient, char turning) //update the direction according to the turn taking
{
    if (turning == 'L') {
        orient--;
        if (orient == -1) {
            orient = 3;
        }
    } else if (turning == 'R') {
        orient++;
        if (orient == 4) {
            orient = 0;
        }
    } else if (turning == 'B') {
        if (orient == 0) {
            orient = 2;
        } else if (orient == 1) {
            orient = 3;
        } else if (orient == 2) {
            orient = 0;
        } else if (orient == 3) {
            orient = 1;
        }
    }
    return orient;

}

void set_coordinates()
{
    int directionflag = 1;
        if (directionflag) 
        {
            switch (orient) 
            {
                case 0:
                    printf("\t North\n");
                    l1=x-1;
                    l2=y;
                    r1=x;
                    r2=y+1;
                    s1=x-1;
                    s2=y;
                    u1=x+1;
                    u2=y;
                    break;
                case 1:
                    printf("\t East\n");
                    l1=x-1;
                    l2=y;
                    r1=x+1;
                    r2=y;
                    s1=x;
                    s2=y+1;
                    u1=x;
                    u2=y-1;
                    break;
                case 2:
                    printf("\t South\n");
                    l1=x;
                    l2=y+1;
                    r1=x;
                    r2=y-1;
                    s1=x-1;
                    s2=y;
                    u1=x+1;
                    u2=y;
                    break;
                case 3:
                    printf("\t West\n");
                    l1=x+1;
                    l2=y;
                    r1=x-1;
                    r2=y;
                    s1=x;
                    s2=y-1;
                    u1=x;
                    u2=y+1;
                    break;
            }
        }
}
void updateCoordinates(int *x, int *y, int orient) {
    //update the present co ordinate or present position of bot in maze/graph
    if (orient == 0) (*y)--;
    if (orient == 1) (*x)++;
    if (orient == 2) (*y)--;
    if (orient == 3) (*x)--;
}

/*void check_only_right()
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
}*/
        
void mazetravel()
{
    
    // Print the values of floodfill_array[x+1][y] and floodfill_array[y][x]
    left_check = 0;
    right_check= 0;
    left_turn = 0;
    right_turn = 0;
    perform_floodfill=0;
    straight=0;
    get_raw_lsa();
    //first directly go to straight
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
        //vTaskDelay(40 / portTICK_PERIOD_MS);//delay
        get_raw_lsa();
        set_coordinates();

        printf("x = %d, y= %d\n", x, y); 
        printf("l1 = %d, l2= %d\n", l1, l2);
        printf("r1= %d, r2= %d\n", r1, r2);
        printf("s1 = %d, s2= %d\n", s1, s2);
        printf("floodfill_array[l1][l2] = %d, floodfill_array[y][x] = %d\n", floodfill_array[l1][l2], floodfill_array[y][x]);
        printf("floodfill_array[r1][r2] = %d, floodfill_array[y][x] = %d\n", floodfill_array[r1][r2], floodfill_array[y][x]);
        printf("floodfill_array[s1][s2] = %d, floodfill_array[y][x] = %d\n", floodfill_array[s1][s2], floodfill_array[y][x]);

        if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000) && (lsa_reading[4] == 0))
        {
            cells_checking = 1;
            while(cells_checking == 1)
            {
            get_raw_lsa();
            // Only Left Node
            while(lsa_reading[0]==1000)
            { 
             get_raw_lsa();
            }

            get_raw_lsa();
            if(lsa_reading[2]==0)
            { 
                if(floodfill_array[l1][l2]<floodfill_array[y][x])
                {
                
                    printf("leftcheck2\n");
                    left_turn = 1;
                    cells_checking = 0;
                
                }
                if(floodfill_array[l1][l2]>floodfill_array[y][x])
                {
                    printf("floodfill\n");
                    perform_floodfill = 1;
                    cells_checking = 0;
                }
            }

            if(lsa_reading[2]==1000)
            {
                if(floodfill_array[l1][l2]<floodfill_array[y][x])
                {
                
                    printf("leftcheck2\n");
                    left_turn = 1;
                    cells_checking = 0;
                
                }
                if((floodfill_array[s1][s2]<floodfill_array[y][x]) && (left_turn == 0))
                {
                
                    printf("straight\n");
                    straight = 1;
                    cells_checking = 0;
                
                }
                if((floodfill_array[s1][s2]>floodfill_array[y][x]) || floodfill_array[l1][l2]>floodfill_array[y][x])
                {
                    printf("floodfill\n");
                    perform_floodfill = 1;
                    cells_checking = 0;
                }
            }
            }
        }
     
        // RIGHT
        if ((lsa_reading[0] == 0) && (lsa_reading[3] == 1000) && (lsa_reading[2] == 1000) && (lsa_reading[4] == 1000))
        {
            cells_checking = 1;
            while(cells_checking == 1)
            {
                get_raw_lsa();
                // Only Left Node
                while(lsa_reading[4]==1000)
                {
                    get_raw_lsa();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }  
                vTaskDelay(10 / portTICK_PERIOD_MS);
                get_raw_lsa();
                if(lsa_reading[2]==0)
                {
                    if(floodfill_array[r1][r2] < floodfill_array[y][x])
                    {
                    
                        printf("right\n");
                        right_turn = 1;
                        cells_checking = 0;
                    
                    }
                    if(floodfill_array[r1][r2]>floodfill_array[y][x])
                    {
                        printf("floodfill2\n");
                        perform_floodfill = 1;
                        cells_checking = 0;
                    }                 
                }

                if(lsa_reading[2]==1000)
                {
                    if((floodfill_array[r1][r2]<floodfill_array[y][x]))
                    {
                    
                        printf("right\n");
                        right_turn = 1;
                        cells_checking = 0;
                    
                    }
                    if((floodfill_array[s1][s2]<floodfill_array[y][x]) && (right_turn == 0) && (lsa_reading[2] != 0))
                    {
                    
                        printf("staight right\n");
                        straight = 1;
                        cells_checking = 0;
                    
                    }
                    if((floodfill_array[s1][s2]>floodfill_array[y][x]) || (floodfill_array[r1][r2]<floodfill_array[y][x]))
                    {
                        printf("floodfill1\n");
                        perform_floodfill = 1;
                        cells_checking = 0;
                    }
                }
            }

        }
    }

    if(left_turn==1 || straight==1 || right_turn==1 || perform_floodfill==1)
    {
        if(left_turn==1)
        {
         orient = direction(orient, 'L');  
         updateCoordinates(&x, &y, orient);
         Leftturn(); 
        }
        if(right_turn==1)
        {
         orient = direction(orient, 'R');
         updateCoordinates(&x, &y, orient);
         Rightturn();
        }
        if(perform_floodfill==1)
        {
         floodfill();
        }

        if(straight==1)
        {
         go_straight();
        }
    }
    
}

void line_follow_task(void *arg)
{

    while (1)
    {
        mazetravel();
        line_follow();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    ESP_ERROR_CHECK(enable_lsa());
    ESP_ERROR_CHECK(enable_motor_driver());

    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, &taskhandle1);
}

