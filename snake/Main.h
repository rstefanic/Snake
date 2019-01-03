#pragma once

#include <iostream>
#include <list>
#include <thread>
#include <chrono>

#include <Windows.h>

struct SnakeSegment
{
    int x;
    int y;
};

enum SNAKE_DIRECTION 
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum DIFFICULTY_LVL
{
    EASY,
    MEDIUM,
    HARD
};

enum CONTROL_SETTINGS
{
    CLASSIC,
    MULTI_DIRECTIONAL
};

enum MM_OPTION_SELECTED
{
    PLAY_GAME,
    DIFFICULTY,
    CONTROLS
};

// Screen/Window Variables
const int ScreenWidth = 120;
const int ScreenHeight = 30;

int total_screen_size = ScreenWidth * ScreenHeight;
wchar_t *screen;
HANDLE hConsole;
DWORD bytes_written;

// Global Game Variables
int games_played = 0;
int high_score = 0;

int foodX;
int foodY;
int score;
SNAKE_DIRECTION snake_direction;
bool is_dead;
bool game_is_paused;

MM_OPTION_SELECTED selection;
CONTROL_SETTINGS controls;
DIFFICULTY_LVL difficulty;

int game_speed;

// Controls
bool key_left;
bool key_right;
bool key_up;
bool key_down;
bool key_esc;
bool key_space;

bool key_left_old;
bool key_right_old;
bool key_up_old;
bool key_down_old;
bool key_esc_old;
bool key_space_old;

// Functions
void MainMenu();
void GetMainMenuInput(MM_OPTION_SELECTED& current_selection,
    CONTROL_SETTINGS& controls, DIFFICULTY_LVL& difficulty, bool& start_game);
MM_OPTION_SELECTED ChangeMainMenuOption(MM_OPTION_SELECTED opt, bool isDown);
void ChangeDifficulty(DIFFICULTY_LVL& difficulty);
void SetupGame(std::list<SnakeSegment>* snake);
void GameLoop();
void ClearScreen(wchar_t* screen);
void DrawUI(wchar_t* screen);
void DrawSnake(wchar_t* screen, std::list<SnakeSegment>* snake);
void DrawFood(wchar_t* screen);
void GetInput(wchar_t* screen);
SNAKE_DIRECTION ChangeDirectionRight(SNAKE_DIRECTION direction);
SNAKE_DIRECTION ChangeDirectionLeft(SNAKE_DIRECTION direction);
SNAKE_DIRECTION ChangeDirectionUp(SNAKE_DIRECTION direction);
SNAKE_DIRECTION ChangeDirectionDown(SNAKE_DIRECTION direction);
void ShowPausedScreen(wchar_t* screen);