#pragma once
#include <windows.h>
#include "game.h"

typedef struct {
    HDC hMemDC;
    HBITMAP hBitmap;
    HBRUSH hBrushBg;
    HBRUSH hBrushPlayer;
    HBRUSH hBrushPlatform;
    HBRUSH hBrushBreakable;
    HBRUSH hBrushSpring;
    HBRUSH hBrushJetpack;
    HBRUSH hBrushEnemy;
    HBRUSH hBrushBullet;
    HPEN hPenOutline;
} RenderContext;

int Render_Create(RenderContext* ctx, HWND hwnd);
void Render_Destroy(RenderContext* ctx);
void Render_Draw(RenderContext* ctx, HDC hdc, const GameState* state);