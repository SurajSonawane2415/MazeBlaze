#include<stdio.h>
#include<stdlib.h>
#define SIZE 10

void main()
{

    int cost[SIZE][SIZE], distance[SIZE], path[SIZE][SIZE],n,goal_node,p,row,column,min_distance,index=1,i,j;
 
    printf("Enter no of nodes : "); 
    scanf("%d", &n);
    printf("Enter matrix of weights(cost) : ");
    for(i=1;i<=n;i++)
    {
        for(j=1;j<=n;j++)
        {
         scanf("%d", &cost[i][j]);
        }

    }
    printf("Enter node to visit(goal) : ");
    scanf("%d", &goal_node);

    printf("Enter paths for the selected node : ");
    scanf("%d", &p);

    printf("Enter path matrix of that paths \n");
    for (i=1;i<=p;i++)
    {

        for(j=1;j<=n;j++)
        {
         scanf("%d", &path[i][j]);
        }

    } 

    //Imp loop that checks required distance for the given paths(matrix's) from cost matrix 
    for (i=1;i<=p;i++)
    {
        distance[i]=0;
        row=1;
        for (j=1;j<n;j++)
        {
          if (row!=goal_node)
          {
            column=path[i][j+1];
            distance[i] = distance[i]+cost[row][column];
          }
          row=column;
        }  
    }


    //which distance to be considered
    min_distance=distance[1];
    for (i=1;i<=p;i++ )
    {
        if(distance[i]<=min_distance)
        {
            min_distance=distance[i];
            index=i;//index=2 because path 2 have min_distance distance
        }
    }

    //Printing minimum distance and path
    printf("min distance is %d\n",min_distance);
    printf("min distance path is\n");
    for( i=1 ;i<=n;i++) //n=5(number of nodes)
    {
       if (path[index][i]!=0)
       printf("->%d", path[index][i]);//index=2 because path 2 hagoal_nodee min distance
    }
}
