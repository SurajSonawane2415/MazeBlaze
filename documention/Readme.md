# MazeBlaze-v3.1

# TABLE OF CONTENTS
- [Left Follow Rule](#left-follow-rule-lfr)
  - [Theory](#theory)
  - [Implementation](#implementation)
  - [Description Of Functions Used](#description-of-functions-used)
  - [Error Descriptions and Solution](#error-descriptions-and-solution)

- [Dijkstra's algorithm](#dijkstra's-algorithm)
- [Printed Circuit Board Design](#printed-circuit-board-design)
  - [Board Images](#board-images)
  - [Getting Started with a Mazeblaze V3 Board](#getting-started-with-a-mazeblaze-board)
  - [Major Changes for Mazeblaze V3 Board](#major-changes-for-mazeblaze-v3-board)



# Left Follow Rule (LFR)
## Theory
The Left-Hand Rule (LFR), also known as the "left-follow-algorithm," is a maze solving technique used to navigate through 2D mazes. To use LFR, you start at the entrance of a maze with your left hand touching the wall to your left. As you navigate through the maze, you follow a set of rules:

1. **Always turn left:** Whenever you reach an intersection or have a choice of paths, you make a left turn if it's possible, keeping your left hand on the wall.

2. **Go straight if a left turn is blocked:** If you come to an intersection where turning left is not an option because it leads to a wall, you continue moving straight while still touching the left wall.

3. **Take a right turn as a last resort:** If you encounter an intersection where you cannot turn left or go straight (usually because of a wall or obstacle), you take a right turn.

4. **Reverse when there are no other options:** If you find yourself in a situation where you can't turn left, go straight, or turn right, it means you've likely reached a dead end. In this case, you need to turn around and backtrack.

## Implementation

The Left Follow algorithm is a method for navigating and tracking the movement of a bot as it encounters and traverses various paths. This algorithm records the bot's movements by assigning numeric values to different directions and then tracking these values in an array named `store_path`. The directions are indexed as follows:

- North: 1
- East: 2
- South: 3
- West: 4

It is important to note that the algorithm assumes that the bot initially faces the North direction. Subsequently, the other directions are determined based on the North direction. These directions are fixed with respect to the world frame, and the bot continually updates its direction as it takes turns.

#### Direction Index Tracking

The program is designed to keep track of the serial numbers (direction indices) based on the assumptions mentioned above. Whenever the bot encounters a node or takes a turn, it records the direction index into the `store_path` array. Four key conditions are defined to track the bot's movement:

1. **Condition for Right Turn**

   When the bot takes a right turn, it increments the `direction` by 1. This is achieved through the code:

   ```c
   direction = (direction + 1) % 4;
   if (direction == 0) direction = 4;
   ```

   This code ensures that the `direction` cycles between 1 and 4, representing North, East, South, and West, respectively.

2. **Condition for Left Turn**

   When the bot takes a left turn, it decrements the `direction` by 1. This is done using:

   ```c
   direction = (direction - 1) % 4;
   if (direction == 0) direction = 4;
   ```

   This code ensures that the `direction` cycles between 1 and 4, representing North, East, South, and West, respectively.

3. **Condition for U-Turn**

   When the bot performs a U-turn, the `direction` is decremented by 2:

   ```c
   direction = (direction - 2) % 4;
   if (direction == 0) direction = 4;
   if (direction == -1) direction = 3;
   ```

   This operation effectively represents a 180-degree turn.

4. **Condition for Moving Straight**

   When the bot continues to move straight, the `direction` remains unchanged:

   ```c
   direction = direction;
   ```

   In this case, the `direction` remains the same, signifying that the bot maintains its current orientation.

When the robot moves and follows these rules, it records its current direction in the store_path array. This helps us keep track of where the robot has been. The robot always knows which direction it's facing, even if it takes many turns.

## Description Of Functions Used

1. 
```c
void LFR()
```
   - **Description:** This function helps the robot explore a maze by following the Left Follow Rule. It detects nodes by using LSA sensor(Line Sensor Array) and take turns according to the type of node.

2. 
```c
void Leftturn()
```
   - **Description:** This function tells the bot to turn left by following specific rules for accurate navigation.

3. 
```c
void Rightturn()
```
   - **Description:** This function function tells the bot to turn right by following specific rules for accurate navigation.

4. 
```c
void Uturn()
```
   - **Description:** This function tells the bot to make a U-turn, which means turning around completely to go in the opposite direction, following specific navigation rules.

5. 
```c
void simplify_path()
```
   - **Description:** Removes all the redundancies from store_path[] and stores the shortest path in another array called final_run[].

6. 
```c
void final_maze_solving()
```
   - The `final_maze_solving()` function manages the actual maze-solving process. It guides the robot to follow the simplified path stored in the `final_run` array, ensuring efficient navigation and the avoidance of unnecessary movements while reaching the destination.

## Error Descriptions and Solution
**1: Task Watchdog Triggered**
![Screenshot 2023-10-27 002829](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/81be0992-ee0b-4599-9c4a-1d6c37175717)
- **Description:** In the above screenshot, an error message, "Task watchdog got triggered," appeared when we flashed our code on the ESP. 
- **Solution:** we found it necessary to add a delay so we resolved this error by adding a 10ms delay, as follows:
```c
get_raw_lsa();
calculate_error();
calculate_correction();

left_duty_cycle = bound((GOOD_DUTY_CYCLE - correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
right_duty_cycle = bound((GOOD_DUTY_CYCLE + correction), MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, left_duty_cycle); /* goes forward in this case */
set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, right_duty_cycle);

vTaskDelay(10 / portTICK_PERIOD_MS); //Delay
```
    
**2:  Unable to turn after detecting a node**
- **Description:** When bot detects the node after that bot didn't taking turn. This error was ocuure because the bot's PWM for turn was 40, which was insufficient to turn, pwm should be grater than 55 for our case. Because when the PWM value was set to 40, it likely provided a relatively low average power to the motors, which resulted in the bot's motors not being able to generate enough torque or force to turn effectively.
- **Solution:** We solved the error by increasing the PWM (Pulse Width Modulation) value from 40 to 70 in the motor control configuration, as shown in the following snippet of code: 

 ```c
   #define PWM 85

   void Leftturn() 
   {
     set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, PWM);
     set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);
     vTaskDelay(10 / portTICK_PERIOD_MS);
   }
 ```
This PWM adjustment provided the necessary power to the motors,   enable to turns correctly after node detection. 

**3: Bot Keeps Turning, Doesn't Stop**
- **Description:**  When the bot detected a node and starts turning, we added a condition to stop turning when the Line Following Sensor Array (LSA) readings turned white. The error occurred because the turning function was placed within a while loop, and causing the LSA sensor readings to not update as expected. Because of this, the condition is not gets satisfied to stop turning. Because of this bot contnuosly starts taking taking turn, as shown below:
![5-Sep-2023](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/ee23dd8d-a288-46ea-8a92-69dfacfa61a0)

- **Solution:** To solv this, we added a function called get_raw_lsa() to update the LSA sensor readings. as shown in the following snippet of code:  
```c
   Leftturn(){
      while(stop)
      {
        get_raw_lsa();//function to update lsa readings
        set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, PWM);
        set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, PWM);

        if (lsa_reading[2] == 1000) //1000=White
        {
            set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
            set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
            vTaskDelay(20 / portTICK_PERIOD_MS);
            stop=0;
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }  
       }
```
   With this change, the condition was satisfied, allowing the bot to stop turning correctly when LSA readings become white. 

**4: Node Misidentification**

- **Description:** The bot mistakenly identified nodes on straight paths.

- **Solution:** To solve this, we performed PID tuning and added flags to check for nodes. When the left or right line sensors detect a white surface (e.g., 1000), a "check-flag" is set to true (1). This flag rechecks the condition for nodes, and if it is false, no turn will be taken. As shown in the following snippet of code: 
```c
get_raw_lsa();
left_check = 0;
if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) //Left sensor=> lsa_reading[0]
{
    // It detects PLUS NODE & Only Left Node
    printf("leftcheck1\n");
    left_check = 1;
}

if (left_check == 1 )
{
    vTaskDelay(40 / portTICK_PERIOD_MS); // delay
    get_raw_lsa();

    if ((lsa_reading[0] == 1000) && (lsa_reading[1] == 1000) && (lsa_reading[2] == 1000)) // check detected node is node or not.
    {
        left_turn = 1;
    }
    else
    {
        left_check = 0;
    }
}
```
**5: Issue with the Final Run**

- **Description:** The problem occurred during the final run., as the bot followed the directions from the `final_run` array after detecting a node. When the bot detected a node and according to the bot should move straight(forward). However, here's where the problem came up: the bot moved forward for only a 10 milliseconds. During this short time, it was still on the same node, and as a result, it detected a second node at that same node. The robot then followed the new direction from the `final_run` array. But bot can follow only one direction per node to complete final run successfully. So, because of this bot is not able to complete the final run and final run gets destroy. You can see this issue in the following video:
  ![WhatsApp Video 2023-10-28 at 03 53 01_90976d8f (1)](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/b31edc9b-3752-4192-852c-722c57c23473)
- **Solution:** To fix this problem, we made a change in the code. We set a condition that makes the bot keep moving straight until the entire node is end (either the left or right sensor sees a white surface), which marks the end of a node. With this adjustment, the bot now moves straight through the entire node without detecting multiple nodes. This allowed it to complete the final run without any issues. As shown in the following snippet of code:
```c
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
}
```
You can see this change in action in the video below:

![WhatsApp Video 2023-10-28 at 04 04 37_c621a96f (1)](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/45dbe1d9-1780-4b4f-a9ab-4ff0af11b0dc)
# Dijkstra's algorithm

# Printed Circuit Board (PCB) design
## Board images: 
- Front side:
![Screenshot 2023-10-27 172828](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/dcec1c55-7f01-4440-8fc9-856f8971ced0)

- Back side:
![Screenshot 2023-10-27 173618](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/652db1fd-b264-4410-975c-1a318c16de0d)


- Pinout:

## Getting Started with a Mazeblaze Board

## Major Changes for Mazeblaze V3 Board
