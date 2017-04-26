#include "GameWorld.h"

GameWorld::GameWorld (ApplicationMode mode) : asset_manager(std::make_shared<GameAssetManager>(mode)) {
  //asset_manager->AddAsset(std::make_shared<CubeAsset>(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0));
  //asset_manager->AddAsset(std::make_shared<CubeAsset>(3.0, 0.0, 0.0, 1.0, 1.0, 2.0, 1.0, 0.0, 0.0));


  srand (time(NULL));

  float i = 0;

  // Create the player cube
  asset_manager->AddAsset(std::make_shared<CubeAsset>(
      0.0, 0.0, 0.0, // Center of the game world
      0.2, 0.2, 0.2, // 0.2 size cube
      1.0, 0.0, 0.0  // red
  ));

  // Create some "buildings"

  for (i = 1; i <= 5; i++) {

    // Height of the building, between 1 and 2 in increments of 0.25:
    float height = 1 + (rand() % 5) / 4.0;
    //std::cout << "random height = " << height << std::endl;

    // Color of the building (used for r,g,b components),
    // between 0.5 and 0.6 in increments of 0.025:
    float color = 0.5 + (rand() % 5) / 40.0;
    //std::cout << "random color = " << color << std::endl;

    asset_manager->AddAsset(std::make_shared<CubeAsset>(
       i, 1.0, 0.0, // Location
       1.0, 1.0, height,
       color, color, color
    ));

  }

  // TODO: Save 'i' as the "end of the buildings" or "start of zombies" index here?

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

  /*
  // Normalize x/y movement (so travelling vertically and horizontally
  // doesn't result in moving faster than intended:
  if (playerInput.x != 0 || playerInput.y != 0) {
    glm::vec3 movement = glm::normalize(glm::vec3(playerInput.x, playerInput.y, 0));
    camera.TopDownMove(movement.x, movement.y, playerInput.z);
    //GameAssetManager::Move(0, 3.0f, 2.0f, 0.0f);
  } else {
    camera.TopDownMove(0, 0, playerInput.z);
  }
  */

  // Move the camera higher or lower:
  camera.TopDownMove(0, 0, playerInput.z);

  // Update the game - moves player in x and y axis (but not z),
  // and makes the camera follow the player cube:
  asset_manager->Update(playerInput.x, playerInput.y, 0);

  // Make the camera follow the player:
  camera.Follow(asset_manager->GetAssetRef(0));

}

void GameWorld::Draw() {
  asset_manager->Draw();
}
