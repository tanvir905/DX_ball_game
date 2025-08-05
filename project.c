#include "raylib.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 70
#define BRICK_HEIGHT 20
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 8

#define NAVY (Color){ 0, 0, 128, 255 }
#define TURQUOISE (Color){ 59, 244, 251, 255 }

typedef struct {
    Rectangle rect;
    bool active;
} Brick;

typedef enum { MENU, GAME, GAME_OVER, LEVEL_COMPLETE } GameState;

const char *menuItems[] = {
    "Level: ",
    "Sound: ",
    "Start Game",
    "Quit"
};

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1");
    SetExitKey(0);  // Disable ESC closing window
    SetTargetFPS(60);
    InitAudioDevice();

    Sound paddleSound = LoadSound("paddle.mp3");
    Sound brickSound = LoadSound("brick.wav");
    Sound gameOverSound = LoadSound("gameover.wav");
    Sound clickSound = LoadSound("click.wav");

    Vector2 ballPos, ballSpeed;
    Rectangle paddle;
    Brick bricks[BRICK_ROWS * BRICK_COLS];

    GameState state = MENU;

    int lives = 3;
    bool soundOn = true;
    int level = 1;
    float speedMultiplier = 1.0f;
    int score = 0;
    int menuIndex = 0;

    bool running = true;

    void ResetGame() {
        ballPos = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
        ballSpeed = (Vector2){ 4*speedMultiplier, -4*speedMultiplier };
        paddle = (Rectangle){ SCREEN_WIDTH/2 - PADDLE_WIDTH/2, SCREEN_HEIGHT - 40, PADDLE_WIDTH, PADDLE_HEIGHT };
        for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
            bricks[i].rect = (Rectangle){
                (i % BRICK_COLS) * (BRICK_WIDTH + 5) + 35,
                (i / BRICK_COLS) * (BRICK_HEIGHT + 5) + 50,
                BRICK_WIDTH, BRICK_HEIGHT
            };
            bricks[i].active = true;
        }
        lives = 3;
        score = 0;
    }

    while (running) {
        if (WindowShouldClose()) {
            running = false;
        }

        if (state == MENU) {
            if (IsKeyPressed(KEY_DOWN)) {
                menuIndex = (menuIndex + 1) % 4;
                if (soundOn) PlaySound(clickSound);
            }
            if (IsKeyPressed(KEY_UP)) {
                menuIndex = (menuIndex + 3) % 4;
                if (soundOn) PlaySound(clickSound);
            }
            if (menuIndex == 0) { // Level selected
                if (IsKeyPressed(KEY_RIGHT)) {
                    level++;
                    if (level > 3) level = 1;
                    if (level == 1) speedMultiplier = 1.0f;
                    else if (level == 2) speedMultiplier = 1.3f;
                    else speedMultiplier = 1.5f;
                    if (soundOn) PlaySound(clickSound);
                }
                else if (IsKeyPressed(KEY_LEFT)) {
                    level--;
                    if (level < 1) level = 3;
                    if (level == 1) speedMultiplier = 1.0f;
                    else if (level == 2) speedMultiplier = 1.3f;
                    else speedMultiplier = 1.5f;
                    if (soundOn) PlaySound(clickSound);
                }
            }
            else if (menuIndex == 1) { 
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                    soundOn = !soundOn;
                    if (soundOn) PlaySound(clickSound);
                }
            }

            Vector2 mousePos = GetMousePosition();
            for (int i = 0; i < 4; i++) {
                Rectangle btn = { SCREEN_WIDTH/2 - 150, 180 + i*60, 300, 50 };
                if (CheckCollisionPointRec(mousePos, btn)) {
                    menuIndex = i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (soundOn) PlaySound(clickSound);

                        if (menuIndex == 0) {
                            level++;
                            if (level > 3) level = 1;
                            if (level == 1) speedMultiplier = 1.0f;
                            else if (level == 2) speedMultiplier = 1.3f;
                            else speedMultiplier = 1.5f;
                        } else if (menuIndex == 1) {
                            soundOn = !soundOn;
                        } else if (menuIndex == 2) {
                            ResetGame();
                            state = GAME;
                        } else if (menuIndex == 3) {
                            running = false;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (soundOn) PlaySound(clickSound);
                if (menuIndex == 0) {
                    level++;
                    if (level > 3) level = 1;
                    if (level == 1) speedMultiplier = 1.0f;
                    else if (level == 2) speedMultiplier = 1.3f;
                    else speedMultiplier = 1.5f;
                } else if (menuIndex == 1) {
                    soundOn = !soundOn;
                } else if (menuIndex == 2) {
                    ResetGame();
                    state = GAME;
                } else if (menuIndex == 3) {
                    running = false;
                }
            }
        }
        else if (state == GAME) {
            if (IsKeyPressed(KEY_ESCAPE)) state = MENU;

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                paddle.x = GetMouseX() - PADDLE_WIDTH / 2;
            } else {
                if (IsKeyDown(KEY_LEFT)) paddle.x -= 10;
                if (IsKeyDown(KEY_RIGHT)) paddle.x += 10;
            }

            if (paddle.x < 0) paddle.x = 0;
            if (paddle.x > SCREEN_WIDTH - PADDLE_WIDTH) paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;

            ballPos.x += ballSpeed.x;
            ballPos.y += ballSpeed.y;

            if (ballPos.x < BALL_RADIUS || ballPos.x > SCREEN_WIDTH - BALL_RADIUS) ballSpeed.x *= -1;
            if (ballPos.y < BALL_RADIUS) ballSpeed.y *= -1;

            if (CheckCollisionCircleRec(ballPos, BALL_RADIUS, paddle)) {
                ballSpeed.y *= -1;
                ballPos.y = paddle.y - BALL_RADIUS;
                if (soundOn) PlaySound(paddleSound);
            }

            for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
                if (bricks[i].active && CheckCollisionCircleRec(ballPos, BALL_RADIUS, bricks[i].rect)) {
                    bricks[i].active = false;
                    ballSpeed.y *= -1;
                    score += 2;
                    if (soundOn) PlaySound(brickSound);
                    break;
                }
            }

            if (ballPos.y > SCREEN_HEIGHT) {
                lives--;
                ballPos = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
                if (lives <= 0) {
                    if (soundOn) PlaySound(gameOverSound);
                    state = GAME_OVER;
                }
            }

            bool allBricksGone = true;
            for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
                if (bricks[i].active) {
                    allBricksGone = false;
                    break;
                }
            }
            if (allBricksGone) {
                state = LEVEL_COMPLETE;
            }
        }

        BeginDrawing();
        ClearBackground(NAVY);

        if (state == MENU) {
            DrawText("PROJECT 1", SCREEN_WIDTH/2 - MeasureText("PROJECT 1", 40)/2, 80, 40, RAYWHITE);
            for (int i = 0; i < 4; i++) {
                Color bg = (i == menuIndex) ? BLUE : GRAY;
                Rectangle btn = { SCREEN_WIDTH/2 - 150, 180 + i*60, 300, 50 };
                DrawRectangleRec(btn, bg);
                DrawRectangleLinesEx(btn, 2, WHITE);

                static char label[64];
                if (i == 0) {
                    sprintf(label, "%s%d", menuItems[i], level);
                } else if (i == 1) {
                    sprintf(label, "%s%s", menuItems[i], soundOn ? "ON" : "OFF");
                } else {
                    sprintf(label, "%s", menuItems[i]);
                }
                DrawText(label, btn.x + 20, btn.y + 15, 20, WHITE);
            }

            DrawText("Use UP/DOWN, LEFT/RIGHT and ENTER/Click", SCREEN_WIDTH/2 - 230, 450, 20, LIGHTGRAY);
        }
        else if (state == GAME) {
            DrawRectangleRec(paddle, RAYWHITE);
            DrawCircleV(ballPos, BALL_RADIUS, ORANGE);

            for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
                if (bricks[i].active) {
                    Color brickColor;
                    if (level == 1) {
                        brickColor = (i % 2 == 0) ? TURQUOISE : SKYBLUE;
                    } else if (level == 2) {
                        brickColor = (i % 2 == 0) ? GREEN : LIME;
                    } else {
                        brickColor = (i % 2 == 0) ? ORANGE : GOLD;
                    }
                    DrawRectangleRec(bricks[i].rect, brickColor);
                }
            }

            for (int i = 0; i < lives; i++) {
                DrawCircle(20 + i*30, 20, 10, RED);
            }

            DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH - 150, 10, 20, WHITE);
        }
        else if (state == LEVEL_COMPLETE) {
            DrawText(TextFormat("CONGRATS! You cleared level %d", level), SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 40, 30, GREEN);
            DrawText("Press Y to continue or ESC/N for menu", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 + 10, 20, LIGHTGRAY);

            if (IsKeyPressed(KEY_Y)) {
                level++;
                if (level > 3) level = 1;
                if (level == 1) speedMultiplier = 1.0f;
                else if (level == 2) speedMultiplier = 1.3f;
                else speedMultiplier = 1.5f;
                ResetGame();
                state = GAME;
            }
            if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                state = MENU;
            }
        }
        else if (state == GAME_OVER) {
            DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 50)/2, SCREEN_HEIGHT/2 - 25, 50, RED);
            DrawText("Press ESC to return to MENU", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 40, 20, LIGHTGRAY);
            if (IsKeyPressed(KEY_ESCAPE)) state = MENU;
        }

        EndDrawing();
    }

    UnloadSound(paddleSound);
    UnloadSound(brickSound);
    UnloadSound(gameOverSound);
    UnloadSound(clickSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
