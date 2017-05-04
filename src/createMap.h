#ifndef CREATEMAP_H
#define CREATEMAP_H

#include <vector>
#include <algorithm>  // std::random_shuffle
#include <iostream> // Including agin because it doesn't come over from main.cc?
#include <boost/python.hpp>

typedef std::vector< std::vector<char> > mapArray;

std::pair<int, int> PickEdgeTile(mapArray);
std::pair<int, int> AddPathNextTo(std::pair<int, int> cell, mapArray, char);
std::pair<int, int> FindEmptyCell(mapArray);
mapArray CreateMap(int width, int height = 0);
void DrawMapInConsole(mapArray);
void Test(int width, int height = 0);

#endif // CREATEMAP_H
