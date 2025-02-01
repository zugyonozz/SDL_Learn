#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

int main(int argc, char* args[]){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cerr <<  "Gagal Inisialisasi Video" << SDL_GetError() << endl;
		return -1;
	}

	SDL_DisplayMode* displayMode;
	if(SDL_GetCurrentDisplayMode(0, displayMode) != 0){
		cerr << "Gagal mendapatkan ukuran layar " << SDL_GetError()  << endl;
		SDL_Quit();
		return -1;
	}

	const int W = displayMode->w;
	const int H = displayMode->h * 0.95;

	SDL_Log("Resolusi Layar %d x %d", W, H);

	SDL_Window* window = SDL_CreateWindow("2nd WINDOW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);

	if(window == nullptr){
		cerr << "Gagal membuat window " << SDL_GetError() << endl;
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(renderer == nullptr){
		cerr <<  "Gagal membuat renderer " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	bool running = true;
	SDL_Event event;

	while(running){
		while(SDL_PollEvent(&event)){
			switch (event.type){
				case SDL_QUIT:
					cout << "Windows Closed!" << endl;
					running = false;
					break;
			
				case SDL_KEYDOWN:
					cout << "Tombol di tekan: " << SDL_GetKeyName(event.key.keysym.sym) << endl;
					break;

				case SDL_KEYUP:
					cout << "Tombol di lepas: " << SDL_GetKeyName(event.key.keysym.sym) << endl;
					break;
				
				case SDL_MOUSEBUTTONDOWN:
					cout << "Klik mouse pada posisi (" << event.button.x << ", " << event.button.y << ")" << endl;
					break;

				default:
					break;
			}
			
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}