#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int array_size = 202;
    int* array = NULL;
    int local_sum = 0;

    // Local size of each rank
    int* sendcounts = (int*)malloc(world_size * sizeof(int));
    int* displs = (int*)malloc(world_size * sizeof(int));
    int sum = 0;
    for (int i = 0; i < world_size; i++) {
        sendcounts[i] = array_size / world_size + (i < array_size % world_size ? 1 : 0);
        displs[i] = sum;
        sum += sendcounts[i];
    }

    int local_size = sendcounts[world_rank];
    int* local_array = (int*)malloc(local_size * sizeof(int));

    if (world_rank == 0) {
        array = (int*)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = i + 1;
        }
    }

    MPI_Scatterv(array, sendcounts, displs, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_size; i++) {
        local_sum += local_array[i];
    }

    // Record local sums to files
    char filename[20];
    sprintf_s(filename, sizeof(filename), "log_rank_%d.txt", world_rank);
    FILE* log_file;
    fopen_s(&log_file, filename, "w");
    if (log_file != NULL) {
        fprintf(log_file, "Local sum for rank %d is %d\n", world_rank, local_sum);
        fclose(log_file);
    }
    else {
        fprintf(stderr, "Error opening file %s\n", filename);
    }

    // Gradually merge the local sum
    int step = 1;
    while (step < world_size) {
        if (world_rank % (2 * step) == 0) {
            if (world_rank + step < world_size) {
                int received_sum;
                MPI_Recv(&received_sum, 1, MPI_INT, world_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += received_sum;
            }
        }
        else {
            MPI_Send(&local_sum, 1, MPI_INT, world_rank - step, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    if (world_rank == 0) {
        printf("Total sum is %d\n", local_sum);
    }

    if (world_rank == 0) {
        free(array);
    }
    free(local_array);
    free(sendcounts);
    free(displs);

    MPI_Finalize();
    return 0;
}