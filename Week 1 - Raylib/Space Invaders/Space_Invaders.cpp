// 1. Windows networking and threading headers MUST come before Raylib (Desktop only)
#ifndef PLATFORM_WEB
#define WIN32_LEAN_AND_MEAN
#define NOGDI             // Prevents Rectangle macro collision
#define NOUSER            // Prevents CloseWindow / ShowCursor collisions
#include <windows.h>
#include <winhttp.h>
#include <process.h>      // For background threading (_beginthread)
#endif

// 2. Include Raylib and C runtime headers
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Defines the bullet array to track the active bullets on screen
#define MAX_BULLETS 5
typedef struct {
    Vector2 position;
    bool active;
} Bullet;

#define MAX_ENEMIES 40
typedef struct {
    Vector2 position;
    Vector2 size;
    bool active;
} Enemy;

// Shared global weather state between API background thread and game loop
typedef struct {
    float apiWindSpeed;    // Baseline wind speed fetched from API (km/h)
    float currentDrift;    // Active smoothed pixel drift per frame
    bool isFetching;       // Prevents launching multiple network threads
} LiveWeather;

static LiveWeather g_Weather = { 12.0f, 0.8f, false };

// Background thread function that queries Open-Meteo without blocking game frames
void FetchWeatherThread(void *param)
{
    (void)param; // Suppress -Wunused-parameter warning

#ifndef PLATFORM_WEB
    g_Weather.isFetching = true;

    // Short string chunks concatenated to avoid line-length truncation
    char pathA[256] = {0};
    strcat(pathA, "/v1/forecast?");
    strcat(pathA, "latitude=51.5074&");
    strcat(pathA, "longitude=-0.1278&");
    strcat(pathA, "current_weather=true");

    wchar_t pathW[256] = {0};
    mbstowcs(pathW, pathA, strlen(pathA));

    HINTERNET hSession = WinHttpOpen(L"SpaceInvaders/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession)
    {
        HINTERNET hConnect = WinHttpConnect(hSession, L"api.open-meteo.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (hConnect)
        {
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", pathW, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            if (hRequest)
            {
                if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WinHttpReceiveResponse(hRequest, NULL))
                {
                    char buffer[4096] = {0};
                    DWORD bytesRead = 0;
                    WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);

                    // Strictly parse "windspeed":XX.X inside current_weather object
                    char *currentWeatherPtr = strstr(buffer, "\"current_weather\"");
                    if (!currentWeatherPtr) currentWeatherPtr = buffer;

                    char *windPtr = strstr(currentWeatherPtr, "\"windspeed\":");
                    if (!windPtr) windPtr = strstr(currentWeatherPtr, "\"wind_speed\":");

                    if (windPtr)
                    {
                        windPtr += strlen("\"windspeed\":");
                        while (*windPtr && (*windPtr < '0' || *windPtr > '9') && *windPtr != '-') windPtr++;

                        float liveWind = (float)atof(windPtr);

                        if (liveWind > 0.0f && liveWind < 150.0f)
                        {
                            g_Weather.apiWindSpeed = liveWind;
                        }
                    }
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }

    g_Weather.isFetching = false;
#endif
}

int main(void)
{
#ifndef PLATFORM_WEB
    // Fetch initial API data on background thread right at boot (Desktop only)
    _beginthread(FetchWeatherThread, 0, NULL);
#endif

    // Creates the window size and title
    InitWindow(800, 600, "Space_Invaders");

    // Sets the target FPS
    SetTargetFPS(60);

    // Audio System Setup
    InitAudioDevice();
    Sound shootSound = LoadSound("Laser.wav");
    Sound gameOverSound = LoadSound("GameOver.wav");
    Sound explosionSound = LoadSound("Explosion.wav");

    // Background Music Streaming Setup
    Music backgroundMusic = LoadMusicStream("BackgroundMusic.wav");
    SetMusicVolume(backgroundMusic, 0.4f); // Set comfortable background volume (40%)
    PlayMusicStream(backgroundMusic);      // Start music stream

    // Game loop control flag
    bool keepRunning = true;

    // Game state variables
    bool gameOver = false;
    int roundNum = 1; // Round counter

    // Score
    int Score = 0;
    int fontSize = 20;

    // Sets the ship's height and width
    float shipHeight = 30.0f;
    float shipWidth = 40.0f;

    // The central position of the player
    Vector2 playerPos = {400.0f, 500.0f};

    // Set the player speed
    float speed = 5.0f;

    // Initialize bullet array and speed
    Bullet bullets[MAX_BULLETS] = {};
    float bulletSpeed = 7.0f;

    // Grid layout
    int columns = 8;
    int rows = 5;

    // Enemy Spawns and Spacing
    int startX = 100;
    int startY = 50;
    int spacingX = 60;
    int spacingY = 40;

    // Enemy position, speed and size
    Enemy enemies[MAX_ENEMIES] = {};
    int enemySpeed = 2; // positive moves right and negative moves left
    float dropDistance = 15.0f; // the amount the enemy drops after hitting a wall
    Vector2 enemySize = {30.0f, 20.0f};

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            int index = (row * columns) + col;
            enemies[index].position.x = startX + (col * spacingX);
            enemies[index].position.y = startY + (row * spacingY);
            enemies[index].size = enemySize;
            enemies[index].active = true;
        }
    }

    // Timers for live real-time weather updates
#ifndef PLATFORM_WEB
    float apiFetchTimer = 0.0f;
#endif
    float liveGustTimer = 0.0f;

    // Main game loop running on boolean flag
    while (keepRunning)
    {
        // ESSENTIAL: Update music stream buffer every single frame
        UpdateMusicStream(backgroundMusic);

        // Exit loop if user clicks the X button or presses ESC
        if (WindowShouldClose())
        {
            keepRunning = false;
        }

        float deltaTime = GetFrameTime();
#ifndef PLATFORM_WEB
        apiFetchTimer += deltaTime;
#endif
        liveGustTimer += deltaTime;

#ifndef PLATFORM_WEB
        // 1. Refresh live weather data from API every 30 seconds asynchronously (Desktop only)
        if (apiFetchTimer >= 30.0f && !g_Weather.isFetching)
        {
            apiFetchTimer = 0.0f;
            _beginthread(FetchWeatherThread, 0, NULL);
        }
#endif

        // 2. DYNAMIC REAL-TIME WIND TURBULENCE:
        float baseDrift = g_Weather.apiWindSpeed * 0.12f;
        float liveGust = sinf(liveGustTimer * 2.0f) * 0.8f;

        g_Weather.currentDrift = baseDrift + liveGust;

        // HARD PLAYABILITY CLAMP: Keep drift noticeably visible (-2.2px to +2.2px)
        if (g_Weather.currentDrift > 2.2f) g_Weather.currentDrift = 2.2f;
        if (g_Weather.currentDrift < -2.2f) g_Weather.currentDrift = -2.2f;

        // Triangle points declared at outer scope so drawing can access them
        Vector2 point1 = {playerPos.x, playerPos.y - (shipHeight / 2)};
        Vector2 point2 = {playerPos.x - (shipWidth / 2), playerPos.y + (shipHeight / 2)};
        Vector2 point3 = {playerPos.x + (shipWidth / 2), playerPos.y + (shipHeight / 2)};

        if (!gameOver)
        {
            // PLAYER MOVEMENT
            // If Left arrow or A is pressed, decrease the player's position by their speed
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                playerPos.x -= speed;
            }
            // If Right arrow or D is pressed, increase the player's position by their speed
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                playerPos.x += speed;
            }

            /* // DEBUG CHEAT: Press T to wipe out all enemies instantly for testing
            if (IsKeyPressed(KEY_T))
            {
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    enemies[i].active = false;
                }
            } */

            // KEEP PLAYER ON SCREEN
            if (playerPos.x - (shipWidth / 2) < 0)
            {
                playerPos.x = shipWidth / 2;
            }
            if (playerPos.x + (shipWidth / 2) > 800)
            {
                playerPos.x = 800 - (shipWidth / 2);
            }

            // RECALCULATE TRIANGLE POINTS
            point1 = (Vector2){playerPos.x, playerPos.y - (shipHeight / 2)};
            point2 = (Vector2){playerPos.x - (shipWidth / 2), playerPos.y + (shipHeight / 2)};
            point3 = (Vector2){playerPos.x + (shipWidth / 2), playerPos.y + (shipHeight / 2)};

            // SHOOT BULLETS
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
            {
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullets[i].active)
                    {
                        // sets the bullet spawn to the top of the triangle
                        bullets[i].position = point1;
                        bullets[i].active = true;

                        // Play shooting sound effect
                        PlaySound(shootSound);
                        break;
                    }
                }
            }

            // UPDATE ACTIVE BULLETS
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    // moves the bullet upwards based off the bullet speed variable
                    bullets[i].position.y -= bulletSpeed;

                    // Apply dynamically changing real-time wind drift to active bullets
                    bullets[i].position.x += g_Weather.currentDrift;

                    // if the bullet goes off screen, it is deactivated
                    if (bullets[i].position.y < 0 || bullets[i].position.x < 0 || bullets[i].position.x > 800)
                    {
                        bullets[i].active = false;
                    }

                    // Kill enemies when a bullet collides with them
                    for (int j = 0; j < MAX_ENEMIES; j++)
                    {
                        if (enemies[j].active)
                        {
                            Rectangle enemyRec = { enemies[j].position.x, enemies[j].position.y, enemies[j].size.x, enemies[j].size.y };

                            if (CheckCollisionCircleRec(bullets[i].position, 4.0f, enemyRec))
                            {
                                bullets[i].active = false;
                                enemies[j].active = false;
                                Score = Score + 100;

                                // Play explosion SFX
                                PlaySound(explosionSound);
                                break;
                            }
                        }
                    }
                }
            }

            // CHECK IF ALL ENEMIES ARE DEFEATED
            int activeEnemyCount = 0;
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    activeEnemyCount++;
                }
            }

            // RESPAWN ENEMY WAVE IF NO ACTIVE ENEMIES REMAIN
            if (activeEnemyCount == 0)
            {
                roundNum++; // Increment wave counter

                // Keep movement direction intact while increasing magnitude
                if (enemySpeed > 0) enemySpeed++;
                else enemySpeed--;

                for (int row = 0; row < rows; row++)
                {
                    for (int col = 0; col < columns; col++)
                    {
                        int index = (row * columns) + col;
                        enemies[index].position.x = startX + (col * spacingX);
                        enemies[index].position.y = startY + (row * spacingY);
                        enemies[index].size = enemySize;
                        enemies[index].active = true;
                    }
                }
            }

            // Collisions
            bool hitWall = false;

            // Spawn enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    // move the enemy right
                    enemies[i].position.x += enemySpeed;

                    if (enemies[i].position.x <= 0 || (enemies[i].position.x + enemies[i].size.x) >= 800)
                    {
                        hitWall = true;
                    }

                    // Check if an enemy touches the player's ship
                    if (enemies[i].position.y + enemies[i].size.y >= playerPos.y - (shipHeight / 2))
                    {
                        if (!gameOver)
                        {
                            gameOver = true;
                            // Play game over SFX & stop background music
                            PlaySound(gameOverSound);
                            StopMusicStream(backgroundMusic);
                        }
                    }
                }
            }

            // If an Enemy hits the border, flip direction and shift everyone down
            if (hitWall)
            {
                enemySpeed = -enemySpeed;

                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemies[i].active)
                    {
                        enemies[i].position.y += dropDistance;
                    }
                }
            }
        }
        else
        {
            // GAME OVER STATE: PRESS ENTER / SPACE TO RESTART
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                // Reset Player
                playerPos = (Vector2){400.0f, 500.0f};

                // Clear Bullets
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;

                // Reset Enemies
                roundNum = 1;
                enemySpeed = 2;
                for (int row = 0; row < rows; row++)
                {
                    for (int col = 0; col < columns; col++)
                    {
                        int index = (row * columns) + col;
                        enemies[index].position.x = startX + (col * spacingX);
                        enemies[index].position.y = startY + (row * spacingY);
                        enemies[index].size = enemySize;
                        enemies[index].active = true;
                    }
                }

                // Reset Score and Game State
                Score = 0;
                gameOver = false;

                // Restart Music Stream
                PlayMusicStream(backgroundMusic);
            }
        }

        // Starts drawing the current frame
        BeginDrawing();

        // Clears the previous frame and gives a black background
        ClearBackground((Color){ 15, 20, 35, 255 });

        if (!gameOver)
        {
            // Draw player ship
            // Creates a triangle using the points provided above
            DrawTriangle(point1, point2, point3, BLUE);

            // Draw LIVE Real-Time Changing Weather HUD (Top Left)
            DrawText(TextFormat("API BASE WIND: %.1f km/h", g_Weather.apiWindSpeed), 20, 20, 16, SKYBLUE);
            DrawText(TextFormat("LIVE WIND DRIFT: %+.2f px/f", g_Weather.currentDrift), 20, 40, 16, YELLOW);

            // Draw Centered Round HUD Header
            const char* roundText = TextFormat("ROUND %d", roundNum);
            int roundWidth = MeasureText(roundText, 22);
            DrawText(roundText, (800 / 2) - (roundWidth / 2), 20, 22, GREEN);

            // Calculates the text's width in pixels
            const char* scoreText = TextFormat("SCORE: %d", Score);
            int textWidth = MeasureText(scoreText, fontSize);

            // Changes the integer to string so that it can be used with DrawText and then displays the score at the set location
            DrawText(scoreText, 800 - textWidth - 20, 20, fontSize, WHITE);

            // Draw Enemy Speed HUD (Bottom Left)
            DrawText(TextFormat("ENEMY SPEED: %d px/f", abs(enemySpeed)), 20, 560, 16, RED);

            // Draw active bullets
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    DrawCircleV(bullets[i].position, 4.0f, YELLOW);
                }
            }

            // Draw the enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    DrawRectangleV(enemies[i].position, enemies[i].size, RED);
                }
            }
        }
        else
        {
            // Display GAME OVER heading centered
            const char* gameOverText = "GAME OVER!";
            int gameOverWidth = MeasureText(gameOverText, 40);
            DrawText(gameOverText, (800 / 2) - (gameOverWidth / 2), 220, 40, RED);

            // Display FINAL SCORE directly below GAME OVER
            const char* finalScoreText = TextFormat("FINAL SCORE: %d", Score);
            int finalScoreWidth = MeasureText(finalScoreText, 24);
            DrawText(finalScoreText, (800 / 2) - (finalScoreWidth / 2), 275, 24, WHITE);

            // Display PRESS ENTER TO RESTART prompt
            const char* restartText = "PRESS ENTER TO RESTART";
            int restartWidth = MeasureText(restartText, 20);
            DrawText(restartText, (800 / 2) - (restartWidth / 2), 335, 20, YELLOW);
        }

        // Finishes the frame and displays it
        EndDrawing();
    }

    // Unload Sound & Music Streams
    UnloadSound(shootSound);
    UnloadSound(explosionSound);
    UnloadSound(gameOverSound);
    UnloadMusicStream(backgroundMusic);

    // Close Audio Engine
    CloseAudioDevice();

    // Close Raylib when the player exits
    CloseWindow();

    return 0;
}