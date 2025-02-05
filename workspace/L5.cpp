#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

const int W = 1000;
const int H = 600;

void Tile(SDL_Renderer* renderer, int x, int y, int w, int h,  int r, int g, int b){
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_Rect rect = {x, y, w, h};
	SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* args[]){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cerr << "Gagal Inisialisasi Video " << SDL_GetError() << endl;
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("L5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);

	if(window == nullptr){
		cerr << "Gagal Memuat Window " << SDL_GetError() << endl;
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(renderer == nullptr){
		cerr << "Gagal Mebuat Renderer " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	bool running = true;
	SDL_Event event;
	int C = 0;

	while(running){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		for(int i = 0; i < 6; i++){
			for(int j = 0; j < 10; j++){
				if(i%2 == 0){
					Tile(renderer, j*100, i*100, 100, 100, 221, 235, 157);
					j++;
					Tile(renderer, j*100, i*100, 100, 100, 255, 255, 255);
					j++;
					Tile(renderer, j*100, i*100, 100, 100, 160, 200, 120);
				}else{
					Tile(renderer, j*100, i*100, 100, 100, 160, 200, 120);
					j++;
					Tile(renderer, j*100, i*100, 100, 100, 221, 235, 157);
				}
			}
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
