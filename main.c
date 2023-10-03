#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <ft2build.h>
#include FT_FREETYPE_H


int WIDTH = 1280;
int HEIGHT = 720;

typedef struct{
    SDL_Rect rect;
    int score;
} Paddle;

typedef struct{
    int speed_x;
    int speed_y;
} BallSpeed;

bool handle_ball_bouncing_off_paddle(SDL_Rect ball, SDL_Rect paddle){
    if((ball.x + ball.w) > (paddle.x) && (ball.x) < (paddle.x + paddle.w) && (ball.y + ball.h) > (paddle.y) && (ball.y) < (paddle.y + paddle.h)){
        return true;
    }

    return false;
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect){
    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

int main(){
    srand(time(NULL));

    Paddle player;
    player.score = 0;
    Paddle dest2;
    dest2.score = 0;
    SDL_Rect ball;


    // SDL INIT
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("ERROR: Could not initialize: %s\n", SDL_GetError());
    }
    SDL_Window *win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

_reset:
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);



    // SDL TTF
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/ubuntu/UbuntuMono-R.ttf", 64);
    if(!font){
        fprintf(stderr, "ERROR: Could not load font\n");
        exit(1);
    }

    SDL_Rect word1;
    SDL_Rect word2;
    SDL_Texture *word_texture1;
    SDL_Texture *word_texture2;

    SDL_Rect net_rect;

    SDL_Surface *surface;
    SDL_Surface *net_surface;
    surface = IMG_Load("texture.jpeg");
    net_surface = IMG_Load("net.png");

    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_Texture *net_tex = SDL_CreateTextureFromSurface(rend, net_surface);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(net_surface);

    SDL_QueryTexture(tex, NULL, NULL, &player.rect.w, &player.rect.h);
    SDL_QueryTexture(tex, NULL, NULL, &dest2.rect.w, &dest2.rect.h);
    SDL_QueryTexture(tex, NULL, NULL, &ball.w, &ball.h);


    SDL_QueryTexture(net_tex, NULL, NULL, &net_rect.w, &net_rect.h);


    net_rect.w = 20;
    net_rect.h = HEIGHT;
    net_rect.x = WIDTH / 2 + 10;
    net_rect.y = 0;

    // Initialize player size and location
    player.rect.w /= 8;
    player.rect.h /= 2;

    player.rect.x = 0 + 20;
    player.rect.y = (HEIGHT - player.rect.h) / 2;

    // Initialize enemy size and location
    dest2.rect.w /= 8;
    dest2.rect.h /= 2;

    dest2.rect.x = (WIDTH - dest2.rect.w) - 20;
    dest2.rect.y = (HEIGHT - dest2.rect.h) / 2;

    // Initialize ball size and location 
    ball.w /= 8;
    ball.h /= 8;

    ball.x = (WIDTH - ball.w) / 2;
    ball.y = (HEIGHT - ball.h) / 2;

    bool close = false;
    bool reset = false;

    int speed = 1000;
    BallSpeed ball_speed;
    ball_speed.speed_x = 400;
    ball_speed.speed_y = 0;
    ball_speed.speed_y = rand() % (200 + 1 - -200) + -200;

    while(!close){
        ball.x += ball_speed.speed_x / 30;
        ball.y += ball_speed.speed_y / 30;
        SDL_Event event;

        if(player.score >= 11 || dest2.score >= 11){
            reset = false;
            break;
        }


        char *player_score = malloc(sizeof(char) * 4);
        char *enemy_score = malloc(sizeof(char) * 4);
        sprintf(player_score, "%d", player.score);
        sprintf(enemy_score, "%d", dest2.score);
        get_text_and_rect(rend, (WIDTH/2) - 100, 0, player_score, font, &word_texture1, &word1);
        get_text_and_rect(rend, (WIDTH/2) + 100, 0, enemy_score, font, &word_texture2, &word2);

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    close = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                            player.rect.y -= speed / 30;
                            break;
                        case SDL_SCANCODE_S:
                            player.rect.y += speed / 30;
                            break;
                        default:
                            break;
                    }
            }
        }

        if(dest2.rect.y < ball.y){
            dest2.rect.y += speed / 100;
        }

        if(dest2.rect.y > ball.y){
            dest2.rect.y -= speed / 100;
        }


        // Clamp player boundries
        if(player.rect.x + player.rect.w > WIDTH){
            player.rect.x = WIDTH - player.rect.w;
        }

        if(player.rect.x < 0){
            player.rect.x = 0;
        }

        if(player.rect.y + player.rect.h > HEIGHT){
            player.rect.y = HEIGHT - player.rect.h;
        }

        if(player.rect.y < 0){
            player.rect.y = 0;
        }

        // handle ball bouncing
        if(ball.x + ball.w > WIDTH){
            ball_speed.speed_x = -ball_speed.speed_x;
            player.score += 1;
            printf("Player: %d, Enemy: %d\n", player.score, dest2.score);
            reset = true;
            break;
        }

        if(handle_ball_bouncing_off_paddle(ball, player.rect)){
            printf("ball y: %d, player y: %d, player height: %d, %f\n", ball.y, player.rect.y, player.rect.h, (float)(ball.y - player.rect.y) / (float)player.rect.h);
            ball_speed.speed_x = -ball_speed.speed_x;

            float speed_y = (float)(ball.y - player.rect.y) / (float)player.rect.h;

            ball_speed.speed_y = (speed_y * (float)1000);
        }

        if(handle_ball_bouncing_off_paddle(ball, dest2.rect)){
            ball_speed.speed_x = -ball_speed.speed_x;

            float speed_y = (float)(ball.y - dest2.rect.y) / (float)dest2.rect.h;

            ball_speed.speed_y = -(speed_y * (float)1000);
        }
        
        if(ball.x < 0){
            ball_speed.speed_x = -ball_speed.speed_x;
            dest2.score += 1;
            printf("Player: %d, Enemy: %d\n", player.score, dest2.score);
            reset = true;
            break;
        }

        if(ball.y + ball.h > HEIGHT){
            ball_speed.speed_y = -ball_speed.speed_y;
        }

        if(ball.y < 0){
            ball_speed.speed_y = -ball_speed.speed_y;
        }


        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
        SDL_RenderPresent(rend);
        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, tex, NULL, &player.rect);
        SDL_RenderCopy(rend, tex, NULL, &dest2.rect);
        SDL_RenderCopy(rend, tex, NULL, &ball);
        SDL_RenderCopy(rend, word_texture1, NULL, &word1);
        SDL_RenderCopy(rend, word_texture2, NULL, &word2);
        SDL_RenderCopy(rend, net_tex, NULL, &net_rect);
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
    }



    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(word_texture1);
    SDL_DestroyTexture(word_texture2);
    SDL_DestroyTexture(net_tex);
    SDL_DestroyRenderer(rend);

    if(reset){
        goto _reset;
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
