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

  // Create the map
  CreateMap(7);

  numberOfBuildings = 0;

  for (float c = 0; c < tileMap.size(); c++) {
    for (float r = 0; r < tileMap[0].size(); r++) {

      if (tileMap[c][r] == '*') {
        // Create the player cube:
        float x = c - (tileMap.size() / 2);
        float y = r - (tileMap[0].size() / 2);
        asset_manager->AddAsset(std::make_shared<CubeAsset>(
          x, y, 0.0, // Location
          0.2, 0.2, 0.2, // 0.2 size cube
          1.0, 0.0, 0.0  // red
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
           c - (tileMap.size() / 2), r - (tileMap[0].size() / 2), 0.0, // Location
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



std::pair<int, int> GameWorld::PickEdgeTile() {

  int c = 0, r = 0;

  // List of edge tiles that haven't already been picked/used:
  std::vector<std::pair<Uint8, Uint8>> edgeList;
  for (c = 0; c < tileMap.size(); c++) {
    for (r = 0; r < tileMap[0].size(); r++) {
      if ((c == 0 || c == tileMap.size()-1 ||
           r == 0 || r == tileMap[0].size()-1) &&
          (tileMap[c][r] == 'O')) {
        edgeList.push_back(std::make_pair(c, r));
      }
    }
  }

  // If some available edges were found and put in the list:
  if (edgeList.size()) {
    // Pick on of 'em at random(ish):
    return edgeList[rand() % edgeList.size()];
  } else {
    // Otherwise return an "error pair":
    return std::make_pair(-1, -1);
  }
}



/**
 * Takes a pair of coordinates and returns a randomly-ish selected
 * pair of coordinates next to it (above, below or to either side, NOT
 * diagonally), that aren't off the map or one of the edge walls.
 */
std::pair<int, int> GameWorld::AddPathNextTo(std::pair<int, int> cell) {

  // Value to return (if if no suitable cells are found this stay as-is):
  std::pair<int, int> nextPathRef = std::make_pair(-1, -1);

  int pathStraightness = 9;

  // Which direction should we check first, then second, then third, etc.
  std::vector<char> check = {'N', 'E', 'S', 'W'};

  // Add multiples of the direction we're already going to the check list:
  if (currentDirection != ' ') {
    for (int pathStraightness = 9; pathStraightness > 0; pathStraightness--) {
      check.push_back(currentDirection);
    }
  }

  // Shuffle the list. Because there are many of the direction we're currently going,
  // it'll be relitively unlikely that we'll change direction (unless really needed).
  std::random_shuffle(check.begin(), check.end());

  // For every element in the direction check list, and while we haven't
  // already figured out the nextPathRef (i.e. it's still at default value):
  for (int i = 0; i < check.size() && nextPathRef.first < 0; i++) {
    int cellToCheckX, cellToCheckY;
    switch(check[i]) {
      case 'N': cellToCheckX = cell.first    ; cellToCheckY = cell.second - 1; break;
      case 'E': cellToCheckX = cell.first + 1; cellToCheckY = cell.second    ; break;
      case 'S': cellToCheckX = cell.first    ; cellToCheckY = cell.second + 1; break;
      case 'W': cellToCheckX = cell.first - 1; cellToCheckY = cell.second    ; break;
      default : std::cout << "Tried to check direction that's not NESW" << std::endl;
    }
    if (cellToCheckX > 0 &&
        cellToCheckX < tileMap.size()-1 &&
        cellToCheckY > 0 &&
        cellToCheckY < tileMap[0].size()-1) {
      //std::cout << "beep";
      currentDirection = check[i];
      nextPathRef = std::make_pair(cellToCheckX, cellToCheckY);
    } else {
      // Basically return false:
      return (std::make_pair(-1, -1));
    }
  }

  return nextPathRef;

}


/**
 * Creates a tilemap for the game world.
 *
 * Two dimensional array, with the following chars:
 * ' ' = empty space
 * 'O' = wall section (where a building asset will be placed)
 * '0' = wall section around the edge of the map that's been "used"...
 * '*' = where the player will be placed
 * TODO: Explain "used".
 *
 * TODO: This should really be in it's own .cpp file.
 *
 */
void GameWorld::CreateMap(Uint8 width, Uint8 height) {

  // Loop counters for columns, rows, and anything else:
  int c = 0, r = 0, i = 0;

  // If no height specified, make the map square:
  if (height == 0) { height = width; }

  // Set the whole map to 'O' (i.e. buildings in each tile):
  for (c = 0; c < width; c++) {
    std::vector<char> row; // Create an empty row
    for (r = 0; r < height; r++) {
      row.push_back('O'); // Add an element (column) to the row
    }
    tileMap.push_back(row); // Add the row to the main vector
  }

  // Replace some building map tiles (excluding outer walls)
  // with empty "path" tiles (i.e. the space character ' '):

  // Variable to store coords of a random wall segment:
  std::pair<int, int> randEdge = PickEdgeTile();

  while ((randEdge.first >= 0) && (i < 999)) {
    i++;

    currentDirection = ' ';

    // Set the edge tile to the "picked/used" char:
    tileMap[randEdge.first][randEdge.second] = '0';
    randEdge = PickEdgeTile();

    std::pair<int, int> nextCell = AddPathNextTo(randEdge);

    for (int pathLength = width + height; pathLength > 0; pathLength--) {
      //std::cout << nextCell.first << std::endl;
      if (nextCell.first >= 0) {
        tileMap[nextCell.first][nextCell.second] = ' ';
      }
      nextCell = AddPathNextTo(nextCell);
    }

  }

  // Finished placing path cells.

  // Figure out where to put the player:
  std::pair<int, int> playerCoords = FindEmptyCell();
  tileMap[playerCoords.first][playerCoords.second] = '*';

  DrawMapInConsole();

}


/**
 * Finds an empty cell as close to the center of the tileMap
 * as possible, and returns it's coordinates as a pair of ints.
 * Returns (-1, -1) if no empty map tile cells are found.
 * Has a fixed maximum number of times to check surrounding cells
 * to ensure no infinite loops if given an unusual tileMap size.
 */
std::pair<int, int> GameWorld::FindEmptyCell() {

  int width = tileMap.size();
  int height = tileMap[0].size();
  // Integer division truncates results as intended. E.g. 5 / 2 = 2:
  int x = width / 2;
  int y = height / 2;

  // Is the center cell already empty?
  if (tileMap[x][y] == ' ') {

    return std::make_pair(x, y);

  // Center cell not empty:
  } else {

    // Which is bigger, the width or height of the map?
    int max = (width > height ?  width : height);

    for (int i = 1; i < max && i < 999; i++) {
      // Check left and right of the center:
      if (x - i >= 0 && x + i < width) {
        if (tileMap[x - i][y    ] == ' ') return std::make_pair(x - i, y    );
        if (tileMap[x + i][y    ] == ' ') return std::make_pair(x + i, y    );
      }
      // Check above and below the center:
      if (y - i >= 0 && y + i < height) {
        if (tileMap[x    ][y - i] == ' ') return std::make_pair(x    , y - i);
        if (tileMap[x    ][y + i] == ' ') return std::make_pair(x    , y + i);
      }
      // Check the diagonals:
      if ((x - i >= 0 && x + i < width) &&
          (y - i >= 0 && y + i < height)) {
        if (tileMap[x - i][y - i] == ' ') return std::make_pair(x - i, y - i);
        if (tileMap[x - i][y + i] == ' ') return std::make_pair(x - i, y + i);
        if (tileMap[x + i][y - i] == ' ') return std::make_pair(x + i, y - i);
        if (tileMap[x + i][y + i] == ' ') return std::make_pair(x + i, y + i);
      }
    }

  }

  // Er... no empty cells were found if we haven't already returned!
  std::cout << "Error: No empty cell found to place something" << std::endl;
  return std::make_pair(-1, -1);

}



void GameWorld::DrawMapInConsole() {
  for (int c = 0; c < tileMap.size(); c++) {
    for (int r = 0; r < tileMap[0].size(); r++) {
      std::cout << tileMap[c][r];
    }
    std::cout << std::endl;
  }
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
