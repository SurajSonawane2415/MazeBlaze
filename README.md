# MazeBlaze
Develop a Line following and Maze Solving bot based on PID controller which can find the shortest path in a given maze.

## Table of Contents

- [Project](#project)
   - [Table of Contents](#table-of-contents)
   - [About The Project](#about-the-project)
   - [PCB MODEL](#pcb-model)
   - [Demo](#demo)
   - [File Structure](#file-structure)
   - [Getting started](#getting-started)
   - [Future Work](#future-work)
   - [Contributors](#contributors)
   - [Resources](#resources)
   - [Acknowledgements](#acknowledgements)
   - [License](#license)

## About The Project
In this project, we will create a Line-following, Maze-solving bot that can find the shortest path in the given maze.

## Domains Explored
Artificial Intelligence, Embedded C, Control Systems, Graph algorithms, Electronics, PCB design.

## Project Workflow
- Learning firmware development by completing [Firmware task](https://github.com/SurajSonawane2415/MazeBlaze/tree/main/task/suraj/firmware). Abd we also understand the firmware used in [SRA Wall-E](https://github.com/SRA-VJTI/Wall-E) and [MazeBlaze-v2](https://github.com/PritK99/MazeBlaze).
- Learning PCB design by completing the line sensor array [pcb_design](https://github.com/SurajSonawane2415/MazeBlaze/tree/main/task/suraj/pcb%20design) task.   
- For bot design Improvement we have created a detailed sheet to make the bot design better. In which we added advantages and disadvantages of many designs we've reviewed and chosen the best design for the MazeBlaze-v3 bot.
- PCB Design for MazeBlaze-v3 by considering all points to make the bot design better.
- Implementing left follow rule.
- Implementing path planning.
- Implementing Dijkstra's algorithm.
- Implementing flood-fill algorithm.
## PCB MODEL
![Screenshot 2023-11-04 191809](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/c39003f3-1a3e-47aa-bcf5-b1e47b84d49c)
## Demo
### LFR algorithm(Left follow rule):

https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/7df0519c-4cf0-41a0-af8f-eac22bb00d04

### Dijkstra's algorithm:

https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/57cf2437-57e8-468f-b269-db0ec31de83d

## File Structure


```
👨‍💻MazeBlaze-v3.1
├ 📂 Firmware
| └ 📂1_left_follow_rule //Contains code for left-follow-rule and path planning
| | ├ 📂main
| | |   ├ 📄1_left_follow_rule.c
| | |   └ 📄CMakeList.txt
| | ├ 📄CMakeList.txt
| ├ 📂2_Dijkstra //Contains code for dijkstra`s algorithm   
| └ 📂Components //Contains all the header and source files used in the project
|   ├ 📂include
|   ├ 📂src
|   ├ 📂esp-wifi-logger
|   ├ 📄CMakeList.txt
├ 📂 assets
├ 📂 documentation //Contains documentation of the project.
├ 📂 pcb_design //Contains pcb_design files of MazeBlaze-v3 board
├ 📂 task
  ├ 📂 Atharv
  | ├ 📂 firmware 
  | |  └📂 1_led_blink.c
  | |    ├ 📂main
  | |    |   ├ 📄1_led_blink.c
  | |    |  └ 📄CMakeList.txt
  | |    ├ 📄CMakeList.txt
  | ├ 📂 pcb_design
  ├ 📂 Suraj
    ├ 📂 firmware //Conatins led blink task files and documentation of the task
    |  └📂 1_led_blink.c 
    |    ├ 📂main
    |    |   ├ 📄1_led_blink.c
    |    |   └ 📄CMakeList.txt
    |    ├ 📄CMakeList.txt
    |    ├ 📄README.md
    ├ 📂 pcb_design //Conatins lsa PCB design task files and documentation of the task
```
## Getting started
### Prerequisites
To download and use this code, the minimum requirements are:
- ESP_IDF
- Windows 7 or later (64-bit), Ubuntu 20.04 or later
- Microsoft VS Code or any other IDE

### Installation
Clone the project by typing the following command in your Terminal/CommandPrompt:
      
     `git clone https://github.com/SurajSonawane2415/MazeBlaze`

Navigate to the MazeBlaze folder:
`cd MazeBlaze`

### Usage

Once the requirements are satisfied, you can easily download the project and use it on your machine. After following the above steps, use the following commands to:

To activate the IDF:
`get_idf`

To build the code:
`idf.py build`

To flash the code:
`idf.py -p (PORT) flash monitor`

## Future Work
- [ ] Implementing better algorithms like DFS/BFS.
- [ ] Achieve higher speeds by increasing the efficiency of the bot.

## Contributors
- [Suraj Sonawane](https://github.com/SurajSonawane2415)
- [Atharva Nerkar](https://github.com/ARN1954)

## Resources
- [MazeBlaze-v2](https://github.com/PritK99/MazeBlaze) GitHub repository of MazeBlaze-v2 
- [MazeBlaze-v1](https://github.com/ChinmayLonkar/MAZEBLAZE) GitHub repository of MazeBlaze-v1
- [Dikstra's](https://www-geeksforgeeks-org.cdn.ampproject.org/v/s/www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/amp/?usqp=mq331AQIUAKwASCAAgM%3D&amp_js_v=a9&amp_gsa=1#referrer=https%3A%2F%2Fwww.google.com&csi=1&ampshare=https%3A%2F%2Fwww.geeksforgeeks.org%2Fprinting-paths-dijkstras-shortest-path-algorithm%2Famp%2F%23referrer%3Dhttps%253A%252F%252Fwww.google.com%26csi%3D1%26ampshare%3Dhttps%253A%252F%252Fwww.geeksforgeeks.org%252Fprinting-paths-dijkstras-shortest-path-algorithm%252F) Implementation of dijkstr's algorithm in c.
- [Floodfill](https://github.com/Isuru-Dissanayake/piccola) Implementation of floodfill algorithm (simulation).

## Acknowledgements 
- [SRA VJTI](https://sravjti.in/) Eklavya 2023
- Special thanks to our mentors [Raj Gupta](https://github.com/RajGupta17), [Pritk Kanadiya](https://github.com/PritK99), and all the seniors at SRA, VJTI for their constant support and guidance throughout the project.

## License
[MIT License](https://opensource.org/license/mit/)
