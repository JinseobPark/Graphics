/**
\file        CameraRender2D.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        9/4/2018
\brief		 Camera rendering in 3D.
*/
#include "CameraRender2D.h"
#include "Projection.h"

CameraRender2D::CameraRender2D(Render & r):render(r)
{
	cam_vertices.clear();
	ndc_vertices.clear();
}

CameraRender2D::~CameraRender2D(void)
{

}

void CameraRender2D::SetCamera(const Camera & cam)
{
	world2camera = WorldToCamera(cam);
	camera2ndc = CameraToNDC(cam);
}

void CameraRender2D::DisplayEdges(Mesh & m, const Affine & A, const Vector & color)
{
	render.SetColor(color);

	for (int i = 0; i < m.EdgeCount(); ++i)
	{
		render.DrawLine(camera2ndc * world2camera * A * m.GetVertex(m.GetEdge(i).index1), 
					    camera2ndc * world2camera * A * m.GetVertex(m.GetEdge(i).index2));
	}
}

void CameraRender2D::DisplayFaces(Mesh & m, const Affine & A, const Vector & color)
{
	Hcoord hP, hQ, hR;
	Point P, Q, R;
	Vector m_vector;
	Point E_Point(0, 0, 0);

	float u_value;
	for (int i = 0; i < m.FaceCount(); ++i)
	{
		hP = world2camera * A * m.GetVertex(m.GetFace(i).index1);
		hQ = world2camera * A * m.GetVertex(m.GetFace(i).index2);
		hR = world2camera * A * m.GetVertex(m.GetFace(i).index3);
		P = Point(hP.x / hP.w, hP.y / hP.w, hP.z / hP.w);
		Q = Point(hQ.x / hQ.w, hQ.y / hQ.w, hQ.z / hQ.w);
		R = Point(hR.x / hR.w, hR.y / hR.w, hR.z / hR.w);

		m_vector = cross(Q - P, R - P);

		if (dot(m_vector,  (E_Point - P)) > 0)
		{
			Vector L_vector(0, 0, 1);
			u_value = abs(dot(m_vector, L_vector)) / (abs(m_vector)*abs(L_vector));
			render.SetColor(u_value * color);

			render.FillTriangle(camera2ndc * P, camera2ndc *  Q, camera2ndc *  R);
		}
	}
}
