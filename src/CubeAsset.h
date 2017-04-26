#ifndef CUBEASSET_H
#define CUBEASSET_H

// Fix for Eclipse not recognising openGL functions:
#define GL_GLEXT_PROTOTYPES

#include <vector>

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GameAsset.h"
#include "Camera.h"

class CubeAsset : public GameAsset {

  public:
    CubeAsset(float, float, float, float, float, float, float, float, float);
    ~CubeAsset();
    glm::vec3 GetCoords();
    glm::vec3 GetSize();
    glm::vec4 color;
  virtual void Draw(GLuint);
  virtual void Move(float, float, float);

  private:
    glm::vec3 coords;
    glm::vec3 size;
    GLuint element_buffer_length;
    GLuint vertex_buffer_token, element_buffer_token;

};


#endif // CUBEASSET_H
