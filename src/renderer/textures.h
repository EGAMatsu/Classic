// textures.h - texture loader

#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <stdlib.h>

int  loadTexture(const char* path);
void texture_bind(int id);

#endif  // TEXTURES_H