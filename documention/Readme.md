# MazeBlaze-v3.1

# TABLE OF CONTENTS
- [Left Follow Rule](#left-follow-rule-lfr)
  - [Theory](#theory)
  - [Implementation](#implementation)
  - [Example](#example)
  - [Description Of Functions Used](#description-of-functions-used)
  - [Error Descriptions and Solution](#error-descriptions-and-solution)

- [Dijkstra's algorithm](#dijkstra's-algorithm)
- [Printed Circuit Board Design](#printed-circuit-board-design)
  - [Board Images](#board-images)
  - [Routing](#routing)
  - [Features of PCB](#features-of-pcb)
  - [Major Changes for Mazeblaze V3 Board](#major-changes-for-mazeblaze-v3-board)



# Left Follow Rule (LFR)
## Theory
The Left-Hand Rule (LFR), also known as the "left-follow-algorithm," is a maze solving technique used to navigate through 2D mazes. To use LFR, you start at the entrance of a maze with your left hand touching the wall to your left. As you navigate through the maze, you follow a set of rules:

1. **Always turn left:** Whenever you reach an intersection or have a choice of paths, you make a left turn if it's possible, keeping your left hand on the wall.

2. **Go straight if a left turn is blocked:** If you come to an intersection where turning left is not an option because it leads to a wall, you continue moving straight while still touching the left wall.

3. **Take a right turn as a last resort:** If you encounter an intersection where you cannot turn left or go straight (usually because of a wall or obstacle), you take a right turn.

4. **Reverse when there are no other options:** If you find yourself in a situation where you can't turn left, go straight, or turn right, it means you've likely reached a dead end. In this case, you need to turn around and backtrack.

## Implementation
For the implementation of left follow rule, we need to record every turn taken by the bot by assigning numeric values to different directions. Then we need to recognize the redundant paths from the given turns.
The directions are indexed as follows:
- North: 1
- East: 2
- South: 3
- West: 4

It is important to note that the algorithm assumes that the bot initially faces the North direction. Subsequently, the other directions are determined based on the North direction. These directions are fixed with respect to the world frame, and the bot continually updates its direction as it takes turns. We records the direction index into the `store_path` array, as follows:
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

When the bot moves and follows these rules, it records its current direction in the store_path array. This helps us keep track of where the bot has been. The bot always knows which direction it's facing, even if it takes many turns.

**Code to store this directions in dry_run array:**
```c
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
//store_path[0] = 1 (The diection which bot faces initially, is always North)
```
## Example 
![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/302faa35-240e-46ec-956d-5b14c28de9ba)

Thus, for our maze, Assuming starting position as North i.e. 1, The array store_path[ ] will store the following,

```store_path[ ] = {1, 4, 1, 2, 1, 4, 2, 1, 4, 1, 4, 2, 1, 2, 1, 4, 3, 1, 4, 3, 1, 4, 3, 2, 3, 2, 4, 3, 2, 3, 3, 4, 4, 1, 4, 2, 1, 4, 2, 1, 4, 2, 1, 4, 2, 1, 4, 2, 1}```

Here, the redundant paths are highlighted in bold

store_path[ ] = {1, 4, 1, 2, 1, **4, 2,** 1, 4, 1, **4, 2,** 1, 2, 1, 4, **3, 1,** 4, **3, 1,** 4, 3, 2, 3, **2, 4,** 3, 2, 3, 3, 4, 4, 1, **4, 2,** 1, **4, 2,** 1, **4, 2,** 1, **4, 2,** 1, **4, 2,** 1}

Thus, when we call simply_path(), it removes all these redundancies and stores the shortest path in an array called final_run[ ].

`final_run[ ] = {1,4,1,2,1,1,4,1,1,2,1,4,4,4,3,2,3,3,2,3,3,4,4,1,1,1,1,1,1}`

Thus, we make use of simplify_path() function in our code to remove all the redundancies from store_path[] and stores the shortest path in another array called final_run[].

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
- **Description:** In the above image, the "Task watchdog got triggered" error message appeared when we flashed our code on the ESP. This error occurs due to the watchdog mechanism in the ESP. The watchdog is like a timer that ensures tasks complete within their expected time. If a task takes too long to execute, it suggests that there might be a problem with task timing.
The error was triggered because a specific task in our code was exceeding its expected execution time, causing the watchdog to trigger.
- **Solution:** To solve this issue, we added a delay of 10 milliseconds (vTaskDelay(10 / portTICK_PERIOD_MS)) after a specific function. As shown in the following snippet of code: 

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
This allowed the task to complete within its designated time frame, preventing the watchdog from triggering.

**2:  Unable to turn after detecting a node**
- **Description:** When bot detects the node after that bot didn't taking turn. This error was ocuure because the bot's PWM for turn was 40, which was insufficient to turn, pwm should be grater than 55 for our case. Because when the PWM value was set to 40, it likely provided a relatively low average power to the motors, which resulted in the bot's motors not being able to generate enough torque or force to turn effectively.

![nt-gif](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/67c8f461-ac9d-493d-bc50-9db837a9c552)
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
   With this change, the condition was satisfied, allowing the bot to stop turning correctly when LSA readings become white. As shown in following video: 

![6-Sep-2023](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/9a9088af-e64c-49ed-b58f-e84a574bcb94)

**4: Node Misidentification**

- **Description:** The bot mistakenly identified nodes on straight paths. & after detecting a node bot starts taking. As you can see in video below:

![pt-gif](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/6f8ffcfd-d173-41ee-9bf7-effe1dbfdb06)

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
After doing all these changes, Bot not detecting nodes on straight path, even if bot deviates from straight line. As you can see in video below:

![7-SEP-2023](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/48521adf-bd28-4f06-8dad-ac21e1278b31)

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
## Theory

Dijkstra's algorithmis an algorithm for finding the shortest paths between nodes in a weighted graph.The algorithm maintains a set of visited vertices and a set of unvisited vertices. It starts at the source vertex and iteratively selects the unvisited vertex with the smallest tentative distance from the source. It then visits the neighbors of this vertex and updates their tentative distances if a shorter path is found. This process continues until the destination vertex is reached, or all reachable vertices have been visited.

## Example

Suppose we have to find the shortest distance from node 0 to node 6. We would follow this steps.

**Step 1:** Start from Node 0 and mark Node as visited as you can check in below image visited Node is marked red.

![Screenshot from 2023-10-31 23-51-32](https://github.com/SurajSonawane2415/MazeBlaze/assets/132119263/66f84c3e-14ec-4144-a542-676d0912c108)



**Step 2:** Check for adjacent Nodes, Now we have to choices (Either choose Node1 with distance 2 or either choose Node 2 with distance 6 ) and choose Node with minimum distance. In this step Node 1 is Minimum distance adjacent Node, so marked it as visited and add up the distance.
Distance: Node 0 -> Node 1 = 2

![Screenshot from 2023-10-31 23-51-51](https://github.com/SurajSonawane2415/MazeBlaze/assets/132119263/7a2faa39-b76c-4a47-b47f-18301ea2b37d)




**Step 3:** Then Move Forward and check for adjacent Node which is Node 3, so marked it as visited and add up the distance, Now the distance will be:
Distance: Node 0 -> Node 1 -> Node 3 = 2 + 5 = 7

![Screenshot from 2023-10-31 23-52-10](https://github.com/SurajSonawane2415/MazeBlaze/assets/132119263/5e6ee0e4-f5aa-45e8-b4c9-6544f0b755c6)





**Step 4:** Again we have two choices for adjacent Nodes (Either we can choose Node 4 with distance 10 or either we can choose Node 5 with distance 15) so choose Node with minimum distance. In this step Node 4 is Minimum distance adjacent Node, so marked it as visited and add up the distance.
Distance: Node 0 -> Node 1 -> Node 3 -> Node 4 = 2 + 5 + 10 = 17

![Screenshot from 2023-10-31 23-52-29](https://github.com/SurajSonawane2415/MazeBlaze/assets/132119263/d1aa3257-d4ae-4273-8042-549f5d6bbf98)





**Step 5:** Again, Move Forward and check for adjacent Node which is Node 6, so marked it as visited and add up the distance, Now the distance will be:
Distance: Node 0 -> Node 1 -> Node 3 -> Node 4 -> Node 6 = 2 + 5 + 10 + 2 = 19

![Screenshot from 2023-10-31 23-52-43](https://github.com/SurajSonawane2415/MazeBlaze/assets/132119263/b178af8a-8ce4-4b2d-82f8-a391c9b9fb01)






## Implementation
**Step I -** First we intialize 3 arrays:-
a)dist[] is an array to store the shortest distance from the source vertex to each vertex in the graph.
b)sptSet[] is a set to keep track of vertices visited in the shortest path(VISITED_NODES).
c)parent[] is an array to store the parent (predecessor) of each vertex in the shortest path.

**Step II -** Then make all elements of dist[]=infinity, sptSet[]=0, parent[]=-1.

**Step III -** 



   ```c
   for (int i = 0; i < V; i++) {
    dist[i] = INT_MAX;
    sptSet[i] = 0;
    parent[i] = -1;
    }

    dist[sourceVertex - 1] = 0;

    for (int count = 0; count < V - 1; count++) {
    int minDist = INT_MAX;
    int minIndex = -1;

    for (int v = 0; v < V; v++) {
    if (sptSet[v] == 0 && dist[v] < minDist) {
    minDist = dist[v];
    minIndex = v;
    }
    }

    sptSet[minIndex] = 1;

    for (int v = 0; v < V; v++) {
    if (!sptSet[v] && costMatrix[minIndex][v] && dist[minIndex] != INT_MAX &&
    dist[minIndex] + costMatrix[minIndex][v] < dist[v]) {
    dist[v] = dist[minIndex] + costMatrix[minIndex][v];
    parent[v] = minIndex;
    }
    }
    }
   ```


With help of above code we update the dist array by storing the values which are the shortest distance of a node from source.
Here first we make the distance of source to source 0 and then update sptSet and parent array.Then in the last for loop we check the nearest node to source and update all other arrays.Then  again go to last loop and find the nearest node from the last and store the distance between current 2 nodes + the distance travelled before.Continues this till we reach the last node.

Finally we get the shortest path stored in parent array and shortest distance stored in th edistance array at index value = the end point.

**Step IV -** 


```c
for (int i = 0; i < size; i++)
    {
    if (finalPathArr[i + 1] - finalPathArr[i] > 1)
    {
    z = (finalPathArr[i + 1] - finalPathArr[i]) - 1;
    for (int j = 0; j < z; j++)
    {
    miss[a] = finalPathArr[i] + c;
    c++;
    a++;
    }
    c=1;
    }
    }
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
  ```


With the help of above code we remove the direction that the bot needs to take to reach the nodes that are not included in shortest path.And store the directions in final_run that the bot needs to take to follow the shortest path.


## Functions

```c
void dijkstra()
```


It is used to find shortest path from source to end point and distance also of the shortest path.




```c
void final_traversal()
```


It is used to make the bot travel along the shortest path.




```c
int dir[] = {1,4,1,2,2,1,4,1,4,1,2,1,1,4,1,4,1,4,3,4,3,4,3,2,3,2,3,4,4,2,3,4,3,4,4,1,1,4,4,1,4,1,4,1,4,1,4,1};
```



In it we have stored the direction that the bot takes to reach a node.





# Printed Circuit Board (PCB) design
## Board images: 
- Front side:

![Screenshot 2023-10-29 195127](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/b979a5ff-222a-4ef1-8e41-d45e825f398c)

- Back side:

![Screenshot 2023-10-29 195227](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/3a02d37d-c7a0-4a80-b833-446d12417579)


##  Routing:
For routing of the control circuitry we used traces of 0.3mm width, for power connections we used 1.5mm (12v) & 1mm (5v), and for motor control circuitry we used  0.6mm width tracks to ensure fairly high current flow.

- Front copper routing

![Screenshot 2023-10-29 200856](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/4f1f479b-ee07-40e1-a0a1-c2c04795312b)

- Back copper kayer:

![Screenshot 2023-10-29 200916](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/82cc5250-dfe9-4450-9f8b-e51b40d1fe28)

- Final routing:

![Screenshot 2023-10-29 200954](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/a1f907a0-4dc7-4782-9e0a-3ae237c5ad30)

## Major components used in the PCB include -

| **Components** | **Description** |
| -------- | -------- |
| ESP32-WROOM-32E    | Microcontroller   |
| TB6612FNG    | Motor Driver   |
| QTR-8RC    | Line following reflectance IR sensor array   |
| Orange 11.1V 1000mAh 40C 3S   | Lithium Polymer Battery   | 
|IC LM2576-S-5| Buck Regulator|
|N20-12V-300 Rpm| Motor|

### Power Supply Unit :

![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/ffb9656b-686d-4be8-9eab-ddcb32443d58)

Microcontrollers (MCUs) typically operate on 3.3V or 5V logic supply voltage, while the input voltage from Li-Po battery for our Mazeblaze board's motor driver is 12V.    
So, in order to have a single input source, a power section which inter converts this 12V to 5V for MCU. This is achieved by using a step-down buck regulator.
Buck Regulator IC LM2576-S-5 is used for stepping down the voltage from 12V to 5V DC.
  
### Motor Driver :

![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/8b1e6dc1-485b-48af-8c17-70827683a810)

Motors typically run on a 12V power supply, while microcontrollers (MCUs) usually output signals at either 5V or 3.3V. To make these work together, an additional external motor driver circuit is used to control motors according to the MCU input. The current and previous versions of the MazeBlaze board feature the TB6612FNG Motor Driver, a high-performance MOS-based H-Bridge motor driver.

### Protection against Reverse Voltage :
  
In the MazeBlaze Board, we use diodes to shield against reverse voltage in the power line. 12V Motor line, MCU and buck regulator have been separated with SS34 and SS24 schottky diodes respectively.

### QTR-8RC line following reflectance IR sensor array :
  
![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/58559787-ffc2-460e-8e8d-c81b8f5e5df7)

The QTR-8RC line following reflectance IR sensor array is used to detect the white/black lined surface and it sends this data to the microcontroller ESP-32. Analyzing and processing the data received by the microcontroller ESP-32, the purpose of line following and node detection is achieved. We use this sensor because it provides precise and reliable line detection, enhancing accuracy and efficiency in tasks like line following and maze navigation. Its infrared sensor ensures stable performance across various surface conditions.

### Orange 11.1V 1000mAh 40C 3S Lithium Polymer Battery :

![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/89102630-e579-4126-8b0e-1be5089a3aa0)

In mazeblaze board we use this battery as power source for motors & microcontroller. This battery is operating at 11.1 volts with a 1000mAh capacity, it provides a reliable and sustained power supply. Its high discharge rate of 40C ensures the ability to meet quick and intense power demands, making it an excellent choice for fast maze solving task.

## Major Changes for Mazeblaze V3 Board
| Features | MazeBlaze-V1 | MazeBlaze-V3 |
| -------- | -------- | -------- |
| Sensor   | Line sensor array(LSA)| QTR-8RC line following reflectance IR sensor array |
| Sensor Placement |on the main PCB and near to the wheels|Placed apart from the main PCB, the sensor can be adjustable and not closed to the wheels.|
| Battery   | 3.7v Li-Po battery |11.1V 1000mAh 40C 3S Li-Po Battery |
| Power Supply Unit| MT3608 Step UP Boost Converter is used for stepping up the voltage from 3.7V DC to 6V.|Buck Regulator IC LM2576-S-5 is used for stepping down the voltage from 12V to 5V DC|

MT3608 Step UP Boost Converter to LM2576/96 Buck Convertor
In MazeBlaze we used 3.7 v battery supply for motors so thats why we need boost converter that converts 3.7v to 6v. In this MazeBlaze-V3 board we are using 11.1 V so we dosent have any need of boost converter. but for MCU(ESP) we need 5v so we used buck converter
The greater efficiency, output current and reliability of LM2576/96 were the reasons for this change. The efficiency of LM2576 is up to 92%. So we it used for stepping down the voltage from 12V to 5V DC
