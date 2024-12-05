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
    bool isMoving;

    void applyFriction();
    void checkCollision(const Maze& maze);
    Vector2 getCollisionNormal(const Rectangle& wall);
    bool canMoveToPosition(const Vector2& newPos, const Maze& maze);

public:
    Player(float x, float y);
    bool Update(const Maze& maze);
    void Draw() const;
    Vector2 GetPosition() const;
};
