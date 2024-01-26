#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void HW3(int SOURCE, int n, int **edge, int *distance) {
    int i, j, count, leastPos;
    int *found = (int *)calloc(n, sizeof(int));

    // Initialize the distance array in parallel
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        found[i] = 0;
        distance[i] = edge[SOURCE][i];
    }

    found[SOURCE] = 1;
    count = 1;

    while (count < n) {
        int least = 987654321;
        leastPos = -1;

        // Find the vertex with the least distance in parallel
        #pragma omp parallel for
        for (i = 0; i < n; i++) {
            if (!found[i]) {
                int private_tmp = distance[i]; // Private variable for each thread
                #pragma omp critical
                {
                    if (private_tmp < least) {
                        least = private_tmp;
                        leastPos = i;
                    }
                }
            }
        }

        found[leastPos] = 1;
        count++;

        // Update distances to other vertices in parallel
        #pragma omp parallel for
        for (i = 0; i < n; i++) {
            if (!found[i]) {
                int new_distance = least + edge[leastPos][i];
                #pragma omp critical
                {
                    if (new_distance < distance[i]) {
                        distance[i] = new_distance;
                    }
                }
            }
        }
    }

    free(found);
}

int main() {
    int n, SOURCE;
    printf("Enter the number of vertices: ");
    scanf("%d", &n);

    int **edge = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        edge[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            if (i != j) {
                printf("Enter the length of edge from vertex %d to vertex %d: ", i, j);
                scanf("%d", &edge[i][j]);
            }
        }
    }

    printf("Enter the SOURCE vertex: ");
    scanf("%d", &SOURCE);

    int distance[n];
    HW3(SOURCE, n, edge, distance);

    printf("Distances from SOURCE %d:\n", SOURCE);
    for (int i = 0; i < n; i++) {
        if (i != SOURCE) {
            printf("Vertex %d: %d\n", i, distance[i]);
        }
    }

    for (int i = 0; i < n; i++) {
        free(edge[i]);
    }
    free(edge);

    return 0;
}
