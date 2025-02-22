#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
using namespace std;

// Constants
const int WINDOW_WIDTH = 970;
const int WINDOW_HEIGHT = 670;
const int TILE_SIZE = 30;
const int BOARD_X = 480;
const int BOARD_Y = 35;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;

// Game states
enum State { HOME, GAME, GAMEOVER, PAUSE, CLOSE };
enum TetrominoType { I, J, L, O, S, T, Z };

// Tetromino class
class Tetromino {
public:
    int x, y;
    vector<vector<int>> shape;
    SDL_Color color;

    Tetromino(TetrominoType type) : x(4), y(0) {
        switch (type) {
            case I:
                shape = {
                    {0,0,0,0},
                    {1,1,1,1},
                    {0,0,0,0},
                    {0,0,0,0}
                };
                color = {0, 240, 240, 255};
                break;
            case J:
                shape = {
                    {1,0,0},
                    {1,1,1},
                    {0,0,0}
                };
                color = {0, 0, 240, 255};
                break;
            case L:
                shape = {
                    {0,0,1},
                    {1,1,1},
                    {0,0,0}
                };
                color = {240, 160, 0, 255};
                break;
            case O:
                shape = {
                    {1,1},
                    {1,1}
                };
                color = {240, 240, 0, 255};
                break;
            case S:
                shape = {
                    {0,1,1},
                    {1,1,0},
                    {0,0,0}
                };
                color = {0, 240, 0, 255};
                break;
            case T:
                shape = {
                    {0,1,0},
                    {1,1,1},
                    {0,0,0}
                };
                color = {160, 0, 240, 255};
                break;
            case Z:
                shape = {
                    {1,1,0},
                    {0,1,1},
                    {0,0,0}
                };
                color = {240, 0, 0, 255};
                break;
        }
    }

    void rotate() {
        int n = shape.size();
        vector<vector<int>> rotated(n, vector<int>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                rotated[j][n-1-i] = shape[i][j];
            }
        }
        shape = rotated;
    }
};

// Texture loading struct
struct TextureInfo {
    SDL_Texture* normal;
    SDL_Texture* hover;
    SDL_Rect rect;
    bool isHovered;
};

// Game state data
struct GameStateData {
    int startIndex;
    int buttonIndex;
    int endIndex;

    GameStateData(State state = HOME) {
        switch (state) {
            case HOME:
                startIndex = 0;
                buttonIndex = 1;
                endIndex = 2;
                break;
            case GAME:
                startIndex = 3;
                buttonIndex = 6;
                endIndex = 6;
                break;
            default:
                startIndex = buttonIndex = endIndex = 0;
        }
    }
};

// Main game class
class TetrisGame {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    State gameState;
    GameStateData stateData;
    Tetromino* currentPiece;
    vector<TextureInfo> textures;
    vector<vector<SDL_Color>> gameGrid;
    SDL_Texture* gridTexture;
    int score;
    Uint32 lastUpdate;
    int level;
    int linesCleared;
    bool isPaused;

    // Initialize SDL and create window
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cout << "SDL initialization failed: " << SDL_GetError() << endl;
            return false;
        }

        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            cout << "SDL_image initialization failed: " << IMG_GetError() << endl;
            return false;
        }

        if (TTF_Init() == -1) {
            cout << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
            return false;
        }

        window = SDL_CreateWindow("Tetris",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            cout << "Window creation failed: " << SDL_GetError() << endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            cout << "Renderer creation failed: " << SDL_GetError() << endl;
            return false;
        }

        font = TTF_OpenFont("fonts/PixelifySans-Medium.ttf", 24);
        if (!font) {
            cout << "Font loading failed: " << TTF_GetError() << endl;
            return false;
        }

        return true;
    }

    // Load texture from file
    SDL_Texture* loadTexture(const char* path) {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            cout << "Failed to load image: " << path << endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }

    // Load all game
    void loadTextures() {
        // Load UI textures
		TextureInfo tex;
        loadUITexture("Asset/Home/Title.png", nullptr, 285, 183);
        loadUITexture("Asset/Home/btnStart.png", "Asset/Home/btnStartHover.png", 380, 293);
        loadUITexture("Asset/Home/btnExit.png", "Asset/Home/btnExitHover.png", 380, 403);
        loadUITexture("Asset/Game/scoreBox.png", nullptr, 102, 180);

		SDL_Color textColor = {255, 255, 255, 255};
		SDL_Surface* pauseSurface = TTF_RenderText_Solid(font, "PAUSED", textColor);
		tex.normal = SDL_CreateTextureFromSurface(renderer, pauseSurface);
		tex.rect = {
			BOARD_X + (GRID_WIDTH * TILE_SIZE - pauseSurface->w) / 2,
			BOARD_Y + (GRID_HEIGHT * TILE_SIZE - pauseSurface->h) / 2,
			pauseSurface->w,
			pauseSurface->h
		};
		tex.hover = tex.normal;
		tex.isHovered = false;
		textures.push_back(tex);
		SDL_FreeSurface(pauseSurface);

        loadUITexture("Asset/Game/btnPause.png", "Asset/Game/btnPauseHover.png", 102, 395);
    }

    // Load single UI texture
    void loadUITexture(const char* normalPath, const char* hoverPath, int x, int y) {
        TextureInfo tex;
        tex.normal = loadTexture(normalPath);
        tex.hover = hoverPath ? loadTexture(hoverPath) : tex.normal;
        
        SDL_QueryTexture(tex.normal, nullptr, nullptr, &tex.rect.w, &tex.rect.h);
        tex.rect.x = x;
        tex.rect.y = y;
        tex.isHovered = false;
        
        textures.push_back(tex);
    }

    // Check collision between piece and grid
    bool checkCollision(int newX, int newY, const vector<vector<int>>& shape) {
        for (int i = 0; i < shape.size(); i++) {
            for (int j = 0; j < shape[i].size(); j++) {
                if (shape[i][j] == 0) continue;
                
                int gridX = newX + j;
                int gridY = newY + i;
                
                if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT ||
                    (gridY >= 0 && gameGrid[gridY][gridX].a != 0)) {
                    return true;
                }
            }
        }
        return false;
    }

    // Merge piece into grid
    void mergePiece() {
        if (!currentPiece) return;
        
        for (int i = 0; i < currentPiece->shape.size(); i++) {
            for (int j = 0; j < currentPiece->shape[i].size(); j++) {
                if (currentPiece->shape[i][j] == 1) {
                    int gridY = currentPiece->y + i;
                    int gridX = currentPiece->x + j;
                    if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH) {
                        gameGrid[gridY][gridX] = currentPiece->color;
                    }
                }
            }
        }
        
        delete currentPiece;
        currentPiece = nullptr;
    }

    // Clear completed lines
    void clearLines() {
        int cleared = 0;
        for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
            bool isLineFull = true;
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (gameGrid[y][x].a == 0) {
                    isLineFull = false;
                    break;
                }
            }
            
            if (isLineFull) {
                cleared++;
                for (int moveY = y; moveY > 0; moveY--) {
                    gameGrid[moveY] = gameGrid[moveY - 1];
                }
                gameGrid[0] = vector<SDL_Color>(GRID_WIDTH, {0, 0, 0, 0});
                y++; // Check same line again
            }
        }
        
        if (cleared > 0) {
            score += cleared * 100 * level;
            linesCleared += cleared;
            level = 1 + (linesCleared / 10);
        }
    }

    // Spawn new piece
    void spawnNewPiece() {
        TetrominoType type = static_cast<TetrominoType>(rand() % 7);
        currentPiece = new Tetromino(type);
        
        if (checkCollision(currentPiece->x, currentPiece->y, currentPiece->shape)) {
            gameState = GAMEOVER;
            delete currentPiece;
            currentPiece = nullptr;
        }
    }

    // Handle input
    void handleInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameState = CLOSE;
                return;
            }
            
            if (event.type == SDL_KEYDOWN && currentPiece && !isPaused) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!checkCollision(currentPiece->x - 1, currentPiece->y, currentPiece->shape))
                            currentPiece->x--;
                        break;
                    case SDLK_RIGHT:
                        if (!checkCollision(currentPiece->x + 1, currentPiece->y, currentPiece->shape))
                            currentPiece->x++;
                        break;
                    case SDLK_DOWN:
                        if (!checkCollision(currentPiece->x, currentPiece->y + 1, currentPiece->shape))
                            currentPiece->y++;
                        break;
                    case SDLK_UP:
                        currentPiece->rotate();
                        if (checkCollision(currentPiece->x, currentPiece->y, currentPiece->shape))
                            currentPiece->rotate(); // Rotate back if invalid
                        break;
                    case SDLK_SPACE:
                        while (!checkCollision(currentPiece->x, currentPiece->y + 1, currentPiece->shape))
                            currentPiece->y++;
                        break;
                    case SDLK_p:
                        isPaused = !isPaused;
                        break;
                }
            }
            
            if (event.type == SDL_MOUSEMOTION) {
                updateButtons(event.motion.x, event.motion.y);
            }
            
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                handleButtonClick(event.button.x, event.button.y);
            }
        }
    }

    // Update game state
    void update() {
        if (isPaused) return;
        
        if (currentPiece) {
            if (!checkCollision(currentPiece->x, currentPiece->y + 1, currentPiece->shape)) {
                currentPiece->y++;
            } else {
                mergePiece();
                clearLines();
                spawnNewPiece();
            }
        } else {
            spawnNewPiece();
        }
    }

    // Draw game state
    void draw() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw board background
        SDL_Rect boardRect = {BOARD_X, BOARD_Y, GRID_WIDTH * TILE_SIZE, GRID_HEIGHT * TILE_SIZE};
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &boardRect);

        // Draw grid
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        for (int i = 0; i <= GRID_WIDTH; i++) {
            SDL_RenderDrawLine(renderer,
                BOARD_X + i * TILE_SIZE, BOARD_Y,
                BOARD_X + i * TILE_SIZE, BOARD_Y + GRID_HEIGHT * TILE_SIZE);
        }
        for (int i = 0; i <= GRID_HEIGHT; i++) {
            SDL_RenderDrawLine(renderer,
                BOARD_X, BOARD_Y + i * TILE_SIZE,
                BOARD_X + GRID_WIDTH * TILE_SIZE, BOARD_Y + i * TILE_SIZE);
        }

        // Draw placed blocks
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (gameGrid[y][x].a != 0) {
                    SDL_Rect blockRect = {
                        BOARD_X + x * TILE_SIZE + 1,
                        BOARD_Y + y * TILE_SIZE + 1,
                        TILE_SIZE - 2,
                        TILE_SIZE - 2
                    };
                    SDL_SetRenderDrawColor(renderer,
                        gameGrid[y][x].r, gameGrid[y][x].g, gameGrid[y][x].b, 255);
                    SDL_RenderFillRect(renderer, &blockRect);
                }
			}
        }

        // Draw current piece
        if (currentPiece) {
            SDL_SetRenderDrawColor(renderer,
                currentPiece->color.r, currentPiece->color.g, currentPiece->color.b, 255);
            for (int i = 0; i < currentPiece->shape.size(); i++) {
                for (int j = 0; j < currentPiece->shape[i].size(); j++) {
                    if (currentPiece->shape[i][j] == 1) {
                        SDL_Rect blockRect = {
                            BOARD_X + (currentPiece->x + j) * TILE_SIZE + 1,
                            BOARD_Y + (currentPiece->y + i) * TILE_SIZE + 1,
                            TILE_SIZE - 2,
                            TILE_SIZE - 2
                        };
                        SDL_RenderFillRect(renderer, &blockRect);
                    }
                }
            }
        }

        // Draw UI
        drawUI();
        SDL_RenderPresent(renderer);
    }

    // Draw UI elements
    void drawUI() {
		for (int i = stateData.startIndex; i <= stateData.endIndex; i++) {
			if (i == 4 && !isPaused) {
				continue; // Hanya skip teks PAUSED saat tidak pause
			}
			SDL_RenderCopy(renderer, 
				textures[i].isHovered ? textures[i].hover : textures[i].normal, 
				nullptr, 
				&textures[i].rect);
		}
		if(gameState == GAME){
			SDL_Color textColor = {255, 255, 255, 255};
			char scoreText[32];
			sprintf(scoreText, "%07d", score);
			SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
			SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
			SDL_Rect scoreRect = {142, 287, 195, 50};
			SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
			SDL_FreeSurface(scoreSurface);
			SDL_DestroyTexture(scoreTexture);
		}
	
		// Khusus Pause Text hanya di-render saat Pause
		if (isPaused) {
			SDL_RenderCopy(renderer, textures[4].normal, nullptr, &textures[4].rect);
		}
    }

    // Update button hover states
    void updateButtons(int mouseX, int mouseY) {
        for (int i = stateData.startIndex; i <= stateData.endIndex; i++) {
            textures[i].isHovered = mouseX >= textures[i].rect.x && mouseX <= textures[i].rect.x + textures[i].rect.w && mouseY >= textures[i].rect.y && mouseY <= textures[i].rect.y + textures[i].rect.h;
        }
    }

    // Handle button clicks
    void handleButtonClick(int mouseX, int mouseY) {
        for (int i = stateData.startIndex; i <= stateData.endIndex; i++) {
            if (textures[i].isHovered) {
                switch (i) {
                    case 1: // Start button
                        gameState = GAME;
                        resetGame();
                        break;
                    case 2: // Exit button
                        gameState = CLOSE;
                        break;
                    case 5: // Pause button
                        isPaused = !isPaused;
                        break;
                }
            }
        }
    }

    // Reset game state
    void resetGame() {
        score = 0;
        level = 1;
        linesCleared = 0;
        isPaused = false;
        if (currentPiece) {
            delete currentPiece;
            currentPiece = nullptr;
        }
        gameGrid = vector<vector<SDL_Color>>(GRID_HEIGHT, vector<SDL_Color>(GRID_WIDTH, {0, 0, 0, 0}));
        lastUpdate = SDL_GetTicks();
    }

public:
    TetrisGame() : window(nullptr), renderer(nullptr), font(nullptr),
                   gameState(HOME), currentPiece(nullptr), score(0),
                   level(1), linesCleared(0), isPaused(false) {
        srand(time(0));
    }

    ~TetrisGame() {
        // Cleanup textures
        for (auto& tex : textures) {
            if (tex.normal != tex.hover) SDL_DestroyTexture(tex.hover);
            SDL_DestroyTexture(tex.normal);
        }
        
        if (currentPiece) delete currentPiece;
        if (font) TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    // Main game loop
    void run() {
        if (!initialize()) {
            cout << "Game initialization failed!" << endl;
            return;
        }

        loadTextures();
        resetGame();

        while (gameState != CLOSE) {
            switch (gameState) {
                case HOME:
					stateData = GameStateData(HOME);
                    runHomeScreen();
                    break;
                case GAME:
					stateData = GameStateData(GAME);
                    runGameLoop();
                    break;
                case GAMEOVER:
                    runGameOverScreen();
                    break;
                default:
                    break;
            }
        }
    }

private:
    // Run home screen
    void runHomeScreen() {
        while (gameState == HOME) {
            handleInput();
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            drawUI();
            SDL_RenderPresent(renderer);
            
            SDL_Delay(16);
        }
    }

    // Run main game loop
    void runGameLoop() {
        while (gameState == GAME) {
            handleInput();
            
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastUpdate > (1000 / level)) {
                update();
                lastUpdate = currentTime;
            }
            
            draw();
            SDL_Delay(16);
        }
    }

    // Run game over screen
    void runGameOverScreen() {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME OVER", textColor);
        SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        SDL_Rect gameOverRect = {
            BOARD_X + (GRID_WIDTH * TILE_SIZE - gameOverSurface->w) / 2,
            BOARD_Y + (GRID_HEIGHT * TILE_SIZE - gameOverSurface->h) / 2,
            gameOverSurface->w,
            gameOverSurface->h
        };

        Uint32 startTime = SDL_GetTicks();
        while (SDL_GetTicks() - startTime < 2000) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw();
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        SDL_FreeSurface(gameOverSurface);
        SDL_DestroyTexture(gameOverTexture);
        gameState = HOME;
    }
};

// Main function
int main(int argc, char* argv[]) {
    TetrisGame game;
    game.run();
    return 0;
}
