#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int W = 1200;
const int H = 600;
const int G = 1;
const int strength = 15;
const int pipeGap = 200;
const int pipeWidth = 80;
const int Psize = 40;
const int pipeSpeed = 5;

class P{
	public:
		int x, y, g;

		P(){
			x = W/4;
			y = H/2;
			g = 0;
		}

		void jump(){
			g = -strength;
		}
		

		void update(){
			y += g;
			g += G;
		}

		SDL_Rect getP(){
			return {x, y, Psize, Psize};
		}
};

class Pipe{
	public:
		int x, height;

		Pipe(int startX){
			x = startX;
			height = rand() % (H - pipeGap);
		}

		void update(){
			x -= pipeSpeed;
		}

		SDL_Rect pipeBot() const {
			return {x, height + pipeGap, pipeWidth, H - height - pipeGap};
		}		

		SDL_Rect pipeTop() const {
			return {x, 0, pipeWidth, height};
		}
};

class Game{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		bool running;
		P player;
		vector<Pipe> pipes;
		int score;
	
		public:
			Game(){
				SDL_Init(SDL_INIT_VIDEO);
				window = SDL_CreateWindow("First zuu's Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
				running = true;
				score = 0;
				srand(time(0));
				pipes.emplace_back(W);
			}

			~Game(){
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
			}

			void run(){
				SDL_Event e;
				while(running){
					HandleEvent(e);
					update();
					render();
					SDL_Delay(16);
				}
			}

			void HandleEvent(SDL_Event e){
				while(SDL_PollEvent(&e)){
					if(e.type == SDL_QUIT){
						running = false;
					}
					if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
						player.jump();
					}
				}
			}

			void update(){
				player.update();

				if(player.y <= 0 || player.y + Psize >= H){
					running = false;
				}

				for(int i = 0; i < pipes.size(); i++){
					pipes[i].update();
				}

				if(pipes.front().x + pipeWidth <= 0){
					pipes.erase(pipes.begin());
					score++;
				}

				if(pipes.back().x < W - pipeGap * 2){
					pipes.emplace_back(W);
				}

				SDL_Rect playerRect = player.getP();
				for(int i = 0; i < pipes.size(); i++){
					SDL_Rect pipeRectTop = pipes[i].pipeTop();
					SDL_Rect pipeRectBot = pipes[i].pipeBot();

					if(SDL_HasIntersection(&playerRect,&pipeRectTop) || SDL_HasIntersection(&playerRect,&pipeRectBot)){
						running = false;
					}
				}
			}

			void render(){
				SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
				SDL_RenderClear(renderer);

				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
				SDL_Rect playerRect = player.getP();
				SDL_RenderFillRect(renderer, &playerRect);

				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				for (int i = 0; i < pipes.size(); i++) {
					SDL_Rect top = pipes[i].pipeTop(), bot = pipes[i].pipeBot();
					SDL_RenderFillRect(renderer, &top);
					SDL_RenderFillRect(renderer, &bot);
				}

				SDL_RenderPresent(renderer);
			}
};

int main(){
	Game game;
	game.run();
	return 0;
}