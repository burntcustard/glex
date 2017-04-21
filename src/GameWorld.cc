#include "GameWorld.h"

GameWorld::GameWorld (ApplicationMode mode) : asset_manager(std::make_shared<GameAssetManager>(mode)) {
  asset_manager->AddAsset(std::make_shared<CubeAsset>(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0));
  asset_manager->AddAsset(std::make_shared<CubeAsset>(3.0, 0.0, 0.0, 1.0, 1.0, 2.0, 1.0, 0.0, 0.0));
}

void GameWorld::Draw() {
  asset_manager->Draw();
}
