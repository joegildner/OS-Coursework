// Joe Gildner
// CSCI 447, HW3
// 05/20/2019

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Simulation.h"
#include "multiqueue.h"

#define LINELEN 256

p_process* processes;
p_process* completions;
int process_count;  

// Prototypes which require all other headers to have already been defined for use
void cpu_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int, int*);
void io_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool);
void print_iopool(p_process* io_pool);
void print_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int tick);
void print_results(int, int quantumA, int quantumB, int quantumC, int preEmp);

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

/* Simulate
 * Simulates a scheduling algorithm in an Operating system, with specified
 * time values for each queue and an option to preempt the current executing
 * process based on priority.
 */
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
    cpu_tick(mqueue, thecpu, io_pool, tick, &running_count);
    multiqueue_tick(mqueue);

    //print_tick(mqueue, thecpu, io_pool, tick);

    tick++;
  }

  print_results(tick, quantumA, quantumB, quantumC, preEmp);
}

/* cpu_tick
 * simulates a tick of the CPU, and decides what to do with the processes
 * in the queue as well as the process in the CPU
 */
void cpu_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int tick, int* running_count){
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
        completions[process_count-*running_count] = thecpu->proc;
        thecpu->proc->complete = tick-thecpu->proc->arrival;
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


/* io_tick
 * operates a single io tick for all of the processes currently in the io queue, decrementing
 * their remaining io times and then adding them back into the queue when complete
 */
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


/* newcpu
 * creates a data structure realization of the cpu for this simulated operating
 * system.
 */
p_cpu newcpu(){
  p_cpu new_cpu = malloc(sizeof(cpu));
  new_cpu->decoded = 0;
  new_cpu->time_left=0;
  new_cpu->proc = NULL;

  return new_cpu;
}


/* build_processes 
 * builds all the process structures from the specifified formatted file
 */
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
        curr_proc->icount++;
        add_instruction(curr_inst, PROC_EXE, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }else
      if(input_line[0] == 'i'){
        curr_proc->icount++;
        add_instruction(curr_inst, PROC_IO, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }
      else
      if(input_line[0] == 't'){
        curr_proc->icount++;
        add_instruction(curr_inst, PROC_TERM, get_val(input_line));
        curr_inst = &((*curr_inst)->next_inst);
      }
    }
  }
}

/* add_instruction
 * adds a new instruction to the linked list of instructions
 * pointed to by instruction
 */
void add_instruction(p_inst* instruction, int type, int time){
  (*instruction)= malloc(sizeof(inst));
  (*instruction)->type = type;
  (*instruction)->time = time;
  (*instruction)->next_inst = NULL;
}

/* new_process
 * creates a new process 
 */
p_process new_process(char* idstr){
  int pid;
  int priority;

  get_pinfo(idstr, &pid, &priority);
  
  p_process newp = malloc(sizeof(process));
  newp->pid = pid;
  newp->priority = priority;
  newp->promote = 0;
  newp->demote =0;
  newp->wait = 0;
  newp->complete = 0;
  newp->icount = 0;
  newp->inst = NULL;

  return newp;

}

/*  
 *
 */
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


/*  
 *
 */
void count_processes(char* filename){

  FILE* infile = fopen(filename, "r");

  char input_line[LINELEN];

  int p_count = 0;

  while(fgets(input_line, LINELEN*sizeof(char), infile ) != NULL){
    if(input_line[0] == 'P') p_count++;
  }

  process_count = p_count;
  processes = malloc(p_count * sizeof *processes);
  completions = malloc(p_count * sizeof *processes);
}


/* 
 *
 */
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


/* add_io
 * adds the process p to the io_pool when executing an io instruction
 */
void add_io(p_process p, p_process* io_pool) {
  for(int i=0; i<process_count; i++){
    if(io_pool[i] == NULL){
      io_pool[i] = p;
      break;
    }
  }
}



// -------------------- print methods, mainly for debugging ---------------- //
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

void print_tick(p_multiqueue mqueue, p_cpu thecpu, p_process* io_pool, int tick){
    printf("t=%d\n",tick);
    print_cpu(thecpu);
    print_multiqueue(mqueue);
    print_iopool(io_pool);
    printf("------------------------------------------------------------\n");
}


/* print_results
 * Calculates and prints the desired analysis statistics for the simulation run.
 */
void print_results(int tick, int quantumA, int quantumB, int quantumC, int preEmp){
  int inst_count = 0;
  int ready_tot = 0;
  int through_tot = 0;
  int ready_max = 0;
  int ready_min = INT_MAX;

  for(int i=0; i<process_count; i++){
    p_process p = processes[i];
    inst_count += p->icount;
    ready_tot += p->wait;
    through_tot += p->complete;
    if(ready_max < p->wait) ready_max = p->wait;
    if(ready_min > p->wait) ready_min = p->wait;
  }

  double ready_avg = (double)ready_tot/(double)process_count;
  double throughput = (double)process_count/(double)through_tot;

  printf("Results (QA: %d, QB: %d, QC: %d, pre-emption: %d)\n", quantumA,  quantumB, quantumC, preEmp);

  printf("Start/end time: %d, %d\n", 0, tick);
  printf("Processes Completed: %d\n", process_count);
  printf("Instructions Completed: %d\n", inst_count);
  printf("Throughput: %.3f processes/unit time\n", throughput);
  printf("Ready time average: %.2f\n", ready_avg);
  printf("Ready time maximum: %d\n", ready_max);
  printf("Ready time minimum: %d\n", ready_min);


  printf("Completion Order: ");
  for(int i=0; i<process_count; i++){
    printf("P%d, ", completions[i]->pid);
  }
  printf("\n\n");
}