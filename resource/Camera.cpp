/**
\file        Camera.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        2/4/2018
\brief		 Camera setting in 3D.
*/
#include "Camera.h"

Camera::Camera(void)
{
	eye = Point(0.f, 0.f, 0.f);
	right = Vector(1.f, 0.f, 0.f);
	up = Vector(0.f, 1.f, 0.f);
	back = Vector(0.f, 0.f, 1.f);
	width = 1.f;
	height = 1.f;
	distance = 0.5f;
	near = 0.1f;
	far = 10.f;

}

/*
n = -l.v / size(l)
u = cross(l, r) / size(cross(l, r))
v = cross(n, u)

distance = (near + far) / 2
width = 2 * distance * tan(fov / 2)
hdight = width / aspect
*/
Camera::Camera(const Point & E, const Vector & look, const Vector & vp, float fov, float aspect, float near, float far)
:near(near), far(far)
{
	eye = E;
	right = normalize(cross(look, vp));
	back = -normalize(look);
	up = normalize(cross(back, right));
	distance = (near + far) * 0.5f;
	width = 2.f * distance * tan(fov * 0.5f);
	height = width / aspect;

}

Point Camera::Eye(void) const
{
	return eye;
}

Vector Camera::Right(void) const
{
	return normalize(right);
}

Vector Camera::Up(void) const
{
	return normalize(up);
}

Vector Camera::Back(void) const
{
	return normalize(back);
}

Vector Camera::ViewportGeometry(void) const
{
	return Vector(width, height, distance);
}

float Camera::NearDistance(void) const
{
	return near;
}

float Camera::FarDistance(void) const
{
	return far;
}

Camera & Camera::Zoom(float factor)
{
	width *= factor;
	height *= factor;
	return *this;
}

Camera & Camera::Forward(float distance_increment)
{
	eye = eye - (distance_increment * back);
	return *this;
}

Camera & Camera::Yaw(float angle)
{
	right = rotate(angle, up) * right;
	back = rotate(angle, up) * back;
	return *this;
}

Camera & Camera::Pitch(float angle)
{
	up = rotate(angle, right) * up;
	back = rotate(angle, right) * back;
	return *this;
}

Camera & Camera::Roll(float angle)
{
	right = rotate(angle, back) * right;
	up = rotate(angle, back) * up;
	return *this;
}
