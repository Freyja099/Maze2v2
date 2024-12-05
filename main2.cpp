#include "raylib.h"
#include "maze.h"
#include "player.h"
#include <memory>
#include <vector>
#include <cmath>

// Constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const Color MENU_COLOR = {52, 73, 94, 255};
const Color MENU_TEXT_COLOR = {236, 240, 241, 255};
const float TEXT_FADE_SPEED = 0.4f;

// Enums
enum class GameState { MENU, PLAYING, FINISHED };
enum class Difficulty { EASY, MEDIUM, HARD };
enum class MenuState { STORY, LEVEL_SELECT };

// Story text structure
struct StoryText {
    const char* text;
    float alpha;
    float y;
    bool isVisible;
};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Improved Maze Game");
    InitAudioDevice();
    SetTargetFPS(60);

    // Load sounds
    Sound menuMusic = LoadSound("resources/menu_music.wav");
    Sound stepSound = LoadSound("resources/step.wav");
    Sound victorySound = LoadSound("resources/victory.wav");
    
    SetSoundVolume(menuMusic, 0.5f);
    SetSoundVolume(stepSound, 0.7f);
    SetSoundVolume(victorySound, 1.0f);

    GameState gameState = GameState::MENU;
    MenuState menuState = MenuState::STORY;
    Difficulty selectedDifficulty = Difficulty::EASY;
    float gameStartTime = 0;
    float gameEndTime = 0;
    float menuTimer = 0;
    
    // Story texts initialization
    std::vector<StoryText> storyTexts = {
        {"In a world of twisting paths and endless corridors...", 0.0f, SCREEN_HEIGHT/2 - 100, true},
        {"A little adventurer finds themselves lost and far from home...", 0.0f, SCREEN_HEIGHT/2, false},
        {"Can you help guide them back to their cozy house?", 0.0f, SCREEN_HEIGHT/2 + 100, false},
        {"Press SPACE to begin your journey...", 0.0f, SCREEN_HEIGHT/2 + 200, false}
    };
    int currentStoryText = 0;
    
    float victoryTimer = 0;
    Vector2 characterHomePos = {0, 0};
    
    std::unique_ptr<Maze> maze;
    std::unique_ptr<Player> player;

    PlaySound(menuMusic);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Loop menu music
        if (!IsSoundPlaying(menuMusic) && gameState == GameState::MENU) {
            PlaySound(menuMusic);
        }

        switch(gameState) {
            case GameState::MENU: {
                ClearBackground(MENU_COLOR);
                
                if (menuState == MenuState::STORY) {
                    menuTimer += GetFrameTime();
                    
                    // Update and draw story texts
                    for (size_t i = 0; i < storyTexts.size(); i++) {
                        if (storyTexts[i].isVisible) {
                            // Fade in text
                            storyTexts[i].alpha = fmin(1.0f, storyTexts[i].alpha + TEXT_FADE_SPEED * GetFrameTime());
                            
                            // Draw text with current alpha
                            Color textColor = MENU_TEXT_COLOR;
                            textColor.a = (unsigned char)(255 * storyTexts[i].alpha);
                            int textWidth = MeasureText(storyTexts[i].text, 30);
                            DrawText(storyTexts[i].text, 
                                   SCREEN_WIDTH/2 - textWidth/2,
                                   (int)storyTexts[i].y,
                                   30,
                                   textColor);
                        }
                    }

                    // Show next text every 2 seconds
                    if (menuTimer >= 2.0f && currentStoryText < storyTexts.size() - 1) {
                        currentStoryText++;
                        storyTexts[currentStoryText].isVisible = true;
                        menuTimer = 0;
                    }

                    if (IsKeyPressed(KEY_SPACE)) {
                        menuState = MenuState::LEVEL_SELECT;
                        menuTimer = 0;
                    }
                }
                else { // LEVEL_SELECT
                    const char* title = "Choose Your Path";
                    DrawText(title, 
                            SCREEN_WIDTH/2 - MeasureText(title, 50)/2,
                            150, 50, MENU_TEXT_COLOR);

                    DrawText("1: A Gentle Start - Perfect for beginners",
                            SCREEN_WIDTH/2 - 250, 400, 25, MENU_TEXT_COLOR);
                    DrawText("2: The Winding Path - For experienced explorers",
                            SCREEN_WIDTH/2 - 250, 450, 25, MENU_TEXT_COLOR);
                    DrawText("3: The Grand Maze - Ultimate challenge",
                            SCREEN_WIDTH/2 - 250, 500, 25, MENU_TEXT_COLOR);

                    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_THREE)) {
                        StopSound(menuMusic);
                        selectedDifficulty = IsKeyPressed(KEY_ONE) ? Difficulty::EASY : 
                                           IsKeyPressed(KEY_TWO) ? Difficulty::MEDIUM : Difficulty::HARD;
                        int mazeSize = (selectedDifficulty == Difficulty::EASY) ? 10 : 
                                     (selectedDifficulty == Difficulty::MEDIUM) ? 15 : 20;
                        
                        maze = std::make_unique<Maze>(mazeSize);
                        player = std::make_unique<Player>(CELL_SIZE * 1.5f, CELL_SIZE * 1.5f);
                        gameState = GameState::PLAYING;
                        gameStartTime = GetTime();
                    }
                }
                break;
            }
            
            case GameState::PLAYING: {
                bool moved = player->Update(*maze);
                if (moved) {
                    PlaySound(stepSound);
                }
                
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
                
                // Draw house at the exit
                int exitX = (mazeSize-2) * CELL_SIZE;
                int exitY = (mazeSize-2) * CELL_SIZE;
                
                // Draw house body
                DrawRectangle(exitX, exitY + CELL_SIZE/3, CELL_SIZE, 2*CELL_SIZE/3, HOUSE_COLOR);
                
                // Draw roof (triangle)
                Vector2 roofTop = {exitX + CELL_SIZE/2.0f, exitY};
                Vector2 roofLeft = {exitX, exitY + CELL_SIZE/3.0f};
                Vector2 roofRight = {exitX + CELL_SIZE, exitY + CELL_SIZE/3.0f};
                DrawTriangle(roofTop, roofLeft, roofRight, RED);
                
                // Draw door
                DrawRectangle(exitX + CELL_SIZE/3, exitY + CELL_SIZE/2,
                             CELL_SIZE/3, CELL_SIZE/2, DARKBROWN);
                
                // Draw doorknob
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
                    PlaySound(victorySound);
                    victoryTimer = 0;
                    characterHomePos = pos;
                }
                break;
            }
            
            case GameState::FINISHED: {
                victoryTimer += GetFrameTime();
                
                const char* congrats = "Home Sweet Home!";
                float scale = 1.0f + 0.1f * sinf(victoryTimer * 5.0f);
                float rotation = sinf(victoryTimer * 3.0f) * 5.0f;
                
                Vector2 textPos = {
                    SCREEN_WIDTH/2.0f,
                    SCREEN_HEIGHT/2.0f - 60
                };
                
                DrawTextPro(GetFontDefault(),
                           congrats,
                           textPos,
                           {MeasureText(congrats, 60) * scale/2.0f, 30},
                           rotation,
                           60,
                           2,
                           GOLD);
                
                DrawText(TextFormat("Time: %.1f seconds", gameEndTime),
                        SCREEN_WIDTH/2 - 100,
                        SCREEN_HEIGHT/2 + 50,
                        30,
                        BLACK);
                
                if (victoryTimer > 1.0f) {
                    float alpha = (sinf(victoryTimer * 2.0f) + 1.0f) * 0.5f;
                    Color promptColor = BLACK;
                    promptColor.a = (unsigned char)(255 * alpha);
                    DrawText("Press SPACE to play again",
                            SCREEN_WIDTH/2 - 120,
                            SCREEN_HEIGHT/2 + 120,
                            20,
                            promptColor);
                }
                
                if (IsKeyPressed(KEY_SPACE)) {
                    gameState = GameState::MENU;
                    menuState = MenuState::STORY;
                    menuTimer = 0;
                    currentStoryText = 0;
                    for (auto& text : storyTexts) {
                        text.alpha = 0.0f;
                        text.isVisible = false;
                    }
                    storyTexts[0].isVisible = true;
                }
                break;
            }
        }

        EndDrawing();
    }

    UnloadSound(menuMusic);
    UnloadSound(stepSound);
    UnloadSound(victorySound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
