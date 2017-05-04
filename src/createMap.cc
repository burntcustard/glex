#include "createMap.h"

typedef std::vector< std::vector<char> > mapArray;



// Boost.Python test:



BOOST_PYTHON_MODULE(createMap)
{
    using namespace boost::python;
    def("test", Test);
}


void Test(int width, int height) {
  mapArray testMap = CreateMap(width, height);
  DrawMapInConsole(testMap);
}



/**
 * Picks one of the maps edge cells, that contains a "wall that hasn't already been
 * picked" char ('O'), and returns it's coordinates (2D array index) as a pair of ints.
 */
std::pair<int, int> PickEdgeTile(mapArray tileMap) {

  int c = 0, r = 0;

  // List of edge tiles that haven't already been picked/used:
  std::vector< std::pair<int, int> > edgeList;
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
std::pair<int, int> AddPathNextTo(std::pair<int, int> cell, mapArray tileMap, char currentDirection) {

  // Value to return (if no suitable cells are found this stay as-is):
  std::pair<int, int> nextPathRef = std::make_pair(-1, -1);

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
 * Finds an empty cell as close to the center of the tileMap
 * as possible, and returns it's coordinates as a pair of ints.
 * Returns (-1, -1) if no empty map tile cells are found.
 * Has a fixed maximum number of times to check surrounding cells
 * to ensure no infinite loops if given an unusual tileMap.
 */
std::pair<int, int> FindEmptyCell(mapArray tileMap) {

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
mapArray CreateMap(int width, int height) {

  char currentDirection;

  std::vector< std::vector<char> > tileMap;

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
  std::pair<int, int> randEdge = PickEdgeTile(tileMap);

  while ((randEdge.first >= 0) && (i < 999)) {
    i++;

    currentDirection = ' ';

    // Set the edge tile to the "picked/used" char:
    tileMap[randEdge.first][randEdge.second] = '0';
    randEdge = PickEdgeTile(tileMap);

    std::pair<int, int> nextCell = AddPathNextTo(randEdge, tileMap, currentDirection);

    for (int pathLength = width + height; pathLength > 0; pathLength--) {
      //std::cout << nextCell.first << std::endl;
      if (nextCell.first >= 0) {
        tileMap[nextCell.first][nextCell.second] = ' ';
      }
      nextCell = AddPathNextTo(nextCell, tileMap, currentDirection);
    }

  }

  // Finished placing path cells.

  // Figure out where to put the player:
  std::pair<int, int> playerCoords = FindEmptyCell(tileMap);
  tileMap[playerCoords.first][playerCoords.second] = '*';

  return tileMap;

}



void DrawMapInConsole(mapArray tileMap) {
  for (int r = 0; r < tileMap[0].size(); r++) {
    for (int c = 0; c < tileMap.size(); c++) {
      std::cout << tileMap[c][r];
    }
    std::cout << std::endl;
  }
}
