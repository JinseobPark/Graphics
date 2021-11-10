/**
\file        Interpolate.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        26/5/2018
\brief		 Interpolate texture

*/
#include "Interpolate.h"

//initialize
Texture *TexturedCoord::texture = 0;
float TexturedCoord::color_scale = 0;

TexturedCoord FineIntersection(TexturedCoord& V1, TexturedCoord& V2, HalfSpace& H)
{
	float t = dot(H, V1) / (dot(H, V1) - dot(H, V2));

	return TexturedCoord((1 - t) * V1 + t * V2, (1.f - t) * V1.u + t * V2.u, (1 - t) * V1.v + t * V2.v);
}

Vector mvector(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
	return cross(Vector(bx - ax, by - ay, bz - az), Vector(cx - ax, cy - ay, cz - az));
}

std::vector<TexturedCoord> NewVert(std::vector<TexturedCoord>& vertices, HalfSpace& H)
{
	std::vector<TexturedCoord> temp;
	unsigned j = 0, k = 0;
	for (unsigned i = 0; i < vertices.size(); ++i)
	{
		j = i % vertices.size();
		k = (i + 1) % vertices.size();
		if ((dot(H, vertices[j]) > 0) && (dot(H, vertices[k]) <= 0))
		{
			temp.push_back(FineIntersection(vertices[j], vertices[k], H));
		}
		else if ((dot(H, vertices[j]) <= 0) && (dot(H, vertices[k]) <= 0))
		{
			temp.push_back(vertices[j]);
		}
		else if ((dot(H, vertices[j]) <= 0) && (dot(H, vertices[k]) > 0))
		{
			temp.push_back(vertices[j]);
			temp.push_back(FineIntersection(vertices[j], vertices[k], H));
		}
	}


	vertices = temp;

	return vertices;
}

void FillTriangle(Raster &r, const TexturedCoord &P, const TexturedCoord &Q, const TexturedCoord &R)
{
	TexturedCoord newP = P, newQ = Q, newR = R;
	TexturedCoord Top, Middle, Bottom;
	bool IsRight;
	int Imax, Imin, Jmax, Jmin, Xmin, Xmax;

	Vector mVector;
	Vector uVector;
	Vector vVector;
	float z, u, v;
	float a_plane, b_plane, c_plane, d_plane; //for z
	float ua, ub, uc, ud; //for u
	float va, vb, vc, vd; //for v

	if (newP.w != 1)
	{
		newP.x /= newP.w;
		newP.y /= newP.w;
		newP.z /= newP.w;
		newP.w = 1;
	}
	if (newQ.w != 1)
	{
		newQ.x /= newQ.w;
		newQ.y /= newQ.w;
		newQ.z /= newQ.w;
		newQ.w = 1;
	}
	if (newR.w != 1)
	{
		newR.x /= newR.w;
		newR.y /= newR.w;
		newR.z /= newR.w;
		newR.w = 1;
	}

	mVector = cross(newQ - newP, newR - newP);
	d_plane = newP.x * mVector.x + newP.y * mVector.y + newP.z * mVector.z;
	a_plane = mVector.x; b_plane = mVector.y; c_plane = mVector.z;

	uVector = mvector(newP.x, newP.y, newP.u, newQ.x, newQ.y, newQ.u, newR.x, newR.y, newR.u);
	ud = newP.x * uVector.x + newP.y * uVector.y + newP.u * uVector.z;
	ua = uVector.x; ub = uVector.y; uc = uVector.z;

	vVector = mvector(newP.x, newP.y, newP.v, newQ.x, newQ.y, newQ.v, newR.x, newR.y, newR.v);
	vd = newP.x * vVector.x + newP.y * vVector.y + newP.v * vVector.z;
	va = vVector.x; vb = vVector.y; vc = vVector.z;
	
	if (newP.y >= newQ.y && newQ.y >= newR.y)
	{
		Top = newP; Middle = newQ; Bottom = newR;
	}
	else if (newP.y >= newR.y && newR.y >= newQ.y)
	{
		Top = newP; Middle = newR; Bottom = newQ;
	}
	else if (newQ.y >= newP.y && newP.y >= newR.y)
	{
		Top = newQ; Middle = newP; Bottom = newR;
	}
	else if (newQ.y >= newR.y && newR.y >= newP.y)
	{
		Top = newQ; Middle = newR; Bottom = newP;
	}
	else if (newR.y >= newP.y && newP.y >= newQ.y)
	{
		Top = newR; Middle = newP; Bottom = newQ;
	}
	else if (newR.y >= newQ.y && newQ.y >= newP.y)
	{
		Top = newR; Middle = newQ; Bottom = newP;
	}

	//lower
	float mMB = (Middle.y - Bottom.y) / (Middle.x - Bottom.x);
	float mTB = (Top.y - Bottom.y) / (Top.x - Bottom.x);
	float mTM = (Top.y - Middle.y) / (Top.x - Middle.x);
	if (mMB == 0) mMB = 1.f / mMB;
	if (mTB == 0) mTB = 1.f / mTB;
	if (mTM == 0) mTM = 1.f / mTM;

	IsRight = ((1.f / mMB) < (1.f / mTB)) ? false : true;

	Jmin = (int)ceil(Bottom.y); Jmax = (int)floor(Middle.y);

	for (int j = Jmin; j <= Jmax; ++j)
	{
		if (!IsRight)
		{
			Xmin = static_cast<int>(Bottom.x + (j - Bottom.y) / mMB);
			Xmax = static_cast<int>(Bottom.x + (j - Bottom.y) / mTB);
			Imin = static_cast<int>(ceil(Xmin)); Imax = static_cast<int>(floor(Xmax));

			r.GotoPoint(Imin, j);
			for (int i = Imin; i < Imax; ++i)
			{
				r.IncrementX();
				z = (d_plane - a_plane*i - b_plane * j) / c_plane;
				u = (ud - ua*i - ub * j) / uc;
				v = (vd - va*i - vb * j) / vc;
				if (z < r.GetZ())
				{
					r.SetColor((char)TexturedCoord::texture->uvToRGB(u, v).x, (char)TexturedCoord::texture->uvToRGB(u, v).y, (char)TexturedCoord::texture->uvToRGB(u, v).z);
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
		else
		{
			Xmin = static_cast<int>(Bottom.x + (j - Bottom.y) / mTB);
			Xmax = static_cast<int>(Bottom.x + (j - Bottom.y) / mMB);
			Imin = static_cast<int>(ceil(Xmin)); Imax = static_cast<int>(floor(Xmax));

			r.GotoPoint(Imin, j);
			for (int i = Imin; i < Imax; ++i)
			{
				r.IncrementX();
				z = (d_plane - a_plane*i - b_plane * j) / c_plane;
				u = (ud - ua*i - ub * j) / uc;
				v = (vd - va*i - vb * j) / vc;

				if (z < r.GetZ())
				{
					r.SetColor((char)TexturedCoord::texture->uvToRGB(u, v).x, (char)TexturedCoord::texture->uvToRGB(u, v).y, (char)TexturedCoord::texture->uvToRGB(u, v).z);
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
	}

	//upper
	Jmin = (int)floor(Middle.y); Jmax = (int)ceil(Top.y);

	for (int j = Jmax-1; j > Jmin; --j)
	{
		if (!IsRight)
		{
			Xmin = static_cast<int>(Top.x - (Top.y - j) / mTM);
			Xmax = static_cast<int>(Top.x - (Top.y - j) / mTB);
			Imin = static_cast<int>(ceil(Xmin)); Imax = static_cast<int>(floor(Xmax));

			r.GotoPoint(Imin, j);
			for (int i = Imin; i < Imax; ++i)
			{
				r.IncrementX();
				z = (d_plane - a_plane*i - b_plane * j) / c_plane;
				u = (ud - ua*i - ub * j) / uc;
				v = (vd - va*i - vb * j) / vc;

				if (z < r.GetZ())
				{
					r.SetColor((char)TexturedCoord::texture->uvToRGB(u, v).x, (char)TexturedCoord::texture->uvToRGB(u, v).y, (char)TexturedCoord::texture->uvToRGB(u, v).z);
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
		else
		{
			Xmax = static_cast<int>(Top.x - (Top.y - j) / mTM);
			Xmin = static_cast<int>(Top.x - (Top.y - j) / mTB);
			Imin = static_cast<int>(ceil(Xmin)); Imax = static_cast<int>(floor(Xmax));

			r.GotoPoint(Imin, j);
			for (int i = Imin; i < Imax; ++i)
			{
				r.IncrementX();
				z = (d_plane - a_plane*i - b_plane * j) / c_plane;
				u = (ud - ua*i - ub * j) / uc;
				v = (vd - va*i - vb * j) / vc;

				if (z < r.GetZ())
				{
					r.SetColor((char)TexturedCoord::texture->uvToRGB(u, v).x, (char)TexturedCoord::texture->uvToRGB(u, v).y, (char)TexturedCoord::texture->uvToRGB(u, v).z);
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
	}
}

bool TextureClip::operator()(std::vector<TexturedCoord>& vertices)
{
	unsigned int i = 0;

	if (vertices.empty())
		return false;
	else
	{
		while (i < half_spaces.size())
		{
			NewVert(vertices, half_spaces[i]);
			if (vertices.empty())
				return false;
			++i;
		}
	}
	return true;
}
