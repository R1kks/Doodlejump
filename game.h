#pragma once
/*объявление констант*/
#define WINDOW_WIDTH    400 //Ширинп окна
#define WINDOW_HEIGHT   600 //Высота окна

#define PLAYER_SIZE     24 //размер игрока
#define PLATFORM_W      70 //ширина платформы
#define PLATFORM_H      12 //высота платформы

#define GRAVITY         0.45f //гравитация
#define JUMP_FORCE      -15.0f //сила обычного прыжка
#define JUMP_SPRING     -30.0f //сила прыжка с жёлтой платформы
#define JUMP_JETPACK    -60.0f //сида прыжка с синей платформы
#define MOVE_SPEED      10.0f

#define MAX_PLATFORMS   20 // максимальное кол-во платформ, которые могут находиться на экране
#define SCROLL_Y        250 //линия камеры
#define PLATFORM_SPACING     140 //расстояние между платформами
#define PLATFORM_SPAWN_CHANCE 50 //шанс появления платформы сверху
#define PLATFORM_SPAWN_GAP   80 //случайная добавка к расстоянию между платформами

/*обозначение платформ*/
#define PLATFORM_NORMAL      0
#define PLATFORM_BREAKABLE   1
#define PLATFORM_SPRING      2
#define PLATFORM_JETPACK     3
/*обозначение платформ*/

#define MAX_ENEMIES     10 //максимальное кол-во врагов, которые могут одновременно находиться на экране
#define MAX_BULLETS     1 //максимально кол-во пуль за 1 выстрел
#define ENEMY_SIZE      60 //размер врагов
#define BULLET_W        4 //ширина пули
#define BULLET_H        10 //высота пули
#define BULLET_SPEED    -15.0f //скорость полёта пули
#define ENEMY_SPAWN_CHANCE 25 //шанс появления врага на обычной платформе
#define SHOOT_COOLDOWN  20 //время перезарядки (в кадрах)
#define SCORE_RATE      0.33f //множитель начисления очков
/*объявление констант*/


typedef struct {
    float x, y; //координаты 
    float vx, vy; //скорость 
    int w, h; //размер
    int canJump; //возможность прыжка
} Player;

typedef struct {
    float x, y; //координаты
    int w, h; //размеры
    int active; //активность
    int type; //тип
} Platform;

typedef struct {
    float x, y; //координаты
    int w, h; //размер
    int active; //активность
    int platformIndex; //индекс платформы в массиве, на которой находится враг
} Enemy;

typedef struct {
    float x, y; //координаты
    int w, h; //размер
    int active; //активность
    float speedY; //скорость полёта
} Bullet;

typedef struct {
    Player player;
    Platform platforms[MAX_PLATFORMS];
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    int score; //текущий счёт
    int highScore; //лучший счёт
    int isGameOver; //флаг окончания игры
    int isGameStarted; //флаг начала игры
    int frameCount; //счётчик кадров
    int shootCooldownTimer; //таймер перезарядки
} GameState;

/* Структура для передачи ввода из UI модуля в Logic модуль */
typedef struct {
    int left;
    int right;
    int shoot;
} InputState;

void Game_Init(GameState* state); //функция, которая возвращает начальный экран
void Game_Update(GameState* state); //функция обновления игры
void Game_HandleInput(GameState* state, const InputState* input); //передаёт нажатие кнопки