// Camera.h
// -- 3D camera class

#ifndef CAMERA_H
#define CAMERA_H

#include "Affine.h"


class Camera {
  public:
    Camera(void);
    Camera(const Point& E, const Vector& look, const Vector& vp,
           float fov, float aspect, float near, float far);
    Point Eye(void) const;
    Vector Right(void) const;
    Vector Up(void) const;
    Vector Back(void) const;
    Vector ViewportGeometry(void) const;
    float NearDistance(void) const;
    float FarDistance(void) const;
    Camera& Zoom(float factor);
    Camera& Forward(float distance);
    Camera& Yaw(float angle);
    Camera& Pitch(float angle);
    Camera& Roll(float angle);
  private:
    Point eye;
    Vector right, up, back;
    float width, height, distance,
          near, far;
};


#endif

