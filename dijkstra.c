#include <stdio.h>
#include <stdlib.h>
#include "mazeblaze2.h"
#include "tuning_http_server.h"
#include "driver/gpio.h"

float error = 0, prev_error = 0, difference, cumulative_error, correction;
float left_duty_cycle = 0, right_duty_cycle = 0;

float kp = 35, ki = 0.01, kd = 60;

const int weights[5] = {3, 1, 0, -1, -3};

int final_run[50];
int array_index = 1;
int f=1;
TaskHandle_t taskhandle1 = NULL;

#define INFINITY_A 999
#define INDEX_MAX_SIZE 50

/*This method involves tuning kp , ski ,kd physically*/
#define GOOD_DUTY_CYCLE 20
#define MIN_DUTY_CYCLE 20
#define MAX_DUTY_CYCLE 20
int i, j, n, s, e, cost[INDEX_MAX_SIZE][INDEX_MAX_SIZE], dist[INDEX_MAX_SIZE], parent[INDEX_MAX_SIZE];
int path[50];// Store the path in an array
int path_length = 0;
int enable_path_matrix=1;
int stop_path_matrix=1;


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

#define PWM 20
int rotation1 = 0;
bool rotation2 = false;


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
   for (int k=0; k<24; k++)
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
   left_final_check=0;
   right_final_check=0;
   final_left_turn_check=0;
   final_right_turn_check=0;
   take_final_right_turn=0;
   take_final_left_turn=0;
   take_straight=0;
   stop=0;
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

void dijkstra(int n, int cost[INDEX_MAX_SIZE][INDEX_MAX_SIZE], int s, int dist[50], int parent[50])
{
    //n=no of nodes
    int i, count = 1, min, visited[INDEX_MAX_SIZE];
    int vertex=0;
    for (i = 1; i <= n; i++)
    {
        visited[i] = 0;
        dist[i] = cost[s][i];
        parent[i] = s; //parrent array to print shortest path
    }
    visited[s] = 1;
    dist[s] = 0;

    //recursion
    while (count < n) {
        min = INFINITY_A;
        for (i = 1; i <= n; i++)
        {
            if (dist[i] < min && !visited[i]) {
                min = dist[i];
                vertex = i;
            }
        }
        visited[vertex] = 1;
        count++;
        for (i = 1; i <= n; i++) {
            if (dist[i] > dist[vertex] + cost[vertex][i]) {
                dist[i] = dist[vertex] + cost[vertex][i];
                parent[i] = vertex; // Update parent array
            }
        }
    }
}

/*this function simplifies the direction array according to the simplified path*/
void simplified_direction()
{
    //int arr[] = {1, 2, 3, 4, 6, 8,11,13,16,17,18,20,22,23,24,25,27,30,31,33,34,35,37,40,42,44,46,48}, 
    int z, a = 0,c=1,k,y=0;
    int dir[] = {1,4,1,2,2,1,4,1,4,1,2,1,1,4,1,4,1,4,3,4,3,4,3,2,3,2,3,4,4,2,3,4,3,4,4,1,1,4,4,1,4,1,4,1,4,1,4,1};
    
    int x = sizeof(dir) / sizeof(dir[0]);
    int size = sizeof(path) / sizeof(path[0]);
    int miss[size];


    for (int i = 0; i < size; i++)
    {
        if (path[i + 1] - path[i] > 1)
        {
            z = (path[i + 1] - path[i]) - 1;
            for (int j = 0; j < z; j++)
            {
                miss[a] = path[i] + c;
                c++;
                a++;
            }
            c=1;
        }
    }
    // for (int i = 0; i < a; i++)
    // {
    //     printf(" %d ", miss[i]);
    // }
    // printf(" \n%d ",x);

    for(int i=0;i<a; i++){
     k = miss[i];
     dir[k-1]=0;

    }
    for(int i=0;i<x;i++){
        printf(" %d ", dir[i]);
    }

    for(int i =0;i<x;i++){
        if(dir[i]!=0){
            final_run[y] = dir[i];
            y++;
        }
    }

    for(int i=0;i<y;i++){
        printf(" \n %d ", final_run[i]);
    }

}

void path_matrix()
{
    while(stop_path_matrix==1)
    {
    // Configuration of boot button    
    printf("Enter the total number of nodes: ");
    scanf("%d", &n);
    printf("Read the cost matrix:\n");
    for (i = 1; i <= n; i++) 
    {
        for (j = 1; j <= n; j++) 
        {
            scanf("%d", &cost[i][j]);
            if (cost[i][j] == 0)
                cost[i][j] = INFINITY_A;
        }
    }
    printf("Enter the source vertex: ");
    scanf("%d", &s);
    printf("Enter the end vertex: ");
    scanf("%d", &e);

    dijkstra(n, cost, s, dist, parent); //DIJKSTRA'S ALGORITHM FUNCTION

    printf("Shortest path from %d to %d is:\n", s, e);

    // Store the path in an array
    int current = e;

    while (current != s) {
        path[path_length++] = current;
        current = parent[current];
    }
    path[path_length++] = s;

    // Print the path in the correct order
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) {
            printf(" --> ");
        }
    }

    printf("\nShortest distance: %d\n", dist[e]);
    
    simplified_direction();
    enable_path_matrix=0;
    stop_path_matrix=0;
    }
}

void final_traversal()
{
    get_raw_lsa();
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


void line_follow_task(void *arg)
{

    while (1)
    {   
        if(enable_path_matrix==1)
        {
         path_matrix();
        }    
        final_traversal();
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
