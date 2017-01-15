#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <iostream>

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


class Camera {

  public:
    Camera(float, float, float);
    ~Camera();
    glm::vec3 GetCoords();
    glm::vec3 GetFacing();
    glm::mat4 GetView();
    glm::vec3 GetVelocity();
    void Rotate(float, float, float); // pitch, yaw, roll, of intended rotation.
    void Move(float, float, float); // xyz intended move direction. Using floats in case we want to move "just a bit".

  private:
    glm::vec3 coords; // The world-space xyz coordinates of the camera.
    glm::vec3 facing; // The direction the camera is pointing, i.e. looking towards.
    glm::vec3 upward; // The direction that is "upwards" i.e. at the top of the screen.
    glm::vec3 velocity; // The current world-space velocity of the camera.

};


#endif // CAMERA_H
