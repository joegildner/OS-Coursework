#ifndef SIM_H
#define SIM_H

enum{
  PROC_EXE,
  PROC_IO,
  PROC_TERM,
};

typedef struct process_st process;
typedef process *p_process;

typedef struct inst_st inst;
typedef inst *p_inst;

typedef struct cpu_st cpu;
typedef cpu *p_cpu;

struct cpu_st{
  int decoded;
  int time_left;
  p_process proc;
};

struct process_st{
  int pid;
  int priority;
  int arrival;

  int promote;
  int demote;

  p_inst inst;
};

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

#endif