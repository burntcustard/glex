#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <memory>

#include <GL/gl.h>
#include <SDL2/SDL.h> // Needed for Uint8 for holding (keyboard) keys?

#include "common.h"
#include "GameAssetManager.h"
#include "CubeAsset.h"
#include "Camera.h"

/**
 * GameWorld allows us to separate the management of the game world from the
 * nuts and bolts of game loop initialisation.  The GameWorld currently has
 * a very simplified scene graph consisiting of a single GameAssetManager.
 */
class GameWorld {
 public:
  /**
   * We thread the ApplicationMode through the GameWorld ss we want to read it
   * in from the user.  Threading the state through the various function calls
   * is preferable (in this case) to having some kind of global state.
   */
  GameWorld(ApplicationMode);

  /**
   * Calling Draw() will draw the entire world.
   */
  void Draw();

  /**
   * Calling Update() will update the entire world.
   * Moving assets that are supposed to move, etc.
   */
  void Update(const Uint8* keys, std::string &heldKeys, glm::vec2 &mouseDelta, Camera &camera);
  bool BuildingsCollisionCheck(GameAsset *asset);
  int GetNoOfBuildings();

 private:
  std::shared_ptr<GameAssetManager> asset_manager;
  int numberOfBuildings;
};
#endif // GAMEWORLD_H
