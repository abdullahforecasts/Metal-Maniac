#include <iostream>
#include <random>
#include <raylib.h>
using namespace std;

#define JetWidth 100  
#define JetHeight 50  
#define MaxJetBullets 5  
const int WindowWidth = 1700;
const int WindowHeight = 750;
const int PlayerWidth = 100;
const int PlayerHeight = 117;
const int MaxBullets = 10;
const int MaxEnemies = 10;
const int jetwidth = 50;

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
    Texture2D enemyTexture[2];
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
    int max_enemies = 5;
    int life = 5;
    int EC = 5;
} sd;

struct Jet {
    float x, y;
    float speed;
    Bullet bullets[MaxJetBullets];
    Texture2D texture;
    float outer_radius = 12;
    float inner_radius = 5;
} jet;

void LoadBackgrounds(Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x) {
    bg1 = LoadTexture("bg1.png");
    bg1.width = WindowWidth;
    bg1.height = WindowHeight;
    bg2 = LoadTexture("bg2.png");
    bg2.width = WindowWidth;
    bg2.height = WindowHeight;
    bg1x = 0;
    bg2x = WindowWidth;
}

void InitHero(Soldier& hero) {
    hero.texture = LoadTexture("hrs.png");
    hero.sdHeight = PlayerHeight;
    hero.sdWidth = PlayerWidth;
    hero.x = (WindowWidth / 2) - hero.sdWidth - 500;
    hero.y = WindowHeight - hero.sdHeight - 150;
    hero.hitbox = { (float)(hero.x + 40), (float)(hero.y + 15), (float)(hero.sdWidth - 60), (float)(hero.sdHeight - 30) };
    hero.speed = 2;
    hero.dir = 1;
    hero.currentFrame = 0;
    hero.frameCounter = 0;
    hero.frameSpeed = 10;
    hero.frameRec = { 0.0f, 0.0f, (float)hero.texture.width / 2, (float)hero.texture.height };
    hero.isJumping = false;
    hero.jumpVelocity = 0.0f;
    hero.gravity = 1.f;
    hero.EC = 5;
}

void InitEnemies(Soldier enemy[]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> rangeSelector(0, 1);
    uniform_int_distribution<> range1(-3 * WindowWidth / 2, -PlayerWidth - 10);
    uniform_int_distribution<> range2(WindowWidth + 10, 3 * WindowWidth / 2);

    for (int i = 0; i < MaxEnemies; i++) {
        enemy[i].enemyTexture[0] = LoadTexture("ers.png");
        enemy[i].enemyTexture[1] = LoadTexture("els.png");

        enemy[i].sdHeight = PlayerHeight;
        enemy[i].sdWidth = PlayerWidth + 125;

        enemy[i].x = (rangeSelector(gen) == 0) ? range1(gen) : range2(gen);
        enemy[i].y = WindowHeight - PlayerHeight - 130;
        enemy[i].hitbox = { (float)(enemy[i].x + 25), (float)(enemy[i].y - 10), (float)(enemy[i].sdWidth - 125), (float)(enemy[i].sdHeight - 30) };
        enemy[i].speed = 2.5;
        enemy[i].dir = 1;
        enemy[i].currentFrame = 0;
        enemy[i].frameCounter = 0;
        enemy[i].frameSpeed = 10;
        enemy[i].frameRec = { 0.0f, 0.0f, (float)enemy[i].enemyTexture[0].width / 2, (float)enemy[i].enemyTexture[0].height };
    }
}

void InitBullets(Soldier& hero, Soldier enemy[]) {
    for (int i = 0; i < MaxBullets; ++i) {
        hero.bullets[i] = { 0.0f, 0.0f, hero.bullets[i].outer_radius, hero.bullets[i].inner_radius, 5.0f, false, 1 };
        for (int j = 0; j < MaxEnemies; j++) {
            enemy[j].bullets[i] = { (float)WindowWidth, 0.0f, enemy[j].bullets[i].outer_radius, enemy[j].bullets[i].inner_radius, 5.0f, false, 1 };
        }
    }
}

void InitJet(Jet& jet) {
    jet.texture = LoadTexture("jet.png");
    jet.x = -3 * JetWidth;
    jet.y = 100;
    jet.speed = 3;

    for (int i = 0; i < MaxJetBullets; i++) {
        jet.bullets[i] = { 0.0f, 0.0f, 5.0f, false, 1 };
    }
}

void InitGame(Soldier& hero, Soldier enemy[], Texture2D& bg1, Texture2D& bg2, int& bg1x, int& bg2x, Jet& jet) {
    LoadBackgrounds(bg1, bg2, bg1x, bg2x);
    InitHero(hero);
    InitEnemies(enemy);
    InitBullets(hero, enemy);
    InitJet(jet);
}

void DrawEnemyBullets(Soldier enemy[], Soldier& hero) {
    for (int i = 0; i < MaxEnemies; i++) {
        for (int j = 0; j < MaxBullets; j++) {
            if (enemy[i].enemyBullets[j].active) {
                if (enemy[i].dir == 0) {
                    DrawCircle(enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y, enemy[i].enemyBullets[j].outer_radius, RED);
                }
                else {
                    DrawCircle(enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y, enemy[i].enemyBullets[j].outer_radius, GREEN);
                }
                DrawCircle(enemy[i].enemyBullets[j].x, enemy[i].enemyBullets[j].y, enemy[i].enemyBullets[j].inner_radius, WHITE);
            }
        }
    }
}

void UpdateJet(Jet& jet) {
    jet.x += jet.speed;
    if (jet.x > WindowWidth) {
        jet.x = -3 * JetWidth;
    }
}

void UpdateJetBullets(Jet& jet, Soldier& hero) {
    for (int i = 0; i < MaxJetBullets; i++) {
        if (jet.bullets[i].active) {
            jet.bullets[i].y += jet.bullets[i].speed;

            if (jet.bullets[i].y > WindowHeight) {
                jet.bullets[i].active = false;
            }

            if (CheckCollisionCircleRec({ jet.bullets[i].x, jet.bullets[i].y }, 5, hero.hitbox)) {
                PlaySound(LoadSound("ahh.mp3"));
                jet.bullets[i].active = false;
                hero.life--;
            }
        }
    }
}

void DropJetBullets(Jet& jet) {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dropChance(1, 100);

    if (dropChance(gen) <= 2) { // 2% chance per frame
        for (int i = 0; i < MaxJetBullets; i++) {
            if (!jet.bullets[i].active) {
                jet.bullets[i].active = true;
                jet.bullets[i].x = jet.x + JetWidth / 2;
                jet.bullets[i].y = jet.y + JetHeight;
                jet.bullets[i].speed = 5.0f;
                break;
            }
        }
    }
}


void DrawGame(Soldier& hero, Soldier enemy[], Texture2D& bg1, Texture2D& bg2, int bg1x, int bg2x, Jet& jet, bool& Addjet) {
    DrawTexture(bg1, bg1x, 0, WHITE);
    DrawTexture(bg2, bg2x, 0, WHITE);
    DrawTexture(jet.texture, jet.x, jet.y, WHITE);
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
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, (float)hero.bullets[i].outer_radius, BLUE);
            DrawCircle((int)hero.bullets[i].x, (int)hero.bullets[i].y, (float)hero.bullets[i].inner_radius, WHITE);
        }
    }

    for (int i = 0; i < MaxEnemies; i++) {
        DrawTexture(enemy[i].texture, (float)enemy[i].x, (float)enemy[i].y, WHITE);
        //DrawRectangle(enemy[i].hitbox.x, enemy[i].hitbox.y, enemy[i].hitbox.width, enemy[i].hitbox.height, WHITE);
        //DrawRectangle(hero.hitbox.x, hero.hitbox.y, hero.hitbox.width, hero.hitbox.height, WHITE);
    }
    DrawTextureRec(hero.texture, hero.frameRec, { (float)hero.x, (float)hero.y }, WHITE);
    DrawTexture(ef, 1585, 10, WHITE);

    DrawText(TextFormat("x%d", hero.EC), 1470, 50, 50, WHITE);

    DrawEnemyBullets(enemy, hero);

    if (Addjet)
    {
        DrawTexture(jet.texture, jet.x, jet.y, WHITE);
        for (int i = 0; i < MaxJetBullets; i++) {
            if (jet.bullets[i].active) {
                DrawCircle(jet.bullets[i].x, jet.bullets[i].y, jet.outer_radius, GOLD);
                DrawCircle(jet.bullets[i].x, jet.bullets[i].y, jet.inner_radius, WHITE);
            }
        }
    }

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

            for (int j = 0; j < MaxEnemies; j++) {
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

void UpdateAnimation(Soldier& hero, Soldier enemy[], Sound& step) {
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
    for (int i = 0; i < MaxEnemies; i++) {
        if (enemy[i].x <= hero.hitbox.x) {
            enemy[i].texture = enemy[i].enemyTexture[0];
        }
        else {
            enemy[i].texture = enemy[i].enemyTexture[1];
        }
    }
}

void left(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    hero.x -= hero.speed + 2;
    PlaySound(step);
}

void right(Soldier& hero, int& bg1x, int& bg2x, Sound& step) {
    hero.x += hero.speed;
    bg1x -= hero.speed + 2;
    bg2x -= hero.speed + 2;
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

    for (int i = 0; i < MaxEnemies; i++) {
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

        if (range3(gen) == 1) {
            for (int j = 0; j < MaxBullets; ++j) {
                if (!enemy[i].enemyBullets[j].active) {
                    enemy[i].enemyBullets[j].active = true;
                    enemy[i].enemyBullets[j].x = enemy[i].x + (enemy[i].dir == 0 ? -5 : PlayerWidth);
                    enemy[i].enemyBullets[j].y = enemy[i].y + 50;
                    enemy[i].enemyBullets[j].dir = enemy[i].dir == 0 ? -1 : 1;
                    enemy[i].enemyBullets[j].speed = 5.0f;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < MaxEnemies; i++) {
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
    static Sound ahh = LoadSound("ahh.mp3");
    for (int i = 0; i < MaxEnemies; i++) {
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
                    PlaySound(ahh);
                    enemy[i].enemyBullets[j].active = false;
                    enemy[i].enemyBullets[j].x = 3 * WindowWidth;
                    hero.life--; // Decrease hero's life
                }
            }
        }
    }
}

void UpdateGame(Soldier& hero, Soldier enemy[], int& bg1x, int& bg2x, Jet& jet, bool& Addjet) {
    static Sound step = LoadSound("step.mp3");

    UpdateBackground(bg1x, bg2x);
    HandleInput(hero, step);
    HandleShooting(hero);
    UpdateJump(hero);
    UpdateBullets(hero, enemy);
    UpdateAnimation(hero, enemy, step);
    UpdateMovement(hero, bg1x, bg2x, step);
    UpdateEnemies(hero, enemy);
    UpdateEnemyBullets(hero, enemy);

    if (Addjet)
    {
        UpdateJet(jet); // Moves the jet left to right
        DropJetBullets(jet); // Drops bullets randomly
        UpdateJetBullets(jet, hero); // Moves bullets & handles collisions
    }
}

bool WinYaLearn(Soldier& hero, bool& currentGameEnded, bool& Addjet)
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
                hero.EC = hero.max_enemies;
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
                hero.max_enemies += 5;
                hero.EC = hero.max_enemies;
                currentGameEnded = false;
                Addjet = true;
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
    Jet jet;

    InitGame(hero, enemy, bg1, bg2, bg1x, bg2x, jet);

    bool currentGameEnded = false;
    bool Addjet = false;

    while (!WindowShouldClose()) {
        UpdateMusicStream(ThemeSong);

        if (!currentGameEnded) {
            UpdateGame(hero, enemy, bg1x, bg2x, jet, Addjet);
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawGame(hero, enemy, bg1, bg2, bg1x, bg2x, jet, Addjet);

        bool wyl = WinYaLearn(hero, currentGameEnded, Addjet);
        if (currentGameEnded) {
            if (wyl) {
                // New game with new enemy setup
                InitGame(hero, enemy, bg1, bg2, bg1x, bg2x, jet);
            }
            currentGameEnded = false;
            wyl = false;
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