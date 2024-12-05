#include "player.h"
#include <cmath>

Player::Player(float x, float y) 
    : position{x, y}, velocity{0, 0}, radius{CELL_SIZE * 0.4f}, speed{3.0f}, friction{0.85f} {}

void Player::Update(const Maze& maze) {
    Vector2 input = {0, 0};
    if (IsKeyDown(KEY_RIGHT)) input.x += 1;
    if (IsKeyDown(KEY_LEFT)) input.x -= 1;
    if (IsKeyDown(KEY_DOWN)) input.y += 1;
    if (IsKeyDown(KEY_UP)) input.y -= 1;

    // Normalize diagonal movement
    if (input.x != 0 && input.y != 0) {
        input.x *= 0.707f;
        input.y *= 0.707f;
    }

    velocity.x += input.x * speed;
    velocity.y += input.y * speed;

    applyFriction();
    checkCollision(maze);

    position.x += velocity.x;
    position.y += velocity.y;
}

void Player::applyFriction() {
    velocity.x *= friction;
    velocity.y *= friction;

    // Stop very small movements to prevent sliding
    if (std::abs(velocity.x) < 0.01f) velocity.x = 0;
    if (std::abs(velocity.y) < 0.01f) velocity.y = 0;
}

void Player::checkCollision(const Maze& maze) {
    int cellX = static_cast<int>(position.x / CELL_SIZE);
    int cellY = static_cast<int>(position.y / CELL_SIZE);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (maze.isWall(cellX + i, cellY + j)) {
                Rectangle wall = {
                    (cellX + i) * CELL_SIZE,
                    (cellY + j) * CELL_SIZE,
                    CELL_SIZE,
                    CELL_SIZE
                };

                Vector2 closestPoint = {
                    Clamp(position.x, wall.x, wall.x + wall.width),
                    Clamp(position.y, wall.y, wall.y + wall.height)
                };

                Vector2 difference = {
                    position.x - closestPoint.x,
                    position.y - closestPoint.y
                };

                float distanceSquared = difference.x * difference.x + difference.y * difference.y;

                if (distanceSquared < radius * radius) {
                    Vector2 normal = getCollisionNormal(wall);
                    float depth = radius - std::sqrt(distanceSquared);

                    position.x += normal.x * depth;
                    position.y += normal.y * depth;

                    float dotProduct = velocity.x * normal.x + velocity.y * normal.y;
                    velocity.x -= 2 * dotProduct * normal.x;
                    velocity.y -= 2 * dotProduct * normal.y;

                    velocity.x *= 0.5f; // Reduce bounce
                    velocity.y *= 0.5f;
                }
            }
        }
    }
}

Vector2 Player::getCollisionNormal(const Rectangle& wall) {
    Vector2 normal = {0, 0};
    float left = std::abs(position.x - wall.x);
    float right = std::abs(position.x - (wall.x + wall.width));
    float top = std::abs(position.y - wall.y);
    float bottom = std::abs(position.y - (wall.y + wall.height));

    float minDist = std::min({left, right, top, bottom});

    if (minDist == left) normal.x = -1;
    else if (minDist == right) normal.x = 1;
    else if (minDist == top) normal.y = -1;
    else if (minDist == bottom) normal.y = 1;

    return normal;
}

void Player::Draw() const {
    DrawCircleV(position, radius, PLAYER_COLOR);
    float eyeSize = radius / 4;
    Vector2 leftEye = {position.x - radius / 3, position.y - radius / 3};
    Vector2 rightEye = {position.x + radius / 3, position.y - radius / 3};
    DrawCircleV(leftEye, eyeSize, WHITE);
    DrawCircleV(rightEye, eyeSize, WHITE);
    Vector2 smileStart = {position.x - radius / 2, position.y + radius / 3};
    Vector2 smileEnd = {position.x + radius / 2, position.y + radius / 3};
    DrawLineEx(smileStart, smileEnd, 2.0f, WHITE);
}

Vector2 Player::GetPosition() const {
    return position;
}