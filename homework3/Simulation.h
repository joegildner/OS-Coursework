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


#endif