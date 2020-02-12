

#include "applyOrthoPoly.hh"
#include <cstdlib>
#include <string.h>
#include <cstdio>
#include <cmath>

// Class to apply a polynomial fit from R in C++
// Niles Oien February 2020.

// Constructor. Reads the fit information as written
// by the R script fitOrthoPoly.R. The file names are
// construed from the base name. After calling, check
// getDegree() to be sure things went OK (a value of -1
// indicates a problem).
applyOrthoPoly::applyOrthoPoly(char *baseName, int debug){
 _debug=debug;
 _baseName=strdup(baseName);
 _degree=-1;

 // The files we read will be along the lines of :
 // myData_modelCoefficients.dat  myData_polyAlphas.dat  myData_polyNorms.dat
 //
 if (_readDoubleVector("polyAlphas", _alpha)){
  fprintf(stderr, "Failed to read alpha vector for %s\n", _baseName);
  return; 
 }

 if (_readDoubleVector("polyNorms", _norm2)){
  fprintf(stderr, "Failed to read norm2 vector for %s\n", _baseName);
  return; 
 }

 if (_readDoubleVector("modelCoefficients", _coeffs)){
  fprintf(stderr, "Failed to read model coeff vector for %s\n", _baseName);
  return; 
 }

 if ((_coeffs.size() +1 != _norm2.size()) || (_coeffs.size() != _alpha.size() + 1)){
  fprintf(stderr, "Size mismatch for %s : Coeffs %ld Norm2 %ld Alpha %ld\n",
   _baseName, _coeffs.size(), _norm2.size(), _alpha.size());
  return;
 }

 _degree = (int)_alpha.size();

 return;
}



// Returns degree, or -1 if class construction was bad.
int applyOrthoPoly::getDegree(){
  return _degree;
}




// Prints internals variables that we got from R. May
// be useful for debugging.
void applyOrthoPoly::printPoly(){

 if (_degree < 0){
  fprintf(stdout, "Class failed to initialize for %s\n", _baseName);
  return;
 }

 fprintf(stdout, "\nDegree %d polynomial fit for %s\n", _degree, _baseName);

 fprintf(stdout, "Alpha values :\n");
 for(unsigned i=0; i < _alpha.size(); i++){
  fprintf(stdout, "%d : %lf\n", i+1, _alpha[i]);
 }
 fprintf(stdout,"\n");


 fprintf(stdout, "Norm2 values :\n");
 for(unsigned i=0; i < _norm2.size(); i++){
  fprintf(stdout, "%d : %lf\n", i+1, _norm2[i]);
 }
 fprintf(stdout,"\n");


 fprintf(stdout, "Model coeff values :\n");
 for(unsigned i=0; i < _coeffs.size(); i++){
  fprintf(stdout, "%d : %lf\n", i+1, _coeffs[i]);
 }
 fprintf(stdout,"\n");

 return;

}


// Applies the fit to a specified X range and writes the
// resulting X Y values to {baseName}_out.dat
int applyOrthoPoly::applyFit(double min, double step, double max){

 if (_degree < 0){
  fprintf(stderr, "Call to applyFit() with a class that didn't initialize properly\n");
  return -1;
 }

 // Get the output file open.
 char outFile[1024];
 sprintf(outFile, "%s_out.dat", _baseName);
 FILE *fp = fopen(outFile, "w");
 if (fp == NULL){
  fprintf(stderr, "ERROR : Failed to open %s\n", outFile);
  return -1;
 }

 // Loop through X values, generating Y values and writing to file
 // as we go.
 double x = min;
 while (x <= max){
  double y = predict(x);
  fprintf(fp, "%lf\t%lf\n", x,y);
  x += step;
 }

 fclose(fp);

 return 0;

}

// Destructor
applyOrthoPoly::~applyOrthoPoly(){
 free(_baseName);
 return;
}

// Internal method. You give it the vector to read into (by reference) and the name, and it reads the vector.
// if the _baseName is "myData" and you pass in "polyAlphas" as name, it will read
// myData_polyAlphas.dat into the vector. Used to read the parameters from R.
int applyOrthoPoly::_readDoubleVector(const char *name, vector<double> &vec){

 char filename[1024];
 sprintf(filename, "%s_%s.dat", _baseName, name);

 FILE *fp = fopen(filename, "r");
 if (fp == NULL){
  fprintf(stderr, "Failed to open %s\n", filename);
  return -1;
 }

 if (_debug){ fprintf(stdout, "\nReading from %s\n", filename); }
 char line[1024];

 int iline=0;
 while (NULL != fgets(line, 1024, fp)){
  iline++;
  double value = atof(line);
  if (_debug){ fprintf(stdout, "Line %d : %lf\n", iline, value); }
  vec.push_back(value);
 }


 fclose(fp);

 return 0;
}


// This is the real "meat" - to get the ortho polys out
// you have to do this recursive function, see :
// https://stackoverflow.com/questions/26728289/extracting-orthogonal-polynomial-coefficients-from-rs-poly-function
// Many of the comments from that page are below (because they're rather hard to find).
// So you pass in :
//
//  * The degree of polynomial, d. With C++ indexing, it works out that :
//  d == -1 => Constant term (we recurse down to this, never call it directly)
//  d == 0 => Linear term (the first one we apply a model coefficient to)
//  d == 1 => Quadratic
//  d == 2 => Cubic
//
//  and :
//
//  * The x value at which to evaluate the polynomial.
//
double applyOrthoPoly::_f(int d, double x){

 // The above web page is a stack overflow question that is essentially, How
 // to do an ortho poly fit in R and then save it out and use it in another language,
 // which is what we're doing here.
 //
 // From that web page :
 //
 // The polynomials are defined recursively using the alpha and norm2
 // coefficients of the poly object you've created. Let's look at an example in R :
 //
 // z <- poly(1:10, 3)
 // attributes(z)$coefs
 // $alpha
 // [1] 5.5 5.5 5.5
 // $norm2
 // [1]    1.0   10.0   82.5  528.0 3088.8
 //
 // For notation, let's call a_d the element in index d of alpha, and 
 // let's call n_d the element in index d of norm2. F_d(x) will be the
 // orthogonal polynomial of degree d that is generated. For some base 
 // cases we have:
 //
 // F_0(x) = 1 / sqrt(n_2)
 // F_1(x) = (x-a_1) / sqrt(n_3)
 //
 // The rest of the polynomials are recursively defined:
 // F_d(x) = [(x-a_d) * sqrt(n_{d+1}) * F_{d-1}(x) - n_{d+1} / sqrt(n_d) * F_{d-2}(x)] / sqrt(n_{d+2})
 //
 // x <- 2.1
 // predict(z, newdata=x)
 // #               1         2         3
 // # [1,] -0.3743277 0.1440493 0.1890351
 // # ...
 //
 // a <- attributes(z)$coefs$alpha
 // n <- attributes(z)$coefs$norm2
 // f0 <- 1 / sqrt(n[2])
 // (f1 <- (x-a[1]) / sqrt(n[3]))
 // # [1] -0.3743277
 //
 // (f2 <- ((x-a[2]) * sqrt(n[3]) * f1 - n[3] / sqrt(n[2]) * f0) / sqrt(n[4]))
 // # [1] 0.1440493
 //
 // (f3 <- ((x-a[3]) * sqrt(n[4]) * f2 - n[4] / sqrt(n[3]) * f1) / sqrt(n[5]))
 // # [1] 0.1890351
 //
 // The most compact way to export your polynomials to your C++ code would probably be to 
 // export attributes(z)$coefs$alpha and attributes(z)$coefs$norm2 and then use the recursive
 // formula in C++ to evaluate your polynomials.

 // Which is what we'll do here.

 // Deal with an error condition that I doubt will ever come up.
 if ((d > _degree-1) || (d < -1)){
  fprintf(stderr, "ERROR : Poly was called for degree that does not exist\n");
  return 0;
 }

 // Zeroth degree, using C indexing this gets pushed back to d == -1
 // F_0(x) = 1 / sqrt(n_2) 
 if (d == -1){
  return 1.0 / sqrt(_norm2[1]);
 }

 // First degree, at d== 0. This is the linear term, the first "real" one.
 // F_1(x) = (x-a_1) / sqrt(n_3)
 if (d == 0){
  return (x - _alpha[0]) / sqrt(_norm2[2]);
 }

 // Other, higher degrees, d inclusively in range [ 1 to _degree-1 ] are recursive :
 // F_d(x) = [(x-a_d)    * sqrt(n_{d+1})     * F_{d-1}(x) - n_{d+1}    / sqrt(n_d)       * F_{d-2}(x)] / sqrt(n_{d+2})
 return ((x - _alpha[d]) * sqrt(_norm2[d+1]) * _f(d-1,x) - _norm2[d+1] / sqrt(_norm2[d]) * _f(d-2, x)) / sqrt(_norm2[d+2]);

}


// Given an X value, get the Y value predicted by the model.
// Has to generate the polynomials to do this.
double applyOrthoPoly::predict(double x){

 if (_degree < 0){
  fprintf(stderr, "ERROR : Call to predict() with a class that did not ititialize properly\n");
  return 0;
 }

 double y=_coeffs[0];
 // fprintf(stdout, "Starting Y is %lf\n", y);
 for (int i=0; i < _degree; i++){
  y += _coeffs[i+1]*_f(i, x); 
  // fprintf(stdout, " After adding term %d I have %lf\n", i+1, y);
 }

 return y;

}


// Handy debugging option - you can save the polynomials over a range
// of X values to the file {baseName}_savePolys.dat which may be good
// for comparing to the polynomials in R.
int applyOrthoPoly::savePolys(double min, double step, double max){

 if (_degree < 0){
  fprintf(stderr, "Call to a class that didn't initialize properly\n");
  return -1;
 }


 char outFile[1024];
 sprintf(outFile, "%s_savePolys.dat", _baseName);
 FILE *fp = fopen(outFile, "w");
 if (fp == NULL){
  fprintf(stderr, "ERROR : Failed to open %s\n", outFile);
  return -1;
 }

 // Put header on output file.
 fprintf(fp, "dataValue\t");
 for (int i=0; i < _degree; i++){
  fprintf(fp, "polynomial_%02d\t", i+1);
 }
 fprintf(fp, "\n");

 // Loop through adding values.
 double x=min;
 while (x <= max){
  fprintf(fp, "%lf\t", x);
  for (int i=0; i < _degree; i++){
   fprintf(fp, "%lf\t", _f(i,x));
  }
  fprintf(fp, "\n");
  x += step;
 }

 fclose(fp);

 return 0;

}


