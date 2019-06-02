/*
 *	PageSim.c
 *
 *	Demo of CSCI 447 memory management simulator
 *
 *	Filip Jagodzinski, Computer Science, WWU, 21 February 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include "MemSim.h"
#include "PageSim.h"

#define TABLESIZE	2048		// size of hash table of processes
#define MAXPROC		64		// maximum number of processes in the system
#define PAGESIZE	4096		// system page size
#define LFUHALFREQ 5000

enum{
  METHOD_LRU = 1,
  METHOD_LFU,
  METHOD_RAND,
};

// data structure for process hash table entries


ptable* pagetable[TABLESIZE];   // process hash table
int procs[MAXPROC];
int procCount = 0;    // number of processes
int lastReplace;
int method;

int writeBackFaults = 0;
int readFaults = 0;
int pageAttempts = 0;

int main(int argc, char* argv[]) {
  srand(time(0)); 

  if(argc < 2){
    printf("Please specify a valid replacement method (LRU, LFU, Random");
    exit(1);
  }
  else if (!strcmp(argv[1], "LRU")){
    method = METHOD_LRU;
  }
  else if (!strcmp(argv[1], "LFU")){
    method = METHOD_LFU;
  }
  else if (!strcmp(argv[1], "Random")){
    method = METHOD_RAND;
  } else{
    printf("Please specify a valid replacement method (LRU, LFU, Random");
    exit(1);
  }

  for(int i=0; i<TABLESIZE; i++){
    pagetable[i] = NULL;
  }

  for(int i=0; i<MAXPROC; i++){
    procs[i] = -1;
  }

  printf("MMU simulation with %s replacement strategy started\n", argv[1]);
  Simulate(TABLESIZE*1000);

  printf("\n");
  printf("MMU simulation with %s replacement strategy finished\n", argv[1]);

  printf("\n");
  printf("Total Page Attempts: %d\n", pageAttempts);
  printf("Write Back Faults: %d\n", writeBackFaults);
  printf("Read Faults: %d\n", readFaults);
  printf("Total Faults: %d\n\n", writeBackFaults+readFaults);

  double faultFreq = 100*(double)(writeBackFaults+readFaults)/(double)(pageAttempts);
  double wFaultFreq = 100*(double)writeBackFaults/(double)(pageAttempts);

  printf("Fault Freq: %.2f%%\n",faultFreq);
  printf("Write Back Fault Freq: %.2f%%\n",wFaultFreq);
}

// called in response to a memory access attempt by process pid to memory address
int Access(int pid, int address, int write) {
  pageAttempts++;

  if(pageAttempts%LFUHALFREQ == 0) halfReferences();

  int page = address/PAGESIZE;
  int retval = 0;

  if(validProcess(pid)){
    if(available(pid, page, write));
    else if(replaceEmpty(pid, page, write));
    else if(method == METHOD_LRU){
      replaceLRU(pid, page, write);
    }
    else if(method == METHOD_LFU){
      replaceLFU(pid, page, write);
    }
    else if(method == METHOD_RAND){
      replaceRand(pid, page, write);
    }
    else{
      printf("No method specified \n");
      exit(1);
    }
    retval = 1;

    //printf("pid %d wants %s access to address %d on page %d \n",pid, (write) ? "write" : "read", address, address/PAGESIZE);
  }
  else if(procCount < MAXPROC){
    fillProcSlot(pid);
    pageAttempts--;
    Access(pid, address, write);
  }
  else{
    //printf("pid %d refused, max processes reached \n", pid);
    retval = 0;
  }

  return retval;
}

bool available(int pid, int page, int write){
  bool success = false;
  for(int i=0; i<TABLESIZE; i++){

    if(pagetable[i] != NULL && pagetable[i]->pid == pid && pagetable[i]->page == page){
      pagetable[i]->dirty = write;
      pagetable[i]->ref = 1;
      pagetable[i]->refCount++;
      success = true;
      break;
    }

  }
  return success;
}

bool replaceEmpty(int pid, int page, int write){
  bool success = false;
  for(int i=0; i<TABLESIZE; i++){

    if(pagetable[i] == NULL){
      pagetable[i] = newTableEntry(pid, page, write);
      readFaults++;
      success = true;
      lastReplace = i;
      break;
    }

  }
  return success;

}

void replaceLRU(int pid, int page, int write){
  bool found = false;

  if(!found){
    for(int i=0; i<TABLESIZE; i++){
      int choice = (i+lastReplace) % TABLESIZE;
      ptable* p = pagetable[choice];

      if(p != NULL && p->dirty == 0 && p->ref ==0){
        found = true;
        pagetable[choice] = newTableEntry(pid, page, write);
        readFaults++;
        lastReplace = choice;
        break;
      }else if(p != NULL){
        p->ref = 0;
      }

    }
  }

  if(!found){
    for(int i=0; i<TABLESIZE; i++){
      int choice = (i+lastReplace) % TABLESIZE;
      ptable* p = pagetable[choice];

      if(p != NULL && p->dirty == 1 && p->ref ==0){
        found = true;
        pagetable[choice] = newTableEntry(pid, page, write);
        writeBackFaults++;
        lastReplace = choice;
        break;
      }else if(p != NULL){
        p->ref = 0;
      }
    }
  }

  if(!found){
    for(int i=0; i<TABLESIZE; i++){
      int choice = (i+lastReplace) % TABLESIZE;
      ptable* p = pagetable[choice];

      if(p != NULL && p->dirty == 0 && p->ref == 1){
        found = true;
        pagetable[choice] = newTableEntry(pid, page, write);
        readFaults++;
        lastReplace = choice;
        break;
      }else if(p != NULL){
        p->ref = 0;
      }

    }
  }

  if(!found){
    for(int i=0; i<TABLESIZE; i++){
      int choice = (i+lastReplace) % TABLESIZE;
      ptable* p = pagetable[choice];

      if(p != NULL && p->dirty == 1 && p->ref ==1){
        found = true;
        pagetable[choice] = newTableEntry(pid, page, write);
        writeBackFaults++;
        lastReplace = choice;
        break;
      }else if(p != NULL){
        p->ref = 0;
      }

    }
  }

}

void replaceLFU(int pid, int page, int write){
  int minRef = INT_MAX;
  int minRefIndex =-1;

  for(int i=0; i<TABLESIZE; i++){
    ptable* p = pagetable[i];

    if(p != NULL && p->ref < minRef){
      minRef = p->ref;
      minRefIndex = i;
    }
  }

  if(minRefIndex == -1){
    printf("You messed up\n");
    exit(1);
  }

  ptable* evicted = pagetable[minRefIndex];

  if(evicted->dirty){
    writeBackFaults++;
  }else{
    readFaults++;
  }

  printf("%d\n",minRef);
  pagetable[minRefIndex] = newTableEntry(pid, page, write);
  lastReplace = minRefIndex;

}

void replaceRand(int pid, int page, int write){
  int choice = (rand() % (TABLESIZE));
  ptable* evicted = pagetable[choice];
  pagetable[choice] = newTableEntry(pid,page,write);
  lastReplace = choice;

  if(evicted == NULL){
    printf("That shouldn't have happened\n");
    printf("Choice: %d\n",choice);
    exit(1);
  }

  if(evicted->dirty){
    writeBackFaults++;
  }else{
    readFaults++;
  }

  
}

// called when process terminates
void Terminate(int pid) {

  for(int i=0; i<MAXPROC; i++){

    if(procs[i] == pid){
      //printf("pid %d terminated\n", pid);
      procs[i] = -1;
      procCount--;
      break;
    }

    for(int i=0; i<TABLESIZE; i++){

      if(pagetable[i] != NULL && pagetable[i]->pid == pid){
        if(pagetable[i]->dirty) writeBackFaults++;
        pagetable[i] = NULL;
      }

    }

  }
  
}

ptable* newTableEntry(int pid, int page, int write){
  ptable* newEntry = malloc(sizeof(ptable));

  if(newEntry == NULL){
    perror("Malloc");
    exit(1);
  }

  newEntry->pid = pid;
  newEntry->page =page;
  newEntry->dirty = write;
  newEntry->ref = 1;
  newEntry->refCount = 0;

  return newEntry;
}

bool validProcess(int pid){
  bool valid = false;

  for(int i=0; i<MAXPROC; i++){
    if(procs[i] == pid) valid = true;
  }

  return valid;

}

void fillProcSlot(int pid){

  for(int i=0; i<MAXPROC; i++){

    if(procs[i] == -1){
      procs[i] = pid;
      procCount++;
      break;
    }

  }

}

void halfReferences(){
  for(int i=0; i<TABLESIZE; i++){
    if(pagetable[i] != NULL){
      pagetable[i]->ref /= 2;
    }
  }
}