/**
\file        SimpleRender.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        2/4/2018
\brief		 Render 3D line segments and triangles

*/

#include "SimpleRender.h"

SimpleRender::SimpleRender(Render & r) : render(r)
{
	PersProj[0][0] = 1.f; PersProj[1][0] = 0.f; PersProj[2][0] = 0.f;          PersProj[3][0] = 0.f;
	PersProj[0][1] = 0.f; PersProj[1][1] = 1.f; PersProj[2][1] = 0.f;          PersProj[3][1] = 0.f;
	PersProj[0][2] = 0.f; PersProj[1][2] = 0.f; PersProj[2][2] = 1.f/11.f;     PersProj[3][2] = -(5.f/11.f);
	PersProj[0][3] = 0.f; PersProj[1][3] = 0.f; PersProj[2][3] = -(2.f/11.f);  PersProj[3][3] = 10.f/11.f;
	world_vertices.clear();
	proj_vertices.clear();
}

SimpleRender::~SimpleRender(void)
{
	proj_vertices.clear();
	world_vertices.clear();
}

void SimpleRender::DisplayEdges(Mesh & m, const Affine & A, const Vector & color)
{
	render.SetColor(color);

	for (int i = 0; i < m.EdgeCount(); ++i)
	{
		render.DrawLine(PersProj * A * m.GetVertex(m.GetEdge(i).index1), PersProj * A * m.GetVertex(m.GetEdge(i).index2));
	}
}

void SimpleRender::DisplayFaces(Mesh & m, const Affine & A, const Vector & color)
{
	Point P, Q, R;
	Vector m_vector;
	Point E_Point(0, 0, 5);
	float u_value;

	for (int i = 0; i < m.FaceCount(); ++i)
	{
		P =  A * m.GetVertex(m.GetFace(i).index1);
		Q =  A * m.GetVertex(m.GetFace(i).index2);
		R =  A * m.GetVertex(m.GetFace(i).index3);
		m_vector = cross(Q - P, R - P);

		if (dot(m_vector, E_Point - P) > 0)
		{
			Vector L_vector(0, 0, 1);
			u_value = abs(dot(m_vector, L_vector)) / (abs(m_vector)*abs(L_vector));
			render.SetColor(u_value * color);
			render.FillTriangle(PersProj * P, PersProj * Q, PersProj * R);
		}
	}
}
