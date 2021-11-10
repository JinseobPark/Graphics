/**
\file        DrawFrame.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        11/6/2018
*/
#include "DrawFrame.h"
#include "SnubDodecMesh.h"
#include "RasterUtilities.h"
#include "Clip.h"
#include "Projection.h"
#include "CubeMesh.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

Camera cam[3];
Camera cam3;
const float PI = 4.0f*atan(1.0f);
double stime;
const Point O(0, 0, 0);
Point C_center(0.f, 0.f, 0.f);
const Vector EX(1, 0, 0),
EY(0, 1, 0),
EZ(0, 0, 1);
SnubDodecMesh snub_mesh;
CubeMesh cube_mesh;
const Vector WHITE(255, 255, 255),
PURPLE(255, 0, 255),
BLACK(0, 0, 0),
RED(255, 0, 0),
GREEN(0, 255, 0),
YELLOW(255, 255, 0),
GRAY(178, 178, 178),
BLUE(0, 0, 255),
NAME(76, 178, 153),
NAME2(76, 25, 25);

Affine cube2world,
snub2world,
rotating_object,
myAround2World[9],
myRect2World_1[9],
myRect2World_2[8],
myRect2World_3[3],
myRect2World_4[3],
myRect2World_5[3],
myRect2World_6[3],
myRect2World_7[12];

double snub_rot_rate;
Vector snub_rot_axis;
Point snub_center;
Point myAroundCenter;

Affine ndc_to_dev;
Matrix world_to_ndc[3];

Clip tclip;
double hos = 0.f;
double maxline = 1.f;
double camera_move = 0.f;

void DisplayFaces(Raster& r, Mesh & m, const Affine & A, const Vector & color, int cam_num)
{
	vector<Point> world_verts;
	vector<Hcoord> clipped_verts;
	Clip cube_clip;
	world_verts.clear();
	for (int i = 0; i < m.VertexCount(); ++i)
		world_verts.push_back(A * m.GetVertex(i));

	for (int i = 0; i < m.FaceCount(); ++i)
	{
		const Point &Pworld = world_verts[m.GetFace(i).index1],
			&Qworld = world_verts[m.GetFace(i).index2],
			&Rworld = world_verts[m.GetFace(i).index3];
		Hcoord Ptex(Pworld),
			Qtex(Qworld),
			Rtex(Rworld);
		clipped_verts.clear();
		clipped_verts.push_back(Ptex);
		clipped_verts.push_back(Qtex);
		clipped_verts.push_back(Rtex);

		if (cube_clip(clipped_verts))
		{
			for (unsigned j = 0; j < clipped_verts.size(); ++j)
			{
				const Hcoord& Pworld = clipped_verts[j];
				Hcoord Pndc = world_to_ndc[cam_num] * Pworld;
				clipped_verts[j] = Hcoord(Pndc);
			}
			if (tclip(clipped_verts))
			{
				Vector m = cross(Qworld - Pworld, Rworld - Pworld);
				float mu = min(abs(dot(m, cam[cam_num].Back())) / abs(m) + 0.1f, 1.0f);
				r.SetColor(mu*color.x, mu*color.y, mu*color.z);
				for (unsigned k = 0; k < clipped_verts.size(); ++k)
				{
					const Hcoord& Pndc = clipped_verts[k];
					Hcoord Pdev = ndc_to_dev * Pndc;
					clipped_verts[k] = Hcoord(Pdev);
				}
				for (unsigned k = 2; k < clipped_verts.size(); ++k)
					FillTriangle(r, clipped_verts[0], clipped_verts[k - 1], clipped_verts[k]);
			}
		}
	}
}

void InitFrame(int width, int height)
{
	snub_rot_rate = 2.f * PI / 10.0f;
	snub_rot_axis = EY;

	snub_center = Point(0, 0, -3);
	myAroundCenter = Point(0, 0, -3);

	snub2world = translate(snub_center - O);
	cube2world = translate(Vector(0, 3.f, -3.f))
		* scale(0.7f, 0.3f, 0.7f);

	rotating_object = translate(Vector((4 + sin(2 * PI / 25))*sin(PI / 4 * (2 + sin(2 * PI / 35)))*cos(PI*(1 + sin(2 * PI / 35))),
		(4 + sin(2 * PI / 25))*cos(PI / 4 * (2 + sin(2 * PI / 35))),
		(4 + sin(2 * PI / 25))*sin(PI / 4 * (2 + sin(2 * PI / 35)))*sin(PI*(1 + sin(2 * PI / 35))))) *
		scale(0.22f, 0.22f, 0.22f);

	for (int i = 0; i < 9; ++i)
	{
		myAround2World[i] = translate(Vector((i % 3) - 1, -2.5f, ((float)i / 3.f) - 4))
			* scale(0.3f, 0.3f, 0.3f);
		myRect2World_1[i] = translate(Vector(-2.5, ((float)i / 3.f) - 1, (i % 3) - 4))
			* scale(0.33f, 0.33f, 0.33f);
	}
	for (int i = 0; i < 8; ++i)
	{
		myRect2World_2[i] = translate(Vector(1.4f * sin(PI * i / 4.f), 2.5f, 1.4f *cos(PI * i / 4.f) - 3.f))
			* scale(0.33f, 0.83f, 0.33f);
	}
	for (int i = 0; i < 3; i++)
	{
		myRect2World_3[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f), 1.4f *cos(PI * i * 2.f / 3.f) - 3.f))
			* scale(0.33f, 0.33f, 0.33f);
		myRect2World_4[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI / 8.f), 1.4f *cos(PI * i * 2.f / 3.f + PI / 8.f) - 3.f))
			* scale(0.27f, 0.27f, 0.27f);
		myRect2World_5[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI / 4.f), 1.4f *cos(PI * i * 2.f / 3.f + PI / 4.f) - 3.f))
			* scale(0.22f, 0.22f, 0.22f);
		myRect2World_6[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI * 3.f / 8.f), 1.4f *cos(PI * i * 2.f / 3.f + PI * 3.f / 8.f) - 3.f))
			* scale(0.22f, 0.22f, 0.22f);
	}


	for (int i = 0; i < 12; ++i)
	{
		myRect2World_7[i] = translate(Vector(2.f * sin(PI * i / 6.f), 0.5f * sin(PI * i / 6.f), 2.f *cos(PI * i / 6.f) - 3.f))
			* scale(0.22f, 0.22f, 0.22f);
	}

	float aspect = float(width) / float(height);
	cam[0] = Camera(O + 3 * EX - 3 * EZ, -EX, EY, 0.5f*PI, 1, 0.01f, 10);

	ndc_to_dev = translate(Vector(-0.5f, -0.5f, 0))
		* scale(0.5f*width, 0.5f*height, 1)
		* translate(Vector(1, 1, 0));

	tclip = Clip(Polyhedron(true));
	stime = 0;
}

void DrawFrame(Raster & r, double dt, int cam_num)
{
	r.SetColor(180, 123, 13);
	ClearBuffers(r);
	camera_move += dt;

	Point C_center_p = Point(0, 0, -3);
	C_center = Point((4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*cos(PI*(1 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*cos(PI / 4 * (2 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*sin(PI*(1 + sin(2 * PI / 35 * camera_move))));
	Point Cam3 = Point((4 + sin(2 * PI / 25 * stime))*sin(PI / 4 * (2 + sin(2 * PI / 35 * stime)))*cos(PI*(1 + sin(2 * PI / 35 * stime))),
		(4 + sin(2 * PI / 25 * stime))*cos(PI / 4 * (2 + sin(2 * PI / 35 * stime))),
		(4 + sin(2 * PI / 25 * stime))*sin(PI / 4 * (2 + sin(2 * PI / 35 * stime)))*sin(PI*(1 + sin(2 * PI / 35 * stime))));

	Point p_Cam3 = Point((4 + sin(2 * PI / 25 * (stime + dt))*sin(PI / 4 * (2 + sin(2 * PI / 35 * (stime + dt))))*cos(PI*(1 + sin(2 * PI / 35 * (stime + dt)))),
		(4 + sin(2 * PI / 25 * (stime + dt))*cos(PI / 4 * (2 + sin(2 * PI / 35 * (stime + dt)))),
		(4 + sin(2 * PI / 25 * (stime + dt))*sin(PI / 4 * (2 + sin(2 * PI / 35 * (stime + dt))))*sin(PI*(1 + sin(2 * PI / 35 * (stime + dt))))))));
	//derivative of position
	const Vector look = C_center - C_center_p;
	const Vector look_center = snub_center - C_center;

	cam[1] = Camera(Cam3, normalize(p_Cam3 - Cam3), cross(cam3.Right(), p_Cam3 - Cam3), 0.5f * PI, 1, 0.01f, 10);
	cam[2] = Camera(C_center, look_center, EY, 0.5f*PI, 1, 0.01f, 10);

	for (int i = 0; i < 3; ++i)
	{
		world_to_ndc[i] = CameraToNDC(cam[i]) * WorldToCamera(cam[i]);
	}

	// Draw cube
	rotating_object = translate(Vector((4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*cos(PI*(1 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*cos(PI / 4 * (2 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*sin(PI*(1 + sin(2 * PI / 35 * camera_move)))))
		* rotate(snub_rot_rate*dt, snub_rot_axis)
		* translate(-Vector((4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*cos(PI*(1 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*cos(PI / 4 * (2 + sin(2 * PI / 35 * camera_move))),
			(4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*sin(PI*(1 + sin(2 * PI / 35 * camera_move)))))
		* rotating_object;
	DisplayFaces(r, cube_mesh, rotating_object, BLUE, cam_num);

	for (int i = 0; i < 9; ++i)
	{
		//myaround2world
		myAround2World[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt, snub_rot_axis)
			* translate(O - myAroundCenter)
			* myAround2World[i];
		DisplayFaces(r, cube_mesh, myAround2World[i], PURPLE, cam_num);

		//myrect2world_1
		myRect2World_1[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt * 2, EX)
			* translate(O - myAroundCenter)
			* myRect2World_1[i];
		DisplayFaces(r, cube_mesh, myRect2World_1[i], GREEN, cam_num);

	}

	if (hos < 4.f && (cam_num == 0))
		hos += 0.03f;

	if (maxline < 10.f)
		maxline += 0.1f;

	for (int i = 0; i < 8; i++)
	{
		//myrect2world_7
		myRect2World_2[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt, Vector(0, 1, 0))
			* translate(O - myAroundCenter)
			* myRect2World_2[i];
		//DisplayFaces(r, cube_mesh, myRect2World_2[i], Vector(25*hos, 50*hos, 25*hos), cam_num);
	}
	for (int i = 0; i < 3; i++)
	{
		//myrect2world_3
		myRect2World_3[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, EX)
			* translate(O - myAroundCenter)
			* myRect2World_3[i];
		DisplayFaces(r, cube_mesh, myRect2World_3[i], WHITE, cam_num);
		//myrect2world_4
		myRect2World_4[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, EX)
			* translate(O - myAroundCenter)
			* myRect2World_4[i];
		DisplayFaces(r, cube_mesh, myRect2World_4[i], WHITE, cam_num);
		//myrect2world_5
		myRect2World_5[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, EX)
			* translate(O - myAroundCenter)
			* myRect2World_5[i];
		DisplayFaces(r, cube_mesh, myRect2World_5[i], WHITE, cam_num);
		//myrect2world_6
		myRect2World_6[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, EX)
			* translate(O - myAroundCenter)
			* myRect2World_6[i];
		DisplayFaces(r, cube_mesh, myRect2World_6[i], WHITE, cam_num);
	}

	for (int i = 0; i < 12; i++)
	{
		//myrect2world_7
		myRect2World_7[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt * maxline, Vector(0, 1, 0))
			* translate(O - myAroundCenter)
			* myRect2World_7[i];
		DisplayFaces(r, cube_mesh, myRect2World_7[i], NAME, cam_num);
	}


	//cube2world
	cube2world = translate(snub_center - O)
		* rotate(snub_rot_rate*dt, snub_rot_axis)
		* translate(O - snub_center)
		* cube2world;
	//DisplayFaces(r, snub_mesh, cube2world, GRAY, cam_num);

	snub2world = translate(snub_center - O)
		* rotate(snub_rot_rate*dt * hos * hos, EX)
		* translate(O - snub_center)
		* snub2world;
	DisplayFaces(r, snub_mesh, snub2world, BLUE, cam_num);

	stime += dt;
}
