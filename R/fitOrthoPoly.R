#!/bin/Rscript

# R script to fit an Nth degree ortho polynomial to data.
# Input on command line is :
#
# [1] The input file name. Input file is just a set of x y pairs like so :
#
# -0.9026  0.217196999999996
# -0.72812  0.203766999999994
# -0.64088  0.184278000000005
# -0.9026  0.217196999999996
#
# [2] The degree of polynomial to fit, 2=quadratic, 3=cubic etc
#
# [3] A "base name" for output files, like "myData". Output files will then be :
#
# myData.png - Plot of data with fitted polynomial
# myData_modelCoefficients.dat - Model coefficients to apply to polynomials, degree+1 of them (includes intercept)
# myData_polyAlphas.dat - Alpha values to describe polynomials, degree of them
# myData_polyNorms.dat  - norm2 values to describe polynomials, degree+2 of them
#
# These can then be read into a C++ or other program to apply the fit.
#
# Niles Oien February 2020
#

args = commandArgs(trailingOnly=TRUE)
if (length(args) < 3){
 cat("Need an input file, a polynomial degree, and a base output name on the command line\n")
 cat("EXAMPLE : fitOrthoPoly.R in.dat 3 myData\n")
 q(save='no')
}
inFile <- args[1]
deg <- as.numeric(args[2])
baseName <- args[3]

if (!(file.exists(inFile))){
 cat(paste("ERROR :", inFile, "not found.\n"))
 q(save='no')
}

# Read the input file.
data <- read.table(inFile, header=FALSE)

x <- data[,1]
y <- data[,2]

# Sort the data into order by x value, largely for plotting the fit.
y <- y[order(x)]
x <- x[order(x)]

# Generate the othogonal polynomials and fit the data to them.
z <- poly(raw=FALSE, x, deg)
model <- lm(y ~ z)

# Print model details.
summary(model)

# Save the polynomial alpha and norm2 values to file.
polyAlphas <- attr(z,"coefs")$alpha
polyNorms <- attr(z,"coefs")$norm2

alphaFile <- paste(sep="", baseName, "_polyAlphas.dat")
write.table(row.names=FALSE, col.names=FALSE, file=alphaFile, polyAlphas)

normFile <- paste(sep="", baseName, "_polyNorms.dat")
write.table(row.names=FALSE, col.names=FALSE, file=normFile, polyNorms)

# Ditto the model coefficients.
modelCoeffs <- model$coefficients

modelFile <- paste(sep="", baseName, "_modelCoefficients.dat")
write.table(row.names=FALSE, col.names=FALSE, file=modelFile, modelCoeffs)


# Do a plot of the data and the fit.
plotFile <- paste(sep="", args[3], ".png")
png(plotFile, height=500, width=800)

xLimits <- c(min(x), max(x))
yLimits <- c(min(y), max(y))

# Data
plot(x,y,xlim=xLimits, ylim=yLimits, main=paste("Data in", inFile, "(green) with degree", deg, " polynomial fit (red)"),
      pch=20, col='green', sub=paste(length(x), "points found"))

# Fit
par(new=T)
plot(x, fitted(model), type='l', col='red',
     xlab="", ylab="", main="", xaxt='n', yaxt='n', xlim=xLimits, ylim=yLimits)




q(save='no')

