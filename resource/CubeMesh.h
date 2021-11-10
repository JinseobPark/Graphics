// CubeMesh.h
// -- standard cube

#ifndef CUBEMESH_H
#define CUBEMESH_H

#include "Mesh.h"

class CubeMesh : public Mesh {
  public:
    int VertexCount(void);
    Point GetVertex(int i);
    Vector Dimensions(void);
    Point Center(void);
    int FaceCount(void);
    Face GetFace(int i);
    int EdgeCount(void);
    Edge GetEdge(int i);
  private:
    static const Point vertices[8];
    static const Face faces[12];
    static const Edge edges[12];
};

#endif

