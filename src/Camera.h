#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include <GL/gl.h>
#include <glm/glm.hpp>


class Camera {

  public:
    Camera(float, float, float);
    ~Camera();
    glm::vec3 GetCoords();
    glm::vec3 GetFacing();
    glm::mat4 GetView();

  private:
    glm::vec3 coords; // The world-space xyz coordinates of the camera.
    glm::vec3 facing; // The direction the camera is pointing, i.e. looking towards.
    glm::vec3 upward; // The direction that is "upwards" i.e. at the top of the screen.

};


#endif // CAMERA_H
