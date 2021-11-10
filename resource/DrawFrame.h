// DrawFrame.h

#ifndef DRAWFRAME_H
#define DRAWFRAME_H

#include "Raster.h"

void InitFrame(int width, int height);

void DrawFrame(Raster& r, double dt, int cam_num);

#endif

