#pragma once

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 600

#define PLAYER_SIZE 24
#define PLATFORM_W 70
#define PLATFORM_H 12

#define GRAVITY 0.45f
#define JUMP_FORCE -15.0f
#define JUMP_SPRING -30.0f
#define JUMP_JETPACK -60.0f
#define MOVE_SPEED 10.0f

#define MAX_PLATFORMS 20
#define SCROLL_Y 250
#define PLATFORM_SPACING 140
#define PLATFORM_SPAWN_CHANCE 50
#define PLATFORM_SPAWN_GAP 80

#define PLATFORM_NORMAL 0
#define PLATFORM_BREAKABLE 1
#define PLATFORM_SPRING 2
#define PLATFORM_JETPACK 3

#define MAX_ENEMIES 10
#define MAX_BULLETS 1
#define ENEMY_SIZE 60
#define BULLET_W 4
#define BULLET_H 10
#define BULLET_SPEED -15.0f
#define ENEMY_SPAWN_CHANCE 25
#define SHOOT_COOLDOWN 20
#define SCORE_RATE 0.33f

typedef struct {
    float x, y;
    float vx, vy;
    int w, h;
    int canJump;
} Player;

typedef struct {
    float x, y;
    int w, h;
    int active;
    int type;
} Platform;

typedef struct {
    float x, y;
    int w, h;
    int active;
    int platformIndex;
} Enemy;

typedef struct {
    float x, y;
    int w, h;
    int active;
    float speedY;
} Bullet;

typedef struct {
    Player player;
    Platform platforms[MAX_PLATFORMS];
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    int score;
    int highScore;
    int isGameOver;
    int isGameStarted;
    int frameCount;
    int shootCooldownTimer;
} GameState;

typedef struct {
    int left;
    int right;
    int shoot;
} InputState;

void Game_Init(GameState* state);
void Game_Update(GameState* state);
void Game_HandleInput(GameState* state, const InputState* input);
