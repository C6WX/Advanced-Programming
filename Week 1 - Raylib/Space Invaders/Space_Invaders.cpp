#include "raylib.h"

int main (void)
{
    // creates the window size and title
    InitWindow(800, 600, "Space_Invaders");
    
    // Sets the target FPS
    SetTargetFPS(60);
    
    //Sets the ship's height and width
    float shipHeight = 30.0f;
    float shipWidth = 40.0f;
    
    // The central position of the player
    Vector2 playerPos = {400.0f, 500.0f};
    
    // Top Point
    Vector2 point1 = {playerPos.x, playerPos.y - (shipHeight / 2)};
    // Left Point
    Vector2 point2 = {playerPos.x - (shipWidth / 2), playerPos.y + (shipHeight / 2)};
    // Right Point
    Vector2 point3 = {playerPos.x + (shipWidth / 2), playerPos.y + (shipHeight / 2)};
          
    // Set the player speed
    float speed = 5.0f;        
    
    // Create the game loop
    while(!WindowShouldClose())
    {
        // Starts drawing the current frame
        BeginDrawing();
        
        // Clears the previous frame and gives a black background
        ClearBackground(BLACK);
                  
        // Creates a triangle using the points provided above
        DrawTriangle(point1, point2, point3, BLUE);
        
        // Finishes the frame and displays it
        EndDrawing();
    }
    
    // Close Raylib when the player exits
    CloseWindow();
    
    return 0;
}