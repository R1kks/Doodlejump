#include "game.h"
#include <stdlib.h>

static void SpawnPlatform(GameState* state, float x, float y, int type) {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (!state->platforms[i].active) {
            state->platforms[i].x = x;
            state->platforms[i].y = y;
            state->platforms[i].w = PLATFORM_W;
            state->platforms[i].h = PLATFORM_H;
            state->platforms[i].active = 1;
            state->platforms[i].type = type;
            break;
        }
    }
}

static void SpawnEnemyOnPlatform(GameState* state, int platformIdx) {
    if (platformIdx < 0 || platformIdx >= MAX_PLATFORMS) return;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!state->enemies[i].active) {
            state->enemies[i].active = 1;
            state->enemies[i].x = state->platforms[platformIdx].x + (PLATFORM_W - ENEMY_SIZE) / 2.0f;
            state->enemies[i].y = state->platforms[platformIdx].y - ENEMY_SIZE;
            state->enemies[i].w = ENEMY_SIZE;
            state->enemies[i].h = ENEMY_SIZE;
            state->enemies[i].platformIndex = platformIdx;
            break;
        }
    }
}

static void SpawnBullet(GameState* state) {
    if (state->shootCooldownTimer > 0) return;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!state->bullets[i].active) {
            state->bullets[i].active = 1;
            state->bullets[i].x = state->player.x + state->player.w / 2.0f - BULLET_W / 2.0f;
            state->bullets[i].y = state->player.y;
            state->bullets[i].w = BULLET_W;
            state->bullets[i].h = BULLET_H;
            state->bullets[i].speedY = BULLET_SPEED;
            state->shootCooldownTimer = SHOOT_COOLDOWN;
            break;
        }
    }
}

static void UpdateEnemies(GameState* state) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!state->enemies[i].active) continue;
        int pIdx = state->enemies[i].platformIndex;
        if (pIdx < 0 || pIdx >= MAX_PLATFORMS || !state->platforms[pIdx].active) {
            state->enemies[i].active = 0;
        }
    }
}

static void UpdateBullets(GameState* state) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (state->bullets[i].active) {
            state->bullets[i].y += state->bullets[i].speedY;
            if (state->bullets[i].y < -BULLET_H) {
                state->bullets[i].active = 0;
            }
        }
    }
    if (state->shootCooldownTimer > 0) {
        state->shootCooldownTimer--;
    }
}

static void CheckCollisions(GameState* state) {
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (!state->bullets[b].active) continue;
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!state->enemies[e].active) continue;

            if (state->bullets[b].x < state->enemies[e].x + state->enemies[e].w &&
                state->bullets[b].x + state->bullets[b].w > state->enemies[e].x &&
                state->bullets[b].y < state->enemies[e].y + state->enemies[e].h &&
                state->bullets[b].y + state->bullets[b].h > state->enemies[e].y) {

                state->bullets[b].active = 0;
                state->enemies[e].active = 0;
                state->score += 50;
                break;
            }
        }
    }

    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (!state->enemies[e].active) continue;
        if (state->player.x < state->enemies[e].x + state->enemies[e].w &&
            state->player.x + state->player.w > state->enemies[e].x &&
            state->player.y < state->enemies[e].y + state->enemies[e].h &&
            state->player.y + state->player.h > state->enemies[e].y) {

            state->isGameOver = 1;
            if (state->score > state->highScore) {
                state->highScore = state->score;
            }
            break;
        }
    }
}

void Game_Init(GameState* state) {
    state->player.x = (float)(WINDOW_WIDTH / 2 - PLAYER_SIZE / 2);
    state->player.y = (float)(WINDOW_HEIGHT - 150);
    state->player.vx = 0;
    state->player.vy = 0;
    state->player.w = PLAYER_SIZE;
    state->player.h = PLAYER_SIZE;
    state->player.canJump = 0;
    state->score = 0;
    state->isGameOver = 0;
    state->frameCount = 0;
    state->shootCooldownTimer = 0;

    for (int i = 0; i < MAX_PLATFORMS; i++) state->platforms[i].active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) state->enemies[i].active = 0;
    for (int i = 0; i < MAX_BULLETS; i++) state->bullets[i].active = 0;

    SpawnPlatform(state, (float)(WINDOW_WIDTH / 2 - PLATFORM_W / 2), (float)(WINDOW_HEIGHT - 100), PLATFORM_NORMAL);

    for (int i = 1; i < MAX_PLATFORMS; i++) {
        float x = (float)(rand() % (WINDOW_WIDTH - PLATFORM_W));
        float y = (float)(WINDOW_HEIGHT - 100 - i * PLATFORM_SPACING);
        int roll = rand() % 100;
        int type;

        if (i <= 3) type = PLATFORM_NORMAL;
        else if (roll < 70) type = PLATFORM_NORMAL;
        else if (roll < 90) type = PLATFORM_BREAKABLE;
        else if (roll < 97) type = PLATFORM_SPRING;
        else type = PLATFORM_JETPACK;

        int platIdx = -1;
        for (int j = 0; j < MAX_PLATFORMS; j++) {
            if (!state->platforms[j].active) {
                state->platforms[j].x = x;
                state->platforms[j].y = y;
                state->platforms[j].w = PLATFORM_W;
                state->platforms[j].h = PLATFORM_H;
                state->platforms[j].active = 1;
                state->platforms[j].type = type;
                platIdx = j;
                break;
            }
        }

        if (platIdx >= 0 && i > 3 && type == PLATFORM_NORMAL && (rand() % 100) < ENEMY_SPAWN_CHANCE) {
            SpawnEnemyOnPlatform(state, platIdx);
        }
    }
}

void Game_Update(GameState* state) {
    if (state->isGameOver || !state->isGameStarted) return;

    state->frameCount++;
    state->player.vy += GRAVITY;
    state->player.x += state->player.vx;
    state->player.y += state->player.vy;

    if (state->player.x + state->player.w < 0) {
        state->player.x = (float)WINDOW_WIDTH;
    }
    else if (state->player.x > WINDOW_WIDTH) {
        state->player.x = (float)-state->player.w;
    }

    state->player.canJump = 0;
    if (state->player.vy > 0) {
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (!state->platforms[i].active) continue;

            if (state->player.x < state->platforms[i].x + state->platforms[i].w &&
                state->player.x + state->player.w > state->platforms[i].x &&
                state->player.y + state->player.h >= state->platforms[i].y &&
                state->player.y + state->player.h <= state->platforms[i].y + state->platforms[i].h + state->player.vy + 3) {

                switch (state->platforms[i].type) {
                case PLATFORM_BREAKABLE:
                    state->player.vy = JUMP_FORCE;
                    state->platforms[i].active = 0;
                    for (int e = 0; e < MAX_ENEMIES; e++) {
                        if (state->enemies[e].active && state->enemies[e].platformIndex == i) {
                            state->enemies[e].active = 0;
                        }
                    }
                    break;
                case PLATFORM_SPRING: state->player.vy = JUMP_SPRING; break;
                case PLATFORM_JETPACK: state->player.vy = JUMP_JETPACK; break;
                default: state->player.vy = JUMP_FORCE; break;
                }
                state->player.canJump = 1;
                break;
            }
        }
    }

    if (state->player.y < SCROLL_Y) {
        float diff = SCROLL_Y - state->player.y;
        state->player.y = SCROLL_Y;

        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (state->platforms[i].active) {
                state->platforms[i].y += diff;
                if (state->platforms[i].y > WINDOW_HEIGHT) {
                    state->platforms[i].active = 0;
                    for (int e = 0; e < MAX_ENEMIES; e++) {
                        if (state->enemies[e].active && state->enemies[e].platformIndex == i) {
                            state->enemies[e].active = 0;
                        }
                    }
                }
            }
        }

        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (state->enemies[e].active) {
                int pIdx = state->enemies[e].platformIndex;
                if (pIdx >= 0 && pIdx < MAX_PLATFORMS && state->platforms[pIdx].active) {
                    state->enemies[e].y = state->platforms[pIdx].y - ENEMY_SIZE;
                }
            }
        }
        state->score += (int)(diff * SCORE_RATE);
    }

    float highestY = WINDOW_HEIGHT + 100;
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (state->platforms[i].active && state->platforms[i].y < highestY) {
            highestY = state->platforms[i].y;
        }
    }

    if (highestY > -50) {
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (!state->platforms[i].active) {
                if ((rand() % 100) < PLATFORM_SPAWN_CHANCE) {
                    state->platforms[i].x = (float)(rand() % (WINDOW_WIDTH - PLATFORM_W));
                    state->platforms[i].y = highestY - PLATFORM_SPACING - (float)(rand() % PLATFORM_SPAWN_GAP);
                    int roll = rand() % 100;
                    if (roll < 70) state->platforms[i].type = PLATFORM_NORMAL;
                    else if (roll < 90) state->platforms[i].type = PLATFORM_BREAKABLE;
                    else if (roll < 97) state->platforms[i].type = PLATFORM_SPRING;
                    else state->platforms[i].type = PLATFORM_JETPACK;

                    state->platforms[i].w = PLATFORM_W;
                    state->platforms[i].h = PLATFORM_H;
                    state->platforms[i].active = 1;

                    if (state->platforms[i].type == PLATFORM_NORMAL && (rand() % 100) < ENEMY_SPAWN_CHANCE) {
                        SpawnEnemyOnPlatform(state, i);
                    }
                }
                break;
            }
        }
    }

    UpdateBullets(state);
    UpdateEnemies(state);
    CheckCollisions(state);

    if (state->player.y > WINDOW_HEIGHT) {
        state->isGameOver = 1;
        if (state->score > state->highScore) {
            state->highScore = state->score;
        }
    }
}

void Game_HandleInput(GameState* state, const InputState* input) {
    if (input->left) state->player.vx = -MOVE_SPEED;
    else if (input->right) state->player.vx = MOVE_SPEED;
    else state->player.vx = 0;

    if (input->shoot) {
        SpawnBullet(state);
    }
}
