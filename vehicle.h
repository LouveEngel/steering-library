#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <limits>
#include <cmath>

using namespace sf;
using namespace std;

int findClosestPathPointIndex(const Vector2f& pos, const vector<Vector2f>& path_points);

vector<int> findShortestPath(int start, int goal, const map<int, vector<int>>& graph);
