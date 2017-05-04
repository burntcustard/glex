#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <memory>
#include <vector>
#include <algorithm>  // std::random_shuffle

#include <GL/gl.h>
#include <SDL2/SDL.h> // Needed for Uint8 type

#include "common.h"
#include "GameAssetManager.h"
#include "CubeAsset.h"
#include "Camera.h"
#include "createMap.h"

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
  GameWorld(ApplicationMode, Camera &camera);

  // These are now in createMap.cc
  //std::pair<int, int> PickEdgeTile();
  //std::pair<int, int> AddPathNextTo(std::pair<int, int> cell);
  //std::pair<int, int> FindEmptyCell();
  //void CreateMap(Uint8 width, Uint8 height = 0);
  //void DrawMapInConsole();

  /**
   * Calling Draw() will draw the entire world.
   */
  void Draw();

  /**
   * Calling Update() will update the entire world.
   * Moving assets that are supposed to move, etc.
   */
  void Update(const Uint8* keys, std::string &heldKeys, glm::vec2 &mouseDelta, Camera &camera);

  bool CollisionCheck(GameAsset *assetA, GameAsset *assetB);
  bool BuildingsCollisionCheck(GameAsset *asset);
  int GetNoOfBuildings();

 private:

  std::shared_ptr<GameAssetManager> asset_manager;
  int numberOfBuildings;

  // 2D array of the "tiles" in the game world - currently only used for buildings.
  // tileMap.size() and tileMap[0].size() can be used to get width and height of tileMap.
  std::vector<std::vector<char>> tileMap;

  // Variable to hold which way the map generator is "looking". Should be moved
  // inside one of the map functions, but it's way easier having it here for now.
  //char currentDirection;

};
#endif // GAMEWORLD_H
