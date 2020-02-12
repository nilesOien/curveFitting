#!/bin/bash

# Clean up from any previous run
find . -name myData\* -exec /bin/rm -f {} \;
/bin/rm -f verify/*.dat
/bin/rm -f verify/*.png
cd Cpp/
make clean
cd ../

# Build C++ program.
cd Cpp/
make
if [ ! -f applyOrthoPoly ]
then
 echo Failed to build applyOrthoPoly in Cpp/ dir
 exit -1
fi
cd ..


# Use R to fit a cubic curve to the data in in.dat
# Using "myData" as the base name, this writes the files :
# myData_modelCoefficients.dat -  N+1 Coefficients to apply to the polynomials (includes intercept)
# myData_polyAlphas.dat -         N alpha values that define Nth degree polynomials.
# myData_polyNorms.dat -          N+2 norm2 values that define Nth degree polynomials
# myData.png -                    Plot showing data and modelled fit
#
echo PART ONE : FITTING THE DATA WITH R :
cd R/
./fitOrthoPoly.R in.dat 3 myData
cd ..

# Copy the output over into the Cpp/ directory.
cp -v R/myData*.dat Cpp/

# Run the C++ code to read in the polynomial definition and model coefficients and
# then generate fitted data over a wider range of data. This writes the files :
# myData_out.dat - The fitted X and Y over the wider range, and
# myData_savePolys.dat - The individual polynomials, mostly for debugging (only saved in debug mode).
echo PART TWO : APPLYING THE FIT IN C++ :
cd Cpp/
# Run in normal mode :
# ./applyOrthoPoly myData -1.5 0.01 1.5

# For debug mode, add "debug" argument :
./applyOrthoPoly myData -1.5 0.01 1.5 debug
cd ..

# Make a plot to verify the fit.
echo PART THREE : VISUAL VERIFICATION, MAKING PLOT FILES
cd verify
cp -v ../R/in.dat .
cp -v ../Cpp/myData_out.dat .
./verify.R in.dat myData_out.dat

echo
echo Original data with R fit shown in R/myData.png 
echo Fit applied over larger range by C++ code is shown in verify/verify.png
echo

exit 0


