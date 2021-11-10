// Projection.h
// -- world-to-camera, and camera-to-ndc transforms

#ifndef PROJECTION_H
#define PROJECTION_H

#include "Camera.h"


Affine CameraToWorld(const Camera& cam);
Affine WorldToCamera(const Camera& cam);
Matrix CameraToNDC(const Camera& cam);


#endif

