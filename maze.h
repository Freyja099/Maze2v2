#pragma once
#include "raylib.h"
#include <vector>

const int CELL_SIZE = 40;
const Color WALL_COLOR = {44, 62, 80, 255};
const Color PATH_COLOR = {236, 240, 241, 255};
const Color PLAYER_COLOR = {231, 76, 60, 255};
const Color EXIT_COLOR = {46, 204, 113, 255};
const Color HOUSE_COLOR = {155, 89, 182, 255};

class Maze {
private:
    std::vector<std::vector<bool>> walls;
    int size;
    
    void generateMaze(int x, int y);
    void ensurePath();

public:
    Maze(int mazeSize);
    bool isWall(int x, int y) const;
    void Draw() const;
    int GetSize() const;
};