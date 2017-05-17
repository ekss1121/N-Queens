/**
 * @file    mpi_nqueens.cpp
 * @author  Patrick Flick <patrick.flick@gmail.com>
 * @brief   Implements the parallel, master-worker nqueens solver.
 *
 * Copyright (c) 2016 Georgia Institute of Technology. All Rights Reserved.
 */

/*********************************************************************
 *                  Implement your solutions here!                   *
 *********************************************************************/
/**
Questions:
1. How to send termination to every worker without knowing p?


 **/
#include "mpi_nqueens.h"

#include <mpi.h>
#include <vector>
#include "nqueens.h"

MPI_Status Stat;
// Solution store structure.
struct SolutionStore {
    // store solutions in a static member variable
    static std::vector<unsigned int>& solutions() {
        static std::vector<unsigned int> sols;
        return sols;
    }
    static void add_solution(const std::vector<unsigned int>& sol) {
        // add solution to static member
        solutions().insert(solutions().end(), sol.begin(), sol.end());
    }
    static void clear_solutions() {
        solutions().clear();
    }
};
/**
 * @brief The master's call back function for each found solution.
 *
 * This is the callback function for the master process, that is called
 * from within the nqueens solver, whenever a valid solution of level
 * `k` is found.
 *
 * This function will send the partial solution to a worker which has
 * completed his previously assigned work. As such this function must
 * also first receive the solution from the worker before sending out
 * the new work.
 *
 * @param solution      The valid solution. This is passed from within the
 *                      nqueens solver function.
 */
void master_solution_func(std::vector<unsigned int>& solution) {
    // Receive solutions or work-requests from a worker and then
    // proceed to send this partial solution to that worker.
    int size_of_job = 0;
    int dest = 0;
    MPI_Recv(&size_of_job, 1, MPI_INT, MPI_ANY_SOURCE, 100,
         MPI_COMM_WORLD, &Stat);
    dest = Stat.MPI_SOURCE;
    std::vector<unsigned int> master_solution(size_of_job,99);
    MPI_Recv(&master_solution[0], size_of_job, MPI_INT, dest, 200,
         MPI_COMM_WORLD, &Stat);

    if (master_solution[0] == 99){ // 99 is the job request sign
	// Send partial to the worker
        MPI_Send(&solution[0], solution.size(), MPI_INT, dest, 200, MPI_COMM_WORLD);
    }
    else{
	// Store the return solutions and send out job
        SolutionStore::add_solution(master_solution);
        MPI_Send(&solution[0], solution.size(), MPI_INT, dest, 200, MPI_COMM_WORLD);
    }

}



/**
 * @brief   Performs the master's main work.
 *
 * This function performs the master process' work. It will sets up the data
 * structure to save the solution and call the nqueens solver by passing
 * the master's callback function.
 * After all work has been dispatched, this function will send the termination
 * message to all worker processes, receive any remaining results, and then return.
 *
 * @param n     The size of the nqueens problem.
 * @param k     The number of levels the master process will solve before
 *              passing further work to a worker process.
 */
std::vector<unsigned int> master_main(unsigned int n, unsigned int k) {
    // Send parameters (n,k) to workers via broadcast (MPI_Bcast)
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); //Broadcast n
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD); //Broadcast k
    int num_workers; // MPI_Size
    MPI_Comm_size(MPI_COMM_WORLD, &num_workers);
    // allocate the vector for the solution permutations
    std::vector<unsigned int> pos(n);

    // generate all partial solutions (up to level k) and call the
    // master solution function
    nqueens_by_level(pos, 0, k, &master_solution_func);

    // Get remaining solutions from workers and send termination messages
    int i = 0; // iteration index
    int size_of_job = 0; //Store the size of return solutions
    int dest = 0;
    // initialize termination sign
    std::vector<unsigned int> termination(n,999);

    while(1){
        MPI_Recv(&size_of_job, 1, MPI_INT, MPI_ANY_SOURCE, 100,
                 MPI_COMM_WORLD, &Stat);
      dest = Stat.MPI_SOURCE;
        std::vector<unsigned int> master_solution(size_of_job,0);
        MPI_Recv(&master_solution[0], size_of_job, MPI_INT, dest, 200,
                     MPI_COMM_WORLD, &Stat);

    if(master_solution[0] != 99){
        // Receive and store remaining solutions and send termination
        SolutionStore::add_solution(master_solution);
        MPI_Send(&termination[0], n, MPI_INT, dest, 200, MPI_COMM_WORLD);
    }
    else{
        // Send out termination if receive request sign
        MPI_Send(&termination[0], n, MPI_INT, dest, 200, MPI_COMM_WORLD);
    }
    i++;
    if(i == num_workers-1) break;
    }

    std::vector<unsigned int> allsolutions = SolutionStore::solutions();
    //Return all solutions
    return allsolutions;
}

// std::vector<std::vector<unsigned int>> queue_list;

// void worker_receive(){
	// while(true){
		
		// int flag = false;
		// MPI_Status status;
		
		// nonblocking test for existence of incoming message
		// MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);//MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
		
		// if(flag){ //flag is true when there's a message
			
			// nonblocking receive problem to solve
			// std:vector<unsigned int> queue;
			// MPI_Irecv(&queue, n, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &MPI_REQUEST_NULL);
			// queue_list.push_back(queue);
		// }else{
			// break;
		// }
		
	// }
// }

/**
 * @brief The workers' call back function for each found solution.
 *
 * This is the callback function for the worker processes, that is called
 * from within the nqueens solver, whenever a valid solution is found.
 *
 * This function saves the solution into the worker's solution cache.
 *
 * @param solution      The valid solution. This is passed from within the
 *                      nqueens solver function.
 */
void worker_solution_func(std::vector<unsigned int>& solution) {
    // Save the solution into a local cache
    SolutionStore::add_solution(solution);

}

/**
 * @brief   Performs the worker's main work.
 *
 * This function implements the functionality of the worker process.
 * The worker will receive partially completed work items from the
 * master process and will then complete the assigned work and send
 * back the results. Then again the worker will receive more work from the
 * master process.
 * If no more work is available (termination message is received instead of
 * new work), then this function will return.
 */
void worker_main() {
    unsigned int n, k;

    // TODO receive the parameters `n` and `k` from the master process via MPI_Bcast

    //Receive the parameters `n` and `k` from the master process via MPI_Bcast
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Implement the worker's functions: receive partially completed solutions,
    // calculate all possible solutions starting with these queen positions
    // and send solutions to the master process. then ask for more work.

    std::vector<unsigned int> partial_job(n);
    std::vector<unsigned int> request_job(1, 99);

    int size_of_request = 1; //Initialize the size of request sign.

    // Send job request and receive job; Calculate all possible remaining solutions
    // until termination recieved.
    MPI_Send(&size_of_request, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
    MPI_Send(&request_job[0], 1, MPI_INT, 0, 200, MPI_COMM_WORLD);
    while(1){

        MPI_Recv(&partial_job[0], n, MPI_INT, 0, 200, MPI_COMM_WORLD, &Stat);

        if (partial_job[0] == 999) break; // Termination sign vector.
	// Run nqueens solver to find remaining solutions.

        nqueens_by_level(partial_job, k, n, &worker_solution_func); //TODO: check here
	// Store all all found solutions to local_solutions.
        std::vector<unsigned int> local_solutions = SolutionStore::solutions();
        SolutionStore::clear_solutions();
        int solution_size = local_solutions.size();
      if (solution_size == 0){
        solution_size =1;
        local_solutions.push_back(99);
      }
        // Send solution size.
      MPI_Send(&solution_size, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
        // Send solution in local cache to master processor
        MPI_Send(&local_solutions[0], solution_size, MPI_INT, 0, 200, MPI_COMM_WORLD);

    }

}



