// Joe Gildner
// CSCI 447, HW3
// 05/20/2019

#ifndef SIM_H
#define SIM_H

//Types of execution
enum{
  PROC_EXE,
  PROC_IO,
  PROC_TERM,
};

//reference variables for process structure
typedef struct process_st process;
typedef process *p_process;

//reference variables for instruction linked list structure
typedef struct inst_st inst;
typedef inst *p_inst;

//reference variables for cpu structure
typedef struct cpu_st cpu;
typedef cpu *p_cpu;

//Structure with variables to represent the cpu
struct cpu_st{
  int decoded;
  int time_left;
  p_process proc;
};

//Structure with variables to represent a process
struct process_st{
  int pid;
  int priority;
  int arrival;
  int wait;
  int complete;
  int icount;

  int promote;
  int demote;

  //linked list of instructions
  p_inst inst;
};

//Structure with variables to represent a linked list of instructions
struct inst_st{
  int type;
  int time;
  p_inst next_inst;
};


void build_processes(char* filename);
p_process new_process(char* idstr);
void get_pinfo(char* idstr,int* pid, int* priority);
void count_processes(char* filename);
int get_val(char* line);
void add_instruction(p_inst*, int type, int time);
p_cpu newcpu();
void print_cpu(p_cpu thecpu);
void add_io(p_process p, p_process* io_pool);
void Simulate(int quantumA, int quantumB, int quantumC, int preEmp);

#endif