#include "Camera.h"

#include <vector>

Camera::Camera(float x, float y, float z) {
  coords = glm::vec3(x, y, z);
  facing = -glm::normalize(coords); // Defaults to facing towards the center of the world.
  upward = glm::vec3(0, 1, 0); // Default y is up (the world goes up)
}

Camera::~Camera() {
  // TODO: Camera destructor;
}

void Camera::Rotate(float pitch, float yaw, float roll) {

  // Pitch (up and down)
  glm::vec3 right = glm::normalize(glm::cross(upward, facing)); // Get sideways direction
  glm::vec3 tmpFacing = glm::vec3(glm::normalize(glm::rotate(pitch, right) * glm::normalize(glm::vec4(facing, 0.0))));
  glm::vec3 tmpUpward = glm::normalize(glm::cross(tmpFacing, right));
  // Stop being able to flip:
  if (tmpUpward.y > 0) {
    facing = tmpFacing;
    upward = tmpUpward;
  }

  // Yaw (side to side)
  glm::mat4 rotation = glm::rotate(-yaw, glm::vec3(0,1,0));
  facing = glm::vec3(glm::normalize(rotation * glm::vec4(facing, 0.0)));
  upward = glm::vec3(glm::normalize(rotation * glm::vec4(upward, 0.0)));

}

void Camera::FPSMove(float x, float y, float z) {

  float speed = 0.01;

  glm::vec3 direction = glm::vec3(x, y, z);

  /*
  // Useful console output that shows intdended x/y/z direction if keys pressed
  if (direction.x != 0 || direction.y != 0 || direction.z != 0) {
    std::cout << direction.x << direction.y << direction.z << std::endl;
  }
  */

  // Move the camera forward and backward in relation to where it's facing:
  if (direction.z) {
    coords += speed * direction.z * facing;
  }
  // Move the camera left and right in relation to where it's facing:
  if (direction.x) {
    coords += speed * direction.x * glm::normalize(glm::cross(facing, upward));
  }
  // Move the camera up and down in relation to the world:
  if (direction.y) {
    coords.y += speed * direction.y;
  }

}

void Camera::TopDownMove(float x, float y, float z) {

  float speed = 0.01;

  glm::vec3 direction = glm::vec3(x, y, z);

  coords += speed * direction;

}

glm::vec3 Camera::GetCoords() {
  return coords;
}

glm::vec3 Camera::GetFacing() {
  return facing;
}

glm::mat4 Camera::GetView() {
  return glm::lookAt(coords, coords + facing, upward);
}
