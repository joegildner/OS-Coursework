/*
 *	PageSim.h
 *
 *	Header file for CSCI 447 memory management exercise
 *
 *	Filip Jagodzinski, Computer Science, WWU, 21 February 2018
 */

#include <stdbool.h>

typedef struct table_struct {
  int pid;
  int page;
  bool dirty;
  bool ref;
  int refCount;
} ptable;

void Terminate(int pid);	// inform student code that the process has terminated

int Access(int pid, int address, int write);	
					// process pid wants to access address
					// write access if write is non-zero
					// return 0 indicates process request not satisfied

bool validProcess(int pid);
bool available(int,int,int);
bool replaceEmpty(int,int,int);
void replaceLRU(int, int, int);
void replaceLFU(int, int, int);
void replaceRand(int, int, int);
void fillProcSlot(int pid);
ptable* newTableEntry(int pid, int page, int write);
void halfReferences();
