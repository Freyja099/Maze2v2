#include "raylib.h"
#include "maze.h"
#include "player.h"
#include <memory>

// Constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const Color MENU_COLOR = {52, 73, 94, 255};
const Color MENU_TEXT_COLOR = {236, 240, 241, 255};

// Enums
enum class GameState { MENU, PLAYING, FINISHED };
enum class Difficulty { EASY, MEDIUM, HARD };

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Improved Maze Game");
    SetTargetFPS(60);

    GameState gameState = GameState::MENU;
    Difficulty selectedDifficulty = Difficulty::EASY;
    float gameStartTime = 0;
    float gameEndTime = 0;
    
    std::unique_ptr<Maze> maze;
    std::unique_ptr<Player> player;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(gameState) {
            case GameState::MENU: {
                ClearBackground(MENU_COLOR);
                
                const char* title = "MAZE ADVENTURE";
                const char* subtitle = "Help the player find their way home!";
                const char* instruction = "Navigate through the maze to reach your house";
                
                int titlePosX = SCREEN_WIDTH/2 - MeasureText(title, 50)/2;
                int subtitlePosX = SCREEN_WIDTH/2 - MeasureText(subtitle, 30)/2;
                int instructionPosX = SCREEN_WIDTH/2 - MeasureText(instruction, 25)/2;
                
                DrawText(title, titlePosX, 150, 50, MENU_TEXT_COLOR);
                DrawText(subtitle, subtitlePosX, 220, 30, MENU_TEXT_COLOR);
                DrawText(instruction, instructionPosX, 270, 25, MENU_TEXT_COLOR);
                
                DrawText("Select your challenge:", SCREEN_WIDTH/2 - 150, 340, 30, MENU_TEXT_COLOR);
                DrawText("Press 1: Simple Level - Perfect for beginners", SCREEN_WIDTH/2 - 250, 400, 25, MENU_TEXT_COLOR);
                DrawText("Press 2: Medium Level - For experienced players", SCREEN_WIDTH/2 - 250, 450, 25, MENU_TEXT_COLOR);
                DrawText("Press 3: Hard Level - Ultimate maze challenge", SCREEN_WIDTH/2 - 250, 500, 25, MENU_TEXT_COLOR);

                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_THREE)) {
                    selectedDifficulty = IsKeyPressed(KEY_ONE) ? Difficulty::EASY : 
                                         IsKeyPressed(KEY_TWO) ? Difficulty::MEDIUM : Difficulty::HARD;
                    int mazeSize = (selectedDifficulty == Difficulty::EASY) ? 10 : 
                                   (selectedDifficulty == Difficulty::MEDIUM) ? 15 : 20;
                    
                    maze = std::make_unique<Maze>(mazeSize);
                    player = std::make_unique<Player>(CELL_SIZE * 1.5f, CELL_SIZE * 1.5f);
                    gameState = GameState::PLAYING;
                    gameStartTime = GetTime();
                }
                break;
            }
            
            case GameState::PLAYING: {
                player->Update(*maze);
                
                int mazeSize = maze->GetSize();
                int mazePixelSize = mazeSize * CELL_SIZE;
                int offsetX = (SCREEN_WIDTH - mazePixelSize) / 2;
                int offsetY = (SCREEN_HEIGHT - mazePixelSize) / 2;
                
                BeginMode2D((Camera2D){
                    .offset = {(float)offsetX, (float)offsetY},
                    .target = {0, 0},
                    .rotation = 0,
                    .zoom = 1.0f
                });
                
                maze->Draw();
                
                int exitX = (mazeSize-2) * CELL_SIZE;
                int exitY = (mazeSize-2) * CELL_SIZE;
                
                DrawRectangle(exitX, exitY + CELL_SIZE/3, CELL_SIZE, 2*CELL_SIZE/3, HOUSE_COLOR);
                
                Vector2 roofTop = {exitX + CELL_SIZE/2, exitY};
                Vector2 roofLeft = {exitX, exitY + CELL_SIZE/3};
                Vector2 roofRight = {exitX + CELL_SIZE, exitY + CELL_SIZE/3};
                DrawTriangle(roofTop, roofLeft, roofRight, RED);
                
                DrawRectangle(exitX + CELL_SIZE/3, exitY + CELL_SIZE/2,
                             CELL_SIZE/3, CELL_SIZE/2, BROWN);
                
                DrawCircle(exitX + 2*CELL_SIZE/3 - 5, exitY + 3*CELL_SIZE/4,
                           3, GOLD);
                
                player->Draw();
                
                EndMode2D();
                
                float currentTime = GetTime() - gameStartTime;
                DrawText(TextFormat("Time: %.1f", currentTime), 10, 10, 20, BLACK);
                
                Vector2 pos = player->GetPosition();
                if (pos.x > (mazeSize-2) * CELL_SIZE && pos.y > (mazeSize-2) * CELL_SIZE) {
                    gameState = GameState::FINISHED;
                    gameEndTime = currentTime;
                }
                break;
            }
            
            case GameState::FINISHED: {
                int titlePosX = SCREEN_WIDTH/2 - MeasureText("Congratulations!", 40)/2;
                DrawText("Congratulations!", titlePosX, SCREEN_HEIGHT/2 - 60, 40, BLACK);
                DrawText(TextFormat("Time: %.1f seconds", gameEndTime), 
                        SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 30, BLACK);
                DrawText("Press SPACE to play again", 
                        SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 60, 20, BLACK);
                
                if (IsKeyPressed(KEY_SPACE)) {
                    gameState = GameState::MENU;
                }
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
