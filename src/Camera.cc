#include "Camera.h"

#include <vector>

Camera::Camera(float x, float y, float z) {
  coords = glm::vec3(x, y, z);
  facing = glm::vec3(0, 0, 0); // Defaults to facing towards the center of the world.
  upward = glm::vec3(0, 0, 1); // Defaults to y being up. MIght be upside down.
}

Camera::~Camera() {
  // TODO: Camera destructor;
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
