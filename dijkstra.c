#include <stdio.h>
#include <stdlib.h>

#define INFINITY 999
#define INDEX_MAX_SIZE 50

//VISITED AND PARRENT MATRIX IS USE WITHOUT PATH MATRIX
void dijkstra(int n, int cost[INDEX_MAX_SIZE][INDEX_MAX_SIZE], int s, int dist[10], int parent[10])
{
    //n=no of nodes
    int i, vertex, count = 1, min, visited[INDEX_MAX_SIZE];

    for (i = 1; i <= n; i++)
    {
        visited[i] = 0;
        dist[i] = cost[s][i];
        parent[i] = s; //parrent array to print shortest path
    }
    visited[s] = 1;
    dist[s] = 0;

    while (count < n) {
        min = INFINITY;
        for (i = 1; i <= n; i++)
        {
            if (dist[i] < min && !visited[i]) {
                min = dist[i];
                vertex = i;
            }
        }
        visited[vertex] = 1;
        count++;
        for (i = 1; i <= n; i++) {
            if (dist[i] > dist[vertex] + cost[vertex][i]) {
                dist[i] = dist[vertex] + cost[vertex][i];
                parent[i] = vertex; // Update parent array
            }
        }
    }
}

int main()
{
    int i, j, n, s, e, cost[INDEX_MAX_SIZE][INDEX_MAX_SIZE], dist[INDEX_MAX_SIZE], parent[INDEX_MAX_SIZE];
    printf("Enter the total number of nodes: ");
    scanf("%d", &n);
    printf("Read the cost matrix:\n");
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= n; j++) {
            scanf("%d", &cost[i][j]);
            if (cost[i][j] == 0)
                cost[i][j] = INFINITY;
        }
    }
    printf("Enter the source vertex: ");
    scanf("%d", &s);
    printf("Enter the end vertex: ");
    scanf("%d", &e);

    dijkstra(n, cost, s, dist, parent);

    printf("Shortest path from %d to %d is:\n", s, e);

    // Store the path in an array
    int path[10];
    int current = e;
    int path_length = 0;

    while (current != s) {
        path[path_length++] = current;
        current = parent[current];
    }
    path[path_length++] = s;

    // Print the path in the correct order
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) {
            printf(" --> ");
        }
    }

    printf("\nShortest distance: %d\n", dist[e]);

    return 0;
}
