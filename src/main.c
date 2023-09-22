#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <stdbool.h>
#include <time.h>
#include <stdio.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int BALL_SIZE = 10;

typedef struct Ball {
    float x, y;
    float speedX, speedY;
    int size;
} Ball;

const float MAX_BALL_SPEED = 600.f;
float BALL_SPEED = 130.f;

typedef struct Player {
    int score;
    float y;
} Player;

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;
float PLAYER_SPEED = 150.f;

Player player;
Player aiPlayer;

Ball ball;
bool served = false;

SDL_Window* window;
SDL_Renderer* renderer;

static bool init(void);
static void update(float);
static void stop(void);

static bool colliding(Ball*, float, float);

static Ball create_ball(int);
static void update_ball(Ball*, float);
static void render_ball(const Ball*);

static Player create_player(void);
static void update_players(float);
static void render_players(void);

int main() {
    srand(time(NULL));
    atexit(stop);
    
    if (!init()) {
        exit(1);
    }

    bool quit = false;
    SDL_Event event;
    Uint32 lastTick = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        Uint32 tick = SDL_GetTicks();
        Uint32 dTicks = tick - lastTick;
        float elapsed = dTicks / 1000.f;
        update(elapsed);
        lastTick = tick;
    }
    exit(0);
}

static bool init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    ball = create_ball(BALL_SIZE);
    player = create_player();
    aiPlayer = create_player();
    return true;
}

static void update(float elapsed) {
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);

    update_ball(&ball, elapsed);
    render_ball(&ball);
 
    update_players(elapsed);
    render_players();

    SDL_RenderPresent(renderer);
}

static void stop(void) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

static bool colliding(Ball* ball, float x, float y) {
    //x += BALL_SIZE / 2;
    //y += BALL_SIZE / 2;
    return ball->x < x + PLAYER_WIDTH &&
        ball->x + ball->size > x &&
        ball->y < y + PLAYER_HEIGHT &&
        ball->size + ball->y > y;
}

static bool rand_bool(void) {
    return rand() % 2;
}

static Ball create_ball(int size) {
    Ball b = {
        .x = WINDOW_WIDTH / 2.f - size / 2.f,
        .y = WINDOW_HEIGHT / 2.f - size / 2.f,
        .size = size,
        .speedX = BALL_SPEED * (rand_bool() ? 1 : -1),
        .speedY = BALL_SPEED * (rand_bool() ? 1 : -1)
    };
    return b; 
}

static void render_ball(const Ball* ball) {
    int halfSize = ball->size / 2;
    SDL_Rect rect = {
        .x = ball->x - halfSize,
        .y = ball->y - halfSize,
        .w = ball->size,
        .h = ball->size
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

static void update_ball(Ball* ball, float elapsed) {
    if (!served) return;

    ball->x += ball->speedX * elapsed;
    ball->y += ball->speedY * elapsed;
    
    if (ball->x < BALL_SIZE / 2 || ball->x > WINDOW_WIDTH - BALL_SIZE / 2) {
        ball->x = WINDOW_WIDTH / 2 - ball->size / 2,
        ball->y = WINDOW_HEIGHT / 2 - ball->size / 2,
        ball->speedX = BALL_SPEED * (rand_bool() ? 1 : -1);
        ball->speedY = BALL_SPEED * (rand_bool() ? 1 : -1);
        served = !served;
        aiPlayer.y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2;
        player.y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2;
        
        if (BALL_SPEED < MAX_BALL_SPEED) {
            BALL_SPEED *= 2.f;
            PLAYER_SPEED *= 1.5f;
        }
    }

    if (ball->y < BALL_SIZE / 2 || ball->y > WINDOW_HEIGHT - BALL_SIZE / 2) {
        ball->speedY *= -1;
    }

    if (colliding(ball, PLAYER_MARGIN, aiPlayer.y) || colliding(ball, WINDOW_WIDTH - PLAYER_MARGIN - PLAYER_WIDTH, player.y)) {
        ball->speedX *= -1;
    }
}

static Player create_player(void) {
    Player p = {
        .score = 0,
        .y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2
    };
    return p;
}

static void update_players(float elapsed) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    if (keystate[SDL_SCANCODE_SPACE]) {
        served = true;
    }

    if (!served) return;

    if (keystate[SDL_SCANCODE_UP]) {
        player.y -= PLAYER_SPEED * elapsed;
    }

    if (keystate[SDL_SCANCODE_DOWN]) {
        player.y += PLAYER_SPEED * elapsed;
    }

    aiPlayer.y = ball.y - PLAYER_HEIGHT / 2;
}

static void render_players(void) {
    SDL_Rect left = {
        .x = PLAYER_MARGIN,
        .y = aiPlayer.y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    SDL_Rect right = {
        .x = WINDOW_WIDTH - PLAYER_MARGIN - PLAYER_WIDTH,
        .y = player.y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
}
