/**
\file        RasterUtilities.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        23/4/2018
\brief		 Fill Trangle by dot.

*/

#include "RasterUtilities.h"
#include <iostream>

using namespace std;

void ClearBuffers(Raster & r, float z)
{
	for (int i = 0; i < r.Width(); ++i)
	{
		r.IncrementX();
		for (int j = 0; j < r.Height(); ++j)
		{
			r.IncrementY();
			r.GotoPoint(i, j);
			r.WritePixel();
			r.WriteZ(z);
		}

	}

}

void FillTriangle(Raster & r, const Hcoord & P, const Hcoord & Q, const Hcoord & R)
{
	Hcoord newP = P, newQ = Q, newR = R;
	Hcoord Top, Middle, Bottom;
	bool IsRight;
	int Imax, Imin, Jmax, Jmin, Xmin, Xmax;

	Vector mVector;
	float z;
	float a_plane, b_plane, c_plane, d_plane;

	if(newP.w != 1)
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
	else
	{
		cerr << "setting top middle bottom error \n";
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
				if (z < r.GetZ())
				{
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
				if (z < r.GetZ())
				{
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
	}

	//upper
	Jmin = (int)floor(Middle.y); Jmax = (int)ceil(Top.y);

	for (int j = Jmax; j > Jmin; --j)
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
				if (z < r.GetZ())
				{
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
				if (z < r.GetZ())
				{
					r.WritePixel();
					r.WriteZ(z);
				}
			}
		}
	}
}
