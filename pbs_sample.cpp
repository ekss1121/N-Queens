#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
  MPI_Init(&argc, &argv);
  
  MPI_Comm comm = MPI_COMM_WORLD;
  int rank, size;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  printf("hello from rank %i%i.\n", rank, size);

  MPI_Finalize();
  return 0;

}