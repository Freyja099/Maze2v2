#pragma once
#include "raylib.h"
#include "maze.h"

class Player {
private:
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;
    float friction;

    void applyFriction();
    void checkCollision(const Maze& maze);
    Vector2 getCollisionNormal(const Rectangle& wall);

public:
    Player(float x, float y);
    void Update(const Maze& maze);
    void Draw() const;
    Vector2 GetPosition() const;
};