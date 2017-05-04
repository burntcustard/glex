#include "GameWorld.h"



GameWorld::GameWorld (ApplicationMode mode, Camera &camera) : asset_manager(std::make_shared<GameAssetManager>(mode)) {

  srand (time(NULL));

  /**
   * Explanation of pseudo random number generation used in this function:
   *
   *        Minimum      Size of increments
   *           |                 |
   * float x = 4 + rand() % 3 * 0.5
   *                        |
   *               Number of increments
   *
   * So in this example, the random number would be:
   * " rand() % 3 " = 0, 1, or 2
   * " * 0.5      " = 0, 0.5, or 1
   * " 4 +        " = 4, 4.5, or 5 <- Final potential values.
   *
   */

  // Create the tileMap
  tileMap = CreateMap(7);

  // Draw the map ascii-art style in console so we
  // can see what's it's supposed to look like:
  DrawMapInConsole(tileMap);

  numberOfBuildings = 0;

  for (float c = 0; c < tileMap.size(); c++) {
    for (float r = 0; r < tileMap[0].size(); r++) {

      float x = c - (tileMap.size() / 2);
      float y = - r - (tileMap[0].size() / 2); // TODO: Investigate negative y-axis bug.

      if (tileMap[c][r] == '*') {
        // Create the player cube:
        asset_manager->AddAsset(std::make_shared<CubeAsset>(
          x, y, 0.0, // Location
          0.2, 0.2, 0.2, // 0.2 size cube
          1.0, 0.0, 0.0  // Red
        ), 0); // Add to the beginning (index 0) of the draw_list

        camera.SetXYCoords(x, y);

      }

      // Could do "else if" or switch statement here, but simple ifs for
      // the player and the buildings in the tileMap is cleaner... but
      // probably a little slower.

      if (tileMap[c][r] == 'O' ||
          tileMap[c][r] == '0') {

        numberOfBuildings++;

        // Height of the building, between 0.6 and 1.2 in increments of 0.2:
        float height = 0.6 + rand() % 4 * 0.2;
        //std::cout << "random height = " << height << std::endl;

        // Color of the building (used for r,g,b components),
        // between 0.5 and 0.6 in increments of 0.025:
        float color = 0.5 + rand() % 5 * 0.025;
        //std::cout << "random color = " << color << std::endl;

        asset_manager->AddAsset(std::make_shared<CubeAsset>(
           x, y, 0.0,
           1.0, 1.0, height,
           color, color, color
        ));

      }
    }
  }

  /*
   * Old way of moving the player out of the way of the buildings (before player
   * location was decided when the map was created). This DIDN'T WORK, because
   * player->Move caused the program to crash for some reason :(
   */
  /*
  // If one of the buildings has been put on top of the
  // player, try to move the player out of the way.
  // Note: This could go VERY badly if it never manages to
  // find somewhere open to move the player to (infinite loop).
  // Currently swapped from while to if, to try to figure out why it crashes :(
  GameAsset *player = asset_manager->GetAssetRef(0).get();
  while (BuildingsCollisionCheck(player)) {
    player->Move(
      rand() & (tileMap.size() / 2),
      rand() & (tileMap[0].size() / 2),
      0
    );
  }
  */

  //std::cout << "Number of buildings at start: " << numberOfBuildings << std::endl;

  // TODO: Add zombies here?

}







void GameWorld::Update(const Uint8* keys, std::string &heldKeys, glm::vec2 &mouseDelta, Camera &camera) {

  // Process the player's input:

  // Is a key held down? If it is, add it to the "held keys" list.

  glm::vec3 playerInput = glm::vec3(0,0,0);

  if (keys[SDL_SCANCODE_W]) {
    heldKeys += "W";
    playerInput.y++;
  }
  if (keys[SDL_SCANCODE_A]) {
    heldKeys += "A";
    playerInput.x--;
  }
  if (keys[SDL_SCANCODE_S]) {
    heldKeys += "S";
    playerInput.y--;
  }
  if (keys[SDL_SCANCODE_D]) {
    heldKeys += "D";
    playerInput.x++;
  }
  if (keys[SDL_SCANCODE_UP]) {
    heldKeys += "[UP]";
    playerInput.y++;
  }
  if (keys[SDL_SCANCODE_LEFT]) {
    heldKeys += "[LEFT]";
    playerInput.x--;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    heldKeys += "[DOWN]";
    playerInput.y--;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    heldKeys += "[RIGHT]";
    playerInput.x++;
  }
  if (keys[SDL_SCANCODE_SPACE]) {
    heldKeys += "[SPACE]";
    playerInput.z++;
  }
  if (keys[SDL_SCANCODE_LSHIFT]) {
    heldKeys += "[SHIFT]";
    playerInput.z--;
  }

  // Make sure move speed isn't doubled if for
  // example, right arrow key and 'd' are held:
  if (playerInput.x >  1) { playerInput.x =  1; }
  if (playerInput.y >  1) { playerInput.y =  1; }
  if (playerInput.z >  1) { playerInput.z =  1; }
  if (playerInput.x < -1) { playerInput.x = -1; }
  if (playerInput.y < -1) { playerInput.y = -1; }
  if (playerInput.z < -1) { playerInput.z = -1; }

  // Move the camera higher or lower:
  camera.TopDownMove(0, 0, playerInput.z);

  // Update the game - moves player in x and y axis (but not z),
  // and makes the camera follow the player cube:
  asset_manager->Update(playerInput.x, playerInput.y, 0);

  // Make the camera follow the player:
  camera.Follow(asset_manager->GetAssetRef(0));

  //std::cout << numberOfBuildings << std::endl;

}



void GameWorld::Draw() {
  asset_manager->Draw();
}



int GameWorld::GetNoOfBuildings() {
  return numberOfBuildings;
}



/**
 * Simple axis-aligned bounding box collision detection between two assets.
 *
 * Currently only checks x,y values, but could be modified to check on the z,
 * or even any number of axis (with quite a bit of work as right now it's all vec3s).
 *
 * Sizes are halfed as they are the width/height of the shapes, when we actually want
 * the distance from the center to the side of the asset (assuming it's rectangular...)
 *
 */
bool GameWorld::CollisionCheck(GameAsset *assetA, GameAsset *assetB) {

  glm::vec3 aCoords = assetA->GetCoords();
  glm::vec3 aSize = assetA->GetSize() / 2;
  glm::vec3 bCoords = assetB->GetCoords();
  glm::vec3 bSize = assetB->GetSize() / 2;

  if ((std::abs(aCoords.x - bCoords.x) < aSize.x + bSize.x) &&
      (std::abs(aCoords.y - bCoords.y) < aSize.y + bSize.y)) {
    return true;
  } else {
    return false;
  }

}



/**
 * Checks if an asset is colliding with any of the "building" assets in the game.
 */
bool GameWorld::BuildingsCollisionCheck(GameAsset *asset) {

  for(int i = 1; i <= numberOfBuildings; i++) {

    // Get a pointer to the building asset. Uses .get() because
    // GetAssetRef actually returns a shared_ptr not a regular pointer.
    // TODO: Figure out if it's better to keep this as is, or use shared_ptrs
    //       everywhere, or change GetAssetRef() to not return shared_ptr?
    GameAsset *building = asset_manager->GetAssetRef(i).get();

    if (GameWorld::CollisionCheck(asset, building)) {
      //std::cout << "Collision with a building detect!" << std::endl;
      return true;
    } else {
      // Don't return false yet, gotta check for collisions with other buildings!
    }
  }

  // If we haven't already returned true, the asset isn't colliding with any buildings, so:
  return false;

}
