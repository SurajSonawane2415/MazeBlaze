# MazeBlaze
Develop a Line following and Maze Solving bot based on PID controller which can find the shortest path in a given maze.

## Table of Contents

- [Project](#project)
   - [Table of Contents](#table-of-contents)
   - [About The Project](#about-the-project)
   - [Demo](#demo)
   - [File Structure](#file-structure)
   - [Getting started](#getting-started)
   - [Future Work](#future-work)
   - [Contributors](#contributors)
   - [Resources](#resources)
   - [Acknowledgements](#acknowledgements)
   - [License](#license)

## About The Project
In this project, we will create a Line-following, Maze-solving bot which can find the shortest path in the given maze.

## Domains Explored
 Artificial Intelligence, Embedded C, Control Systems, Graph algorithms, Electronics, PCB design.

## Project Workflow
- Learn to design PCB and understand the design of PCB used in MazeBlaze bot
- Implementing left follow rule
- Implementing path planing
- Implementing Dijsktra's Algorithm
- Implementing Flood Fill Algorithm
  
## Demo

## File Structure

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

Once the requirements are satisfied, you can easily download the project and use it on your machine. After following the above steps , use the following commands to:

To activate the IDF:
`get_idf`

To build the code:
`idf.py build`

To flash the code:
`idf.py -p (PORT) flash monitor`

## Future Work
- [ ] Implementing better algorithms like DFS/BFS.
- [ ] Achive higher speeds by increasing the efficiency of bot.

## Contributors
- [Suraj Sonawane](https://github.com/SurajSonawane2415)
- [Atharva Nerkar](https://github.com/ARN1954)

## Resources
- [MazeBlaze-v2](https://github.com/PritK99/MazeBlaze) Github repository of MazeBlaze-v2 
- [MazeBlaze-v1](https://github.com/ChinmayLonkar/MAZEBLAZE) Github repository of MazeBlaze-v1
- [Dikstra's](https://www-geeksforgeeks-org.cdn.ampproject.org/v/s/www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/amp/?usqp=mq331AQIUAKwASCAAgM%3D&amp_js_v=a9&amp_gsa=1#referrer=https%3A%2F%2Fwww.google.com&csi=1&ampshare=https%3A%2F%2Fwww.geeksforgeeks.org%2Fprinting-paths-dijkstras-shortest-path-algorithm%2Famp%2F%23referrer%3Dhttps%253A%252F%252Fwww.google.com%26csi%3D1%26ampshare%3Dhttps%253A%252F%252Fwww.geeksforgeeks.org%252Fprinting-paths-dijkstras-shortest-path-algorithm%252F) Implemnetation of dijkstr's algorithm in c.
- [Floodfill](https://github.com/Isuru-Dissanayake/piccola) Implementation of floodfill algorithm (simulation).

## Acknowledgements 
- [SRA VJTI](https://sravjti.in/) Eklavya 2023
- Special thanks to our mentors [Raj Gupta](https://github.com/RajGupta17) , [Pritk Kannadiya](https://github.com/PritK99)  and all the seniors at SRA, VJTI for their constant support and guidance throughout the project.

## License

