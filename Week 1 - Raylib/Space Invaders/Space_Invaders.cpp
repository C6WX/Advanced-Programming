#include "raylib.h"

//Defines the bullet array to track the active bullets on screen
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

int main (void)
{
    // Creates the window size and title
    InitWindow(800, 600, "Space_Invaders");
    
    // Sets the target FPS
    SetTargetFPS(60);
    
    // Game over variable
    bool gameOver = false;
    
    //Score
    int Score = 0;
    
    // Sets the ship's height and width
    float shipHeight = 30.0f;
    float shipWidth = 40.0f;
    
    // The central position of the player
    Vector2 playerPos = {400.0f, 500.0f};
          
    // Set the player speed
    float speed = 5.0f;        
    
    // Initialize bullet array and speed
    Bullet bullets[MAX_BULLETS] = {0};
    float bulletSpeed = 7.0f;
            
    //Grid layout
    int columns = 8;
    int rows = 5;
    //Enemy Spawns and Spacing
    int startX = 100;
    int startY = 50;
    int spacingX = 60;
    int spacingY = 40;
    
    //Enemy position, speed and size
    Enemy enemies[MAX_ENEMIES] = {0};
    float enemySpeed = 2.0f; // positive moves right and negative moves left
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

    // Create the game loop
    while (!WindowShouldClose())
    {  
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
                        //sets the bullet spawn to the top of the triangle
                        bullets[i].position = point1;
                        bullets[i].active = true;
                        break;
                    }
                }
            }

            // UPDATE ACTIVE BULLETS
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    //moves the bullet upwards based off the bullet speed variable
                    bullets[i].position.y -= bulletSpeed;
                    
                    //if the bullet goes off screen, it is deactivated
                    if (bullets[i].position.y < 0)
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
                                Score = score + 100;
                                break;
                            }
                        }
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
                        gameOver = true;
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
       
        // Starts drawing the current frame      
        BeginDrawing();

        // Clears the previous frame and gives a black background
        ClearBackground(BLACK);
                  
        if (!gameOver)
        {
            // Draw player ship
            // Creates a triangle using the points provided above
            DrawTriangle(point1, point2, point3, BLUE);
            
            // Draw active bullets
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    DrawCircleV(bullets[i].position, 4.0f, YELLOW);
                }
            }
            
            //Draw the enemies
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
            DrawText("GAME OVER", 280, 260, 40, RED);
        }

        // Finishes the frame and displays it
        EndDrawing();
    }
    
    // Close Raylib when the player exits
    CloseWindow();
    
    return 0;
}