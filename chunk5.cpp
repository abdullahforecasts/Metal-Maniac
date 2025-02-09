#include <raylib.h>

const int WindowWidth = 1700;
const int WindowHeight = 750;
const int PlayerWidth = 100;
const int PlayerHeight = 117;
const int MaxBullets = 25; // Maximum number of bullets that can be fired at once

struct ground {
    int r;
    int c;
};

struct Bullet {
    float x, y;
    float speed;
    bool active;
    int dir;  // Direction bullet is moving (1 for right, -1 for left)
};

struct Soldier {
    int x, y;
    int speed;
    Texture2D texture;
    int sdWidth;
    int sdHeight;
    Rectangle hitbox;
    ground base;
    int currentFrame;
    int frameCounter;
    int frameSpeed;
    int dir;
    Rectangle frameRec; // Rectangle to define the current frame of the sprite sheet
    bool isJumping;     // New variable to track jumping state
    float jumpVelocity; // Jump speed
    float gravity;      // Gravity effect on the jump
    Bullet bullets[MaxBullets]; // Array of bullets
    int life = 4;
} sd;

bool StartAnimation();
void InitGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x);
void UpdateGame(Soldier& hero, int& bg1x, int& bg2x);
void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x);

int main() {
    // Initialize Raylib Window & Audio
    InitWindow(WindowWidth, WindowHeight, "Metal Maniac");
    SetTargetFPS(60);

    InitAudioDevice();

    // Load & Play Music
    Music ThemeSong = LoadMusicStream("theme.mp3");
    SetMusicVolume(ThemeSong, 0.05f);
    PlayMusicStream(ThemeSong); // Music loops automatically

    // Run Start Animation
    bool startGame = StartAnimation();
    WaitTime(0.1);

    if (!startGame) {
        // If 'E' was pressed in StartAnimation, exit the game immediately
        UnloadMusicStream(ThemeSong);
        CloseAudioDevice();
        CloseWindow();
        return 0;
    }

    // Initialize Game Elements
    Texture2D bg1, bg2;
    int bg1x, bg2x;
    Soldier hero;
    InitGame(hero, bg1, bg2, bg1x, bg2x);

    // Game Loop
    while (!WindowShouldClose()) {
        UpdateMusicStream(ThemeSong);
        UpdateGame(hero, bg1x, bg2x);

        BeginDrawing();
        ClearBackground(WHITE);
        DrawGame(hero, bg1, bg2, bg1x, bg2x);
        EndDrawing();

        if (IsKeyDown(KEY_ESCAPE)) {
            break;
        }
    }

    // Cleanup Resources Before Exit
    UnloadMusicStream(ThemeSong);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void InitGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x) {
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
    hero.hitbox.x = (float)(hero.x + 40);
    hero.hitbox.y = (float)(hero.y + 15);
    hero.hitbox.width = (float)(hero.sdWidth - 60);
    hero.hitbox.height = (float)(hero.sdHeight - 30);
    hero.speed = 2;
    hero.dir = 1;
    hero.currentFrame = 0;
    hero.frameCounter = 0;
    hero.frameSpeed = 10;
    hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
    hero.isJumping = false;
    hero.jumpVelocity = 0.0f;
    hero.gravity = 0.5f;

    for (int i = 0; i < MaxBullets; ++i) {
        hero.bullets[i] = { 0.0f, 0.0f, 5.0f, false, 1 }; // Initialize all bullets as inactive
    }
}

void UpdateGame(Soldier& hero, int& bg1x, int& bg2x) {
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

    if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT)) {
        hero.dir = 0;
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT)) {
        hero.dir = 1;
    }

    if (IsKeyPressed(KEY_SPACE) && !hero.isJumping) {  // Trigger jump
        hero.isJumping = true;
        hero.jumpVelocity = -10.0f;
    }

    // Handle jumping logic
    if (hero.isJumping) {
        hero.y += (int)hero.jumpVelocity;
        hero.jumpVelocity += hero.gravity;

        if (hero.y >= WindowHeight - hero.sdHeight - 140) {  // When hero lands
            hero.y = WindowHeight - hero.sdHeight - 140;  // Reset position to the ground
            hero.isJumping = false;
            hero.jumpVelocity = 0.0f;
        }
    }
  static Sound shoot = LoadSound("efs.mp3");
    // Shooting logic (when F is pressed)
    if (IsKeyPressed(KEY_F)) {
        for (int i = 0; i < MaxBullets; ++i) {
            if (!hero.bullets[i].active) {
                PlaySound(shoot);
                hero.bullets[i].active = true;
                hero.bullets[i].x = (float)(hero.x + (hero.dir == 1 ? hero.sdWidth : 0));  // Start bullet at the front of the soldier
                hero.bullets[i].y = (float)(hero.y + (hero.sdHeight / 2 + 15));
                hero.bullets[i].dir = hero.dir == 1 ? 1 : -1;
                

                break;
            }
        }
    }

    // Update bullet positions
    for (int i = 0; i < MaxBullets; ++i) {
        if (hero.bullets[i].active) {
            hero.bullets[i].x += hero.bullets[i].speed * hero.bullets[i].dir;
            if (hero.bullets[i].x < 0 || hero.bullets[i].x > WindowWidth) {  // If bullet goes off screen, deactivate it
                hero.bullets[i].active = false;
            }
        }
    }

    // Update the soldier's animation
    hero.frameCounter++;
    if (hero.frameCounter >= (60 / hero.frameSpeed)) {
        hero.frameCounter = 0;
        hero.currentFrame++;

        if (hero.dir == 0) {
            hero.texture = LoadTexture("hls.png");
            if (IsKeyDown(KEY_LEFT)) {
                hero.texture = LoadTexture("hlr.png");
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
        }
        else if (hero.dir == 1) {
            hero.texture = LoadTexture("hrs.png");
            if (IsKeyDown(KEY_RIGHT)) {
                hero.texture = LoadTexture("hrr.png");
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
        }

        hero.frameRec.x = (float)hero.currentFrame * (float)hero.texture.width / (hero.dir == 0 ? (IsKeyDown(KEY_LEFT) ? 12 : 2) : (IsKeyDown(KEY_RIGHT) ? 12 : 2));
    }

    // Update position and hitbox for movement
    if (IsKeyDown(KEY_RIGHT) && hero.x <= WindowWidth / 3) {
        hero.x += hero.speed;
        hero.hitbox.x = (float)(hero.x);
        bg1x -= hero.speed + 1;
        bg2x -= hero.speed + 1;
    }
    if (IsKeyDown(KEY_RIGHT) && hero.x > WindowWidth / 6) {
        bg1x -= hero.speed + 2;
        bg2x -= hero.speed + 2;
    }
    if (IsKeyDown(KEY_LEFT) && hero.x > -WindowWidth / 60) {
        hero.x -= hero.speed + 2;
    }
    hero.hitbox.x = (float)(hero.x + 40);
    hero.hitbox.y = (float)(hero.y + 10);
}

//void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x) {
//    DrawTexture(bg1, bg1x, 0, WHITE);
//    DrawTexture(bg2, bg2x, 0, WHITE);
//
//    // Load & Draw Top-Left Image (hf.png)
//    static Texture2D hf = LoadTexture("hf.png"); // Load once to prevent reloading every frame
//    DrawTexture(hf, 10, 10, WHITE); // Position it at the top-left corner with slight padding
//
//    // Draw bullets if active
//    for (int i = 0; i < MaxBullets; ++i) {
//        if (hero.bullets[i].active) {
//            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, 10, RED);
//            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, 4, WHITE);
//        }
//    }
//
//    // Ensure frameRec has valid values
//   /* if (hero.frameRec.width == 0 || hero.frameRec.height == 0) {
//        hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
//    }*/
//
//    Vector2 position = { (float)hero.x, (float)hero.y };
//    DrawTextureRec(hero.texture, hero.frameRec, position, WHITE);
//}
void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x) {
    DrawTexture(bg1, bg1x, 0, WHITE);
    DrawTexture(bg2, bg2x, 0, WHITE);

    // Load & Draw Top-Left Image (hf.png)
    static Texture2D hf = LoadTexture("hf.png"); // Load once to prevent reloading every frame
    DrawTexture(hf, 10, 10, WHITE); // Position it at the top-left corner

    // Load health bar images (only once)
    static Texture2D h5 = LoadTexture("h5.png");
    static Texture2D h4 = LoadTexture("h4.png");
    static Texture2D h3 = LoadTexture("h3.png");
    static Texture2D h2 = LoadTexture("h2.png");
    static Texture2D h1 = LoadTexture("h1.png");

    // Select the correct health image based on hero.life
    Texture2D currentHealth;
    switch (hero.life) {
    case 5: currentHealth = h5; break;
    case 4: currentHealth = h4; break;
    case 3: currentHealth = h3; break;
    case 2: currentHealth = h2; break;
    case 1: currentHealth = h1; break;
    default: currentHealth = h1; break; // Default to lowest health if unexpected value
    }

    // Draw health bar next to hf.png (position it correctly)
    DrawTexture(currentHealth, 120, 10, WHITE); // Adjust X position to place it next to hf.png

    // Draw bullets if active
    for (int i = 0; i < MaxBullets; ++i) {
        if (hero.bullets[i].active) {
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, 10, RED);
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, 4, WHITE);
        }
    }
    // Ensure frameRec has valid values
//   /* if (hero.frameRec.width == 0 || hero.frameRec.height == 0) {
//        hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
//    }*/
    // Draw the hero sprite
    Vector2 position = { (float)hero.x, (float)hero.y };
    DrawTextureRec(hero.texture, hero.frameRec, position, WHITE);
}


bool StartAnimation()
{
    // Load Textures
    Texture2D sl = LoadTexture("sl.png");
    Texture2D s1 = LoadTexture("s1.png");
    Texture2D s2 = LoadTexture("s2.png");
    Texture2D s3 = LoadTexture("s3.png");
    Texture2D s4 = LoadTexture("s4.png");
    Texture2D so = LoadTexture("so.png");
    Texture2D sc = LoadTexture("sc.png");
    Texture2D se = LoadTexture("se.png");

    // Display Logo
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(sl, 0, 0, WHITE);
    EndDrawing();
    WaitTime(2.0);

    Texture2D loadingScreens[] = { s1, s2, s3, s4 };
    for (int i = 0; i < 4; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(loadingScreens[i], 0, 0, WHITE);
        EndDrawing();
        WaitTime(1.25);
    }

    while (!IsKeyPressed(KEY_ENTER))
    {
        if (WindowShouldClose())
            return false;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(s4, 0, 0, WHITE);
        EndDrawing();
    }

    while (true)
    {
        if (WindowShouldClose())
            return false;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(so, 0, 0, WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_P))
        {
            BeginDrawing();
            ClearBackground(WHITE);
            EndDrawing();

            UnloadTexture(sl); UnloadTexture(s1); UnloadTexture(s2); UnloadTexture(s3);
            UnloadTexture(s4); UnloadTexture(so); UnloadTexture(sc); UnloadTexture(se);
            return true;
        }

        if (IsKeyPressed(KEY_C))
        {
            while (!IsKeyPressed(KEY_BACKSPACE))
            {
                if (WindowShouldClose())
                    return false;

                BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(sc, 0, 0, WHITE);
                EndDrawing();
            }
        }

        if (IsKeyPressed(KEY_E))
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(se, 0, 0, WHITE);
            EndDrawing();
            WaitTime(5.0);

            UnloadTexture(sl); UnloadTexture(s1); UnloadTexture(s2); UnloadTexture(s3);
            UnloadTexture(s4); UnloadTexture(so); UnloadTexture(sc); UnloadTexture(se);
            return false;
        }
    }
}