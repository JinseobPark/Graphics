/**
\file        CubeMesh.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        19/3/2018
\brief		 3D Triangular Mesh interface

*/
#include "CubeMesh.h"

const Point CubeMesh::vertices[8] = {Point(-1, -1, -1), Point(1, -1, -1), Point(1, 1, -1), Point(-1, 1, -1),
									 Point(-1, -1, 1), Point(1, -1, 1), Point(1, 1, 1), Point(-1, 1, 1) };
const Mesh::Face CubeMesh::faces[12] = { Face(0, 3, 2),Face(2, 1, 0),Face(4, 5, 6),Face(6, 7, 4),
										Face(5, 1, 2),Face(2, 6, 5),Face(6, 2, 3),Face(3, 7, 6),
										Face(7, 3, 0),Face(0, 4, 7),Face(4, 0, 1),Face(1, 5, 4)
};

const Mesh::Edge CubeMesh::edges[12] = { Edge(0, 1), Edge(1, 2), Edge(2, 3), Edge(3, 0),
										Edge(4, 5), Edge(5, 6), Edge(6, 7), Edge(7, 4),
										Edge(0, 4), Edge(1, 5), Edge(2, 6), Edge(3, 7) };


int CubeMesh::VertexCount(void)
{
	return (sizeof(vertices) / sizeof(*vertices));
}

Point CubeMesh::GetVertex(int i)
{
	return vertices[i];
}

Vector CubeMesh::Dimensions(void)
{
	return Vector(GetVertex(GetEdge(0).index2).x - GetVertex(GetEdge(0).index1).x,
				  GetVertex(GetEdge(0).index2).y - GetVertex(GetEdge(0).index1).y,
				  GetVertex(GetEdge(0).index2).z - GetVertex(GetEdge(0).index1).z);
}

Point CubeMesh::Center(void)
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

int CubeMesh::FaceCount(void)
{
	return (sizeof(faces) / sizeof(*faces));
}

Mesh::Face CubeMesh::GetFace(int i)
{
	return faces[i];
}

int CubeMesh::EdgeCount(void)
{
	return (sizeof(edges) / sizeof(*edges));
}

Mesh::Edge CubeMesh::GetEdge(int i)
{
	return edges[i];
}
