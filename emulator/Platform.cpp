#include "Platform.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }
	window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
		
	framebuffer = (unsigned char*)malloc(64*32);

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 8192) < 0){
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
    gHigh = Mix_LoadWAV("b.wav");
    if(gHigh == NULL){
        printf("Failed to load beep sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }	
}

void Platform::Translate(void const* buffer){
	unsigned char* buffer1 = (unsigned char*)buffer;

	for (unsigned char y = 0;y<32;y++){
        for (unsigned char x = 0;x<8;x++){
            unsigned char byte = buffer1[x+y*8];
			for (unsigned char b = 0;b<8;b++){
				unsigned char bit = (byte >> b) & 0x1;
				if (bit)
					framebuffer[8*x+64*y+7-b] = 0x92;
				else
					framebuffer[8*x+64*y+7-b] = 0x00;
			}
        }
    }
}

void Platform::beep(){
	Mix_PlayChannel(-1, gHigh, 0);
}

Platform::~Platform(){
	Mix_FreeChunk(gHigh);
	gHigh = NULL;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch){
	Translate(buffer);
	/*
	for (unsigned char y = 0;y<32;y++){
        for (unsigned char x = 0;x<64;x++){
            printf("%02x", framebuffer[x+y*64]);
        }
        printf("\n");
    }
	*/
	SDL_RenderClear(renderer);
	SDL_UpdateTexture(texture, NULL, framebuffer, pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys){
	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				quit = true;
			} break;

			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					{
						quit = true;
					} break;

					case SDLK_x:
					{
						keys[0] = 1;
					} break;

					case SDLK_1:
					{
						keys[1] = 1;
					} break;

					case SDLK_2:
					{
						keys[2] = 1;
					} break;

					case SDLK_3:
					{
						keys[3] = 1;
					} break;

					case SDLK_q:
					{
						keys[4] = 1;
					} break;

					case SDLK_w:
					{
						keys[5] = 1;
					} break;

					case SDLK_e:
					{
						keys[6] = 1;
					} break;

					case SDLK_a:
					{
						keys[7] = 1;
					} break;

					case SDLK_s:
					{
						keys[8] = 1;
					} break;

					case SDLK_d:
					{
						keys[9] = 1;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 1;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 1;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 1;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 1;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 1;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 1;
					} break;
				}
			} break;

			case SDL_KEYUP:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_x:
					{
						keys[0] = 0;
					} break;

					case SDLK_1:
					{
						keys[1] = 0;
					} break;

					case SDLK_2:
					{
						keys[2] = 0;
					} break;

					case SDLK_3:
					{
						keys[3] = 0;
					} break;

					case SDLK_q:
					{
						keys[4] = 0;
					} break;

					case SDLK_w:
					{
						keys[5] = 0;
					} break;

					case SDLK_e:
					{
						keys[6] = 0;
					} break;

					case SDLK_a:
					{
						keys[7] = 0;
					} break;

					case SDLK_s:
					{
						keys[8] = 0;
					} break;

					case SDLK_d:
					{
						keys[9] = 0;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 0;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 0;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 0;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 0;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 0;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 0;
					} break;
				}
			} break;
		}
	}

	return quit;
}
