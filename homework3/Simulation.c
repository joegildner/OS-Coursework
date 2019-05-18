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
  int arrival;
  p_inst inst;
};

struct inst_st{
  int type;
  int time;
  p_inst next_inst;
};

p_process* processes;
int process_count;

void Simulate(int quantumA, int quantumB, int quantumC, int preEmp) {
  // A function whose input is the quanta for queues A, B, and C, as
  // well as whether pre-emption is enabled.
  for(int i=0; i<process_count; i++){
    printf("P%d, Prio:%d, A:%d\n", processes[i]->pid, processes[i]->priority,processes[i]->arrival);
    p_inst curr_inst = processes[i]->inst;
    while(curr_inst != NULL){
      printf("INST: %d, TIME: %d\n", curr_inst->type, curr_inst->time);
      curr_inst=curr_inst->next_inst;
    }
  }

}

int main(int argc, char* argv[]) {
  // Run simulation

  if(argc != 6){
    printf("Wrong number of command line arguments");
    return 0;
  }

  count_processes(argv[1]);
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

  p_process curr_proc = NULL;
  p_inst* curr_inst = NULL;
  int p_index = 0;

  while(fgets(input_line, LINELEN*sizeof(char), infile ) != NULL){
    
    if(input_line[0] == 'P'){
      p_process newp = new_process(input_line);
      curr_proc = newp;
      curr_inst = &(newp->inst);
      processes[p_index++] = newp;
    }
    else{
      if(input_line[0] == 'a'){
        curr_proc->arrival = get_val(input_line);
      }
      else
      if(input_line[0] == 'e'){
        add_instruction(curr_inst, PROC_EXE, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }else
      if(input_line[0] == 'i'){
        add_instruction(curr_inst, PROC_IO, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }
      else
      if(input_line[0] == 't'){
        add_instruction(curr_inst, PROC_TERM, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }
    }
  }
}

p_process new_process(char* idstr){
  int pid;
  int priority;

  get_pinfo(idstr, &pid, &priority);
  
  p_process newp = malloc(sizeof(process));
  newp->pid = pid;
  newp->priority = priority;
  newp->inst = NULL;

  return newp;

}

void get_pinfo(char* idstr, int* pid, int* priority){
  char pidstr[16];
  char prioritystr[16];

  int i = 1;
  int pid_i = 0;

  while(idstr[i] != ':' && idstr[i] != '\n' && idstr[i] != '\0'){

    pidstr[pid_i] = idstr[i];
    pidstr[pid_i+1] = '\0';
    i++;
    pid_i++;

  }

  i++;
  int priority_i = 0;

  while(idstr[i] != '\n' && idstr[i] != '\0'){
    prioritystr[priority_i] = idstr[i];
    prioritystr[priority_i+1] = '\0';
    i++;
    priority_i++;
  }

  *pid = atoi(pidstr);
  *priority = atoi(prioritystr);

}

void count_processes(char* filename){

  FILE* infile = fopen(filename, "r");

  char input_line[LINELEN];

  int p_count = 0;

  while(fgets(input_line, LINELEN*sizeof(char), infile ) != NULL){
    if(input_line[0] == 'P') p_count++;
  }

  process_count = p_count;
  processes = malloc(p_count * sizeof *processes);
}

int get_val(char* line){
  
  int i=0;
  while(line[i] != ':') i++;
  i++;

  char val[16];
  int val_i = 0;

  while(line[i] != '\n' && line[i] != '\0'){
    val[val_i] = line[i];
    val[val_i+1] = '\0';
    i++;
    val_i++;
  }

  return atoi(val);

}

void add_instruction(p_inst* instruction, int type, int time){
  (*instruction)= malloc(sizeof(inst));
  (*instruction)->type = type;
  (*instruction)->time = time;
  (*instruction)->next_inst = NULL;
}

