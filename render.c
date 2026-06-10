// render.c
#include "render.h"
#include <stdio.h>

int Render_Create(RenderContext* ctx, HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    if (!hdc) return 0;

    ctx->hMemDC = CreateCompatibleDC(hdc);
    if (!ctx->hMemDC) {
        ReleaseDC(hwnd, hdc);
        return 0;
    }

    ctx->hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!ctx->hBitmap) {
        DeleteDC(ctx->hMemDC);
        ReleaseDC(hwnd, hdc);
        return 0;
    }

    SelectObject(ctx->hMemDC, ctx->hBitmap);
    ReleaseDC(hwnd, hdc);

    ctx->hBrushBg = CreateSolidBrush(RGB(245, 250, 255));
    ctx->hBrushPlayer = CreateSolidBrush(RGB(70, 180, 70));
    ctx->hBrushPlatform = CreateSolidBrush(RGB(120, 80, 40));
    ctx->hBrushBreakable = CreateSolidBrush(RGB(200, 50, 50));
    ctx->hBrushSpring = CreateSolidBrush(RGB(220, 200, 0));
    ctx->hBrushJetpack = CreateSolidBrush(RGB(0, 150, 220));
    ctx->hBrushEnemy = CreateSolidBrush(RGB(120, 0, 180));
    ctx->hBrushBullet = CreateSolidBrush(RGB(255, 140, 0));
    ctx->hPenOutline = CreatePen(PS_SOLID, 1, RGB(40, 40, 40));

    return 1;
}

void Render_Destroy(RenderContext* ctx) {
    if (ctx->hBitmap) DeleteObject(ctx->hBitmap);
    if (ctx->hMemDC) DeleteDC(ctx->hMemDC);
    if (ctx->hBrushBg) DeleteObject(ctx->hBrushBg);
    if (ctx->hBrushPlayer) DeleteObject(ctx->hBrushPlayer);
    if (ctx->hBrushPlatform) DeleteObject(ctx->hBrushPlatform);
    if (ctx->hBrushBreakable) DeleteObject(ctx->hBrushBreakable);
    if (ctx->hBrushSpring) DeleteObject(ctx->hBrushSpring);
    if (ctx->hBrushJetpack) DeleteObject(ctx->hBrushJetpack);
    if (ctx->hBrushEnemy) DeleteObject(ctx->hBrushEnemy);
    if (ctx->hBrushBullet) DeleteObject(ctx->hBrushBullet);
    if (ctx->hPenOutline) DeleteObject(ctx->hPenOutline);
}

void Render_Draw(RenderContext* ctx, HDC hdc, const GameState* state) {
    SelectObject(ctx->hMemDC, ctx->hBrushBg);
    Rectangle(ctx->hMemDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(ctx->hMemDC, ctx->hPenOutline);

    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (state->platforms[i].active) {
            switch (state->platforms[i].type) {
            case PLATFORM_BREAKABLE: SelectObject(ctx->hMemDC, ctx->hBrushBreakable); break;
            case PLATFORM_SPRING: SelectObject(ctx->hMemDC, ctx->hBrushSpring); break;
            case PLATFORM_JETPACK: SelectObject(ctx->hMemDC, ctx->hBrushJetpack); break;
            default: SelectObject(ctx->hMemDC, ctx->hBrushPlatform); break;
            }
            Rectangle(ctx->hMemDC,
                (int)state->platforms[i].x, (int)state->platforms[i].y,
                (int)state->platforms[i].x + state->platforms[i].w,
                (int)state->platforms[i].y + state->platforms[i].h);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemies[i].active) {
            SelectObject(ctx->hMemDC, ctx->hBrushEnemy);
            Ellipse(ctx->hMemDC,
                (int)state->enemies[i].x, (int)state->enemies[i].y,
                (int)state->enemies[i].x + state->enemies[i].w,
                (int)state->enemies[i].y + state->enemies[i].h);

            SelectObject(ctx->hMemDC, ctx->hBrushBg);
            Ellipse(ctx->hMemDC, (int)state->enemies[i].x + 6, (int)state->enemies[i].y + 7, (int)state->enemies[i].x + 14, (int)state->enemies[i].y + 13);
            Ellipse(ctx->hMemDC, (int)state->enemies[i].x + 14, (int)state->enemies[i].y + 7, (int)state->enemies[i].x + 22, (int)state->enemies[i].y + 13);
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (state->bullets[i].active) {
            SelectObject(ctx->hMemDC, ctx->hBrushBullet);
            Rectangle(ctx->hMemDC,
                (int)state->bullets[i].x, (int)state->bullets[i].y,
                (int)state->bullets[i].x + state->bullets[i].w,
                (int)state->bullets[i].y + state->bullets[i].h);
        }
    }

    SelectObject(ctx->hMemDC, ctx->hBrushPlayer);
    Ellipse(ctx->hMemDC,
        (int)state->player.x, (int)state->player.y,
        (int)state->player.x + state->player.w,
        (int)state->player.y + state->player.h);

    SetBkMode(ctx->hMemDC, TRANSPARENT);
    char buf[64];

    sprintf_s(buf, sizeof(buf), "Score: %d", state->score);
    SetTextColor(ctx->hMemDC, RGB(0, 0, 0));
    TextOutA(ctx->hMemDC, 10, 10, buf, (int)strlen(buf));

    sprintf_s(buf, sizeof(buf), "Best: %d", state->highScore);
    TextOutA(ctx->hMemDC, WINDOW_WIDTH - 90, 10, buf, (int)strlen(buf));

    if (state->isGameStarted && !state->isGameOver) {
        if (state->shootCooldownTimer > 0) {
            SetTextColor(ctx->hMemDC, RGB(150, 150, 150));
            TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 40, 10, "[RELOADING]", 11);
        }
        else {
            SetTextColor(ctx->hMemDC, RGB(0, 150, 0));
            TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 30, 10, "[READY]", 7);
        }
    }

    if (!state->isGameStarted) {
        SetTextColor(ctx->hMemDC, RGB(30, 30, 150));
        TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 65, WINDOW_HEIGHT / 2 - 50, "DOODLE JUMP", 11);
        SetTextColor(ctx->hMemDC, RGB(0, 0, 0));
        TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 20, "A / D - Move", 12);
        TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 + 5, "SPACE - Start", 13);
    }
    else if (state->isGameOver) {
        SetTextColor(ctx->hMemDC, RGB(200, 0, 0));
        TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 45, WINDOW_HEIGHT / 2 - 20, "GAME OVER", 9);
        SetTextColor(ctx->hMemDC, RGB(0, 0, 0));
        TextOutA(ctx->hMemDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 15, "Press SPACE to Restart", 22);
    }

    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ctx->hMemDC, 0, 0, SRCCOPY);
}