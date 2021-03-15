#include "parser.h"
#include "mysdl.h"

int main(int argc, char* argv[])
{
    FILE *fp;
    turtle t;
    instruction ins;
    SDL_Simplewin sw;
    line l;
    vector v;
    color c;
    c.colorcnt = 0;
    l.cx1 = 640;
    l.cy1 = 360;
    v.angle = 0;
    getSdlInit(&sw);

    checkArgc(argc);
    turtleInit(&t);
    insInit(&ins);
    openFile(fp, argv[1], &t);

    parseProgram(&t, &ins);

    do{ 
        l.cx1 = 640;
        l.cy1 = 360;
        v.angle = 0;
        colorStuff(&sw, &c, 48);
        executeIns(&ins, &sw, &l, &v, &c);
    }while(!sw.finished);
    
    SDL_Quit();
    atexit(SDL_Quit);

    return 0;
}