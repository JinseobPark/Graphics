/**
\file        HalfSpace.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        07/5/2018
\brief		 Constructing halfspaces and convex polyhedra

*/
#include "HalfSpace.h"

HalfSpace::HalfSpace(const Vector & m, const Point & C)
{
	x = m.x;
	y = m.y;
	z = m.z;
	w = -(m.x * C.x + m.y * C.y + m.z * C.z);
}

HalfSpace::HalfSpace(const Point & A, const Point & B, const Point & C, const Point & P)
{
	Vector c_a = C - A;
	Vector b_a = B - A;
	Vector m = cross(b_a, c_a);

	x = m.x;
	y = m.y;
	z = m.z;
	w = -(m.x * A.x + m.y * A.y + m.z * A.z);

	if (x*P.x + y*P.y + z*P.z + w <= 0) return;
	else
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

}


Polyhedron::Polyhedron(bool cube)
{
	Point   V1(1, 1, 1),
			V2(1, 1, -1),
			V3(1, -1, -1),
			V4(1, -1, 1),
			V5(-1, 1, 1),
			V6(-1, 1, -1),
			V7(-1, -1, -1),
			V8(-1, -1, 1);
	if (cube)
	{
			half_spaces.push_back(HalfSpace(V3, V2, V1, V4)); //right
			half_spaces.push_back(HalfSpace(V5, V6, V7, V8)); //left
			half_spaces.push_back(HalfSpace(V1, V2, V6, V5)); //top
			half_spaces.push_back(HalfSpace(V4, V8, V7, V3)); //bottom
			half_spaces.push_back(HalfSpace(V1, V5, V8, V4)); //front
			half_spaces.push_back(HalfSpace(V7, V6, V2, V3)); //back
	}
	else
	{
		half_spaces.clear();
	}
}

Polyhedron::Polyhedron(float fov, float a, float N, float F)
{
	float Wn = 2.f * N * tan(fov * 0.5f);
	float Hn = Wn / a;
	float Wf = 2.f * F * tan(fov * 0.5f);
	float Hf = Wf / a;

	Point NearFace[4] = { Point(0.5f * Wn, 0.5f * Hn, -N),Point(-0.5f * Wn, 0.5f * Hn, -N) ,Point(-0.5f * Wn, -0.5f * Hn, -N) ,Point(0.5f * Wn, -0.5f * Hn, -N) };
	Point Far_Face[4] =  { Point(0.5f * Wf, 0.5f * Hf, -F),Point(-0.5f * Wf, 0.5f * Hf, -F),Point(-0.5f * Wf, -0.5f * Hf, -F),Point(0.5f * Wf, -0.5f * Hf, -F) };

	half_spaces.push_back(HalfSpace(NearFace[0], NearFace[3], Far_Face[3], Far_Face[0])); //right
	half_spaces.push_back(HalfSpace(NearFace[0], Far_Face[0], Far_Face[1], NearFace[1])); //up
	half_spaces.push_back(HalfSpace(NearFace[1], Far_Face[1], Far_Face[2], NearFace[2])); //left
	half_spaces.push_back(HalfSpace(NearFace[2], Far_Face[2], Far_Face[3], NearFace[3])); //down
	half_spaces.push_back(HalfSpace(NearFace[0], NearFace[1], NearFace[2], NearFace[3])); //for
	half_spaces.push_back(HalfSpace(Far_Face[0], Far_Face[3], Far_Face[2], Far_Face[1])); //back
}

bool contains(const Polyhedron & polyhedron, const Hcoord & P)
{
	for (unsigned int i = 0; i < polyhedron.half_spaces.size(); ++i)
	{
		if (dot(polyhedron.half_spaces[i], P) > 0)
			return false;
	}
	return true;
}
