#include "maze.h"
#include <random>
#include <algorithm>

Maze::Maze(int mazeSize) : size(mazeSize) {
    walls = std::vector<std::vector<bool>>(size, std::vector<bool>(size, true));
    generateMaze(1, 1);
    walls[1][1] = false;
    walls[size-2][size-2] = false;
    ensurePath();
}

void Maze::generateMaze(int x, int y) {
    walls[x][y] = false;
    
    std::vector<std::pair<int, int>> directions = {{0,2}, {2,0}, {0,-2}, {-2,0}};
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(directions.begin(), directions.end(), gen);
    
    for (const auto& dir : directions) {
        int nextX = x + dir.first;
        int nextY = y + dir.second;
        
        if (nextX > 0 && nextX < size-1 && nextY > 0 && nextY < size-1 && walls[nextX][nextY]) {
            walls[x + dir.first/2][y + dir.second/2] = false;
            generateMaze(nextX, nextY);
        }
    }
}

void Maze::ensurePath() {
    for (int i = 0; i < 3; i++) {
        walls[1][i+1] = false;
        walls[size-2][size-2-i] = false;
    }
}

bool Maze::isWall(int x, int y) const {
    if (x < 0 || x >= size || y < 0 || y >= size) return true;
    return walls[x][y];
}

void Maze::Draw() const {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (walls[i][j]) {
                DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, 
                              CELL_SIZE, CELL_SIZE, WALL_COLOR);
            }
        }
    }
}

int Maze::GetSize() const {
    return size;
}