#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sra_board.h"
#include "tuning_http_server.h"
#include <stdbool.h> // Include this header for true and false constants

#define MODE NORMAL_MODE
#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define bound_LSA_LOW 0
#define bound_LSA_HIGH 1000
#define BLACK_BOUNDARY  700    // Boundary value to distinguish between black and white readings
#define Transition_Value  700  //FOR WHITE TO BLACK OR BLACK TO WHITE TRANSITION
#define pwm 40
/*
 * weights given to respective line sensor
 */
const int weights[5] = {-5, -3, 1, 3, 5};
int current_reading[5];
int prev_readings[5];
bool Right_flag=false;

/*
 * Motor value boundts
 */
int optimum_duty_cycle = 68;
int lower_duty_cycle = 55;
int higher_duty_cycle = 81;
float left_duty_cycle = 0, right_duty_cycle = 0;

/*
 * Line Following PID Variables
 */
float error=0, prev_error=0, difference, cumulative_error, correction;

/*
 * Union containing line sensor readings
 */
line_sensor_array line_sensor_readings;


// void lsa_to_bar()
// {   
//     uint8_t var = 0x00;                     
//     bool number[8] = {0,0,0,0,0,0,0,0};
//     for(int i = 0; i < 5; i++)
//     {
//         number[7-i] = (line_sensor_readings.adc_reading[i] < BLACK_BOUNDARY) ? 0 : 1; //If adc value is less than black margin, then set that bit to 0 otherwise 1.
//         var = bool_to_uint8(number);  //A helper function to convert bool array to unsigned int.
//         ESP_ERROR_CHECK(set_bar_graph(var)); //Setting bar graph led with unsigned int value.
//     }
// }

void calculate_correction()
{
    error = error*10;  // we need the error correction in range 0-100 so that we can send it directly as duty cycle paramete
    difference = error - prev_error;
    cumulative_error += error;

    cumulative_error = bound(cumulative_error, -30, 30);

    correction = read_pid_const().kp*error + read_pid_const().ki*cumulative_error + read_pid_const().kd*difference;
    prev_error = error;
}

void calculate_error()
{
    int all_black_flag = 1; // assuming initially all black condition
    float weighted_sum = 0, sum = 0; 
    float pos = 0; int k = 0;

    for(int i = 0; i < 5; i++)
    {
        if(line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY)
        {
            all_black_flag = 0;
        }
        if(line_sensor_readings.adc_reading[i] > 700)
        {
            k = 1;
        }
        if(line_sensor_readings.adc_reading[i] < 700)
        {
            k = 0;
        }
        weighted_sum += (float)(weights[i]) * k;
        sum = sum + k;
    }

    if(sum != 0) // sum can never be 0 but just for safety purposes
    {
        pos = (weighted_sum - 1) / sum; // This will give us the position wrt line. if +ve then bot is facing left and if -ve the bot is facing to right.
    }

    if(all_black_flag == 1)  // If all black then we check for previous error to assign current error.
    {
        if(prev_error > 0)
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

//&&=BOTH -- ||=Only One
void Leftturn()
{
    bool left_turn = true;
    bool turn_flag1= true;

    while (left_turn) 
    {

        if (turn_flag1) 
        {
            set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, pwm);
            set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, pwm);  
            vTaskDelay(300 / portTICK_PERIOD_MS);

            if (current_reading[0] < Transition_Value && current_reading[1] > Transition_Value &&
                current_reading[2] > Transition_Value && current_reading[3] > Transition_Value &&
                current_reading[4] < Transition_Value) 
            {
                turn_flag1 = false;
                left_turn = false;
            }
        }

    }
    
}

void Rightturn()
{

    bool Right_turn=true;

    while(Right_turn)
    {
        bool turn_flag2=true;

        if(turn_flag2)
        {
            set_motor_speed(MOTOR_A_0,MOTOR_BACKWARD,pwm);
            set_motor_speed(MOTOR_A_1,MOTOR_FORWARD,pwm);
            //TURN TILL IT WILL DETECT WHITE LINE WHITE LINE DETECTED 2 3 4>Transition_Value
        }

        if(current_reading[0]<Transition_Value && current_reading[1]>Transition_Value && current_reading[2]>Transition_Value && current_reading[3]>Transition_Value && current_reading[4]<Transition_Value)
        {
            turn_flag2=false;
            Right_turn=false;
            continue;

        }

    }
    
}

void Uturn()
{

    bool U_turn=true;

    while(U_turn)
    {
        bool turn_flag3=true; 

        if(turn_flag3){
        set_motor_speed(MOTOR_A_0,MOTOR_FORWARD,pwm);
        set_motor_speed(MOTOR_A_1,MOTOR_BACKWARD,pwm);
        //TURN TILL IT WILL DETECT WHITE LINE WHITE LINE DETECTED 2 3 4>Transition_Value
        }

        if(current_reading[0]<Transition_Value && current_reading[1]>Transition_Value && current_reading[2]>Transition_Value && current_reading[3]>Transition_Value && current_reading[4]<Transition_Value)
        {
            turn_flag3=false;
            U_turn=false;
            continue;

        }

    }
    
} 

void LFR()
{
    for(int j = 0; j < 5; j++)
    {
       current_reading[j]= line_sensor_readings.adc_reading[j];
    }

    /*if ((prev_readings[0]<Transition_Value && current_reading[0]>Transition_Value) || (prev_readings[1]>Transition_Value && current_reading[1]<Transition_Value) || (prev_readings[2]>Transition_Value && current_reading[2]<Transition_Value) || (prev_readings[3]>Transition_Value && current_reading[3]<Transition_Value) || (prev_readings[4]<Transition_Value && current_reading[4]>Transition_Value)) //checking for transitions (BLACK TO WHITE FOR SENSOR 1&5) (WHITE TO BLACK for sensor 2,3,4)*/
    if ((prev_readings[0]<Transition_Value && current_reading[0]>Transition_Value) || (prev_readings[4]<Transition_Value && current_reading[4]>Transition_Value))
    {
        if(current_reading[0]>Transition_Value && (current_reading[1]>Transition_Value || current_reading[2]>Transition_Value))

        {
            //It detects PLUS NODE & Only Left Node
            Leftturn();
        }

        /*if(current_reading[0]<Transition_Value && current_reading[4]>Transition_Value)
        {
            //TO CHECK ONLY RIGHT NODE
            Right_flag=true;

        }
        
        if(current_reading[0]<Transition_Value || current_reading[1]<Transition_Value ||current_reading[2]<Transition_Value || current_reading[3]<Transition_Value || current_reading[4]<Transition_Value)
        {
            //Dead END
            Uturn();
        }*/

        

    }
    
    for(int k = 0; k < 5; k++)
    {
       prev_readings[k] = current_reading[k];
    }


    if(Right_flag)
    {
        if(current_reading[0]<Transition_Value && current_reading[1]<Transition_Value && current_reading[2]<Transition_Value && current_reading[3]<Transition_Value && current_reading[4]<Transition_Value)
        {
            //IF ONLY RIGHT NODE IS DETECT
            Rightturn();
            Right_flag= false;
        
        }

        if(current_reading[0]<Transition_Value && current_reading[1]>Transition_Value && current_reading[2]<Transition_Value && current_reading[3]<Transition_Value && current_reading[4]<Transition_Value)
        {
            //IF STRAIGHT + RIGHT NODE DETECT THEN SIMPLY Right_flag becomes zero and bot dont take any turn.
            Right_flag=false;
        
        }
    
    }

}


void line_follow_task(void* arg)
{
    //bool line_follow_flag = true;
    ESP_ERROR_CHECK(enable_motor_driver(a, NORMAL_MODE));
    ESP_ERROR_CHECK(enable_line_sensor());
    // ESP_ERROR_CHECK(enable_bar_graph());
// #ifdef CONFIG_ENABLE_OLED
//     // Initialising the OLED
//     ESP_ERROR_CHECK(init_oled());
//     vTaskDelay(100);

//     // Clearing the screen
//     lv_obj_clean(lv_scr_act());

//#endif
    while(true)
    {
        //bool line_follow_flag = true;
	    line_sensor_readings = read_line_sensor();
		for(int i = 0; i < 5; i++)
		{
		    line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
		    line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
		    line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
		}

	    /*if(line_follow_flag)
	    {*/
		// line_sensor_readings = read_line_sensor();
		// for(int i = 0; i < 5; i++)
		// {
		//     line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
		//     line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, bound_LSA_LOW, bound_LSA_HIGH);
		//     line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
		// }
        LFR();
		calculate_error();
		calculate_correction();
		//lsa_to_bar();

		left_duty_cycle = bound((optimum_duty_cycle + correction), lower_duty_cycle, higher_duty_cycle);
		right_duty_cycle = bound((optimum_duty_cycle - correction), lower_duty_cycle, higher_duty_cycle);

		set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, left_duty_cycle);
		set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, right_duty_cycle);


		//ESP_LOGI("debug","left_duty_cycle:  %f    ::  right_duty_cycle :  %f  :: error :  %f  correction  :  %f  \n",left_duty_cycle, right_duty_cycle, error, correction);
		ESP_LOGI("debug", "KP: %f ::  KI: %f  :: KD: %f", read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
	// #ifdef CONFIG_ENABLE_OLED
	//         // Diplaying kp, ki, kd values on OLED 
	//         if (read_pid_const().val_changed)
	//         {
	//             display_pid_values(read_pid_const().kp, read_pid_const().ki, read_pid_const().kd);
	//             reset_val_changed_pid_const();
	//         }
	// #endif

	//         vTaskDelay(10 / portTICK_PERIOD_MS);
	    
	}    

    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(&line_follow_task, "line_follow_task", 4096, NULL, 1, NULL);
    start_tuning_http_server();
}
