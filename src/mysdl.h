#include <SDL2/SDL.h>
#include <math.h>

#define WWIDTH 1280
#define WHEIGHT 720
#define MAXLENGTH 50
#define MAXLINE  200
#define PI  3.14159265358979323846

#ifndef _INS_
#define _INS_
struct instruction{
    char i[MAXLENGTH][MAXLINE];
    int num[MAXLINE];
    int cnt;
};
typedef struct instruction instruction;
#endif

/*Source: Neill's neillsdl2.h*/
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
   SDL_Texture *display;
};
typedef struct SDL_Simplewin SDL_Simplewin;

struct line{
    int cx1;
    int cx2;
    int cy1;
    int cy2;
};
typedef struct line line;

struct vector{
    double length;
    double angle;
};
typedef struct vector vector;

struct color{
    int red;
    int green;
    int blue;
    int colorcnt;
};
typedef struct color color;

struct sdlData{
    SDL_Simplewin sw;
    line l;
    vector v;
    color c;
};
typedef struct sdlData sdlData;

/*Mostly from Neill's neillsdl2.h*/
void getSdlInit(SDL_Simplewin *sw);
void getSdlEvents(SDL_Simplewin *sw);
void getUpdateScreen(SDL_Simplewin *sw);

void getCoordinate(line* l, vector* v);
void getRainbow(color* c, int i, int valve);
void colorStuff(SDL_Simplewin *sw, color* c, int valve);

void executeIns(instruction *ins, SDL_Simplewin *sw, line *l, vector *v);

/*Mostly from Neill's neillsdl2.h*/
void getSdlInit(SDL_Simplewin *sw)
{
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "\nUnable to initiallize SDL\n");
        SDL_Quit();
        exit(1);
    }
    sw->finished = 0;
    sw->win = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, WWIDTH, WHEIGHT,SDL_WINDOW_SHOWN); 
    if(sw->win == NULL){
        fprintf(stderr, "\nUnable to initiallize SDL Window\n");
        SDL_Quit();
        exit(1);
    }
    sw->renderer = SDL_CreateRenderer(sw->win, -1,
        SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
   if(sw->renderer == NULL){
        fprintf(stderr, "\nUnable to initiallize SDL Renderer\n");
        SDL_Quit();
        exit(1);
   }
   SDL_SetRenderDrawBlendMode(sw->renderer,SDL_BLENDMODE_BLEND);
   sw->display = SDL_CreateTexture(sw->renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, WWIDTH, WHEIGHT);
   if(sw->display == NULL){
        fprintf(stderr, "\nUnable to initiallize SDL Texture\n");
        SDL_Quit();
        exit(1);
   }
   SDL_SetRenderTarget(sw->renderer, sw->display);
   SDL_SetRenderDrawColor(sw->renderer, 0, 0, 0, 255);
   SDL_RenderClear(sw->renderer);
}

void getSdlEvents(SDL_Simplewin *sw)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {      
        switch (event.type){
          case SDL_QUIT:
          case SDL_MOUSEBUTTONDOWN:
             sw->finished = 1;
        }
    }
}
void getUpdateScreen(SDL_Simplewin *sw)
{
    SDL_SetRenderTarget(sw->renderer, NULL);
    SDL_RenderCopy(sw->renderer, sw->display, NULL, NULL);
    SDL_RenderPresent(sw->renderer);
    SDL_SetRenderTarget(sw->renderer, sw->display);
}

void colorStuff(SDL_Simplewin *sw, color* c, int valve)
{
    c->colorcnt = c->colorcnt % (valve+1);
    getRainbow(c, c->colorcnt, valve);
    SDL_SetRenderDrawColor(sw->renderer, c->red, c->green, c->blue, SDL_ALPHA_OPAQUE);
    c->colorcnt++;
}

void getCoordinate(line* l, vector* v)
{
    l->cx2 = l->cx1 + sin(v->angle) * v->length + 0.5;
    l->cy2 = l->cy1 - cos(v->angle) * v->length + 0.5;
}

void getRainbow(color* c, int i, int valve)
{
    int range = valve / 6;
    if(i<=range){
        c->red = 255;
        c->green = i * 255 / range;
        c->blue = 0;
    }
    if(i > range && i <= range * 2){
        c->green = 255;
        c->red = 255 - ((i-range) * 255 / range);
        c->blue = 0;
    }
    if(i > range * 2 && i <= range * 3){
        c->green = 255;
        c->blue = (i-range*2) * 255 / range;
        c->red = 0;
    }
    if(i > range * 3&& i <= range * 4){
        c->blue = 255;
        c->green = 255 - ((i-range*3) * 255 / range);
        c->red = 0;
    }
    if(i > range * 4 && i <= range * 5){
        c->blue = 255;
        c->red = (i-range*4) * 255 / range;
        c->green = 0;
    }
    if(i > range * 5 && i <= range * 6){
        c->red = 255;
        c->blue = 255 - ((i-range*5) * 255 / range);
        c->green = 0;
    }
}

void executeIns(instruction *ins, SDL_Simplewin *sw, line *l, vector *v)
{
    int j;
    for(j = 0; j < ins->cnt; j++){
        if(strcmp(ins->i[j],"LT")==0){
            v->angle += (ins->num[j]/360.0 * 2.0 * PI);
        }
        if(strcmp(ins->i[j],"RT")==0){
            v->angle -= (ins->num[j]/360.0 * 2.0 * PI);
        }
        if(strcmp(ins->i[j],"FD")==0){
            v->length = ins->num[j];
            getCoordinate(l, v);
            SDL_RenderDrawLine(sw->renderer, l->cx1, l->cy1, l->cx2, l->cy2);
            l->cx1 = l->cx2;
            l->cy1 = l->cy2;
            getUpdateScreen(sw);
            SDL_Delay(0.02);
        }
    }
    getUpdateScreen(sw);
    getSdlEvents(sw);
}
