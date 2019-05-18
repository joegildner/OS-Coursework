/*
 *  File: Simulation.c ... a skeleton file
 *  Author: Filip Jagodzinski <filip.jagodzinski@wwu.edu>
 *  Last update : 08 February 2018
 */

#include <stdio.h>
#include <stdlib.h>

#include "Simulation.h"

#define LINELEN 256

/* If there are custom classes/source files that you write, with
   custom functions, and you want those functions available for use in
   THIS .c file, then include the header file for the custom .c
   file(s) you've written, using the #include directive. For example:

   #include "SomeFile.h"

 */

struct process_st{
  int pid;
  int priority;
  p_inst inst;
};

struct inst_st{
  int id;
  int time;
  p_inst next_inst;
};

p_process* processes;


void Simulate(int quantumA, int quantumB, int quantumC, int preEmp) {
  // A function whose input is the quanta for queues A, B, and C, as
  // well as whether pre-emption is enabled.
  printf("Quantum A val received : %d\n", quantumA);
  printf("Quantum B val received : %d\n", quantumB);
  printf("Quantum C val received : %d\n", quantumC);
  printf("preEmpt val received   : %d\n", preEmp);

}

int main(int argc, char* argv[]) {
  // Run simulation

  if(argc != 6){
    printf("Wrong number of command line arguments");
    return 0;
  }

  build_processes(argv[1]);


  int qA = atoi(argv[2]);
  int qB = atoi(argv[3]);
  int qC = atoi(argv[4]);
  int preEmp = atoi(argv[5]);


  Simulate(qA, qB, qC, preEmp);

}

void build_processes(char* filename){

  FILE* infile = fopen(filename, "r");

  char input_line[LINELEN];

  while(fgets(input_line, LINELEN*sizeof(char), infile ) != NULL){
    
    if(input_line[0] == 'P'){
      new_process(input_line);
    }

  }



}

p_process new_process(char* idstr){
  int pid;
  int priority;

  get_pinfo(idstr, &pid, &priority);
  
  p_process newp = malloc(sizeof(process));

  return newp;

}

void get_pinfo(char* idstr, int* pid, int* priority){

}

