/*
 *  File: Simulation.c ... a skeleton file
 *  Author: Filip Jagodzinski <filip.jagodzinski@wwu.edu>
 *  Last update : 08 February 2018
 */

#include <stdio.h>
#include <stdlib.h>

/* If there are custom classes/source files that you write, with
   custom functions, and you want those functions available for use in
   THIS .c file, then include the header file for the custom .c
   file(s) you've written, using the #include directive. For example:

   #include "SomeFile.h"

 */

void Simulate(int quantumA, int quantumB, int quantumC, int preEmp) {
  // A function whose input is the quanta for queues A, B, and C, as
  // well as whether pre-emption is enabled.
  printf("Quantum A val received : %d\n", quantumA);
  printf("Quantum B val received : %d\n", quantumB);
  printf("Quantum C val received : %d\n", quantumC);
  printf("preEmpt val received   : %d\n", preEmp);

}

int main() {
  // Run simulation
  Simulate(15, 20, 30, 0);

}
