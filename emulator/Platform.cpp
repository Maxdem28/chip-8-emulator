#include "Platform.hpp"
#include <SDL2/SDL.h>


Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_AUDIO);
 
	window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
		
	framebuffer = (unsigned char*)malloc(64*32);
	//memset(framebuffer, 0xFF, 64*32);
}

void Platform::beep(){
	// load WAV file
	SDL_AudioSpec wavSpec;
	unsigned int wavLength;
	unsigned char *wavBuffer;

	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, 0), 0, &wavSpec, NULL, 0);
	if (deviceId == 0){
		printf("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
	}
	SDL_LoadWAV("beep.wav", &wavSpec, &wavBuffer, &wavLength);
	int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	if (success < 0){
		printf("SDL_QueueAudio failed: %s\n", SDL_GetError());
	}
	SDL_PauseAudioDevice(deviceId, 0);
	SDL_CloseAudioDevice(deviceId);
	SDL_FreeWAV(wavBuffer);
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

Platform::~Platform(){
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
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
	//SDL_RenderClear(renderer);
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
