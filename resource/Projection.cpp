/**
\file        Projection.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        9/4/2018
\brief		 Camera projection in 3D.

*/
#include "Projection.h"
Affine CameraToWorld(const Camera & cam)
{
	return Affine((cam.Right()), (cam.Up()), (cam.Back()), (cam.Eye()));
}

Affine WorldToCamera(const Camera & cam)
{
	return inverse(CameraToWorld(cam));
}

Matrix CameraToNDC(const Camera & cam)
{
	Matrix result;

	result[0][0] = 2 * cam.ViewportGeometry().z / cam.ViewportGeometry().x;
	result[1][1] = 2 * cam.ViewportGeometry().z / cam.ViewportGeometry().y;
	result[2][2] = (cam.NearDistance() + cam.FarDistance()) / (cam.NearDistance() - cam.FarDistance());
	result[2][3] = 2 * cam.NearDistance() * cam.FarDistance() / (cam.NearDistance() - cam.FarDistance());
	result[3][2] = -1;

	return result;
}
