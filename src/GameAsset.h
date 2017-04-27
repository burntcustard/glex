#ifndef GAMEASSET_H
#define GAMEASSET_H

#include <iostream>

#include <GL/gl.h>

class GameAsset {
 public:
 virtual glm::vec3 GetCoords() = 0;
 virtual glm::vec3 GetSize() = 0;
 virtual void Draw(GLuint) = 0;
 virtual void Move(float, float, float) = 0;

};

#endif
