#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Macro for the sign function
#define sgn(x) (((x) < 0.0) ? (-1.0) : (1.0))

// Constants
#define c1 1.23456
#define c2 6.54321
#define NUM_PARTICLES 123456

// Serial force calculation function
void calc_force(int num_particles, double *x, double *f) {
    for (int i = 1; i < num_particles; i++) {
        for (int j = 0; j < i; j++) {
            double diff = x[i] - x[j];
            double tmp = c1 / (diff * diff * diff) - c2 * sgn(diff) / (diff * diff);
            f[i] += tmp;
            f[j] -= tmp;
        }
    }
}

// Parallel force calculation function
void calc_force_parallel(int num_particles, double *x, double *f) {
    int i, j;
    double diff, tmp;

    // OpenMP parallelization using a reduction clause for summing forces
    #pragma omp parallel for private(j, diff, tmp) reduction(+:f[:num_particles])
    for (i = 1; i < num_particles; i++)
        for (j = 0; j < i; j++) {
            diff = x[i] - x[j];
            tmp = c1 / (diff * diff * diff) - c2 * sgn(diff) / (diff * diff);
            f[i] += tmp;
            f[j] -= tmp;
        }
}

int main() {
    // Allocate memory for particle positions and forces
    double *x = (double *)malloc(NUM_PARTICLES * sizeof(double));
    double *f = (double *)malloc(NUM_PARTICLES * sizeof(double));

    // Initialize particles with sample x values
    for (int i = 0; i < NUM_PARTICLES; i++) {
        x[i] = i * 0.1;  // Sample values; you can adjust this based on your scenario
    }

    double start_time, end_time;

    // Serial force calculation
    start_time = omp_get_wtime();
    calc_force(NUM_PARTICLES, x, f);
    end_time = omp_get_wtime();
    printf("Serial Execution Time: %f seconds\n", end_time - start_time);
    printf("f[NUM_PARTICLES] in Serial: %f\n", f[NUM_PARTICLES - 1]);

    // Parallel force calculation
    start_time = omp_get_wtime();
    calc_force_parallel(NUM_PARTICLES, x, f);
    end_time = omp_get_wtime();
    printf("Parallel Execution Time: %f seconds\n", end_time - start_time);
    printf("f[NUM_PARTICLES] in Parallel: %f\n", f[NUM_PARTICLES - 1]);

    // Print or use the results as needed

    // Free allocated memory
    free(x);
    free(f);
    return 0;
}
