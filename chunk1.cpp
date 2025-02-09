#include <raylib.h>

const int WindowWidth = 1700;
const int WindowHeight = 750;
const int PlayerWidth = 80;
const int PlayerHeight = 150;

struct ground {
    int r;
    int c;
};

struct Soldier {
    int x, y;
    int speed;
    Texture2D texture;
    int sdWidth;
    int sdHeight;
    ground base;
    int currentFrame;
    int frameCounter;
    int frameSpeed;
    int dir;
    Rectangle frameRec; 
} sd;

void InitGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x) {
    InitWindow(WindowWidth, WindowHeight, "Metal Slug");
    SetTargetFPS(60);

    bg1 = LoadTexture("bg1.png");
    bg1.width = WindowWidth;
    bg1.height = WindowHeight;
    bg2 = LoadTexture("bg2.png");
    bg2.width = WindowWidth;
    bg2.height = WindowHeight;
    bg1x = 0;
    bg2x = WindowWidth;

    hero.texture = LoadTexture("hrs.png");  // 2 frames
    hero.sdHeight = PlayerHeight;
    hero.sdWidth = PlayerWidth;
    hero.x = (WindowWidth / 2) - hero.sdWidth - 500;
    hero.y = WindowHeight - hero.sdHeight - 140;
    hero.speed = 2;
    hero.dir = 1;
    hero.currentFrame = 0;
    hero.frameCounter = 0;
    hero.frameSpeed = 10; 
    hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height }; // Assuming 2 frames for initial texture
}

void UpdateGame(Soldier& hero, int& bg1x, int& bg2x) {
    float imgSizefixH = 13.5f;
    float imgSizefixW = 27;

    if (bg1x + WindowWidth <= 0) {
        bg1x = bg2x + WindowWidth;
    }
    if (bg2x + WindowWidth <= 0) {
        bg2x = bg1x + WindowWidth;
    }
    if (bg1x >= WindowWidth) {
        bg1x = bg2x - WindowWidth;
    }
    if (bg2x >= WindowWidth) {
        bg2x = bg1x - WindowWidth;
    }

    // Update frame counter
    hero.frameCounter++;
    if (hero.frameCounter >= (60 / hero.frameSpeed)) {
        hero.frameCounter = 0;
        hero.currentFrame++;

        if (hero.dir == 0) {
            if (IsKeyDown(KEY_LEFT)) {
                hero.texture = LoadTexture("hlr.png"); // 12 frames
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
            else {
                hero.texture = LoadTexture("hls.png"); // 2 frames
                hero.frameRec.width = (float)hero.texture.width / 2;
                if (hero.currentFrame >= 2) hero.currentFrame = 0;
            }
        }
        else if (hero.dir == 1) {
            if (IsKeyDown(KEY_RIGHT)) {
                hero.texture = LoadTexture("hrr.png"); // 12 frames
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
            else {
                hero.texture = LoadTexture("hrs.png"); // 2 frames
                hero.frameRec.width = (float)hero.texture.width / 2;
                if (hero.currentFrame >= 2) hero.currentFrame = 0;
            }
        }

        hero.frameRec.x = (float)hero.currentFrame * (float)hero.texture.width / (hero.dir == 0 ? (IsKeyDown(KEY_LEFT) ? 12 : 2) : (IsKeyDown(KEY_RIGHT) ? 12 : 2));
    }

    if (IsKeyPressed(KEY_LEFT)) {
        hero.dir = 0;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        hero.dir = 1;
    }

    if (IsKeyDown(KEY_RIGHT) && hero.x <= WindowWidth / 3) {
        hero.x += hero.speed;
        hero.texture.height = PlayerHeight + imgSizefixH;
        hero.texture.width = PlayerWidth * imgSizefixW;

        bg1x -= hero.speed + 1;
        bg2x -= hero.speed + 1;
    }
    if (IsKeyDown(KEY_RIGHT) && hero.x > WindowWidth / 6) {
        bg1x -= hero.speed + 2;
        bg2x -= hero.speed + 2;
        hero.texture.height = PlayerHeight + imgSizefixH;
        hero.texture.width = PlayerWidth * imgSizefixW;

    }
    if (IsKeyDown(KEY_LEFT) && hero.x > -WindowWidth / 60) {
        hero.x -= hero.speed;
        bg1x += hero.speed + 2;
        bg2x += hero.speed + 2;
        hero.texture.height = PlayerHeight + imgSizefixH;
        hero.texture.width = PlayerWidth * imgSizefixW;

    }
}

void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(bg1, bg1x, 0, WHITE);
    DrawTexture(bg2, bg2x, 0, WHITE);

    // Ensure frameRec has valid values
    if (hero.frameRec.width == 0 || hero.frameRec.height == 0) {
        hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
    }
    Vector2 position = { (float)hero.x, (float)hero.y };
    DrawTextureRec(hero.texture, hero.frameRec, position, WHITE);

    EndDrawing();
}


int main() {
    InitAudioDevice();
    Music ThemeSong = LoadMusicStream("theme.mp3");
    SetMusicVolume(ThemeSong, 0.01f);

    Texture2D bg1, bg2;
    int bg1x, bg2x;
    Soldier hero;
    InitGame(hero, bg1, bg2, bg1x, bg2x);
    PlayMusicStream(ThemeSong);
    while (!WindowShouldClose()) {
        UpdateMusicStream(ThemeSong);
        UpdateGame(hero, bg1x, bg2x);
        DrawGame(hero, bg1, bg2, bg1x, bg2x);
        if (IsKeyDown(KEY_ESCAPE)) {
            StopMusicStream(ThemeSong);
            break;
        }
    }
    UnloadMusicStream(ThemeSong);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}