#Makefile for CIS 4450 Assignment 1

# Path to pilot libraries
PILOTHOME = /home/mhoyle/pilothome

#flags
CC = mpicc -intel
CPPFLAGS = -I$(PILOTHOME)/include
LDFLAGS = -L$(PILOTHOME)/lib -lpilot

FC = mpif90 -intel
FFLAGS = -fpp -I$(PILOTHOME)/include
# LDFLAGS same as above


# "make wp" will compile wp.c using implicit make rules
wp: wp.o
	$(CC) $< $(LDFLAGS) -o $@

# "make runwp" will run wp
runwp: clean wp
	sqsub -q mpi -r 10 -n 4 -o wplog.txt ./wp  parallaldos targets
# "make clean" will clean up all but source files
clean:
	$(RM) wp.o wp wplog.txt
