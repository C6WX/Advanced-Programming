#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

/* =========================================================
   GALAGA STYLE GAME
   C99 + Raylib
   ========================================================= */

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_SPEED 400.0f
#define PLAYER_Y 540.0f

#define MAX_PLAYER_BULLETS 30
#define MAX_ENEMY_BULLETS 40
#define MAX_ENEMIES 24

#define PLAYER_BULLET_SPEED 650.0f
#define ENEMY_BULLET_SPEED 260.0f

#define FORMATION_X 400.0f
#define FORMATION_Y 130.0f

#define FORMATION_SPACING_X 70.0f
#define FORMATION_SPACING_Y 50.0f

#define ATTACK_INTERVAL_MIN 1.0f
#define ATTACK_INTERVAL_MAX 2.5f


/* =========================================================
   ENUMS
   ========================================================= */

typedef enum
{
    ENEMY_BEE,
    ENEMY_BUTTERFLY,
    ENEMY_BOSS
} EnemyType;


typedef enum
{
    ENEMY_ENTERING,
    ENEMY_FORMATION,
    ENEMY_ATTACKING,
    ENEMY_RETURNING
} EnemyState;


/* =========================================================
   STRUCTS
   ========================================================= */

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
} PlayerBullet;


typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
} EnemyBullet;


typedef struct
{
    Vector2 position;
    Vector2 formationPosition;

    Vector2 attackStart;
    Vector2 attackTarget;

    float attackTime;
    float attackDuration;
    float rotation;

    EnemyType type;
    EnemyState state;

    bool active;
} Enemy;


/* =========================================================
   HELPER FUNCTIONS
   ========================================================= */

static float DistanceBetween(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return sqrtf(dx * dx + dy * dy);
}


/* =========================================================
   PLAYER DRAWING
   ========================================================= */

static void DrawPlayer(Vector2 position)
{
    Vector2 top =
    {
        position.x,
        position.y - 25.0f
    };

    Vector2 left =
    {
        position.x - 30.0f,
        position.y + 20.0f
    };

    Vector2 right =
    {
        position.x + 30.0f,
        position.y + 20.0f
    };

    /* Main ship */

    DrawTriangle(
        top,
        left,
        right,
        BLUE
    );

    /* Wings */

    DrawRectangle(
        (int)position.x - 32,
        (int)position.y + 5,
        14,
        20,
        DARKBLUE
    );

    DrawRectangle(
        (int)position.x + 18,
        (int)position.y + 5,
        14,
        20,
        DARKBLUE
    );

    /* Cockpit */

    DrawCircle(
        (int)position.x,
        (int)position.y - 5,
        7,
        SKYBLUE
    );

    /* Engine */

    DrawTriangle(
        (Vector2)
        {
            position.x - 8,
            position.y + 18
        },

        (Vector2)
        {
            position.x,
            position.y + 35
        },

        (Vector2)
        {
            position.x + 8,
            position.y + 18
        },

        ORANGE
    );
}


/* =========================================================
   BEE ENEMY
   ========================================================= */

static void DrawBee(Vector2 position)
{
    /* Wings */

    DrawEllipse(
        (int)position.x - 18,
        (int)position.y,
        15,
        22,
        LIGHTGRAY
    );

    DrawEllipse(
        (int)position.x + 18,
        (int)position.y,
        15,
        22,
        LIGHTGRAY
    );

    /* Body */

    DrawEllipse(
        (int)position.x,
        (int)position.y,
        15,
        23,
        YELLOW
    );

    /* Black stripes */

    DrawRectangle(
        (int)position.x - 13,
        (int)position.y - 6,
        26,
        5,
        BLACK
    );

    DrawRectangle(
        (int)position.x - 11,
        (int)position.y + 6,
        22,
        5,
        BLACK
    );

    /* Eyes */

    DrawCircle(
        (int)position.x - 5,
        (int)position.y - 12,
        3,
        RED
    );

    DrawCircle(
        (int)position.x + 5,
        (int)position.y - 12,
        3,
        RED
    );
}


/* =========================================================
   BUTTERFLY ENEMY
   ========================================================= */

static void DrawButterfly(Vector2 position)
{
    /* Upper wings */

    DrawEllipse(
        (int)position.x - 19,
        (int)position.y - 8,
        17,
        24,
        MAGENTA
    );

    DrawEllipse(
        (int)position.x + 19,
        (int)position.y - 8,
        17,
        24,
        MAGENTA
    );

    /* Lower wings */

    DrawEllipse(
        (int)position.x - 17,
        (int)position.y + 12,
        13,
        17,
        PURPLE
    );

    DrawEllipse(
        (int)position.x + 17,
        (int)position.y + 12,
        13,
        17,
        PURPLE
    );

    /* Body */

    DrawEllipse(
        (int)position.x,
        (int)position.y,
        8,
        25,
        YELLOW
    );

    /* Eyes */

    DrawCircle(
        (int)position.x - 4,
        (int)position.y - 10,
        3,
        RED
    );

    DrawCircle(
        (int)position.x + 4,
        (int)position.y - 10,
        3,
        RED
    );
}


/* =========================================================
   BOSS ENEMY
   ========================================================= */

static void DrawBoss(Vector2 position)
{
    /* Large wings */

    DrawEllipse(
        (int)position.x - 27,
        (int)position.y,
        27,
        32,
        RED
    );

    DrawEllipse(
        (int)position.x + 27,
        (int)position.y,
        27,
        32,
        RED
    );

    /* Body */

    DrawEllipse(
        (int)position.x,
        (int)position.y,
        18,
        32,
        ORANGE
    );

    /* Head */

    DrawCircle(
        (int)position.x,
        (int)position.y - 18,
        15,
        YELLOW
    );

    /* Eyes */

    DrawCircle(
        (int)position.x - 6,
        (int)position.y - 20,
        4,
        RED
    );

    DrawCircle(
        (int)position.x + 6,
        (int)position.y - 20,
        4,
        RED
    );
}


/* =========================================================
   DRAW ENEMY
   ========================================================= */

static void DrawEnemy(Enemy *enemy)
{
    switch (enemy->type)
    {
        case ENEMY_BEE:
            DrawBee(enemy->position);
            break;

        case ENEMY_BUTTERFLY:
            DrawButterfly(enemy->position);
            break;

        case ENEMY_BOSS:
            DrawBoss(enemy->position);
            break;
    }
}


/* =========================================================
   CREATE GALAGA FORMATION
   ========================================================= */

static void CreateFormation(Enemy enemies[])
{
    int index = 0;

    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 6; column++)
        {
            if (index >= MAX_ENEMIES)
            {
                return;
            }

            float x =
                FORMATION_X +
                (column - 2.5f) *
                FORMATION_SPACING_X;

            float y =
                FORMATION_Y +
                row *
                FORMATION_SPACING_Y;

            enemies[index].formationPosition =
                (Vector2)
                {
                    x,
                    y
                };

            enemies[index].position =
                (Vector2)
                {
                    x,
                    -100.0f -
                    index * 25.0f
                };

            enemies[index].attackStart =
                enemies[index].position;

            enemies[index].attackTarget =
                (Vector2)
                {
                    x,
                    SCREEN_HEIGHT + 100.0f
                };

            enemies[index].attackTime = 0.0f;
            enemies[index].attackDuration = 0.0f;
            enemies[index].rotation = 0.0f;

            /*
                Top row = bosses
                Second row = butterflies
                Bottom rows = bees
            */

            if (row == 0)
            {
                enemies[index].type =
                    ENEMY_BOSS;
            }
            else if (row == 1)
            {
                enemies[index].type =
                    ENEMY_BUTTERFLY;
            }
            else
            {
                enemies[index].type =
                    ENEMY_BEE;
            }

            enemies[index].state =
                ENEMY_ENTERING;

            enemies[index].active = true;

            index++;
        }
    }
}


/* =========================================================
   FIRE PLAYER BULLET
   ========================================================= */

static void FirePlayerBullet(
    Vector2 playerPosition,
    PlayerBullet bullets[])
{
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].position =
                (Vector2)
                {
                    playerPosition.x,
                    playerPosition.y - 30.0f
                };

            bullets[i].velocity =
                (Vector2)
                {
                    0.0f,
                    -PLAYER_BULLET_SPEED
                };

            bullets[i].active = true;

            return;
        }
    }
}


/* =========================================================
   UPDATE PLAYER BULLETS
   ========================================================= */

static void UpdatePlayerBullets(
    PlayerBullet bullets[])
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            continue;
        }

        bullets[i].position.x +=
            bullets[i].velocity.x * dt;

        bullets[i].position.y +=
            bullets[i].velocity.y * dt;

        if (bullets[i].position.y < -20.0f)
        {
            bullets[i].active = false;
        }
    }
}


/* =========================================================
   FIRE ENEMY BULLET
   ========================================================= */

static void FireEnemyBullet(
    Vector2 enemyPosition,
    Vector2 playerPosition,
    EnemyBullet bullets[])
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            Vector2 direction =
            {
                playerPosition.x -
                enemyPosition.x,

                playerPosition.y -
                enemyPosition.y
            };

            float length =
                sqrtf(
                    direction.x * direction.x +
                    direction.y * direction.y
                );

            if (length > 0.0f)
            {
                direction.x /= length;
                direction.y /= length;
            }

            bullets[i].position =
                enemyPosition;

            bullets[i].velocity =
                (Vector2)
                {
                    direction.x *
                    ENEMY_BULLET_SPEED,

                    direction.y *
                    ENEMY_BULLET_SPEED
                };

            bullets[i].active = true;

            return;
        }
    }
}


/* =========================================================
   UPDATE ENEMY BULLETS
   ========================================================= */

static void UpdateEnemyBullets(
    EnemyBullet bullets[])
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            continue;
        }

        bullets[i].position.x +=
            bullets[i].velocity.x * dt;

        bullets[i].position.y +=
            bullets[i].velocity.y * dt;

        if (bullets[i].position.x < -30.0f ||
            bullets[i].position.x >
                SCREEN_WIDTH + 30.0f ||
            bullets[i].position.y < -30.0f ||
            bullets[i].position.y >
                SCREEN_HEIGHT + 30.0f)
        {
            bullets[i].active = false;
        }
    }
}


/* =========================================================
   START ENEMY ATTACK
   ========================================================= */

static void StartEnemyAttack(
    Enemy *enemy,
    Vector2 playerPosition)
{
    enemy->state =
        ENEMY_ATTACKING;

    enemy->attackStart =
        enemy->position;

    enemy->attackTarget =
        (Vector2)
        {
            playerPosition.x +
            (float)GetRandomValue(-120, 120),

            SCREEN_HEIGHT + 120.0f
        };

    enemy->attackTime = 0.0f;

    enemy->attackDuration =
        (float)GetRandomValue(15, 25) / 10.0f;

    enemy->rotation = 0.0f;
}


/* =========================================================
   UPDATE ENEMY
   ========================================================= */

static void UpdateEnemy(
    Enemy *enemy)
{
    float dt = GetFrameTime();

    if (!enemy->active)
    {
        return;
    }

    /* -------------------------
       Enter formation
       ------------------------- */

    if (enemy->state ==
        ENEMY_ENTERING)
    {
        Vector2 difference =
        {
            enemy->formationPosition.x -
            enemy->position.x,

            enemy->formationPosition.y -
            enemy->position.y
        };

        float distance =
            sqrtf(
                difference.x * difference.x +
                difference.y * difference.y
            );

        if (distance < 8.0f)
        {
            enemy->position =
                enemy->formationPosition;

            enemy->state =
                ENEMY_FORMATION;

            return;
        }

        if (distance > 0.0f)
        {
            float speed = 300.0f;

            enemy->position.x +=
                difference.x / distance *
                speed * dt;

            enemy->position.y +=
                difference.y / distance *
                speed * dt;
        }

        return;
    }

    /* -------------------------
       Formation
       ------------------------- */

    if (enemy->state ==
        ENEMY_FORMATION)
    {
        float movement =
            sinf(
                (float)GetTime() * 2.0f +
                enemy->formationPosition.x * 0.01f
            ) * 3.0f;

        enemy->position.x =
            enemy->formationPosition.x;

        enemy->position.y =
            enemy->formationPosition.y +
            movement;

        return;
    }

    /* -------------------------
       Attack
       ------------------------- */

    if (enemy->state ==
        ENEMY_ATTACKING)
    {
        enemy->attackTime += dt;

        float t =
            enemy->attackTime /
            enemy->attackDuration;

        if (t >= 1.0f)
        {
            t = 1.0f;

            enemy->state =
                ENEMY_RETURNING;
        }

        float curve =
            sinf(t * PI * 2.0f) *
            180.0f;

        enemy->position.x =
            enemy->attackStart.x +
            (enemy->attackTarget.x -
             enemy->attackStart.x) *
            t +
            curve;

        enemy->position.y =
            enemy->attackStart.y +
            (enemy->attackTarget.y -
             enemy->attackStart.y) *
            t;

        enemy->rotation =
            sinf(t * PI * 2.0f);

        return;
    }

    /* -------------------------
       Return to formation
       ------------------------- */

    if (enemy->state ==
        ENEMY_RETURNING)
    {
        Vector2 difference =
        {
            enemy->formationPosition.x -
            enemy->position.x,

            enemy->formationPosition.y -
            enemy->position.y
        };

        float distance =
            sqrtf(
                difference.x * difference.x +
                difference.y * difference.y
            );

        if (distance < 10.0f)
        {
            enemy->position =
                enemy->formationPosition;

            enemy->state =
                ENEMY_FORMATION;

            enemy->rotation = 0.0f;

            return;
        }

        if (distance > 0.0f)
        {
            float speed = 400.0f;

            enemy->position.x +=
                difference.x / distance *
                speed * dt;

            enemy->position.y +=
                difference.y / distance *
                speed * dt;
        }
    }
}


/* =========================================================
   RANDOM ENEMY ATTACK
   ========================================================= */

static void TryStartAttack(
    Enemy enemies[],
    Vector2 playerPosition,
    float *attackTimer)
{
    float dt = GetFrameTime();

    *attackTimer -= dt;

    if (*attackTimer > 0.0f)
    {
        return;
    }

    int available[MAX_ENEMIES];
    int count = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active &&
            enemies[i].state ==
                ENEMY_FORMATION)
        {
            available[count] = i;
            count++;
        }
    }

    if (count > 0)
    {
        int selected =
            available[
                GetRandomValue(
                    0,
                    count - 1
                )
            ];

        StartEnemyAttack(
            &enemies[selected],
            playerPosition
        );
    }

    *attackTimer =
        (float)GetRandomValue(
            10,
            25
        ) / 10.0f;
}


/* =========================================================
   ENEMY SHOOTING
   ========================================================= */

static void EnemyShoot(
    Enemy enemies[],
    Vector2 playerPosition,
    EnemyBullet bullets[])
{
    if (GetRandomValue(0, 35) != 0)
    {
        return;
    }

    int available[MAX_ENEMIES];
    int count = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            available[count] = i;
            count++;
        }
    }

    if (count == 0)
    {
        return;
    }

    int selected =
        available[
            GetRandomValue(
                0,
                count - 1
            )
        ];

    FireEnemyBullet(
        enemies[selected].position,
        playerPosition,
        bullets
    );
}


/* =========================================================
   PLAYER BULLET COLLISIONS
   ========================================================= */

static void CheckPlayerBullets(
    PlayerBullet bullets[],
    Enemy enemies[],
    int *score)
{
    for (int b = 0;
         b < MAX_PLAYER_BULLETS;
         b++)
    {
        if (!bullets[b].active)
        {
            continue;
        }

        for (int e = 0;
             e < MAX_ENEMIES;
             e++)
        {
            if (!enemies[e].active)
            {
                continue;
            }

            float hitRadius = 22.0f;

            if (enemies[e].type ==
                ENEMY_BOSS)
            {
                hitRadius = 30.0f;
            }

            if (DistanceBetween(
                    bullets[b].position,
                    enemies[e].position) <
                hitRadius)
            {
                bullets[b].active = false;

                enemies[e].active = false;

                if (enemies[e].type ==
                    ENEMY_BOSS)
                {
                    *score += 150;
                }
                else if (enemies[e].type ==
                         ENEMY_BUTTERFLY)
                {
                    *score += 100;
                }
                else
                {
                    *score += 80;
                }

                break;
            }
        }
    }
}


/* =========================================================
   PLAYER HIT BY BULLET
   ========================================================= */

static bool CheckPlayerHit(
    Vector2 playerPosition,
    EnemyBullet bullets[])
{
    for (int i = 0;
         i < MAX_ENEMY_BULLETS;
         i++)
    {
        if (!bullets[i].active)
        {
            continue;
        }

        if (DistanceBetween(
                playerPosition,
                bullets[i].position) <
            22.0f)
        {
            bullets[i].active = false;

            return true;
        }
    }

    return false;
}


/* =========================================================
   PLAYER HIT BY DIVING ENEMY
   ========================================================= */

static bool CheckEnemyCollision(
    Vector2 playerPosition,
    Enemy enemies[])
{
    for (int i = 0;
         i < MAX_ENEMIES;
         i++)
    {
        if (!enemies[i].active)
        {
            continue;
        }

        if (enemies[i].state !=
            ENEMY_ATTACKING)
        {
            continue;
        }

        if (DistanceBetween(
                playerPosition,
                enemies[i].position) <
            28.0f)
        {
            enemies[i].active = false;

            return true;
        }
    }

    return false;
}


/* =========================================================
   CHECK WAVE
   ========================================================= */

static bool WaveComplete(
    Enemy enemies[])
{
    for (int i = 0;
         i < MAX_ENEMIES;
         i++)
    {
        if (enemies[i].active)
        {
            return false;
        }
    }

    return true;
}


/* =========================================================
   BACKGROUND
   ========================================================= */

static void DrawStars(void)
{
    for (int i = 0; i < 100; i++)
    {
        int x =
            (i * 83) %
            SCREEN_WIDTH;

        int y =
            (i * 47) %
            SCREEN_HEIGHT;

        DrawPixel(
            x,
            y,
            LIGHTGRAY
        );
    }
}


/* =========================================================
   HUD
   ========================================================= */

static void DrawHUD(
    int score,
    int lives,
    int wave)
{
    DrawText(
        TextFormat(
            "SCORE %06d",
            score
        ),
        20,
        15,
        22,
        WHITE
    );

    DrawText(
        TextFormat(
            "WAVE %d",
            wave
        ),
        SCREEN_WIDTH / 2 - 40,
        15,
        22,
        WHITE
    );

    DrawText(
        TextFormat(
            "LIVES %d",
            lives
        ),
        SCREEN_WIDTH - 110,
        15,
        22,
        WHITE
    );
}


/* =========================================================
   DRAW GAME
   ========================================================= */

static void DrawGame(
    Vector2 playerPosition,
    PlayerBullet playerBullets[],
    Enemy enemies[],
    EnemyBullet enemyBullets[],
    int score,
    int lives,
    int wave,
    bool gameOver)
{
    BeginDrawing();

    ClearBackground(BLACK);

    DrawStars();

    /* Player */

    if (!gameOver)
    {
        DrawPlayer(
            playerPosition
        );
    }

    /* Player bullets */

    for (int i = 0;
         i < MAX_PLAYER_BULLETS;
         i++)
    {
        if (playerBullets[i].active)
        {
            DrawRectangle(
                (int)playerBullets[i].position.x - 2,
                (int)playerBullets[i].position.y - 8,
                4,
                16,
                WHITE
            );
        }
    }

    /* Enemies */

    for (int i = 0;
         i < MAX_ENEMIES;
         i++)
    {
        if (enemies[i].active)
        {
            DrawEnemy(
                &enemies[i]
            );
        }
    }

    /* Enemy bullets */

    for (int i = 0;
         i < MAX_ENEMY_BULLETS;
         i++)
    {
        if (enemyBullets[i].active)
        {
            DrawCircle(
                (int)enemyBullets[i].position.x,
                (int)enemyBullets[i].position.y,
                4,
                RED
            );
        }
    }

    /* HUD */

    DrawHUD(
        score,
        lives,
        wave
    );

    /* Game over */

    if (gameOver)
    {
        DrawRectangle(
            0,
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            Fade(
                BLACK,
                0.75f
            )
        );

        DrawText(
            "GAME OVER",
            SCREEN_WIDTH / 2 - 125,
            SCREEN_HEIGHT / 2 - 50,
            40,
            RED
        );

        DrawText(
            TextFormat(
                "SCORE %06d",
                score
            ),
            SCREEN_WIDTH / 2 - 75,
            SCREEN_HEIGHT / 2 + 10,
            22,
            WHITE
        );

        DrawText(
            "PRESS ENTER TO PLAY AGAIN",
            SCREEN_WIDTH / 2 - 150,
            SCREEN_HEIGHT / 2 + 55,
            18,
            WHITE
        );
    }

    EndDrawing();
}


/* =========================================================
   RESET GAME
   ========================================================= */

static void ResetGame(
    Enemy enemies[],
    PlayerBullet playerBullets[],
    EnemyBullet enemyBullets[],
    Vector2 *playerPosition,
    int *score,
    int *lives,
    int *wave,
    float *attackTimer,
    bool *gameOver)
{
    *playerPosition =
        (Vector2)
        {
            SCREEN_WIDTH / 2.0f,
            PLAYER_Y
        };

    *score = 0;

    *lives = 3;

    *wave = 1;

    *attackTimer = 2.0f;

    *gameOver = false;

    for (int i = 0;
         i < MAX_PLAYER_BULLETS;
         i++)
    {
        playerBullets[i].active = false;
    }

    for (int i = 0;
         i < MAX_ENEMY_BULLETS;
         i++)
    {
        enemyBullets[i].active = false;
    }

    CreateFormation(
        enemies
    );
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Galaga"
    );

    SetTargetFPS(60);

    Enemy enemies[MAX_ENEMIES];

    PlayerBullet playerBullets[
        MAX_PLAYER_BULLETS
    ];

    EnemyBullet enemyBullets[
        MAX_ENEMY_BULLETS
    ];

    Vector2 playerPosition;

    int score = 0;
    int lives = 3;
    int wave = 1;

    float attackTimer = 2.0f;

    bool gameOver = false;

    /* Start game */

    ResetGame(
        enemies,
        playerBullets,
        enemyBullets,
        &playerPosition,
        &score,
        &lives,
        &wave,
        &attackTimer,
        &gameOver
    );

    /* Main loop */

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        /* Restart */

        if (gameOver &&
            IsKeyPressed(KEY_ENTER))
        {
            ResetGame(
                enemies,
                playerBullets,
                enemyBullets,
                &playerPosition,
                &score,
                &lives,
                &wave,
                &attackTimer,
                &gameOver
            );
        }

        if (!gameOver)
        {
            /* -------------------------
               PLAYER MOVEMENT
               ------------------------- */

            if (IsKeyDown(KEY_A) ||
                IsKeyDown(KEY_LEFT))
            {
                playerPosition.x -=
                    PLAYER_SPEED * dt;
            }

            if (IsKeyDown(KEY_D) ||
                IsKeyDown(KEY_RIGHT))
            {
                playerPosition.x +=
                    PLAYER_SPEED * dt;
            }

            /* Keep player on screen */

            if (playerPosition.x < 30.0f)
            {
                playerPosition.x = 30.0f;
            }

            if (playerPosition.x >
                SCREEN_WIDTH - 30.0f)
            {
                playerPosition.x =
                    SCREEN_WIDTH - 30.0f;
            }

            /* -------------------------
               SHOOTING
               ------------------------- */

            if (IsKeyPressed(KEY_SPACE))
            {
                FirePlayerBullet(
                    playerPosition,
                    playerBullets
                );
            }

            /* -------------------------
               BULLETS
               ------------------------- */

            UpdatePlayerBullets(
                playerBullets
            );

            UpdateEnemyBullets(
                enemyBullets
            );

            /* -------------------------
               ENEMIES
               ------------------------- */

            for (int i = 0;
                 i < MAX_ENEMIES;
                 i++)
            {
                UpdateEnemy(
                    &enemies[i]
                );
            }

            /* -------------------------
               ATTACKS
               ------------------------- */

            TryStartAttack(
                enemies,
                playerPosition,
                &attackTimer
            );

            /* -------------------------
               ENEMY SHOOTING
               ------------------------- */

            EnemyShoot(
                enemies,
                playerPosition,
                enemyBullets
            );

            /* -------------------------
               COLLISIONS
               ------------------------- */

            CheckPlayerBullets(
                playerBullets,
                enemies,
                &score
            );

            if (CheckPlayerHit(
                    playerPosition,
                    enemyBullets))
            {
                lives--;

                playerPosition.x =
                    SCREEN_WIDTH / 2.0f;

                if (lives <= 0)
                {
                    gameOver = true;
                }
            }

            if (CheckEnemyCollision(
                    playerPosition,
                    enemies))
            {
                lives--;

                playerPosition.x =
                    SCREEN_WIDTH / 2.0f;

                if (lives <= 0)
                {
                    gameOver = true;
                }
            }

            /* -------------------------
               NEW WAVE
               ------------------------- */

            if (WaveComplete(enemies))
            {
                wave++;

                CreateFormation(
                    enemies
                );

                attackTimer = 2.0f;
            }
        }

        /* Draw */

        DrawGame(
            playerPosition,
            playerBullets,
            enemies,
            enemyBullets,
            score,
            lives,
            wave,
            gameOver
        );
    }

    CloseWindow();

    return 0;
}