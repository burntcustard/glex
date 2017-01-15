#include "Camera.h"

#include <vector>

Camera::Camera(float x, float y, float z) {
  coords = glm::vec3(x, y, z);
  facing = glm::vec3(0, 0, 0); // Defaults to facing towards the center of the world.
  upward = glm::vec3(0, 0, 1); // Defaults to y being up. Might be upside down.
}

Camera::~Camera() {
  // TODO: Camera destructor;
}

void Camera::Move(float x, float y, float z) {

  float speed = 0.01;

  // Get the direction that we intend to move in.
  //  - normalized so that you don't move too fast diagonally on x & y axis.
  //glm::vec3 direction = (glm::vec3(glm::normalize(glm::vec2(x, y)), z));

  glm::vec3 direction = glm::vec3(x, y, z);

  // Useful console output that shows intdended x/y/z direction if keys pressed
  if (direction[0] != 0 || direction[1] != 0 || direction[2] != 0) {
    std::cout << direction[0] << direction[1] << direction[2] << std::endl;
  }

  coords[2] += direction[2] * speed;

}

glm::vec3 Camera::GetCoords() {
  return coords;
}

glm::vec3 Camera::GetFacing() {
  return facing;
}

glm::mat4 Camera::GetView() {
  return glm::lookAt(coords, facing, upward);
}
