\name{ncdf}
\alias{ncdf}
\title{Read, write, and create netCDF files}
\description{
 Read from or write to existing netCDF format files, or create new ones.
}
\references{
 http://www.unidata.ucar.edu/packages/netcdf/
}
\details{
 The netCDF data file format from Unidata is a platform-independent, binary
 file that also contains metadata describing the contents and format of the data
 in the file.  NetCDF files contain one or more variables, which are 
 structured as regular N-dimensional arrays.  They also contain dimensions, which 
 describe the extent of the variables' arrays.  Data can be read from
 or written to variables in arbitrary hyperslabs.  The R package 'ncdf' allows
 reading from, writing to, and creation of netCDF files.  Note that the
 netCDF library must already be installed on your machine for this R interface
 to the library to work.

 This package ("ncdf") is not compatible with the "netCDF" package by Thomas Lumley.
 On the other had, it implements considerably more functionality than the
 "netCDF" package, including creating and writing to netCDF files instead
 of simply reading from them.
}
\seealso{ 
 \code{\link[ncdf]{att.put.ncdf}}, \code{\link[ncdf]{att.get.ncdf}},
 \code{\link[ncdf]{close.ncdf}}, \code{\link[ncdf]{create.ncdf}},
 \code{\link[ncdf]{dim.def.ncdf}},
 \code{\link[ncdf]{get.var.ncdf}},
 \code{\link[ncdf]{put.var.ncdf}},
 \code{\link[ncdf]{open.ncdf}},
 \code{\link[ncdf]{print.ncdf}},
 \code{\link[ncdf]{set.missval.ncdf}},
 \code{\link[ncdf]{sync.ncdf}},
 \code{\link[ncdf]{var.def.ncdf}}.
}
\author{David W. Pierce \email{dpierce@ucsd.edu}}
\keyword{utilities}
