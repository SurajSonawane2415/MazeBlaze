#include <stdio.h>
int x=0, y=0, orient=0, cell = 0;
char turning;
int que[100];
int H=8, W=8; //Height and width of our maze

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
        if (cells[y][x] == 15) {
              return 1; // True
         } */

        // 0==>false 1==>true
        if (x == x1) {
            if (y > y1) {
                if (cells[y][x] == 4 || cells[y][x] == 5 || cells[y][x] == 6 || cells[y][x] == 10 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 14 || cells[y1][x1] == 2 || cells[y1][x1] == 7 || cells[y1][x1] == 8 || cells[y1][x1] == 10 || cells[y1][x1] == 12 || cells[y1][x1] == 13 || cells[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            } else {
                if (cells[y][x] == 2 || cells[y][x] == 7 || cells[y][x] == 8 || cells[y][x] == 10 || cells[y][x] == 12 || cells[y][x] == 13 || cells[y][x] == 14 || cells[y1][x1] == 4 || cells[y1][x1] == 5 || cells[y1][x1] == 6 || cells[y1][x1] == 10 || cells[y1][x1] == 11 || cells[y1][x1] == 12 || cells[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            }
        } else if (y == y1) {
            if (x > x1) {
                if (cells[y][x] == 1 || cells[y][x] == 5 || cells[y][x] == 8 || cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 13 || cells[y][x] == 14 || cells[y1][x1] == 3 || cells[y1][x1] == 6 || cells[y1][x1] == 7 || cells[y1][x1] == 9 || cells[y1][x1] == 11 || cells[y1][x1] == 12 || cells[y1][x1] == 13) {
                    return 0; 
                } else {
                    return 1; 
                }
            } else {
                if (cells[y][x] == 3 || cells[y][x] == 6 || cells[y][x] == 7 || cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 13 || cells[y1][x1] == 1 || cells[y1][x1] == 5 || cells[y1][x1] == 8 || cells[y1][x1] == 9 || cells[y1][x1] == 11 || cells[y1][x1] == 13 || cells[y1][x1] == 14) {
                    return 0;
                } else {
                    return 1; 
                }
            }
        }

        return 0; // False bydefault
    }


    return 0;
}
