#!/bin/Rscript

# Do a plot to verify fit to data. Two arguments are :
#  * The input raw data file that went into the fit, and
#  * The fitted data file. Both are X Y pairs.
# Niles Oien February 2020
#

args = commandArgs(trailingOnly=TRUE)
if (length(args) < 2){
 cat("Need original raw data file and fitted data file on the command line\n")
 cat("EXAMPLE : verify.R in.dat myData_out.dat\n")
 q(save='no')
}
rawFile <- args[1]
fitFile <- args[2]

if (!(file.exists(rawFile))){
 cat(paste("ERROR :", rawFile, "not found.\n"))
 q(save='no')
}

if (!(file.exists(fitFile))){
 cat(paste("ERROR :", fitFile, "not found.\n"))
 q(save='no')
}




# Read the input files.
rawData <- read.table(rawFile, header=FALSE)
fitData <- read.table(fitFile, header=FALSE)

# Get the overall min and max in X and Y for setting plot limits.
rawX <- rawData[,1]
rawY <- rawData[,2]

fitX <- fitData[,1]
fitY <- fitData[,2]

maxX <- max(c(max(rawX), max(fitX)))
minX <- min(c(min(rawX), min(fitX)))

maxY <- max(c(max(rawY), max(fitY)))
minY <- min(c(min(rawY), min(fitY)))

xLimits <- c(minX, maxX)
yLimits <- c(minY, maxY)

# Do the plot.
png("verify.png", height=500, width=800)


# Data
plot(rawX,rawY,xlim=xLimits, ylim=yLimits, main=paste("Data in", rawFile, "(green) with", 
      "polynomial fit from", fitFile, " (red)"), xlab="X", ylab="Y",
      pch=20, col='green', sub=paste(length(rawX), " data points found,", length(fitX), "fitted points generated"))

# Fit
par(new=T)
plot(fitX, fitY, type='l', col='red',
     xlab="", ylab="", main="", xaxt='n', yaxt='n', xlim=xLimits, ylim=yLimits)

cat("Verification plot in verify.png\n")

q(save='no')

