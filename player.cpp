#include "player.h"
#include <cmath>
#include <raymath.h>

Player::Player(float x, float y) 
    : position{x, y}, velocity{0, 0}, radius{CELL_SIZE * 0.4f}, 
      speed{CELL_SIZE}, friction{0.9f}, isMoving{false} {}

bool Player::Update(const Maze& maze) {
    bool moved = false;
    
    // Only accept new movement input if the player is not currently moving
    if (!isMoving) {
        Vector2 newPosition = position;
        bool shouldMove = false;

        if (IsKeyPressed(KEY_RIGHT)) {
            newPosition.x += CELL_SIZE;
            shouldMove = true;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            newPosition.x -= CELL_SIZE;
            shouldMove = true;
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            newPosition.y += CELL_SIZE;
            shouldMove = true;
        }
        else if (IsKeyPressed(KEY_UP)) {
            newPosition.y -= CELL_SIZE;
            shouldMove = true;
        }

        // Check if the new position is valid (not inside a wall)
        if (shouldMove) {
            int newCellX = static_cast<int>(newPosition.x / CELL_SIZE);
            int newCellY = static_cast<int>(newPosition.y / CELL_SIZE);
            
            if (!maze.isWall(newCellX, newCellY)) {
                // Instantly move to the new position
                position = newPosition;
                moved = true;
            }
        }
    }
    
    return moved;
}

void Player::Draw() const {
    DrawCircleV(position, radius, PLAYER_COLOR);

    // Draw eyes
    float eyeSize = radius / 4;
    Vector2 leftEye = {position.x - radius / 3, position.y - radius / 3};
    Vector2 rightEye = {position.x + radius / 3, position.y - radius / 3};
    DrawCircleV(leftEye, eyeSize, WHITE);
    DrawCircleV(rightEye, eyeSize, WHITE);

    // Draw smile
    Vector2 smileStart = {position.x - radius / 2, position.y + radius / 3};
    Vector2 smileEnd = {position.x + radius / 2, position.y + radius / 3};
    DrawLineEx(smileStart, smileEnd, 2.0f, WHITE);
}

Vector2 Player::GetPosition() const {
    return position;
}
