#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

const int HScreen = 480;
const int WScreen = 640;

int main(int argc, char* args[]){
	//inisialisasi SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cerr << "Gagal menginialisasi SDL: " << SDL_GetError() << endl;
		return -1;
	}

	// Deklarasi Window
	SDL_Window* window = SDL_CreateWindow("FIRST WINDOW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, HScreen, WScreen, SDL_WINDOW_SHOWN);

	// Kondisi jika window tidak terbuka
	if(window == nullptr){
		cerr << "Gagal Membuat Jendela!" << SDL_GetError() << endl;
		SDL_Quit();
		return -1;
	}

	// Membuat render
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Kondisi Jika renderer tidak mau terbuka
	if(renderer == nullptr){
		cerr << "Gagal Membuat renderer" << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	bool running = true;

	SDL_Event event;

	while(running){

		SDL_RenderClear(renderer);

		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				running = false;
			}
		}
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}