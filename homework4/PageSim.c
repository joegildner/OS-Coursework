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


ptable* pagetable[TABLESIZE];   // page table
int procs[MAXPROC];             // current process table
int procCount = 0;              // currenty process count

int lastReplace;                //index in pagetable of last replacement 
int method;                     //replacement strategy, per the enum above

int writeBackFaults = 0;        //running total of write-back faults
int readFaults = 0;             //running total of read faults
int pageAttempts = 0;           //total paging attempts


/* main entry point
 * the main function deciphers which page replacement method is being used from the
 * input arguments and assigns it to the method variable. The simulation is then
 * run, after which this method tallies up the statistics of the run.
 */
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

/* available
 * First check in the Access sequence, if the page requested is already in the page table
 * then no faults occur but reference values are adjusted.
 */
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

/* replaceEmpty
 * Second check in the Access sequence, if there is an empty slot in the page table, fill
 * that slot with the requested page.
 */
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

/* replaceLRU
 * Third check in the Access sequence, if method is LRU. Replace the page which was used least
 * recently checking the reference and dirty bits to make an informed decision
 */
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

/* replaceLFU
 * Third check in the Access sequence, if LFU is the specified method. replace the 
 * least used page in the table as tallied so far by the program.
 */
void replaceLFU(int pid, int page, int write){
  int minRef = INT_MAX;
  int minRefIndex =-1;

  for(int i=0; i<TABLESIZE; i++){
    ptable* p = pagetable[i];

    if(p != NULL && p->ref <= minRef){
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

  pagetable[minRefIndex] = newTableEntry(pid, page, write);
  lastReplace = minRefIndex;
}

/* replaceRand
 * Third check in the Access sequence, if Random is the specified replacement method.
 * randomly choose an index in the pagetable and replace that page with the request.
 */
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

/* Terminate
 * Called when a process Terminates, that process is removed from the process list
 * and all of its pages in the pagetable are removed.
 */
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


/* newTableEntry
 * creates a new ptable "object" which contains the information about
 * the page for the page table
 */
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

/* validProcess
 * Checks if the process with pid is already in the system
 */
bool validProcess(int pid){
  bool valid = false;

  for(int i=0; i<MAXPROC; i++){
    if(procs[i] == pid) valid = true;
  }

  return valid;

}

/* fillProcSlot
 * puts the pid of the process into an open slot in the process array
 */
void fillProcSlot(int pid){

  for(int i=0; i<MAXPROC; i++){

    if(procs[i] == -1){
      procs[i] = pid;
      procCount++;
      break;
    }

  }

}

/* halfReferences
 * divides the reference count of all the processes in half when called
 * for use in the LFU scheme.
 */
void halfReferences(){
  for(int i=0; i<TABLESIZE; i++){
    if(pagetable[i] != NULL){
      pagetable[i]->ref /= 2;
    }
  }
}