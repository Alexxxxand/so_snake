#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
};

std::vector<Point> snake;
Point food;
int dirX = 1, dirY = 0;
bool running = true;

const int cols = 20;
const int rows = 20;
const int cellSize = 20;
const int width = cols * cellSize;
const int height = rows * cellSize;

void place_food() {
    food.x = rand() % cols;
    food.y = rand() % rows;
}

void reset_game() {
    snake.clear();
    snake.push_back({cols / 2, rows / 2});
    dirX = 1;
    dirY = 0;
    place_food();
}

extern "C" __attribute__((constructor))
void start_game() {
    srand(time(nullptr));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Injected Snake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    reset_game();

    Uint32 lastMoveTime = SDL_GetTicks();
    const Uint32 moveDelay = 150;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: if (dirY != 1) { dirX = 0; dirY = -1; } break;
                    case SDLK_s: if (dirY != -1) { dirX = 0; dirY = 1; } break;
                    case SDLK_a: if (dirX != 1) { dirX = -1; dirY = 0; } break;
                    case SDLK_d: if (dirX != -1) { dirX = 1; dirY = 0; } break;
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - lastMoveTime > moveDelay) {
            lastMoveTime = now;

            Point newHead = snake.front();
            newHead.x += dirX;
            newHead.y += dirY;

            if (newHead.x < 0 || newHead.x >= cols || newHead.y < 0 || newHead.y >= rows) {
                reset_game();
                goto render;
            }

            for (const auto& segment : snake) {
                if (segment.x == newHead.x && segment.y == newHead.y) {
                    reset_game();
                    goto render;
                }
            }

            snake.insert(snake.begin(), newHead);

            if (newHead.x == food.x && newHead.y == food.y) {
                place_food();
            } else {
                snake.pop_back();
            }
        }

        render:
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = {food.x * cellSize, food.y * cellSize, cellSize, cellSize};
        SDL_RenderFillRect(renderer, &foodRect);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& segment : snake) {
            SDL_Rect r = {segment.x * cellSize, segment.y * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
