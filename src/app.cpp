#include "app.h"
#include "assets.h"
#include "debug.h"
#include "game.h"
#include "intro.h"
#include "ending.h"

// Load the application
void Application::Load() {
    InitWindow(screenWidth, screenHeight, "Sandy Tetris");
    InitAudioDevice();
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);

    LoadAssets();

    Shader &heatShader = GetShader(Shaders::Heat);

    iResoluationLoc = GetShaderLocation(heatShader, "iResolution");
    iTimeLoc = GetShaderLocation(heatShader, "iTime");
    time = 0;

    float res[2] = {(float) screenWidth, (float) screenHeight};

    SetShaderValue(heatShader, iResoluationLoc, res, SHADER_UNIFORM_VEC2);
    SetShaderValue(heatShader, iTimeLoc, 0, SHADER_UNIFORM_FLOAT);

    const char fontCharacters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,;:?!-_~#\"\'&()[]{}^|`/\\@+=%$<>";

    font = PixelFont(GetTexture(Textures::font), fontCharacters, 4, Color {255, 0, 0, 255});

    game = new Game(this);
    game->Load();
    intro = new Intro(this);
    intro->Load();
    ending = new Ending(this);
    ending->Load();

    if (settings.music) {
        music = LoadMusicStream("assets/music/Gerudo Valley 8 Bit.mp3");
        PlayMusicStream(music);
        SetMusicVolume(music, 0.5f);
    }

    state = Application::States::Intro;
}

// Run the application
void Application::Run() {
    while (!WindowShouldClose()) {
        if (settings.music) {
            UpdateMusicStream(music);
        }

        time += 1;
        float value[1] = {(float) time / 60};
        SetShaderValue(GetShader(Shaders::Heat), iTimeLoc, value, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            
        switch (state) {
            case Application::States::Intro:
                intro->Update();
                break;
            case Application::States::Game:
                game->Update();
                break;
            case Application::States::Ending:
                ending->Update();
                break;
        }
        
        for (auto it = transitions.cbegin(); it != transitions.cend();) {
            if (it->second->isFinished()) {
                transitions.erase(it++);
            } else {
                it->second->Draw();
                ++it;
            }
        }

        EndDrawing();
    }
}

// Unload the applicaiton
void Application::Unload() {
    UnloadAssets();
    CloseWindow();
}
