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


void build_processes(char* filename);
p_process new_process(char* idstr);
void get_pinfo(char* idstr,int* pid, int* priority);
void count_processes(char* filename);
int get_val(char* line);
void add_instruction(p_inst*, int type, int time);

#endif