# MazeBlaze-v3.1
## Left Follow Rule (LFR)
### Theory
The Left-Hand Rule (LFR), also known as the "left-follow-algorithm," is a maze solving technique used to navigate through 2D mazes. To use LFR, you start at the entrance of a maze with your left hand touching the wall to your left. As you navigate through the maze, you follow a set of rules:

1. **Always turn left:** Whenever you reach an intersection or have a choice of paths, you make a left turn if it's possible, keeping your left hand on the wall.

2. **Go straight if a left turn is blocked:** If you come to an intersection where turning left is not an option because it leads to a wall, you continue moving straight while still touching the left wall.

3. **Take a right turn as a last resort:** If you encounter an intersection where you cannot turn left or go straight (usually because of a wall or obstacle), you take a right turn.

4. **Reverse when there are no other options:** If you find yourself in a situation where you can't turn left, go straight, or turn right, it means you've likely reached a dead end. In this case, you need to turn around and backtrack.

The Left-Hand Rule ensures that you explore the entire maze systematically while keeping your left hand in contact with the wall. It guarantees that you will eventually find your way out of the maze, provided there is a continuous path from the entrance to the exit. However, it may not work in mazes where the entrance and exit are not connected by walls, potentially causing you to loop back to the starting point. 
