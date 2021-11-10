// Render.h
// -- simple rendering using OpenGL

#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <GL/gl.h>
#include "Affine.h"


class Render {
  public:
    Render(void);
    ~Render(void);
    void ClearBuffers(const Vector& c);
    void SetColor(const Vector& c);
    void DrawLine(const Hcoord& P, const Hcoord& Q);
    void FillTriangle(const Hcoord& P, const Hcoord& Q, const Hcoord& R);
  private:
    GLint program;
    GLint ucolor, aposition;
};


#endif

