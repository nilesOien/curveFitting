# curveFitting
A system for using R to fit orthogonal polynomials

The essential premise here is that given a set of X Y points, the R
statistical package can be used to fit orthogonal polynomials of
degree 1 to N to the data. 

The parameters of the polynomials, and the coefficients of the model
fit, can then be saved out in small ASCII files. These can then be
read into another language (C++ here) to apply the fit over a
range of X values.

Basically, exporting R's strong ability to do orthogonal polynomial
fits into another language.

The details of applying the fit are somewhat arcane and the specific
information was a bit hard to find, so this example code is documented
reasonably heavily.

Niles Oien nilesOien@gmail.com February 2020.

