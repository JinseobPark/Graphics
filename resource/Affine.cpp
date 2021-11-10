/**
\file        Affine.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        9/3/2018
\brief		 math library of points, vectors, transformations in 3D
*/

#include "Affine.h"

Affine::Affine(void)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
				row[i][j] = 0;
		}
	}
	row[3][3] = 1;

}

Affine::Affine(const Vector & Lx, const Vector & Ly, const Vector & Lz, const Point & D)
{
	row[0][0] = Lx.x; row[0][1] = Ly.x; row[0][2] = Lz.x; row[0][3] = D.x;
	row[1][0] = Lx.y; row[1][1] = Ly.y; row[1][2] = Lz.y; row[1][3] = D.y;
	row[2][0] = Lx.z; row[2][1] = Ly.z; row[2][2] = Lz.z; row[2][3] = D.z;
	row[3][0] = Lx.w; row[3][1] = Ly.w; row[3][2] = Lz.w; row[3][3] = D.w;
}

Vector::Vector(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
	w = 0;
}

Point::Point(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
	w = 1;
}

Hcoord::Hcoord(float X, float Y, float Z, float W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
}


Hcoord operator+(const Hcoord& u, const Hcoord& v)
{
	return Hcoord(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}
Hcoord operator-(const Hcoord& u, const Hcoord& v)
{
	return Hcoord(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}
Hcoord operator-(const Hcoord & v)
{
	return Hcoord(-v.x, -v.y, -v.z,-v.w);
}
Hcoord operator*(float r, const Hcoord & v)
{
	return Hcoord(r*v.x, r*v.y, r*v.z, r*v.w);
}
Hcoord operator*(const Matrix & A, const Hcoord & v)
{
	Hcoord result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result[i] += A[i][j] * v[j];
		}
	}

	return result;
}
Matrix operator*(const Matrix & A, const Matrix & B)
{
	Matrix result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	return result;
}
float dot(const Vector & u, const Vector & v)
{
	return (u.x * v.x + u.y * v.y + u.z * v.z);
}
float abs(const Vector & v)
{
	float result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (result >= 0) { return result;}
	else { return -result; }
}

Vector normalize(const Vector & v)
{
	return Vector(v.x/abs(v), v.y/abs(v), v.z/abs(v));
}

Vector cross(const Vector & u, const Vector & v)
{
	return Vector((u.y*v.z) - (u.z*v.y), (u.z*v.x) - (u.x*v.z), (u.x*v.y) - (u.y*v.x));
}

Affine rotate(float t, const Vector & v)
{
	Affine result, First, Second, Third, OuterMatrix, CrossMatrix;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			OuterMatrix[i][j] = v[i] * v[j];
		}
	}

	CrossMatrix[0][0] = 0;   	CrossMatrix[0][1] = -v.z;	CrossMatrix[0][2] = v.y;
	CrossMatrix[1][0] = v.z;	CrossMatrix[1][1] = 0;	    CrossMatrix[1][2] = -v.x;
	CrossMatrix[2][0] = -v.y;	CrossMatrix[2][1] = v.x;	CrossMatrix[2][2] = 0;

	First  = scale(cos(t));
	Second = scale((1 - cos(t)) / (abs(v)*abs(v)))*OuterMatrix;
	Third  = scale(sin(t) / abs(v))*CrossMatrix;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			result[i][j] += First[i][j] + Second[i][j] + Third[i][j];
	}
	return result;
}

Affine translate(const Vector & v)
{
	return Affine(Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1), Point(v.x, v.y, v.z));
}

Affine scale(float r)
{
	return Affine(Vector(r, 0, 0), Vector(0, r, 0), Vector(0, 0, r), Point(0, 0, 0));
}

Affine scale(float rx, float ry, float rz)
{
	return Affine(Vector(rx, 0, 0), Vector(0, ry, 0), Vector(0, 0, rz), Point(0, 0, 0));
}

Affine inverse(const Affine & A)
{
	float det = 0;
	Affine Lmatrix, Tmatrix, result;
	Affine First, Second;
	Vector Transvector;

	for (int i = 0; i < 3; ++i)
	{
		Transvector[i] = -A[i][3];
	}
	
	for (int i = 0; i < 3; ++i)
	{
		det += A[0][i] * (A[1][(i + 1) % 3] * A[2][(i + 2) % 3] - A[1][(i + 2) % 3] * A[2][(i + 1) % 3]);
	}

	//Lmatrix is inverse matrix 'L'part
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Lmatrix[i][j] = ((A[(j + 1) % 3][(i + 1) % 3] * A[(j + 2) % 3][(i + 2) % 3]) - (A[(j + 1) % 3][(i + 2) % 3] * A[(j + 2) % 3][(i + 1) % 3])) / det;
		}
	}

	Tmatrix = translate(Transvector);
	result = Lmatrix * Tmatrix;

	return result;
}
