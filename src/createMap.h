#ifndef CREATEMAP_H
#define CREATEMAP_H

#include <vector>
#include <algorithm>  // std::random_shuffle
#include <iostream> // Including agin because it doesn't come over from main.cc?
#include <boost/python.hpp>

typedef std::vector< std::vector<char> > mapArray;

std::pair<int, int> PickEdgeTile(mapArray);
std::pair<int, int> AddPathNextTo(std::pair<int, int> cell, mapArray, char &currentDirection);
std::pair<int, int> FindEmptyCell(mapArray);
mapArray CreateMap(int width = 0, int height = 0, int pathLength = 0, int pathStraightness = 0);
void DrawMapInConsole(mapArray);
void Test(int width = 0, int height = 0, int pathLength = 0, int pathStraightness = 0);

#endif // CREATEMAP_H
