#include <stdio.h>
int x=0, y=0, orient=0, cell = 0;
int x0, y0, x1, y1, x2, y2, x3, y3;
char turning;
int que[100];
int H=8, W=8; //Height and width of our maze
#define SIZE 8 

int main() { //Flood 2D array of our maze at unknown walls/nodes condition
    int floodfill_array[H][W] = {
        {2, 1, 2, 3, 4, 5, 6, 7},
        {1, 0, 1, 2, 3, 4, 5, 6},
        {2, 1, 2, 3, 4, 5, 6, 7},
        {3, 2, 3, 4, 5, 6, 7, 8},
        {4, 3, 4, 5, 6, 7, 8, 9},
        {5, 4, 5, 6, 7, 8, 9, 10},
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
    printf("floodfill_array[0][0] = %d\n", floodfill_array[0][0]); // Should print 2
    printf("floodfill_array[3][4] = %d\n", floodfill_array[3][4]); // Should print 5
    
    int direction(orient, turning) //update the direction according to the turn taking
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
    
    void updateCoordinates(int *x, int *y, int orient) {
        //update the present co ordinate or present position of bot in maze/graph
        if (orient == 0) (*y)++;
        if (orient == 1) (*x)++;
        if (orient == 2) (*y)--;
        if (orient == 3) (*x)--;
    }

    void updateNodes(x, y, orient, L, R, F, nodes[H][W]){
        //this function updated the type of node
        if (L && R && F) {
            if (orient == 0) {
                nodes[y][x] = 13;
            } else if (orient == 1) {
                nodes[y][x] = 12;
            } else if (orient == 2) {
                nodes[y][x] = 11;
            } else if (orient == 3) {
                nodes[y][x] = 14;
            }
        } else if (L && R && !F) {
            if (orient == 0 || orient == 2) {
                nodes[y][x] = 9;
            } else if (orient == 1 || orient == 3) {
                nodes[y][x] = 10;
            }
        } else if (L && F && !R) {
            if (orient == 0) {
                nodes[y][x] = 8;
            } else if (orient == 1) {
                nodes[y][x] = 7;
            } else if (orient == 2) {
                nodes[y][x] = 6;
            } else if (orient == 3) {
                nodes[y][x] = 5;
            }
        } else if (R && F && !L) {
            if (orient == 0) {
                nodes[y][x] = 7;
            } else if (orient == 1) {
                nodes[y][x] = 6;
            } else if (orient == 2) {
                nodes[y][x] = 5;
            } else if (orient == 3) {
                nodes[y][x] = 8;
            }
        } else if (F) {
            if (orient == 0) {
                nodes[y][x] = 2;
            } else if (orient == 1) {
                nodes[y][x] = 3;
            } else if (orient == 2) {
                nodes[y][x] = 4;
            } else if (orient == 3) {
                nodes[y][x] = 1;
            }
        } else if (L) {
            if (orient == 0) {
                nodes[y][x] = 1;
            } else if (orient == 1) {
                nodes[y][x] = 2;
            } else if (orient == 2) {
                nodes[y][x] = 3;
            } else if (orient == 3) {
                nodes[y][x] = 4;
            }
        } else if (R) {
            if (orient == 0) {
                nodes[y][x] = 3;
            } else if (orient == 1) {
                nodes[y][x] = 4;
            } else if (orient == 2) {
                nodes[y][x] = 1;
            } else if (orient == 3) {
                nodes[y][x] = 2;
            }
        } else {
            nodes[y][x] = 15;
        }
    }

    int isAccessible(int x, int y, int x1, int y1) {
        /*this function checks whether a bot can move from cell (x, y) to cell (x1, y1) in a grid..
          the (x, y) are the current coordinates.. and the target coordinates are (x1, y1) in a grid*/
        
        /* to check a specific condition.
        if (nodes[y][x] == 15) {
              return 1; // True
         } */

        // 0==>false 1==>true
        if (x == x1) {
            if (y > y1) {
                if (nodes[y][x] == 4 || nodes[y][x] == 5 || nodes[y][x] == 6 || nodes[y][x] == 10 || nodes[y][x] == 11 || nodes[y][x] == 12 || nodes[y][x] == 14 || nodes[y1][x1] == 2 || nodes[y1][x1] == 7 || nodes[y1][x1] == 8 || nodes[y1][x1] == 10 || nodes[y1][x1] == 12 || nodes[y1][x1] == 13 || nodes[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            } else {
                if (nodes[y][x] == 2 || nodes[y][x] == 7 || nodes[y][x] == 8 || nodes[y][x] == 10 || nodes[y][x] == 12 || nodes[y][x] == 13 || nodes[y][x] == 14 || nodes[y1][x1] == 4 || nodes[y1][x1] == 5 || nodes[y1][x1] == 6 || nodes[y1][x1] == 10 || nodes[y1][x1] == 11 || nodes[y1][x1] == 12 || nodes[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            }
        } else if (y == y1) {
            if (x > x1) {
                if (nodes[y][x] == 1 || nodes[y][x] == 5 || nodes[y][x] == 8 || nodes[y][x] == 9 || nodes[y][x] == 11 || nodes[y][x] == 13 || nodes[y][x] == 14 || nodes[y1][x1] == 3 || nodes[y1][x1] == 6 || nodes[y1][x1] == 7 || nodes[y1][x1] == 9 || nodes[y1][x1] == 11 || nodes[y1][x1] == 12 || nodes[y1][x1] == 13) {
                    return 0; 
                } else {
                    return 1; 
                }
            } else {
                if (nodes[y][x] == 3 || nodes[y][x] == 6 || nodes[y][x] == 7 || nodes[y][x] == 9 || nodes[y][x] == 11 || nodes[y][x] == 12 || nodes[y][x] == 13 || nodes[y1][x1] == 1 || nodes[y1][x1] == 5 || nodes[y1][x1] == 8 || nodes[y1][x1] == 9 || nodes[y1][x1] == 11 || nodes[y1][x1] == 13 || nodes[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            }
        }

        return 0; // False bydefault
    }
    
    
   void getSurrounds(int x, int y, int *x0, int *y0, int *x1, int *y1, int *x2, int *y2, int *x3, int *y3) {
        /*It calculates the coordinates of the 4 neighboring nodes:

        (x0, y0) are at north of (x, y).
        (x1, y1) are at east of (x, y).
        (x2, y2) are at south of (x, y).
        (x3, y3) are at west of (x, y).*/
        
        // North
        *x0 = x;
        *y0 = y + 1;

        // East
        *x1 = x + 1;
        *y1 = y;

        // South
        *x2 = x;
        *y2 = y - 1;

        // West
        *x3 = x - 1;
        *y3 = y;

        // Check and handle grid boundaries (wrap-around to -1 if needed).
        if (*x1 >= SIZE) {
            *x1 = -1;
        }
        if (*y0 >= SIZE) {
            *y0 = -1;
        }
    }

    //1st fllodfill algorithm process
    void changeDestination(int maze[H][W], int destinationx, int destinationy) {
        /*change the destination coordinates for the maze traversal. 
          that is the minimum manhattan distance to reach from the each cell to goal cell*/
        int i, j;

        for (j = 0; j < W; j++) {
            for (i = 0; i < H; i++) {
                flood[i][j] = 255;
            }
        }

        int queue[2 * H * W];
        maze[destinationy][destinationx] = 0; //destination is set as 0

        queue[0] = destinationy;
        queue[1] = destinationx;

        while (queue[0] != 0) {
            int yrun = queue[0];
            int xrun = queue[1];
            queue[0] = 0; // Pop the front element
            queue[1] = 0; // Pop the front element

            int x0, y0, x1, y1, x2, y2, x3, y3;
            getSurrounds(xrun, yrun, &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);

            if (x0 >= 0 && y0 >= 0) {
                if (maze[y0][x0] == 255) {
                    maze[y0][x0] = maze[yrun][xrun] + 1;
                    queue[0] = y0;
                    queue[1] = x0;
                }
            }
            if (x1 >= 0 && y1 >= 0) {
                if (maze[y1][x1] == 255) {
                    maze[y1][x1] = maze[yrun][xrun] + 1;
                    queue[0] = y1;
                    queue[1] = x1;
                }
            }
            if (x2 >= 0 && y2 >= 0) {
                if (maze[y2][x2] == 255) {
                    maze[y2][x2] = maze[yrun][xrun] + 1;
                    queue[0] = y2;
                    queue[1] = x2;
                }
            }
            if (x3 >= 0 && y3 >= 0) {
                if (maze[y3][x3] == 255) {
                    maze[y3][x3] = maze[yrun][xrun] + 1;
                    queue[0] = y3;
                    queue[1] = x3;
                }
            }
        }
    
    }

    return 0;
}
