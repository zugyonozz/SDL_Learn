#include <iostream>
#include <SDL2/SDL.h>
using namespace std;

int main(int argc, char* args[]){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		cerr << "Gagal memuat video " << SDL_GetError() << endl;
		return -1;
	}

	SDL_DisplayMode setDisplay;
	if(SDL_GetCurrentDisplayMode(0, &setDisplay) != 0){
		cerr << "Gagal memuat displayMode " << SDL_GetError() << endl;
		SDL_Quit();
		return -1;
	}

	const int W = setDisplay.w;
	const int H = setDisplay.h * 0.94;

	SDL_Window* window = SDL_CreateWindow("Third Part 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);

	if(window == nullptr){
		cerr << "Gagal membuat window " << SDL_GetError() << endl;
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(renderer == nullptr){
		cerr << "Gagal membuat renderer " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit;
		return -1;
	}

	int x = 0;
	int y = 0;
	int speed = 5;

	bool running = true;
	SDL_Event event;

	while(running){
		while(SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT){
				running = false;
			}else if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_UP){
					y -= speed;
				}else if(event.key.keysym.sym == SDLK_DOWN){
					y += speed;
				}else if(event.key.keysym.sym == SDLK_LEFT){
					x -= speed;
				}else if(event.key.keysym.sym == SDLK_RIGHT){
					x += speed;
				}
			}
		}

		
		if(x > W){
			x = 0;
		}else if(x < -50){
			x = W-1;
		}
		
		if(y > H){
			y = 0;
		}else if(y < -50){
			y = H-1;
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
		SDL_Rect rect = {x, y, 50, 50};
		SDL_RenderFillRect(renderer, &rect);

		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}