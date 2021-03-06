#pragma once

#include <cstdint>
#include <SDL2/SDL_mixer.h>
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;


class Platform{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);
    void Translate(void const* buffer);
    void beep();
    unsigned char pixelsize();

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
    unsigned int* framebuffer;
    Mix_Chunk *beeps;
};