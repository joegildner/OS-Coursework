/*
 *  File: Simulation.c ... a skeleton file
 *  Author: Filip Jagodzinski <filip.jagodzinski@wwu.edu>
 *  Last update : 08 February 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Simulation.h"
#include "multiqueue.h"

#define LINELEN 256

/* If there are custom classes/source files that you write, with
   custom functions, and you want those functions available for use in
   THIS .c file, then include the header file for the custom .c
   file(s) you've written, using the #include directive. For example:

   #include "SomeFile.h"

 */

p_process* processes;
int process_count;

void cpu_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int*);
void io_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool);
void print_iopool(p_process* io_pool);

void Simulate(int quantumA, int quantumB, int quantumC, int preEmp) {
  // A function whose input is the quanta for queues A, B, and C, as
  // well as whether pre-emption is enabled.

  int running_count = process_count;
  p_process io_pool[process_count];
  for(int i=0; i<process_count; i++){
    io_pool[i] = NULL;
  }

  p_cpu thecpu = newcpu();
  p_multiqueue mqueue = new_multiqueue(quantumA, quantumB, quantumC, preEmp);

  int tick = 0;
  while(running_count>0){

    for(int i=0; i<process_count; i++){
      if(processes[i]->arrival == tick)
        add_process(processes[i], mqueue, thecpu);
    }

    io_tick(mqueue, thecpu, io_pool);
    cpu_tick(mqueue, thecpu, io_pool, &running_count);

    printf("t=%d\n",tick);
    print_cpu(thecpu);
    print_multiqueue(mqueue);
    print_iopool(io_pool);
    printf("------------------------------------------------------------\n");

    tick++;
  }
}

void cpu_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int* running_count){
  if(thecpu->time_left == 0){

    if(thecpu->proc != NULL){
      if(thecpu->proc->promote < 3) thecpu->proc->demote++;
      add_process(thecpu->proc, mqueue, thecpu);
      thecpu->proc = NULL;
    }
  }

  if(thecpu->proc == NULL){
    next_process(mqueue, thecpu);
  }

  if(thecpu->proc != NULL){
    if(!thecpu->decoded){
      thecpu->time_left--;
      thecpu->decoded = 1;

      if(thecpu->proc->inst->type == PROC_IO){
        if(thecpu->proc->demote < 3) thecpu->proc->promote++;
        add_io(thecpu->proc, io_pool);
        thecpu->proc = NULL;
      }else
      if(thecpu->proc->inst->type == PROC_TERM){
        thecpu->proc = NULL;
        (*running_count)--;
      }

    }else{
      thecpu->time_left--;
      if(--thecpu->proc->inst->time == 0){
        thecpu->proc->inst = thecpu->proc->inst->next_inst;
        thecpu->decoded = 0;
      }
    }
  }
}

void io_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool){
  for(int i=0; i<process_count; i++){
    if(io_pool[i]!=NULL){
      io_pool[i]->inst->time--;

      if(io_pool[i]->inst->time < 0){
        io_pool[i]->inst = io_pool[i]->inst->next_inst;
        add_process(io_pool[i], mqueue, thecpu);
        io_pool[i] = NULL;
      }
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

p_cpu newcpu(){
  p_cpu new_cpu = malloc(sizeof(cpu));
  new_cpu->decoded = 0;
  new_cpu->time_left=0;
  new_cpu->proc = NULL;

  return new_cpu;
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
  newp->promote = 0;
  newp->demote =0;
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

void add_io(p_process p, p_process* io_pool) {
  for(int i=0; i<process_count; i++){
    if(io_pool[i] == NULL){
      io_pool[i] = p;
      break;
    }
  }
}

void print_cpu(p_cpu thecpu){
  if(thecpu->proc != NULL){
    printf("CPU: P%d (#%d), %ds\n", thecpu->proc->pid, thecpu->proc->priority, thecpu->time_left);
    printf("\tExecution type: %d, %ds\n", thecpu->proc->inst->type, thecpu->proc->inst->time);
  }else{
    printf("CPU: (empty), %ds\n",thecpu->time_left);
  }
}

void print_iopool(p_process* io_pool){
  printf("IO: ");
  for(int i=0; i<process_count; i++){
    if(io_pool[i] != NULL)
      printf("P%d (#%d) %ds, ", io_pool[i]->pid, io_pool[i]->priority, io_pool[i]->inst->time);
  }
  printf("\n");
}