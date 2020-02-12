
#include <stdlib.h>
#include <cstdio>
#include "applyOrthoPoly.hh"

// Small shell program to show what the applyOrthoPoly class can do.
// Reads a orthogonal polynomial fit from parameters written by the R
// script fitOrthoPoly.R and applies that fit over a range of X values as
// specified on the command line.
//
// Niles Oien nilesOien@gmail.com February 2020.

int main(int argc, char *argv[]){

 if (argc < 5){
  fprintf(stderr, "Need an input base name, min, step and max on the command line (followed by optional debug arg)\n");
  fprintf(stderr, "EXAMPLE : %s myData -1.5 0.01 1.5 debug\n", argv[0]);
  return -1;
 }

 // Convert command line args to floating point as required.
 double min = atof(argv[2]);
 double step = atof(argv[3]);
 double max = atof(argv[4]);
 char *baseName = argv[1];

 int debug=0;
 if (argc > 5) debug=1;

 // Initialize the class.
 applyOrthoPoly A(baseName, debug);

 if (A.getDegree() < 0){
  fprintf(stderr, "Failed to initialze class from base name %s\n", baseName);
  fprintf(stderr, "Was an R orthogonal polynomial fit saved here under that name by fitOrthoPoly.R?\n");
  return -1;
 }

 // If we initialized OK in debug mode, print class state
 if (debug)
   A.printPoly();

 // If we're in debug mode, save out the polynomials we fit against.
 if (debug)
  A.savePolys(min, step, max);

 // Apply the fit. Resulting X Y pairs written to {baseName}_out.dat
 A.applyFit(min, step, max);


 return 0;

}

