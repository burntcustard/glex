#include "GameWorld.h"

GameWorld::GameWorld (ApplicationMode mode) : asset_manager(std::make_shared<GameAssetManager>(mode)) {
  //asset_manager->AddAsset(std::make_shared<CubeAsset>(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0));
  //asset_manager->AddAsset(std::make_shared<CubeAsset>(3.0, 0.0, 0.0, 1.0, 1.0, 2.0, 1.0, 0.0, 0.0));

  // Create some "buildings"

  srand (time(NULL));

  for (float i = 0; i < 5; i++) {

    // Height of the building, between 1 and 2 in increments of 0.25:
    float height = 1 + (rand() % 5) / 4.0;
    //std::cout << "random height = " << height << std::endl;

    // Color of the building (used for r,g,b components),
    // between 0.5 and 0.6 in increments of 0.025:
    float color = 0.5 + (rand() % 5) / 40.0;
    //std::cout << "random color = " << color << std::endl;

    asset_manager->AddAsset(std::make_shared<CubeAsset>(
       i, 0.0, 0.0, // Location
       1.0, 1.0, height,
       color, color, color
    ));

  }

}

void GameWorld::Draw() {
  asset_manager->Draw();
}
