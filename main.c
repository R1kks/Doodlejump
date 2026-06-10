#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "render.h"

typedef struct {
    GameState gameState;
    RenderContext renderCtx;
    InputState input;
    HWND hwnd;
} AppContext;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AppContext* app = (AppContext*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CREATE: {
        app = (AppContext*)malloc(sizeof(AppContext));
        if (!app) return -1;

        ZeroMemory(app, sizeof(AppContext));
        app->hwnd = hwnd;

        if (!Render_Create(&app->renderCtx, hwnd)) {
            free(app);
            return -1;
        }

        srand((unsigned int)time(NULL));
        Game_Init(&app->gameState);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
        return 0;
    }

    case WM_KEYDOWN:
        if (!app) break;
        switch (wParam) {
        case 'A': case VK_LEFT: app->input.left = 1; break;
        case 'D': case VK_RIGHT: app->input.right = 1; break;
        case VK_SPACE:
            if (!app->gameState.isGameStarted || app->gameState.isGameOver) {
                Game_Init(&app->gameState);
                app->gameState.isGameStarted = 1;
            }
            app->input.shoot = 1;
            break;
        }
        return 0;

    case WM_KEYUP:
        if (!app) break;
        switch (wParam) {
        case 'A': case VK_LEFT: app->input.left = 0; break;
        case 'D': case VK_RIGHT: app->input.right = 0; break;
        case VK_SPACE: app->input.shoot = 0; break;
        }
        return 0;

    case WM_PAINT: {
        if (!app) break;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Render_Draw(&app->renderCtx, hdc, &app->gameState);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (app) {
            Render_Destroy(&app->renderCtx);
            free(app);
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    const char CLASS_NAME[] = "LDoodleJumpGame";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        return 0;
    }

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    int x = (sw - WINDOW_WIDTH) / 2;
    int y = (sh - WINDOW_HEIGHT) / 2;

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Doodle Jump",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInst, NULL
    );

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    LARGE_INTEGER freq, lastTime, currentTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

    const double FIXED_DT = 1.0 / 60.0;
    double accumulator = 0.0;
    MSG msg = { 0 };

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            QueryPerformanceCounter(&currentTime);
            double deltaTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / (double)freq.QuadPart;
            lastTime = currentTime;

            if (deltaTime > 0.05) deltaTime = 0.05;

            accumulator += deltaTime;

            while (accumulator >= FIXED_DT) {
                AppContext* app = (AppContext*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if (app) {
                    Game_HandleInput(&app->gameState, &app->input);
                    Game_Update(&app->gameState);

                    app->input.shoot = 0;
                }
                accumulator -= FIXED_DT;
            }

            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return (int)msg.wParam;
}

int main(int argc, char* argv[]) {
    return WinMain(GetModuleHandleA(NULL), NULL, GetCommandLineA(), SW_SHOW);
}
