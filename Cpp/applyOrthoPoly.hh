
#ifndef APPLY_POLY_HH
#define APPLY_POLY_HH

using namespace std;

#include <vector>

class applyOrthoPoly {

public :

 // Constructor
 applyOrthoPoly(char *baseName, int debug);

 // Returns degree, or -1 if construction was bad.
 int getDegree();

 // Print class internal values.
 void printPoly();

 // Write the polynomial as fit over a specified range to a file.
 int applyFit(double min, double step, double max);

 // For a given x value get the modelled y value.
 double predict(double x);

 // Save out polynomials to file - maybe useful for debugging.
 int savePolys(double min, double step, double max);

 // Destructor
 ~applyOrthoPoly();

protected:
private:

 // Method to read a file into a vector of floating point values, one value per line
 int _readDoubleVector(const char *name, vector<double> &vec); 

 // Get y value for polynomial of degree d at point x
 double _f(int d, double x);


 // Internal variables.
 vector<double> _alpha;
 vector<double> _norm2;
 vector<double> _coeffs;
 int _degree;
 int _debug;
 char *_baseName;

};

#endif

