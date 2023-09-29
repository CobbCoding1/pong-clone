#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

int WIDTH = 1280;
int HEIGHT = 720;

typedef struct{
    int speed_x;
    int speed_y;
} BallSpeed;

int main(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("ERROR: Could not initialize: %s\n", SDL_GetError());
    }
    SDL_Window *win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

    SDL_Surface *surface;
    surface = IMG_Load("texture.jpeg");

    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);

    SDL_FreeSurface(surface);

    SDL_Rect player;
    SDL_Rect dest2;
    SDL_Rect ball;

    SDL_QueryTexture(tex, NULL, NULL, &player.w, &player.h);
    SDL_QueryTexture(tex, NULL, NULL, &dest2.w, &dest2.h);
    SDL_QueryTexture(tex, NULL, NULL, &ball.w, &ball.h);

    // Initialize player size and location
    player.w /= 8;
    player.h /= 2;

    player.x = 0 + 20;
    player.y = (HEIGHT - player.h) / 2;

    // Initialize enemy size and location
    dest2.w /= 8;
    dest2.h /= 2;

    dest2.x = (WIDTH - dest2.w) - 20;
    dest2.y = (HEIGHT - dest2.h) / 2;

    // Initialize ball size and location 
    ball.w /= 8;
    ball.h /= 8;

    ball.x = (WIDTH - ball.w) / 2;
    ball.y = (HEIGHT - ball.h) / 2;

    bool close = false;

    int speed = 800;
    BallSpeed ball_speed;
    ball_speed.speed_x = 300;
    ball_speed.speed_y = 0;

    while(!close){
        ball.x += ball_speed.speed_x / 30;
        ball.y += ball_speed.speed_y / 30;
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    close = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                            player.y -= speed / 30;
                            dest2.y -= speed / 30;
                            break;
                        case SDL_SCANCODE_S:
                            player.y += speed / 30;
                            dest2.y += speed / 30;
                            break;
                        default:
                            break;
                    }
            }
        }

        // Clamp player boundries
        if(player.x + player.w > WIDTH){
            player.x = WIDTH - player.w;
        }

        if(player.x < 0){
            player.x = 0;
        }

        if(player.y + player.h > HEIGHT){
            player.y = HEIGHT - player.h;
        }

        if(player.y < 0){
            player.y = 0;
        }

        // handle ball bouncing
        if(ball.x + ball.w > WIDTH || (ball.x + ball.w) < (player.w + player.x)){
            ball_speed.speed_x = -ball_speed.speed_x;
        }

        if((ball.x - ball.w) < (player.w - player.x) && ((ball.y - ball.h) > (player.y - player.h) && (ball.y + ball.h) < (player.y + player.h))){
            ball_speed.speed_x = -ball_speed.speed_x;
            ball_speed.speed_y = 300; 
        }

        if(((ball.x + ball.w) > (dest2.w + dest2.x)-ball.w) && ((ball.y - ball.h) < (dest2.y + dest2.h) + dest2.h) && ((ball.y + ball.h) > (dest2.y + dest2.h) - dest2.h)){
            ball_speed.speed_x = -ball_speed.speed_x;
            ball_speed.speed_y = -300; 
        }

        if(ball.x < 0){
            ball_speed.speed_x = -ball_speed.speed_x;
        }

        if(ball.y + ball.h > HEIGHT){
            ball_speed.speed_y = -ball_speed.speed_y;
        }

        if(ball.y < 0){
            ball_speed.speed_y = -ball_speed.speed_y;
        }

        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, tex, NULL, &player);
        SDL_RenderCopy(rend, tex, NULL, &dest2);
        SDL_RenderCopy(rend, tex, NULL, &ball);
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
