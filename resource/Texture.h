// Texture.h
// -- bitmaps and texture coordinates

#ifndef TEXTURE_H
#define TEXTURE_H


#include "Affine.h"


class Texture {
  public:
    explicit Texture(unsigned W=64, unsigned H=64);
    Texture(const char *bitmap);
    ~Texture(void);
    Vector uvToRGB(float u, float v);
  private:
    enum { R=0, G=1, B=2 };
    unsigned char *data;
    int width, height, stride;
    Texture& operator=(const Texture&);
    Texture(const Texture&);
};


#endif

