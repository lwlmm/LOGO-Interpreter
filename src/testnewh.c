#include "mysdl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAXLENGTH  50
#define MAXLINE   200
#define COLORVALVE 48

struct turtle{
    char words[MAXLINE][MAXLENGTH];
    int linecounter;
    int blockcounter;
};
typedef struct turtle turtle;

struct variablePool{
    char varName[MAXLENGTH][MAXLENGTH];
    double  varNum[MAXLINE];
    bool varStatus[MAXLINE];
    int  varCnt;
    double fd;
    double lt;
    double rt;
};
typedef struct variablePool variablePool;

struct stackSpace{
    double operand[MAXLENGTH];
    int cnt;
};
typedef struct stackSpace stackSpace;

void sdlDataInit(sdlData *sdl)
{
    sdl->l.cx1 = WWIDTH / 2;
    sdl->l.cy1 = WHEIGHT / 2;
    sdl->v.angle = 0;
}

void refreshCoordinate(sdlData *sdl)
{
    sdl->l.cx1 = sdl->l.cx2;
    sdl->l.cy1 = sdl->l.cy2;
}

int main()
{
    int i;
    sdlData *sdl;
    sdl = (sdlData*)malloc(sizeof(sdlData));
    getSdlInit(&(sdl->sw));
    sdl->c.colorcnt = 0;
    do{
        sdlDataInit(sdl);
        colorStuff(&(sdl->sw), &(sdl->c), COLORVALVE);
        for(i = 1; i <= 50; i++){
            sdl->v.length = i;
            sdl->v.angle = i;
            getCoordinate(&(sdl->l), &(sdl->v));
            SDL_RenderDrawLine(sdl->sw.renderer, sdl->l.cx1, sdl->l.cy1, sdl->l.cx2, sdl->l.cy2);
            refreshCoordinate(sdl);
            getUpdateScreen(&(sdl->sw));
            SDL_Delay(0.2);
            getUpdateScreen(&(sdl->sw));
            getSdlEvents(&(sdl->sw));
        }
    }while(!sdl->sw.finished);
    SDL_Quit();
    atexit(SDL_Quit);
    free(sdl);
}