# PCB Design Task:
In this task I designed a PCB for line sensor array(LSA) used in [SRA board](https://github.com/SRA-VJTI/sra-board-hardware-design). 
## Board images:
- Front side:
  ![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/fa1b6c5a-2b7d-4a52-9643-440c6fd58297)
- Back side:
  ![Screenshot 2023-11-03 041715](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/99374659-6f9f-4f76-81ba-6fc47b1aad6a)

## Routing:
For routing of the control circuitry and power connections we used traces of 0.3mm width.
- Back copper routing
![Screenshot 2023-11-03 042303](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/47dc7535-cadf-42c2-8545-2d86c358180f)

## Overview:
The Line Sensor Array (LSA) uses TCRT 5000 sensors to detect white and black lines on the surface. It then sends this data to the ESP-32 microcontroller. By analyzing and processing the data, the microcontroller ESP-32 accomplishes the task of line following.

## TCRT 5000
The TCRT 5000 is an optical sensor module commonly used for detecting the presence of objects or lines. It consists of an infrared (IR) emitter and a phototransistor. The emitter emits infrared light, which bounces off objects or surfaces and is detected by the phototransistor. The amount of reflected light is used to determine the proximity of an object or the presence of a surface, typically a white or black line. TCRT 5000 sensors are often employed in applications like line-following bot.

![image](https://github.com/SurajSonawane2415/MazeBlaze/assets/129578177/784ed429-094d-4121-8185-cedd5f6a5d82)
