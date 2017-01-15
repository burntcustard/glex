#include "Camera.h"

#include <vector>

Camera::Camera(float x, float y, float z) {
  coords = glm::vec3(x, y, z);
  facing = -glm::normalize(coords); // Defaults to facing towards the center of the world.
  upward = glm::vec3(0, 0, 1); // Default is z axis.
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

  glm::vec3 facingVector = glm::normalize(coords - facing);
  glm::vec3 cameraRight = glm::normalize(glm::cross(upward, facingVector));

  // Useful console output that shows intdended x/y/z direction if keys pressed
  if (direction.z != 0 || direction.y != 0 || direction.z != 0) {
    std::cout << direction.x << direction.y << direction.z << std::endl;
  }

  // Convert intended x/y/z movement from camera-space to world-space:
  // Camera-space x is sideways, y is up, z is forward.
  // World-space  x is sideways, z is up, y is sideways.

  if (direction.z) {
    coords += speed * direction.z * facing;
  }
  if (direction.x) {
    coords += speed * direction.x * glm::normalize(glm::cross(facing, upward));
  }
  if (direction.z) {
    coords.z += speed * direction.y;
  }
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
