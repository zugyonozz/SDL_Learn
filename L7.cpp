#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
using namespace std;

const int W = 970;
const int H = 670;
const int tile = 30;

enum State{HOME, GAME, GAMEOVER, PAUSE, CLOSE};
enum TetrominoType{I, J, L, O, S, T, Z};

class Tetromino{
	public:
		int x, y;
		vector<vector<int>> index;
		SDL_Color color;
		
		Tetromino(TetrominoType t) : x(W/2), y(0){
			switch(t){
				case I:
					index = {{1, 1, 1, 1}};
					color = {0, 240, 240, 255};
					break;
				case J:
					index = {{0, 1}, {0, 1}, {1, 1}};
					color = {0, 0, 240, 255};
					break;
				case L:
					index = {{1, 0}, {1, 0}, {1, 1}};
					color = {240, 160, 0, 255};
					break;
				case O:
					index = {{1, 1}, {1, 1}};
					color = {240, 240, 0, 255};
					break;
				case S:
					index = {{0, 1, 1}, {1, 1, 0}};
					color = {0, 240, 0, 255};
					break;
				case T:
					index = {{1, 1, 1}, {0, 1, 0}};
					color = {160, 0, 240, 255};
					break;
				case Z:
					index = {{1, 1, 0}, {0, 1, 1}};
					color = {240, 0, 0, 255};
					break;
			}
	}

	void rotate(){
		int rows = index.size();
        int cols = index[0].size();

        vector<vector<int>> newIndex(cols, vector<int>(rows, 0));

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                newIndex[j][rows - i - 1] = index[i][j];
            }
        }
        index = newIndex;
	}

	void print() {
		rotate();
		for (int i = 0; i < index.size(); i++) {
			for (int j = 0; j < index[i].size(); j++) {
				cout << (index[i][j] ? "# " : "  ");
			}
			cout << endl;
		}
		cout << endl;
	}
	
};

struct btn{
	SDL_Texture* normal;
	SDL_Texture* hover;
	SDL_Rect rect;
	bool isHover;
};

struct dataState{
	int start;
	int btnSect;
	int end;

	dataState(State s = HOME) : start(0), btnSect(0), end(0){
		switch (s){
			case HOME:
				start = 0;
				btnSect = 1;
				end = 2;
				break;
			case GAME:
				break;
			case GAMEOVER:
				break;
			case PAUSE:
				break;
			case CLOSE:
				break;								
		}
	}
};

class App{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Event event;
		TTF_Font* font;
		State state;
		dataState data;
		vector<btn> button;
		
		void setRect(SDL_Rect &destrect, int x, int y, int w, int h){
			destrect.x = x;
			destrect.y = y;
			destrect.w = w;
			destrect.h = h;
		}

		SDL_Texture* btnLoadTexture(const char* path){
			SDL_Surface* surface = IMG_Load(path);
			if(!surface){
				cerr << "Gagal memuat: " << path << " - "<< IMG_GetError() << endl;
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			return texture;
		}

		void textureInit(const char* normalPath, const char* hoverPath, int x, int y){
			SDL_Texture* textureHov;
			SDL_Texture* textureNorm = btnLoadTexture(normalPath);

			if(!hoverPath){
				textureHov = textureNorm;
			}else{
				textureHov = btnLoadTexture(hoverPath);
			}

			if(textureNorm){
				btn temp;
				SDL_Rect tempRect;
				SDL_QueryTexture(textureNorm, nullptr, nullptr, &tempRect.w, &tempRect.h);
				setRect(tempRect, x, y, tempRect.w, tempRect.h);

				temp.normal = textureNorm;
				temp.hover = textureHov;
				temp.rect = tempRect;
				temp.isHover = false;
				button.emplace_back(temp);
			}
		}

		void renderButton(){
			for(auto& b : button){
				if(b.isHover){
					SDL_RenderCopy(renderer, b.hover, nullptr, &b.rect);
				}else{
					SDL_RenderCopy(renderer, b.normal, nullptr, &b.rect);
				}
			}
		}

		void updateButton(int mx, int my, dataState data){
			for(int i = data.btnSect; i <= data.end; i++){
				if(mx >= button[i].rect.x && mx <= button[i].rect.x + button[i].rect.w && my >= button[i].rect.y && my <= button[i].rect.y + button[i].rect.h){
					button[i].isHover = true;
				}else{
					button[i].isHover = false;
				}
			}
		}
		
		void eventButton(int mx, int my, dataState data){
			int index = data.btnSect;
			if(event.type == SDL_MOUSEBUTTONDOWN){
				for(int i = data.btnSect; i <= data.end; i++){
					if(mx >= button[i].rect.x && mx <= button[i].rect.x + button[i].rect.w && my >= button[i].rect.y && my <= button[i].rect.y + button[i].rect.h){
						if(index - data.btnSect == 0){
							state = GAME;
							return;
						}
						if(index - data.btnSect == 1){
							state = CLOSE;
						}
					}
					index++;
				}
			}
		}

		void Home(){
			data = dataState(HOME);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			while(state == HOME){
				while(SDL_PollEvent(&event)){
					if(event.type == SDL_QUIT){
						state = CLOSE;
					}
					if(event.type == SDL_MOUSEMOTION){
						updateButton(event.motion.x, event.motion.y, data);
					}
					if(event.type == SDL_MOUSEBUTTONDOWN){
						eventButton(event.button.x, event.button.y, data);
					}
				}
				SDL_RenderClear(renderer);
				renderButton();
				SDL_RenderPresent(renderer);
			}
			SDL_RenderClear(renderer);
		}

	public:
		App() : state(HOME){
			if(SDL_Init(SDL_INIT_VIDEO) < 0){
				cerr << "Gagal Inisialisasi Video " << SDL_GetError() << endl;
				return;
			}
			if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
				cerr << "Gagal inisialisasi PNG " << IMG_GetError() << endl;
				return;
			}
			if(TTF_Init() == -1){
				cerr << "Gagal Inisialisasi TTF " << TTF_GetError() << endl;
				return;
			}
			window = SDL_CreateWindow("Tetris Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
			if(!window){
				cerr << "Gagal Inisialisasi Window " << SDL_GetError() << endl;
				return;
			}
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if(!renderer){
				cerr << "Gagal Inisialisasi Renderer " << SDL_GetError() << endl;
				return;
			}
			font = TTF_OpenFont("fonts/PixelifySans-Medium.ttf", 48);
			if(!font){
				cerr << "Gagal Inisialisasi Font " << SDL_GetError() << endl;
				return;
			}
			state = HOME;
			textureInit("Asset/Home/Title.png", nullptr, 285, 183);
			textureInit("Asset/Home/btnStart.png", "Asset/Home/btnStartHover.png", 380, 293);
			textureInit("Asset/Home/btnExit.png", "Asset/Home/btnExitHover.png", 380, 403);
		}

		~App(){
			for(auto& b : button){
				SDL_DestroyTexture(b.normal);
				SDL_DestroyTexture(b.hover);
			}
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			TTF_CloseFont(font);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
		}

		void run(){
			Home();
		}

};

int main(){
	App app;
	app.run();

	return 0;
}

/*	vector<btn> button:
	1. title Tetris -> home
	2. button start -> home
	3. button exit -> home

*/ 
	
