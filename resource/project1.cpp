/**
\file        project1.cpp
\author      Jinseob Park
\par         email: 0814owl@gmail.com
\date        20/4/2018

*/

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Projection.h"
#include "CubeMesh.h"
#include "SnubDodecMesh.h"
#include "Render.h"
using namespace std;

class Client
{
public:
    Client(SDL_Window* window);
    ~Client(void);
    void draw(double dt);
    void keypress(SDL_Keycode kc);
    void resize(int W, int H);
	void SetCamera(const Camera &cam);
	void DisplayFaces(Mesh& m, const Affine& A, const Vector& color);

private:
    // variables for frame rate:
    SDL_Window* window;
    int frame_count;
    double frame_time;
    // other variables:

	Camera cam1, cam2, cam3,
		cam2_0;
	SnubDodecMesh snub_mesh;
	CubeMesh cube_mesh;
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
	Vector z_axis;
	Vector x_axis;
	Point snub_center;
	Point myAroundCenter;
	int now_cam = 1;
	bool use_cam1,
		draw_solid;
	Render *render;
	Affine world2camera;
	Matrix camera2ndc;

};


const Point O(0.f, 0.f, 0.f);
Point C_center(0.f, 0.f, 0.f);
const Vector EX(1.f, 0.f, 0.f),
EY(0.f, 1.f, 0.f),
EZ(0.f, 0.f, 1.f);
const float PI = 4.0f*atan(1.0f);
const Vector WHITE(1.f, 1.f, 1.f),
PURPLE(1.f, 0.f, 1.f),
BLACK(0.f, 0.f, 0.f),
RED(1.f, 0.f, 0.f),
GREEN(0.f, 1.f, 0.f),
YELLOW(1.f, 1.f, 0.f),
NAME(0.3f, 0.7f, 0.6f),
NAME2(0.3f, 0.1f, 0.1f),
GRAY(0.7f, 0.7f, 0.7f),
BLUE(0.f, 0.f, 1.f);

double hos = 0.f;
double maxline = 1.f;
double camera_move = 0.f;

void Client::SetCamera(const Camera & cam)
{
	world2camera = WorldToCamera(cam);
	camera2ndc = CameraToNDC(cam);
}

void Client::DisplayFaces(Mesh & m, const Affine & A, const Vector & color)
{
	Point P, Q, R;
	Hcoord nP, nQ, nR;
	Point Eye(0.f, 0.f, 0.f);

	for (int i = 0; i < m.FaceCount(); i++)
	{
		nP = world2camera * A * m.GetVertex(m.GetFace(i).index1);
		nQ = world2camera * A * m.GetVertex(m.GetFace(i).index2);
		nR = world2camera * A * m.GetVertex(m.GetFace(i).index3);

		P = Point(nP.x / nP.w, nP.y / nP.w, nP.z / nP.w);
		Q = Point(nQ.x / nQ.w, nQ.y / nQ.w, nQ.z / nQ.w);
		R = Point(nR.x / nR.w, nR.y / nR.w, nR.z / nR.w);

		Vector M = cross(Q - P, R - P);

		if (dot(M, Eye - P) > 0 && P.z <= 0 && Q.z <= 0 && R.z <= 0)
		{
			const Vector L = Vector(0.f, 0.f, 1.f);
			normalize(L);
			float mu = abs(dot(M, L)) / (abs(M)*abs(L));
			render->SetColor(mu * color);
			render->FillTriangle(camera2ndc * P, camera2ndc * Q, camera2ndc * R);
		}
	}
}

Client::Client(SDL_Window* win) : window(win)
{
    frame_count = 0;
    frame_time = 0;
    // initialization code:
	render = new Render();

	snub_rot_rate = 2.f * PI / 10.0f;
	snub_rot_axis = EY;
	z_axis = EZ;
	x_axis = EX;
	snub_center = Point(0, 0, -3);
	myAroundCenter = Point(0, 0, -3);
	use_cam1 = true;
	draw_solid = false;

	cam1 = Camera(O + EZ, -EZ, EY, 0.5f*PI, 1, 0.01f, 10);
	cam2_0 = Camera(O + 3 * EX - 3 * EZ, -EX, EY, 0.5f*PI, 1, 0.01f, 10);
	cam2 = cam2_0; (
		snub2world = translate(snub_center - O));


	cube2world = translate(Vector(0, 3.f, -3.f))
		* scale(0.7f, 0.3f, 0.7f);

	rotating_object = translate(Vector((4 + sin(2 * PI / 25 ))*sin(PI / 4 * (2 + sin(2 * PI / 35 )))*cos(PI*(1 + sin(2 * PI / 35 ))),
		(4 + sin(2 * PI / 25 ))*cos(PI / 4 * (2 + sin(2 * PI / 35 ))),
		(4 + sin(2 * PI / 25 ))*sin(PI / 4 * (2 + sin(2 * PI / 35 )))*sin(PI*(1 + sin(2 * PI / 35 ))))) * 
		scale(0.22f, 0.22f, 0.22f);

	for (int i = 0; i < 9; ++i)
	{
		myAround2World[i] = translate(Vector((i % 3) - 1, -2.5f, ((float)i / 3.f) - 4 ))
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
		myRect2World_3[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f), 1.4f *cos(PI * i * 2.f / 3.f) -3.f))
			* scale(0.33f, 0.33f, 0.33f);
		myRect2World_4[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI / 8.f), 1.4f *cos(PI * i * 2.f / 3.f + PI / 8.f) -3.f))
			* scale(0.27f, 0.27f, 0.27f);
		myRect2World_5[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI / 4.f), 1.4f *cos(PI * i * 2.f / 3.f + PI / 4.f) -3.f))
			* scale(0.22f, 0.22f, 0.22f);
		myRect2World_6[i] = translate(Vector(0.f, 1.4f * sin(PI * i * 2.f / 3.f + PI * 3.f / 8.f), 1.4f *cos(PI * i * 2.f / 3.f + PI * 3.f / 8.f) - 3.f))
			* scale(0.22f, 0.22f, 0.22f);
	}


	for (int i = 0; i < 12; ++i)
	{
		myRect2World_7[i] = translate(Vector(2.f * sin(PI * i / 6.f), 0.5f * sin(PI * i / 6.f), 2.f *cos(PI * i / 6.f) - 3.f))
			* scale(0.22f, 0.22f, 0.22f);
	}
	glEnable(GL_DEPTH_TEST);
}


Client::~Client(void)
{
    // clean-up code:
	delete render;
}


void Client::draw(double dt)
{
    //   dt: time (in seconds) since last animation frame
    // frame rate:
    ++frame_count;
    frame_time += dt; 
	camera_move += dt;
    if (frame_time >= 0.5)
    {
        double fps = frame_count / frame_time;
        frame_count = 0;
        frame_time = 0;
        stringstream ss;
        ss << "CS 250: Project #1 [fps=" << int(fps) << "]";
        SDL_SetWindowTitle(window, ss.str().c_str());
    }
	//moving camera
	Point C_center_p = Point(0, 0, -3);
	C_center = Point((4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*cos(PI*(1 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*cos(PI / 4 * (2 + sin(2 * PI / 35 * camera_move))),
		(4 + sin(2 * PI / 25 * camera_move))*sin(PI / 4 * (2 + sin(2 * PI / 35 * camera_move)))*sin(PI*(1 + sin(2 * PI / 35 * camera_move))));

	//derivative of position
	const Vector look = C_center - C_center_p;
	const Vector look_center = snub_center - C_center;
	cam1 = Camera();
	cam3 = Camera(C_center, look_center, EY, 0.5f*PI, 1, 0.01f, 10);


	const Vector right = normalize(-look_center);
	const Vector look_ = normalize(look);
	const Vector up = normalize(cross(look, right));

    // drawing code:

	render->ClearBuffers(YELLOW);

	Camera& cam = cam1;

	if (now_cam == 1) {
		cam = cam1;
	}
	else if (now_cam == 2) {
		cam = cam2;
	}
	else {
		cam = cam3;
	}


	SetCamera(cam);


	// Draw cube
	rotating_object = translate(myAroundCenter - O)
		* rotate(snub_rot_rate*dt, snub_rot_axis)
		* translate(O - myAroundCenter)
		* rotating_object;
	DisplayFaces(cube_mesh, rotating_object, BLUE);

	for (int i = 0; i < 9; ++i)
	{
		//myaround2world
		myAround2World[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt, snub_rot_axis)
			* translate(O - myAroundCenter)
			* myAround2World[i];
		DisplayFaces(cube_mesh, myAround2World[i], PURPLE);

		//myrect2world_1
		myRect2World_1[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt * 2, x_axis)
			* translate(O - myAroundCenter)
			* myRect2World_1[i];
		DisplayFaces(cube_mesh, myRect2World_1[i], GREEN);

	}
	
	if(hos < 4.f && (now_cam == 2))
	hos += 0.03f;

	if (maxline < 10.f)
		maxline += 0.1f;
	
	for (int i = 0; i < 8; i++)
	{
		//myrect2world_7
		myRect2World_2[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt , Vector(0, 1, 0))
			* translate(O - myAroundCenter)
			* myRect2World_2[i];
		DisplayFaces(cube_mesh, myRect2World_2[i], Vector(0.1*hos, 0.2*hos, 0.1*hos));
	}
	for (int i = 0; i < 3; i++)
	{
		//myrect2world_3
		myRect2World_3[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, x_axis)
			* translate(O - myAroundCenter)
			* myRect2World_3[i];
		DisplayFaces(cube_mesh, myRect2World_3[i], WHITE);
		//myrect2world_4
		myRect2World_4[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, x_axis)
			* translate(O - myAroundCenter)
			* myRect2World_4[i];
		DisplayFaces(cube_mesh, myRect2World_4[i], WHITE);
		//myrect2world_5
		myRect2World_5[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, x_axis)
			* translate(O - myAroundCenter)
			* myRect2World_5[i];
		DisplayFaces(cube_mesh, myRect2World_5[i], WHITE);
		//myrect2world_6
		myRect2World_6[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt* hos * hos, x_axis)
			* translate(O - myAroundCenter)
			* myRect2World_6[i];
		DisplayFaces(cube_mesh, myRect2World_6[i], WHITE);
	}

	for (int i = 0; i < 12; i++)
	{
		//myrect2world_7
		myRect2World_7[i] = translate(myAroundCenter - O)
			* rotate(snub_rot_rate*dt * maxline, Vector(0, 1, 0))
			* translate(O - myAroundCenter)
			* myRect2World_7[i];
		DisplayFaces(cube_mesh, myRect2World_7[i], NAME);
	}


	//	//cube2world
	cube2world = translate(snub_center - O)
		* rotate(snub_rot_rate*dt, snub_rot_axis)
		* translate(O - snub_center)
		* cube2world;
	DisplayFaces(snub_mesh, cube2world, GRAY);

	snub2world = translate(snub_center - O)
		* rotate(snub_rot_rate*dt * hos * hos, x_axis)
		* translate(O - snub_center)
		* snub2world;
	DisplayFaces(snub_mesh, snub2world, BLUE);

}


void Client::keypress(SDL_Keycode kc)
{
    // respond to keyboard input
    //   kc: SDL keycode (e.g., SDLK_SPACE, SDLK_a, SDLK_s)

	const float angle_increment = PI / 180.0f,
		dist_increment = 0.1f,
		zoom_increment = 0.95f;
	switch (kc) {
	case SDLK_SPACE:
		if (now_cam == 1)
			now_cam = 2;
		else if (now_cam == 2)
			now_cam = 3;
		else
			now_cam = 1;
		
		break;
	}
}


void Client::resize(int W, int H)
{
    // respond to window resize
    //   W,H: window width and height (in pixels)
    glViewport(0, 0, W, H);
	float aspect = float(W) / float(H);
	cam1 = Camera(cam1.Eye(), -cam1.Back(), EY, 0.5f*PI, aspect, 0.01f, 10);
	cam2_0 = Camera(cam2.Eye(), -cam2.Back(), EY, 0.5f*PI, aspect, 0.01f, 10);
	cam3 = Camera(Vector(3.f * sin(PI / 6.f), 0.5f * sin(PI / 6.f), 3.f *cos(PI / 6.f) - 3.f), Vector(0, 0, -3) - Vector(3.f * sin(PI / 6.f), 0.5f * sin(PI / 6.f), 3.f *cos(PI / 6.f) - 3.f)
		, EX, 0.5f*PI, aspect, 0.01f, 10);
	cam2 = cam2_0;
}


/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    srand(unsigned(time(0)));

    // SDL: initialize and create a window
    SDL_Init(SDL_INIT_VIDEO);
    const char* title = "CS250: Project #1";
    int width = 600, height = 600;
    SDL_Window* window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // GLEW: get function bindings (if possible)
    glewInit();
    if (!GLEW_VERSION_2_0)
    {
        cout << "needs OpenGL version 3.0 or better" << endl;
        return -1;
    }

    // animation loop
    try
    {
        bool done = false;
        Client* client = new Client(window);
        Uint32 ticks_last = SDL_GetTicks();
        while (!done)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        done = true;
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                            done = true;
                        else
                            client->keypress(event.key.keysym.sym);
                        break;
                    case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                            client->resize(event.window.data1, event.window.data2);
                        break;
                }
            }
            Uint32 ticks = SDL_GetTicks();
            double dt = 0.001 * (ticks - ticks_last);
            ticks_last = ticks;
            client->draw(dt);
            SDL_GL_SwapWindow(window);
        }
        delete client;
    }

    catch (exception& e)
    {
        cout << e.what() << endl;
    }

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
