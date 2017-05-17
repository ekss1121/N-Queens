#!/bin/sh
# This is just a sample PBS script. You should adapt the
# following example script, so that you can run all your experiments.
# For more information about Torque and PBS, check the following sources:
#    https://support.cc.gatech.edu/facilities/instructional-labs/how-to-run-jobs-on-the-instructional-hpc-clusters
#    http://www.democritos.it/activities/IT-MC/documentation/newinterface/pages/runningcodes.html



# allocate 4 of the (24 total) sixcore nodes for up to 5 minutes
#PBS -q class
#PBS -l nodes=4:sixcore
#PBS -l walltime=2:00:00
#PBS -N cse6220-nqueens-schoi332

# TODO: change this to your project directory relative to your home directory
#       (= $HOME)
<<<<<<< HEAD
#export PBS_O_WORKDIR=$HOME/schoi332/hw1

cd $HOME/hw1
MPIRUN=/usr/lib64/openmpi/bin/mpirun

# TODO:
# for your experiments, run your program for different `n`, different master-depth `k`
# and with different number of processors. This will allow you to properly
# analyze the scalability behaviour of each parameter setting.
# Also: you should time each parameter setting multiple times and then average
#       the results to achieve a smoother and more accurate analysis.
#MASTER_DEPTH=4




# loop over number of processors (just an example, uncomment to use the loop)
for p in 2 4 8 
do


#for MASTER_DEPTH in 3 4 5 6
#do
#N=8 # size of the problem # TODO: loop over N as well!
	$MPIRUN -np $p --hostfile $PBS_NODEFILE ./nqueens -t $N $MASTER_DEPTH


#done

#done

done

