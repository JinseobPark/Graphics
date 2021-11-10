// RasterUtilities.h
// -- clear frame/z-buffer and triangle drawing

#ifndef RASTERUTILITIES_H
#define RASTERUTILITIES_H

#include "Raster.h"
#include "Affine.h"


void ClearBuffers(Raster& r, float z=1);

void FillTriangle(Raster& r, const Hcoord& P, const Hcoord& Q, const Hcoord& R);


#endif

