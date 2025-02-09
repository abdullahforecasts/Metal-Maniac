#include <raylib.h>

const int WindowWidth = 1700;
const int WindowHeight = 750;
const int PlayerWidth = 100;
const int PlayerHeight = 117;

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
    Rectangle hitbox;
    ground base;
    int currentFrame;
    int frameCounter;
    int frameSpeed;
    int dir;
    Rectangle frameRec; // Rectangle to define the current frame of the sprite sheet
} sd;

bool StartAnimation();
void InitGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x);
void UpdateGame(Soldier& hero, int& bg1x, int& bg2x);
void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x);

int main() {
    // Initialize Raylib Window & Audio
    InitWindow(WindowWidth, WindowHeight, "Metal Maniac"); // Ensure the window is created
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
        UpdateMusicStream(ThemeSong); // Keep music playing smoothly
        UpdateGame(hero, bg1x, bg2x);

        BeginDrawing();
        ClearBackground(WHITE);
        DrawGame(hero, bg1, bg2, bg1x, bg2x);
        EndDrawing();

        if (IsKeyDown(KEY_ESCAPE)) {
            break; // Exit game loop
        }
    }

    // Cleanup Resources Before Exit
    UnloadMusicStream(ThemeSong);
    CloseAudioDevice();
    CloseWindow(); // Ensure the window is properly closed

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
    hero.hitbox.x = hero.x + 40;
    hero.hitbox.y = hero.y + 15;
    hero.hitbox.width = hero.sdWidth - 60;
    hero.hitbox.height = hero.sdHeight - 30;
    hero.speed = 2;
    hero.dir = 1;
    hero.currentFrame = 0;
    hero.frameCounter = 0;
    hero.frameSpeed = 10; // Adjust this value to control animation speed
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

    if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT)) {
        hero.dir = 0;

    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT)) {
        hero.dir = 1;

    }

    // Update frame counter
    hero.frameCounter++;
    if (hero.frameCounter >= (60 / hero.frameSpeed)) {
        hero.frameCounter = 0;
        hero.currentFrame++;

        if (hero.dir == 0) {
            hero.texture = LoadTexture("hls.png");
            if (IsKeyDown(KEY_LEFT)) {
                hero.texture = LoadTexture("hlr.png"); // 12 frames
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
            else if (IsKeyPressed(KEY_LEFT)) {
                hero.texture = LoadTexture("hls.png"); // 2 frames
                hero.frameRec.width = (float)hero.texture.width / 2;
                if (hero.currentFrame >= 2) hero.currentFrame = 0;
            }
        }
        else if (hero.dir == 1) {
            hero.texture = LoadTexture("hrs.png");
            if (IsKeyDown(KEY_RIGHT)) {
                hero.texture = LoadTexture("hrr.png"); // 12 frames
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
            }
            else if (IsKeyPressed(KEY_RIGHT)) {
                hero.texture = LoadTexture("hrs.png"); // 2 frames
                hero.frameRec.width = (float)hero.texture.width / 2;
                if (hero.currentFrame >= 2) hero.currentFrame = 0;
            }
        }

        hero.frameRec.x = (float)hero.currentFrame * (float)hero.texture.width / (hero.dir == 0 ? (IsKeyDown(KEY_LEFT) ? 12 : 2) : (IsKeyDown(KEY_RIGHT) ? 12 : 2));
    }

    if (IsKeyDown(KEY_RIGHT) && hero.x <= WindowWidth / 3) {
        hero.x += hero.speed;
        hero.hitbox.x = hero.x;
        bg1x -= hero.speed + 1;
        bg2x -= hero.speed + 1;
    }
    if (IsKeyDown(KEY_RIGHT) && hero.x > WindowWidth / 6) {
        bg1x -= hero.speed + 2;
        bg2x -= hero.speed + 2;


    }
    if (IsKeyDown(KEY_LEFT) && hero.x > -WindowWidth / 60) {
        hero.x -= hero.speed + 2;
        /*  bg1x += hero.speed + 2;
          bg2x += hero.speed + 2;*/
    }
    hero.hitbox.x = hero.x + 40;
}

void DrawGame(Soldier& hero, Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x) {
    DrawTexture(bg1, bg1x, 0, WHITE);
    DrawTexture(bg2, bg2x, 0, WHITE);

    // Ensure frameRec has valid values
    if (hero.frameRec.width == 0 || hero.frameRec.height == 0) {
        hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
    }
    Vector2 position = { (float)hero.x, (float)hero.y };
    DrawTextureRec(hero.texture, hero.frameRec, position, WHITE);
    DrawRectangle(hero.hitbox.x, hero.hitbox.y, hero.hitbox.width, hero.hitbox.height, BLACK);
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
    WaitTime(1.0); // 1 second delay

    // Display Loading Screens with Delays
    Texture2D loadingScreens[] = { s1, s2, s3, s4 };
    for (int i = 0; i < 4; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(loadingScreens[i], 0, 0, WHITE);
        EndDrawing();
        WaitTime(1.0); // Delay after each loading screen
    }

    // Wait until ENTER is pressed
    while (!IsKeyPressed(KEY_ENTER))  // ✅ Change to IsKeyPressed() for better responsiveness
    {
        if (WindowShouldClose())  // ✅ Allow user to close the window
            return false;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(s4, 0, 0, WHITE);
        EndDrawing();
    }

    // Show Main Operations Screen
    while (true)
    {
        if (WindowShouldClose())  // ✅ Allow user to close the window
            return false;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(so, 0, 0, WHITE);
        EndDrawing();

        // Handle User Input (P to Play, C for Controls, E to Exit)
        if (IsKeyPressed(KEY_P)) // ✅ Use IsKeyPressed() for a single detection
        {
            BeginDrawing();
            ClearBackground(WHITE);
            EndDrawing();

            UnloadTexture(sl); UnloadTexture(s1); UnloadTexture(s2); UnloadTexture(s3);
            UnloadTexture(s4); UnloadTexture(so); UnloadTexture(sc); UnloadTexture(se);
            return true; // Start the game
        }

        if (IsKeyPressed(KEY_C))
        {
            while (!IsKeyPressed(KEY_BACKSPACE))  // ✅ Detect key press instead of holding
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
            WaitTime(2.0); // 2-second delay before exiting

            UnloadTexture(sl); UnloadTexture(s1); UnloadTexture(s2); UnloadTexture(s3);
            UnloadTexture(s4); UnloadTexture(so); UnloadTexture(sc); UnloadTexture(se);
            return false; // Exit the game
        }
    }
}