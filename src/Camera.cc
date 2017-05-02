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



/**
 * Modifies the camera's coordinates, so that it follows an asset (probably the
 * player), once the asset's gone out of the cameras center "free movement" area.
 * ONLY works as a top-down-style of camera movement, and may behave strangely if
 * the camera isn't oriented straight downwards.
 */
void Camera::Follow(std::shared_ptr<GameAsset> the_asset) {

  glm::vec3 assetCoords = the_asset->GetCoords();

  float xDiff = coords.x - assetCoords.x;
  float yDiff = coords.y - assetCoords.y;

  // Value describing the size of the "free movement" area at the center
  // of the screen, where the camera won't follow until it hits the edge.
  // Actually is 1/2 the width (and height because it's a square) of the area.
  // Could be a rectangle rather than square (requiring separate x/y values).
  // SHOULD be based off camera in some way, rather than just world coordinates?
  // If this is too high, the asset being followed may disappear behind other
  // assets due to the perspective / field of view.
  float fmSize = 1.2;

  // "signbit(x) - 0.5 * 2" is either -1 or 1, depending on if x is positive or
  // negative. So these basically add or subtract the amount that the asset has
  // "gone outside" of the free movement area.
  if (std::abs(xDiff) > fmSize) {
    coords.x += (std::abs(xDiff) - fmSize) * (std::signbit(xDiff) - 0.5) * 2;
  }
  if (std::abs(yDiff) > fmSize) {
    coords.y += (std::abs(yDiff) - fmSize) * (std::signbit(yDiff) - 0.5) * 2;
  }

  /*
  // Old "fixed to center of screen" camera following:
  coords.x = assetCoords.x;
  coords.y = assetCoords.y;
  */
}



glm::vec3 Camera::GetCoords() {
  return coords;
}



/**
 * Set the camera's x,y,z coordinates.
 */
void Camera::SetCoords(float x, float y, float z) {
  coords = glm::vec3(x, y, z);
}



/**
 * Set the camera's x,y coordinates (without modifying z).
 *
 * Unfortunately, this can't have the same name as the function
 * SetCoords(), because although it has different paramenters, to not
 * confuse the compiler, at least one of those parameters would have
 * to be a different type. But I want to use floats!
 *
 * We also couldn't just have one function with optional arguments,
 * because there would be no "default" value that could be set (e.g.
 * SetCoords(float x, float y, float z = 0)), because we don't actually
 * want to set it to 0 (or any other "magic" number), we just want to
 * leave it alone! Using "NULL" would be the same as writing 0, and as far
 * as I know there's no other... anything, that could be used as "default".
 *
 */
void Camera::SetXYCoords(float x, float y) {
  coords.x = x;
  coords.y = y;
}



glm::vec3 Camera::GetFacing() {
  return facing;
}



glm::mat4 Camera::GetView() {
  return glm::lookAt(coords, coords + facing, upward);
}
