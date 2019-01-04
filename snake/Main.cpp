#include "Main.h"

int main()
{
    screen = new wchar_t[total_screen_size];
    ClearScreen(screen);

     hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    SetConsoleActiveScreenBuffer(hConsole);
    bytes_written = 0;

    MainMenu();
    GameLoop();

    return 0;
}

void MainMenu()
{
    MM_OPTION_SELECTED current_selection = PLAY_GAME;
    CONTROL_SETTINGS current_controls = CLASSIC;
    DIFFICULTY_LVL current_difficulty = EASY;
    bool start_game = false;

    static const wchar_t* difficulty_string[] = { L"  EASY  ", L" MEDIUM ", L"  HARD  " };
    static const wchar_t* description[] =
    { L"                        Start the game                               ",
      L"            The difficulty changes the speed of the game.            ",
      L"    Classic : Only Left + Right Arrows -- Non Classic: All Arrows    "
    };

    while (!start_game) {
        wsprintf(&screen[10 * ScreenWidth + 45],
            L"    S N A K E");
        wsprintf(&screen[15 * ScreenWidth + 45],
            L"  %s PLAY GAME", 
            current_selection == PLAY_GAME ? L">" : L" ");
        wsprintf(&screen[16 * ScreenWidth + 45],
            L"  %s DIFFICULTY [%s]", 
            current_selection == DIFFICULTY ? L">" : L" ",
            difficulty_string[current_difficulty]);
        wsprintf(&screen[17 * ScreenWidth + 45],
            L"  %s CLASSIC CONTROLS [%s]", 
            current_selection == CONTROLS ? L">" : L" ",
            current_controls == CLASSIC ? L" ON  " : L" OFF ");
        wsprintf(&screen[22 * ScreenWidth + 25],
            L"%s", description[current_selection]);
        wsprintf(&screen[25 * ScreenWidth + 40],
            L"PRESS 'SPACE' TO MAKE A SELECTION");
        WriteConsoleOutputCharacter(hConsole, screen,
            total_screen_size, { 0, 0 }, &bytes_written);

        GetMainMenuInput(current_selection, current_controls, current_difficulty, start_game);
    }

    selection = current_selection;
    controls = current_controls;
    difficulty = current_difficulty;
}

void GetMainMenuInput(MM_OPTION_SELECTED& current_selection, CONTROL_SETTINGS& controls,
    DIFFICULTY_LVL& difficulty, bool& start_game)
{
    key_up = GetAsyncKeyState(VK_UP) != 0;
    key_down = GetAsyncKeyState(VK_DOWN) != 0;
    key_space = GetAsyncKeyState(VK_SPACE) != 0;

    if (key_up && !key_up_old)
    {
        current_selection = ChangeMainMenuOption(current_selection, false);
    }
    else if (key_down && !key_down_old)
    {
        current_selection = ChangeMainMenuOption(current_selection, true);
    }
    else if (key_space && !key_space_old)
    {
        if (current_selection == PLAY_GAME)
        {
            start_game = true;
        }
        else if (current_selection == DIFFICULTY)
        {
            ChangeDifficulty(difficulty);
        }
        else if (current_selection == CONTROLS)
        {
            if (controls == CLASSIC)
            {
                controls = MULTI_DIRECTIONAL;
            }
            else
            {
                controls = CLASSIC;
            }
        }
    }

    key_up_old = key_up;
    key_down_old = key_down;
    key_space_old = key_space;
}

MM_OPTION_SELECTED ChangeMainMenuOption(MM_OPTION_SELECTED opt, bool isDown)
{
    if (isDown) {
        switch (opt)
        {
        case PLAY_GAME:
            return DIFFICULTY;
        case DIFFICULTY:
            return CONTROLS;
        case CONTROLS:
            return PLAY_GAME;
        }
    }
    else 
    {
        switch (opt)
        {
        case PLAY_GAME:
            return CONTROLS;
        case CONTROLS:
            return DIFFICULTY;
        case DIFFICULTY:
            return PLAY_GAME;
        }
    }

    return opt;
}

void ChangeDifficulty(DIFFICULTY_LVL& difficulty)
{
    switch (difficulty)
    {
    case EASY:
        difficulty = MEDIUM;
        break;
    case MEDIUM:
        difficulty = HARD;
        break;
    case HARD:
        difficulty = EASY;
        break;
    }
}

void SetupGame(std::list<SnakeSegment>* snake)
{
    foodX = 30;
    foodY = 15;
    score = 0;
    snake_direction = LEFT;
    is_dead = false;
    game_is_paused = false;

    snake->push_back({ 60, 15 });
    snake->push_back({ 61, 15 });
    snake->push_back({ 62, 15 });
    snake->push_back({ 63, 15 });
    snake->push_back({ 64, 15 });
    snake->push_back({ 65, 15 });
    snake->push_back({ 66, 15 });
    snake->push_back({ 67, 15 });
    snake->push_back({ 68, 15 });
    snake->push_back({ 69, 15 });
    snake->push_back({ 70, 15 });
    snake->push_back({ 71, 15 });

    key_left = false; 
    key_right = false;
    key_left_old = false;
    key_right_old = false;
    key_esc = false;
    key_esc_old = false;

    switch (difficulty)
    {
    case EASY:
        game_speed = 300;
        break;
    case MEDIUM:
        game_speed = 200;
        break;
    case HARD:
        game_speed = 100;
        break;
    }
}

void GameLoop() {
    while (1)
    {
        std::list<SnakeSegment>* snake = new std::list<SnakeSegment>();

        SetupGame(snake);
        while (!is_dead)
        {

            // Timing and Input
            auto t1 = std::chrono::system_clock::now();
            while ((std::chrono::system_clock::now() - t1) <
                std::chrono::milliseconds(game_speed))
            {
                GetInput(screen);
            }

            if (game_is_paused) 
            {
                ShowPausedScreen(screen);
                continue;
            }

            // Game Logic
            switch (snake_direction)
            {
            case UP:
                snake->push_front({ snake->front().x, snake->front().y - 1 });
                break;
            case RIGHT:
                snake->push_front({ snake->front().x + 1, snake->front().y });
                break;
            case DOWN:
                snake->push_front({ snake->front().x, snake->front().y + 1 });
                break;
            case LEFT:
                snake->push_front({ snake->front().x - 1, snake->front().y });
                break;
            }

            // Map Collision Detection
            if (snake->front().x < 0 || snake->front().x >= ScreenWidth)
            {
                is_dead = true;
            }

            if (snake->front().y < 3 || snake->front().y >= ScreenHeight)
            {
                is_dead = true;
            }

            // Food Detection
            if (snake->front().x == foodX && snake->front().y == foodY)
            {
                score++;
                while (screen[foodY * ScreenWidth + foodX] != L' ')
                {
                    foodX = rand() % ScreenWidth;
                    foodY = (rand() % (ScreenHeight - 3)) + 3;
                }

                for (int i = 0; i < 5; i++)
                {
                    snake->push_back({ snake->back().x, snake->back().y });
                }
            }

            // Snake Collision Detection
            for (std::list<SnakeSegment>::iterator i = snake->begin();
                i != snake->end();
                i++)
            {
                if (i != snake->begin() &&
                    i->x == snake->front().x &&
                    i->y == snake->front().y)
                {
                    is_dead = true;
                }
            }

            snake->pop_back();

            // Display
            ClearScreen(screen);
            DrawUI(screen);
            DrawSnake(screen, snake);
            DrawFood(screen);

            if (is_dead)
            {
                wsprintf(&screen[15 * ScreenWidth + 40],
                    L"  PRESS 'SPACE' TO PLAY AGAIN");
            }

            // Display the current frame
            WriteConsoleOutputCharacter(hConsole, screen,
                total_screen_size, { 0, 0 }, &bytes_written);
        }

        while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
        games_played++;
        if (score > high_score)
        {
            high_score = score;
        }

        delete snake;
    }
}

void ClearScreen(wchar_t* screen) 
{
    for (int i = 0; i < total_screen_size; i++) 
    {
        screen[i] = L' ';
    }
}

void DrawUI(wchar_t* screen) 
{
    for (int i = 0; i < ScreenWidth; i++) 
    {
        screen[i] = L'=';
        screen[2 * ScreenWidth + i] = L'=';
    }
    #ifdef DEBUG
        wsprintf(&screen[ScreenWidth + 5], 
            L"SNAKE -- SCORE: %d    GAMES PLAYED: %d [DEBUG {RIGHTKEY: %s} {LEFTKEY: %s} {OLDRIGHTKEY: %s} {OLDLEFTKEY: %s}]", 
            score, gamesPlayed, keyRight ? L"YES" : L"NO ", keyLeft ? L"YES" : L"NO ",
            keyRightOld ? L"YES" : L"NO ", keyLeftOld ? L"YES" : L"NO ");
    #else
        wsprintf(&screen[ScreenWidth + 5], 
            L"SNAKE                    SCORE: %d                    GAMES PLAYED: %d                    HIGH SCORE: %d", 
            score, games_played, high_score);
    #endif
}

void DrawSnake(wchar_t* screen, std::list<SnakeSegment>* snake)
{
    // Snake body
    for (SnakeSegment s : *snake)
    {
        screen[s.y * ScreenWidth + s.x] = is_dead ? L'+' : L'\x2588';
    }

    // Snake head
    int head = snake->front().y * ScreenWidth + snake->front().x;
    
    // Check to see if the head is in bounds so we're not
    // trying to write to memory that isn't allocated to us
    if (head < total_screen_size) {
        *(screen + head) = is_dead ? L'X' : L'\x2593';
    }
}

void DrawFood(wchar_t* screen)
{
    screen[foodY * ScreenWidth + foodX] = L'*';
}

void GetInput(wchar_t* screen)
{
    if (controls == CLASSIC) {
        key_right = GetAsyncKeyState(VK_RIGHT) != 0;
        key_left = GetAsyncKeyState(VK_LEFT) != 0;
        key_esc = GetAsyncKeyState(VK_ESCAPE) != 0;

        if (key_esc && !key_esc_old)
        {
            game_is_paused = !game_is_paused;
        }

        if (key_right && !key_right_old)
        {
            snake_direction = ChangeDirectionRight(snake_direction);
        }
        else if (key_left && !key_left_old)
        {
            snake_direction = ChangeDirectionLeft(snake_direction);
        }

        key_right_old = key_right;
        key_left_old = key_left;
        key_esc_old = key_esc;
    }
    else
    {
        key_up = GetAsyncKeyState(VK_UP) != 0;
        key_down = GetAsyncKeyState(VK_DOWN) != 0;
        key_right = GetAsyncKeyState(VK_RIGHT) != 0;
        key_left = GetAsyncKeyState(VK_LEFT) != 0;
        key_esc = GetAsyncKeyState(VK_ESCAPE) != 0;

        if (key_esc && !key_esc_old)
        {
            game_is_paused = !game_is_paused;
        }

        // ------------------------------------------------------
        // Make sure that the new direction is not the opposite
        // of the old direction; otherwise the player will
        // just kill themselves.
        // ------------------------------------------------------

        if (key_right && !key_right_old)
        {
            if (snake_direction != LEFT)
            {
                snake_direction = RIGHT;
            }
        }
        else if (key_left && !key_left_old)
        {
            if (snake_direction != RIGHT)
            {
                snake_direction = LEFT;
            }
        }
        else if (key_up && !key_up_old)
        {
            if (snake_direction != DOWN)
            {
                snake_direction = UP;
            }
        }
        else if (key_down && !key_down_old)
        {
            if (snake_direction != UP)
            {
                snake_direction = DOWN;
            }
        }

        key_up_old = key_up;
        key_down_old = key_down;
        key_right_old = key_right;
        key_left_old = key_left;
        key_esc_old = key_esc;
    }
}

SNAKE_DIRECTION ChangeDirectionRight(SNAKE_DIRECTION direction)
{
    switch (direction) 
    {
    case UP:
        return RIGHT;
    case RIGHT:
        return DOWN;
    case DOWN:
        return LEFT;
    case LEFT:
        return UP;
    default:
        return direction;
    }
}

SNAKE_DIRECTION ChangeDirectionLeft(SNAKE_DIRECTION direction)
{
    switch (direction) 
    {
    case UP:
        return LEFT;
    case LEFT:
        return DOWN;
    case DOWN:
        return RIGHT;
    case RIGHT:
        return UP;
    default:
        return direction;
    }
}

SNAKE_DIRECTION ChangeDirectionUp(SNAKE_DIRECTION direction)
{
    switch (direction)
    {
    case UP:
        return LEFT;
    case LEFT:
        return DOWN;
    case DOWN:
        return RIGHT;
    case RIGHT:
        return UP;
    default:
        return direction;
    }
}

SNAKE_DIRECTION ChangeDirectionDown(SNAKE_DIRECTION direction)
{
    switch (direction)
    {
    case UP:
        return LEFT;
    case LEFT:
        return DOWN;
    case DOWN:
        return RIGHT;
    case RIGHT:
        return UP;
    default:
        return direction;
    }
}

void ShowPausedScreen(wchar_t* screen)
{
    ClearScreen(screen);
    wsprintf(&screen[ScreenWidth + 5], 
            L"SNAKE                    SCORE: %d                    GAMES PLAYED: %d                    HIGH SCORE: %d", 
            score, games_played, high_score);
    wsprintf(&screen[15 * ScreenWidth + 50],
        L" <-- PAUSED --> ");
    WriteConsoleOutputCharacter(hConsole, screen,
        total_screen_size, { 0, 0 }, &bytes_written);
}