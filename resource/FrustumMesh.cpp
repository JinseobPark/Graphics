/**
\file        FrustumMesh.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        2/4/2018
\brief		 3D Triangular Mesh interface

*/
#include "FrustumMesh.h"


const Mesh::Face FrustumMesh::faces[12] = { Face(1, 2, 3),Face(3, 4, 1),Face(6, 5, 8),Face(8, 7, 6),
											Face(2, 6, 7),Face(7, 3, 2),Face(3, 7, 8),Face(8, 4, 3),
											Face(4, 8, 5),Face(5, 1, 4),Face(1, 5, 6),Face(6, 2, 1)
};

const Mesh::Edge FrustumMesh::edges[16] =  { Edge(0,1), Edge(0,2), Edge(0,3), Edge(0,4),
											 Edge(1,2), Edge(2,3), Edge(3,4), Edge(4,1),
											 Edge(1,5), Edge(2,6), Edge(3,7), Edge(4,8),
											 Edge(5,6), Edge(6,7), Edge(7,8), Edge(8,5) };

FrustumMesh::FrustumMesh(float fov, float a, float n, float f)
{
	float near_width = 2.f * n * tan(fov * 0.5f);
	float near_height = near_width / a;
	float far_width = 2.f * f * tan(fov * 0.5f);
	float far_height = far_width / a;
	
	vertices[0] = Point(0.f, 0.f, 0.f);
	vertices[1] = Point(-near_width * 0.5f, near_height * 0.5f, -n);
	vertices[2] = Point(-near_width * 0.5f, -near_height * 0.5f, -n);
	vertices[3] = Point(near_width * 0.5f, -near_height * 0.5f, -n);
	vertices[4] = Point(near_width * 0.5f, near_height * 0.5f, -n);
	vertices[5] = Point(-far_width * 0.5f, far_height * 0.5f, -f);
	vertices[6] = Point(-far_width * 0.5f, -far_height * 0.5f, -f);
	vertices[7] = Point(far_width * 0.5f, -far_height * 0.5f, -f);
	vertices[8] = Point(far_width * 0.5f, far_height * 0.5f, -f);

}

int FrustumMesh::VertexCount(void)
{
	return (sizeof(vertices) / sizeof(*vertices));
}

Point FrustumMesh::GetVertex(int i)
{
	return vertices[i];
}

Vector FrustumMesh::Dimensions(void)
{
	return Vector(GetVertex(GetEdge(0).index2).x - GetVertex(GetEdge(0).index1).x,
			    	GetVertex(GetEdge(0).index2).y - GetVertex(GetEdge(0).index1).y,
					GetVertex(GetEdge(0).index2).z - GetVertex(GetEdge(0).index1).z);
}

Point FrustumMesh::Center(void)
{
	Point result;
	for (int i = 0; i < VertexCount(); ++i)
	{
		result.x += GetVertex(i).x;
		result.y += GetVertex(i).y;
		result.z += GetVertex(i).z;
	}
	result.x /= VertexCount();
	result.y /= VertexCount();
	result.z /= VertexCount();

	return result;
}

int FrustumMesh::FaceCount(void)
{
	return (sizeof(faces) / sizeof(*faces));
}

Mesh::Face FrustumMesh::GetFace(int i)
{
	return faces[i];
}

int FrustumMesh::EdgeCount(void)
{
	return (sizeof(edges) / sizeof(*edges));
}

Mesh::Edge FrustumMesh::GetEdge(int i)
{
	return edges[i];
}
