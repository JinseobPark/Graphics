/**
\file        Clip.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        14/5/2018
\brief		 Clipping a line on the plane
*/
#include "Clip.h"
#include <iostream>


Hcoord FineIntersection(Hcoord& V1, Hcoord& V2, HalfSpace& H)
{
	float t = dot(H, V1) / (dot(H, V1) - dot(H, V2));

	return Hcoord((1.f - t) * V1 + t * V2);
}


std::vector<Hcoord> NewVert(std::vector<Hcoord>& vertices, HalfSpace& H)
{
	std::vector<Hcoord> temp;

	for (unsigned j = 0; j < vertices.size() - 1; ++j)
	{
		if ((dot(H, vertices[j]) > 0) && (dot(H, vertices[j + 1]) <= 0))
		{
			temp.push_back(FineIntersection(vertices[j], vertices[j + 1], H));
		}
		else if ((dot(H, vertices[j]) <= 0) && (dot(H, vertices[j + 1]) <= 0))
		{
			temp.push_back(vertices[j]);
		}
		else if ((dot(H, vertices[j]) <= 0) && (dot(H, vertices[j + 1]) > 0))
		{
			temp.push_back(vertices[j]);
			temp.push_back(FineIntersection(vertices[j], vertices[j + 1], H));
		}
	}

	if ((dot(H, vertices[vertices.size() - 1]) > 0) && (dot(H, vertices[0]) <= 0))
	{
		temp.push_back(FineIntersection(vertices[vertices.size() - 1], vertices[0], H));
	}
	else if ((dot(H, vertices[vertices.size() - 1]) <= 0) && (dot(H, vertices[0]) <= 0))
	{
		temp.push_back(vertices[vertices.size() - 1]);
	}
	else if ((dot(H, vertices[vertices.size() - 1]) <= 0) && (dot(H, vertices[0]) > 0))
	{
		temp.push_back(vertices[vertices.size() - 1]);
		temp.push_back(FineIntersection(vertices[vertices.size() - 1], vertices[0], H));
	}

	vertices = temp;

	return vertices;
}

bool Clip::operator()(std::vector<Hcoord>& vertices)
{
	unsigned int i = 0;

	if (vertices.size() == 0)
		return false;
	else
	{
		while (i < half_spaces.size())
		{
			NewVert(vertices, half_spaces[i]);
			if (vertices.size() == 0)
				return false;
			++i;
		}
	}
	return true;
}