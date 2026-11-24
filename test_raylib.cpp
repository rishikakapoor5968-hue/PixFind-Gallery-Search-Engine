#include "raylib.h"

int main()
{
    InitWindow(800, 450, "PixFind - Raylib Test");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Welcome to PixFind!", 220, 200, 30, DARKBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}