#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* args[]) {
    // Inisialisasi SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "Gagal menginisialisasi SDL: " << SDL_GetError() << endl;
        return -1;
    }

    // Inisialisasi SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cerr << "Gagal menginisialisasi SDL_image: " << IMG_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    // Buat window
    SDL_Window* window = SDL_CreateWindow("SDL Texture Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Gagal membuat window: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    // Buat renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Gagal membuat renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Muat gambar sebagai texture
    SDL_Surface* imageSurface = IMG_Load("image.jpg"); // Pastikan file image.png ada di folder yang sama
    if (!imageSurface) {
        cerr << "Gagal memuat gambar: " << IMG_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Konversi ke SDL_Texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);
    if (!texture) {
        cerr << "Gagal membuat texture: " << SDL_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Loop utama
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Render background putih
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render gambar
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Tampilkan render ke layar
        SDL_RenderPresent(renderer);
    }

    // Hapus resource
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
