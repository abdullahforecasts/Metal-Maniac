#include <iostream>
#include <random>
#include <raylib.h>

using namespace std;

const int WindowWidth = 1700;
const int WindowHeight = 750;
const int PlayerWidth = 100;
const int PlayerHeight = 117;
const int MaxBullets = 10;
const int MaxEnemies = 25;

struct ground {
    int r;
    int c;
};

struct Bullet {
    float x, y;
    float outer_radius = 8;
    float inner_radius = 3;
    float speed;
    bool active;
    int dir;
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
    Rectangle frameRec;
    bool isJumping;
    float jumpVelocity;
    float gravity;
    Bullet bullets[MaxBullets];
    Bullet enemyBullets[MaxBullets];
    int MaxEnemies = 5;
    int life = 5;
    int EC = MaxEnemies;
   
} sd;

void InitGame(Soldier& hero, Soldier enemy[], Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x) {
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
    hero.gravity = 1.f;

    random_device rd;
    mt19937 gen(rd());
   uniform_int_distribution<> rangeSelector(0, 1);
   uniform_int_distribution<> range1(-3 * WindowWidth / 2, -PlayerWidth - 10);
   uniform_int_distribution<> range2(WindowWidth + 10, 3 * WindowWidth / 2);

    for (int i = 0; i < hero.MaxEnemies; i++) {
        enemy[i].texture = LoadTexture("ers.png");
        enemy[i].texture.width = PlayerWidth + 125;
        enemy[i].texture.height = PlayerHeight;
        enemy[i].sdHeight = PlayerHeight;
        enemy[i].sdWidth = PlayerWidth + 125;

        if (rangeSelector(gen) == 0) {
            enemy[i].x = range1(gen);
        }
        else {
            enemy[i].x = range1(gen);
        }

        enemy[i].y = WindowHeight - hero.sdHeight - 140;
        enemy[i].hitbox.x = (float)(hero.x + 100);
        enemy[i].hitbox.y = (float)(hero.y + 15);
        enemy[i].hitbox.width = (float)(hero.sdWidth - 30);
        enemy[i].hitbox.height = (float)(hero.sdHeight - 30);
        enemy[i].speed = 2.5;
        enemy[i].dir = 1;
        enemy[i].currentFrame = 0;
        enemy[i].frameCounter = 0;
        enemy[i].frameSpeed = 10;
        enemy[i].frameRec = { 0.0f, 0.0f, (float)enemy[i].texture.width / 2, (float)enemy[i].texture.height };
    }

    for (int i = 0; i < MaxBullets; ++i) {
        hero.bullets[i] = { 0.0f, 0.0f, hero.bullets[i].outer_radius, hero.bullets[i].inner_radius, 5.0f, false, 1 };
        for (int j = 0; j < hero.MaxEnemies; j++) {
            enemy[j].bullets[i] = { (float)WindowWidth, 0.0f, enemy[j].bullets[i].outer_radius, enemy[j].bullets[i].inner_radius, 5.0f, false, 1 };
        }
    }
}

void DrawEnemyBullets(Soldier enemy[], Soldier& hero) {
    for (int i = 0; i < hero.MaxEnemies; i++) {
        for (int j = 0; j < MaxBullets; j++) {
            if (enemy[i].enemyBullets[j].active) {
                /*cout << "-------------------------------------------\n";
                cout << enemy[i].enemyBullets[j].x << ", " << enemy[i].enemyBullets[j].y << endl;*/
                DrawCircle(enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y, enemy[i].enemyBullets[j].outer_radius, BLUE); // Blue color for enemy bullets
                DrawCircle(enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y, enemy[i].enemyBullets[j].inner_radius, WHITE);
            }
        }
    }
}

void DrawGame(Soldier& hero, Soldier enemy[], Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x) {
    DrawTexture(bg1, bg1x, 0, WHITE);
    DrawTexture(bg2, bg2x, 0, WHITE);

    static Texture2D hf = LoadTexture("hf.png");
    DrawTexture(hf, 10, 10, WHITE);
    static Texture2D ef = LoadTexture("ef.png");

    static Texture2D h5 = LoadTexture("h5.png");
    static Texture2D h4 = LoadTexture("h4.png");
    static Texture2D h3 = LoadTexture("h3.png");
    static Texture2D h2 = LoadTexture("h2.png");
    static Texture2D h1 = LoadTexture("h1.png");

    Texture2D currentHealth;
    switch (hero.life) {
    case 5: currentHealth = h5; break;
    case 4: currentHealth = h4; break;
    case 3: currentHealth = h3; break;
    case 2: currentHealth = h2; break;
    case 1: currentHealth = h1; break;
    default: currentHealth = h1; break;
    }

    DrawTexture(currentHealth, 120, 10, WHITE);

    for (int i = 0; i < MaxBullets; ++i) {
        if (hero.bullets[i].active) {
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, (float)hero.bullets[i].outer_radius, RED);
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, (float)hero.bullets[i].inner_radius, WHITE);
        }
    }

    for (int i = 0; i < hero.MaxEnemies; i++) {
        DrawTextureRec(enemy[i].texture, enemy[i].frameRec, { (float)enemy[i].hitbox.x - 15, (float)enemy[i].hitbox.y - 10 }, WHITE);
        //DrawRectangle(enemy[i].hitbox.x, enemy[i].hitbox.y, enemy[i].hitbox.width, enemy[i].hitbox.height, WHITE);
        //DrawRectangle(hero.hitbox.x, hero.hitbox.y, hero.hitbox.width, hero.hitbox.height, WHITE);
    }
    DrawTextureRec(hero.texture, hero.frameRec, { (float)hero.x, (float)hero.y }, WHITE);
    DrawTexture(ef, 1585, 10, WHITE);

    DrawText(TextFormat("x%d", hero.EC), 1470, 50, 50, BLACK);
    /* DrawHero(hero);
     DrawEnemies(enemy);*/
    DrawEnemyBullets(enemy,hero);
}

bool StartAnimation()
{
    Music loadSound = LoadMusicStream("loadSound.mp3");
    PlayMusicStream(loadSound);
    SetMusicVolume(loadSound, 8.0f);
    Texture2D sl = LoadTexture("sl.png");
    Texture2D s1 = LoadTexture("s1.png");
    Texture2D s2 = LoadTexture("s2.png");
    Texture2D s3 = LoadTexture("s3.png");
    Texture2D s4 = LoadTexture("s4.png");
    Texture2D so = LoadTexture("so.png");
    Texture2D sc = LoadTexture("sc.png");
    Texture2D se = LoadTexture("se.png");

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
        UpdateMusicStream(loadSound);
        if (WindowShouldClose())
            return false;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(s4, 0, 0, WHITE);
        EndDrawing();
    }

    while (true)
    {
        UpdateMusicStream(loadSound);
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
                UpdateMusicStream(loadSound);
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
            // UpdateMusicStream(loadSound);
            DrawTexture(se, 0, 0, WHITE);
            EndDrawing();
            WaitTime(5.0);

            UnloadTexture(sl); UnloadTexture(s1); UnloadTexture(s2); UnloadTexture(s3);
            UnloadTexture(s4); UnloadTexture(so); UnloadTexture(sc); UnloadTexture(se);

            return false;
        }
    }
}

void HandleShooting(Soldier& hero) {
    static Sound shoot = LoadSound("efs.mp3");

    if (IsKeyPressed(KEY_F)) {
        for (int i = 0; i < MaxBullets; ++i) {
            if (!hero.bullets[i].active) {
                PlaySound(shoot);
                hero.bullets[i].active = true;
                hero.bullets[i].x = (float)(hero.x + (hero.dir == 1 ? hero.sdWidth : 0));
                hero.bullets[i].y = (float)(hero.y + (hero.sdHeight / 2 + 15));
                hero.bullets[i].dir = hero.dir == 1 ? 1 : -1;

                break;
            }
        }
    }
}

void UpdateBackground(int& bg1x, int& bg2x) {
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
}

void HandleInput(Soldier& hero, Sound& step) {
    if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT)) {
        hero.dir = 0;
        PlaySound(step);
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT)) {
        hero.dir = 1;
        PlaySound(step);
    }

    if (IsKeyPressed(KEY_SPACE) && !hero.isJumping) {
        hero.isJumping = true;
        hero.jumpVelocity = -15.0f;
        PlaySound(step);
    }
}

void UpdateJump(Soldier& hero) {
    if (hero.isJumping) {
        hero.y += (int)hero.jumpVelocity;
        hero.jumpVelocity += hero.gravity;

        if (hero.y >= WindowHeight - hero.sdHeight - 140) {
            hero.y = WindowHeight - hero.sdHeight - 140;
            hero.isJumping = false;
            hero.jumpVelocity = 0.0f;
        }
    }
    hero.hitbox.y = hero.y + 10;
}

void UpdateBullets(Soldier& hero, Soldier enemy[]) {
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<> rangeSelector(0, 1);
    std::uniform_int_distribution<> range1(-3 * WindowWidth / 2, -PlayerWidth - 10);
    std::uniform_int_distribution<> range2(WindowWidth + 10, 3 * WindowWidth / 2);

    for (int i = 0; i < MaxBullets; ++i) {
        if (hero.bullets[i].active) {
            hero.bullets[i].x += hero.bullets[i].speed * hero.bullets[i].dir;
            if (hero.bullets[i].x < 0 || hero.bullets[i].x > WindowWidth) {
                hero.bullets[i].active = false;
                hero.bullets[i].x = -2 * WindowWidth;
            }

            for (int j = 0; j < hero.MaxEnemies; j++) {
                if (CheckCollisionCircleRec({ hero.bullets[i].x, hero.bullets[i].y }, hero.bullets[i].outer_radius, enemy[j].hitbox)) {
                    hero.bullets[i].active = false;
                    hero.bullets[i].x = -2 * WindowWidth;

                    if (rangeSelector(gen) == 0) {
                        enemy[j].x = range1(gen);
                    }
                    else {
                        enemy[j].x = range2(gen);
                    }

                    hero.EC--;
                }
                for (int k = 0; k < MaxBullets; k++) {
                    if (CheckCollisionCircles(
                        { hero.bullets[i].x, hero.bullets[i].y }, hero.bullets[i].outer_radius,
                        { enemy[j].enemyBullets[k].x, enemy[j].enemyBullets[k].y }, enemy[j].enemyBullets[k].outer_radius))
                    {
                        hero.bullets[i].x = -2 * WindowWidth;
                        hero.bullets[i].active = false;
                        enemy[j].enemyBullets[k].x = (3 * WindowWidth);
                        enemy[j].enemyBullets[k].active = false;
                    }
                }
            }
        }
    }
}

void UpdateAnimation(Soldier& hero, Sound& step) {
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
                PlaySound(step);
            }
        }
        else if (hero.dir == 1) {
            hero.texture = LoadTexture("hrs.png");
            if (IsKeyDown(KEY_RIGHT)) {
                hero.texture = LoadTexture("hrr.png");
                hero.frameRec.width = (float)hero.texture.width / 12;
                if (hero.currentFrame >= 12) hero.currentFrame = 0;
                PlaySound(step);
            }
        }

        hero.frameRec.x = (float)hero.currentFrame * (float)hero.texture.width / (hero.dir == 0 ? (IsKeyDown(KEY_LEFT) ? 12 : 2) : (IsKeyDown(KEY_RIGHT) ? 12 : 2));
    }
}


#include <raylib.h>

// Movement functions
void left(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    hero.x -= hero.speed + 2;
    PlaySound(step);
}

void right(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    hero.x += hero.speed;
    bg1x -= hero.speed + 1;
    bg2x -= hero.speed + 1;
    SetSoundPitch(step, 5.0);
    PlaySound(step);
}

void midright(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    bg1x -= hero.speed + 2;
    bg2x -= hero.speed + 2;
    PlaySound(step);
}

void UpdateMovement(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    void (*makemove)(Soldier&, int&, int&, Sound&) = nullptr;  

    if (IsKeyDown(KEY_RIGHT) && hero.x <= WindowWidth / 3) {
        makemove = right;
    }
    else if (IsKeyDown(KEY_RIGHT) && hero.x > WindowWidth / 6) {
        makemove = midright;
    }
    else if (IsKeyDown(KEY_LEFT) && hero.x > -WindowWidth / 60) {
        makemove = left;
    }

    if (makemove) { 
        makemove(hero, bg1x, bg2x, step);
    }

    hero.hitbox.x = (float)(hero.x + 40);
}


void UpdateEnemies(Soldier& hero, Soldier enemy[]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> rangeSelector(0, 1);
    uniform_int_distribution<> range1(-3 * WindowWidth / 2, -PlayerWidth - 10);
    uniform_int_distribution<> range2(WindowWidth + 10, 3 * WindowWidth / 2);
    uniform_int_distribution<> range3(1, 100);

    for (int i = 0; i < hero.MaxEnemies; i++) {
        if (CheckCollisionRecs(hero.hitbox, enemy[i].hitbox)) {
            if (rangeSelector(gen) == 0) {
                enemy[i].x = range1(gen);
            }
            else {
                enemy[i].x = range2(gen);
            }
            hero.life--;
        }

        if (hero.x < enemy[i].x) {
            enemy[i].dir = 0;
        }
        else {
            enemy[i].dir = 1;
        }

        // Enemy shooting logic
        // 1% chance to shoot per frame = 60% chance to shoot per second
        // We want it to shoot once per 5 seconds so........
        // Something like this
        if (range3(gen) == 1) { // 1/3% chance to shoot per frame
            for (int j = 0; j < MaxBullets; ++j) {
                if (!enemy[i].enemyBullets[j].active) {
                    enemy[i].enemyBullets[j].active = true;
                    enemy[i].enemyBullets[j].x = enemy[i].x + (enemy[i].dir == 0 ? -5 : PlayerWidth);
                    enemy[i].enemyBullets[j].y = enemy[i].y + 70;
                    enemy[i].enemyBullets[j].dir = enemy[i].dir == 0 ? -1 : 1;
                    enemy[i].enemyBullets[j].speed = 5.0f; // Adjust speed as needed
                    break;
                }
            }
        }
    }

    for (int i = 0; i < hero.MaxEnemies; i++) {
        if (hero.x > enemy[i].x) {
            enemy[i].x += enemy[i].speed;
        }
        else {
            enemy[i].x -= enemy[i].speed;
        }
        enemy[i].hitbox.x = enemy[i].x;
    }
}

void UpdateEnemyBullets(Soldier& hero, Soldier enemy[]) {
    for (int i = 0; i < hero.MaxEnemies; i++) {
        for (int j = 0; j < MaxBullets; j++) {
            if (enemy[i].enemyBullets[j].active) {
                enemy[i].enemyBullets[j].x += enemy[i].enemyBullets[j].speed * enemy[i].enemyBullets[j].dir;

                // Check if bullet is out of screen
                if (enemy[i].enemyBullets[j].x < 0 || enemy[i].enemyBullets[j].x > WindowWidth) {
                    enemy[i].enemyBullets[j].active = false;
                    enemy[i].enemyBullets[j].x = 3 * WindowWidth;
                }

                // Check collision with hero
                if (CheckCollisionCircleRec({ enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y }, enemy[i].enemyBullets[j].outer_radius, hero.hitbox)) {
                    enemy[i].enemyBullets[j].active = false;
                    enemy[i].enemyBullets[j].x = 3 * WindowWidth;
                    hero.life--; // Decrease hero's life
                    if (hero.life <= 0) {
                        // Handle hero death abhi likhna
                    }
                }
            }
        }
    }
}

void UpdateGame(Soldier& hero, Soldier enemy[], int& bg1x, int& bg2x) {
    static Sound step = LoadSound("step.mp3");

    UpdateBackground(bg1x, bg2x);
    HandleInput(hero, step);
    HandleShooting(hero);
    UpdateJump(hero);
    UpdateBullets(hero, enemy);
    UpdateAnimation(hero, step);
    UpdateMovement(hero, bg1x, bg2x, step);
    UpdateEnemies(hero, enemy);
    UpdateEnemyBullets(hero, enemy); // Add this line to update enemy bullets
}

bool WinYaLearn(Soldier& hero, bool& currentGameEnded)
{
    static Texture2D win = LoadTexture("win.png");
    static Texture2D learn = LoadTexture("learn.png");
    static Music meow = LoadMusicStream("meow.mp3");
    static Music chippi = LoadMusicStream("chippi.mp3");

    if (hero.life == 0)
    {
        currentGameEnded = true;
        PlayMusicStream(meow);
        while (!WindowShouldClose())
        {
            UpdateMusicStream(meow);
            BeginDrawing();
            ClearBackground(WHITE);
            DrawTexture(learn, 0, 0, WHITE);
            EndDrawing();

            if (IsKeyDown(KEY_R))
            {
                hero.life = 5;
                hero.EC = 25;
                currentGameEnded = false;
                StopMusicStream(meow);
                return false;
            }
        }
    }
    else if (hero.EC == 0)
    {
        currentGameEnded = true;
        PlayMusicStream(chippi);
        while (!WindowShouldClose())
        {
            UpdateMusicStream(chippi);
            BeginDrawing();
            ClearBackground(WHITE);
            DrawTexture(win, 0, 0, WHITE);
            EndDrawing();

            if (IsKeyDown(KEY_N))
            {
                hero.life = 5;
                hero.EC = 30;
                currentGameEnded = false;
                StopMusicStream(chippi);
                return true;
            }
        }
    }
    return false;
}


int main() {
    InitWindow(WindowWidth, WindowHeight, "Metal Maniac");
    SetTargetFPS(60);

    InitAudioDevice();

    Music ThemeSong = LoadMusicStream("theme.mp3");
    static Sound step = LoadSound("step.mp3");
    SetMusicVolume(ThemeSong, 0.1f);
    PlayMusicStream(ThemeSong);

    bool startGame = StartAnimation();

    if (!startGame) {
        UnloadMusicStream(ThemeSong);
        CloseAudioDevice();
        CloseWindow();
        return 0;
    }

    Texture2D bg1, bg2;
    int bg1x, bg2x;
    Soldier hero;
    Soldier enemy[MaxEnemies];

    InitGame(hero, enemy, bg1, bg2, bg1x, bg2x);
    bool currentGameEnded = false;

    while (!WindowShouldClose()) {
        UpdateMusicStream(ThemeSong);

        if (!currentGameEnded) {
            UpdateGame(hero, enemy, bg1x, bg2x);
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawGame(hero, enemy, bg1, bg2, bg1x, bg2x);

        bool wyl = WinYaLearn(hero, currentGameEnded);
        if (currentGameEnded) {
            if (wyl) {
                // New game with new enemy setup
                InitGame(hero, enemy, bg1, bg2, bg1x, bg2x);
            }
        }


        EndDrawing();

        if (IsKeyDown(KEY_ESCAPE)) {
            break;
        }
    }

    UnloadMusicStream(ThemeSong);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}