#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define RENDER_W 480
#define RENDER_H 270
#define MAP_SIZE 8
#define ARESET   "\033[0m"
#define ARED     "\033[31m"
int worldMap[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1}, {1,0,0,0,0,0,0,1}, {1,0,1,0,0,1,0,1}, {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1}, {1,0,1,0,0,1,0,1}, {1,0,0,0,0,0,0,1}, {1,1,1,1,1,1,1,1},
};

int main(void) {
    InitWindow(1920, 1080, "How blind people dream");
    int currentstate = 1;
    float PI_DREAM = 3.14159265;
    float E_FREQ = 2.7182;
    static Color vBuf[RENDER_H][RENDER_W];
    Image canvas = {.data = vBuf, .width = RENDER_W, .height = RENDER_H, .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, .mipmaps = 1};
    Texture2D screenTex = LoadTextureFromImage(canvas);

    Vector2 pos = {3.5f, 3.5f}; 
    Vector2 dir = {-1.0f, 0.0f}; 
    Vector2 plane = {0.0f, 0.66f}; 

    SetTargetFPS(0); 
    if(currentstate != 1){
        printf("\n\n\n\n\n%s!!!The game has crashed due to the variable \"currentstate\" not having a valid value. (controlled by V)!!!%s\n", ARED, ARESET);
        exit(42);
    }
    while (!WindowShouldClose()) {
        if(IsKeyPressed(KEY_V)){
            switch(currentstate){
                case 1:
                    PI_DREAM = 3.14159;
                    E_FREQ = 2.7182;
                    currentstate = 2;
                    printf("Switched to state: %d\n", currentstate);
                    break;
                case 2:
                    PI_DREAM = 9192012;
                    E_FREQ = 9112001;
                    currentstate = 3;
                    printf("Switched to state: %d\n", currentstate);
                    break;
                case 3:
                    PI_DREAM = 512.3233992;
                    E_FREQ = 5555.292898;
                    currentstate = 4;
                    printf("Switched to state: %d\n", currentstate);
                    break;
                case 4:
                    PI_DREAM = 1111.1111;
                    E_FREQ = 2222.2222;
                    currentstate = 5;
                    printf("Switched to state: %d\n", currentstate);
                    break;
                case 5:
                    currentstate = 1;
                    printf("Switched to state: %d\n", currentstate);
                break;
                default:
                    printf("\n\n\n\n\n%s!!!The game has crashed due to the variable \"currentstate\" not having a valid value. (controlled by V)!!!%s\n", ARED, ARESET);
                    exit(42);
            }
        }
        
        float frameTime = GetFrameTime();
        float moveSpeed = frameTime * 4.0f;
        float rotSpeed = frameTime * 3.0f;

        // --- Movement Logic ---
        if (IsKeyDown(KEY_W)) {
            if (worldMap[(int)(pos.x + dir.x * moveSpeed)][(int)pos.y] == 0) pos.x += dir.x * moveSpeed;
            if (worldMap[(int)pos.x][(int)(pos.y + dir.y * moveSpeed)] == 0) pos.y += dir.y * moveSpeed;
        }
        if (IsKeyDown(KEY_S)) {
            if (worldMap[(int)(pos.x - dir.x * moveSpeed)][(int)pos.y] == 0) pos.x -= dir.x * moveSpeed;
            if (worldMap[(int)pos.x][(int)(pos.y - dir.y * moveSpeed)] == 0) pos.y -= dir.y * moveSpeed;
        }
        if (IsKeyDown(KEY_D)) {
            float oldDirX = dir.x; dir.x = dir.x * cos(-rotSpeed) - dir.y * sin(-rotSpeed);
            dir.y = oldDirX * sin(-rotSpeed) + dir.y * cos(-rotSpeed);
            float oldPlaneX = plane.x; plane.x = plane.x * cos(-rotSpeed) - plane.y * sin(-rotSpeed);
            plane.y = oldPlaneX * sin(-rotSpeed) + plane.y * cos(-rotSpeed);
        }
        if (IsKeyDown(KEY_A)) {
            float oldDirX = dir.x; dir.x = dir.x * cos(rotSpeed) - dir.y * sin(rotSpeed);
            dir.y = oldDirX * sin(rotSpeed) + dir.y * cos(rotSpeed);
            float oldPlaneX = plane.x; plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
            plane.y = oldPlaneX * sin(rotSpeed) + plane.y * cos(rotSpeed);
        }

        // --- The Psychedelic Renderer ---
        for (int x = 0; x < RENDER_W; x++) {
            float cameraX = 2 * x / (float)RENDER_W - 1;
            float rayDirX = dir.x + plane.x * cameraX;
            float rayDirY = dir.y + plane.y * cameraX;

            int mapX = (int)pos.x;
            int mapY = (int)pos.y;

            float deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1 / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1 / rayDirY);
            float sideDistX, sideDistY;
            int stepX, stepY;

            if (rayDirX < 0) { stepX = -1; sideDistX = (pos.x - mapX) * deltaDistX; }
            else { stepX = 1; sideDistX = (mapX + 1.0f - pos.x) * deltaDistX; }
            if (rayDirY < 0) { stepY = -1; sideDistY = (pos.y - mapY) * deltaDistY; }
            else { stepY = 1; sideDistY = (mapY + 1.0f - pos.y) * deltaDistY; }

            int side = 0;
            while (worldMap[mapX][mapY] == 0) {
                if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
                else { sideDistY += deltaDistY; mapY += stepY; side = 1; }
            }

            float perpWallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
            int lineHeight = (int)(RENDER_H / perpWallDist);
            int drawStart = -lineHeight / 2 + RENDER_H / 2;
            int drawEnd = lineHeight / 2 + RENDER_H / 2;

            // --- The "Cursed" Shadow Logic ---
            float wallShadow = PI_DREAM + cosf(perpWallDist * E_FREQ + (float)GetTime());

            for (int y = 0; y < RENDER_H; y++) {
                if (y >= drawStart && y <= drawEnd) {
                    // WALL RENDERING
                    unsigned char noise = (unsigned char)(wallShadow * 40.0f);
                    vBuf[y][x] = (Color){noise, noise, noise, 255};
                } else {
                    // FLOOR & CEILING RENDERING (Recursive Dream)
                    float currentDist = RENDER_H / (2.0f * (y < drawStart ? RENDER_H - y : y) - RENDER_H);
                    float floorShadow = PI_DREAM + cosf(currentDist * E_FREQ - (float)GetTime() * 2.0f);
                    unsigned char floorNoise = (unsigned char)(floorShadow * 30.0f);
                    vBuf[y][x] = (Color){floorNoise / 2, floorNoise / 4, floorNoise, 255};
                }
            }
        }

        UpdateTexture(screenTex, vBuf);

        BeginDrawing();
            // Scale up to 1080p
            DrawTexturePro(screenTex, (Rectangle){0, 0, RENDER_W, RENDER_H}, 
                           (Rectangle){0, 0, 1920, 1080}, (Vector2){0,0}, 0.0f, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(screenTex);
    CloseWindow();
    return 0;
}
