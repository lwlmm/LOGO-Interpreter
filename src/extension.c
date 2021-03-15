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

/*Simple stack machine to deal with polish*/
void pushStack(stackSpace *stack, double t);
double popStack(stackSpace *stack);

/*Basic parser functions and initializtion*/
void myAlert(char *s);
void checkArgc(int i);
void turtleInit(turtle *t);
void poolInit(variablePool *var);
void sdlDataInit(sdlData *sdl);
void openFile(FILE *fp, char *s, turtle *t);
void stackInit(stackSpace *stack);

/*Variable issue*/
void initNewVar(variablePool *var, char *s);
bool assignVar(variablePool *var, char *s, double i);

/*Three layers of parse program*/
void parseInterProg(turtle *t, variablePool *var, sdlData *sdl);
void parseInterBody(turtle *t, variablePool *var, sdlData *sdl);
void parseInterLine(turtle *t, variablePool *var, sdlData *sdl);

/*Funtion to deal with loop*/
void parseInterRange(turtle *t, variablePool *var, sdlData *sdl, int tail, bool cntline);
void executeLoop(turtle *t, variablePool *var, sdlData *sdl);
void loopCheck(turtle *t);

/*Change directions or draw line*/
void executeFDline(turtle *t, variablePool *var, sdlData *sdl);
void executeLTline(turtle *t, variablePool *var, sdlData *sdl);
void executeRTline(turtle *t, variablePool *var, sdlData *sdl);
void executeFD(double fd, sdlData *sdl);
void executeLT(double lt, sdlData *sdl);
void executeRT(double rt, sdlData *sdl);

/*Variable setting and polish dealing*/
void executeSet(turtle *t, variablePool *var);
int getSemiColumn(turtle *t);
void setCheck(turtle *t);
double executePolish(turtle *t, variablePool *var, int cnt);

/*Other infrastructures*/
void numberChecker(turtle *t, variablePool *var, double *num);
int findMatchBrace(turtle *t, int head);
void refreshCoordinate(sdlData *sdl);
bool returnVarNum(variablePool *var, char *s, double *num);
void myInt2String(int a, char *s);
int myString2Int(char *s);
double myString2Double(char *s);


int main(int argc, char* argv[])
{
    FILE *fp;
    turtle t;
    variablePool var;
    sdlData *sdl;

    fp = (FILE*)malloc(sizeof(FILE*));
    sdl = (sdlData*)malloc(sizeof(sdlData));
    getSdlInit(&(sdl->sw));
    sdl->c.colorcnt = 0;

    do{
        sdlDataInit(sdl);
        checkArgc(argc);
        turtleInit(&t);
        poolInit(&var);
        openFile(fp, argv[1], &t);
        colorStuff(&(sdl->sw), &(sdl->c), COLORVALVE);
        parseInterProg(&t, &var, sdl);
    }while(!sdl->sw.finished);
    
    SDL_Quit();
    atexit(SDL_Quit);
    free(fp);
    free(sdl);

    return 0;
}

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||       Initialization       ||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/*Showing one sentence and exit*/
void myAlert(char *s)
{
    fprintf(stderr,"%s",s);
    exit(EXIT_FAILURE);
}

/*Input format check*/
void checkArgc(int i)
{
    if(i == 2){
        return;
    }
    else myAlert("Wrong Input Format!\n");
}

/*Initialization of the whole language structure*/
void turtleInit(turtle *t)
{
    int i;
    for(i = 0; i < MAXLINE; i++){
        strcpy(t->words[i], "\0");
    }
    t->linecounter = 0;
    t->blockcounter = 0;
}

/*Set the variable pool*/
void poolInit(variablePool *var)
{
    int i;
    for(i = 0; i < MAXLENGTH; i++){
        strcpy(var->varName[i], "\0");
        var->varNum[i] = 0;
        var->varStatus[i] = false;
    }
    var->varCnt = 0;
    var->fd = 0;
    var->lt = 0;
    var->rt = 0;
}

/*Set the starting point to the center of the showing screen*/
void sdlDataInit(sdlData *sdl)
{
    sdl->l.cx1 = WWIDTH / 2;
    sdl->l.cy1 = WHEIGHT / 2;
    sdl->v.angle = 0;
}

/*Open file with grammar check*/
void openFile(FILE *fp, char *s, turtle *t)
{
    char out[MAXLENGTH];
    char temp[MAXLENGTH];
    int i = 0;
    strcpy(out, "Cannot open file: ");
    strcat(out, s);
    strcat(out, " !\n");
    if(!(fp = fopen(s,"r"))){
        myAlert(out);
    }
    while(fscanf(fp, "%s", t->words[i]) == 1){
        i++;
        if(i == MAXLINE){
            if(fscanf(fp, "%s", temp) == 1){
                myAlert("Program is over 100 lines!\n");
            }
        }
    }
    fclose(fp);
}

void stackInit(stackSpace *stack)
{
    int i;
    for(i = 0; i < MAXLENGTH; i++){
        stack->operand[i] = 0;
        stack->cnt = 0;
    }
}

/*Start of using a new variable name*/
void initNewVar(variablePool *var, char *s)
{
    int i;
    for(i = 0; i < MAXLINE; i++){
        if(var->varStatus[i] == true){
            if(strcmp(var->varName[i], s) == 0){
                return;
            }
        }
    }
    strcpy(var->varName[var->varCnt],s);
    var->varStatus[var->varCnt] = true;
    var->varCnt++;
}

/*Assigning value*/
bool assignVar(variablePool *var, char *s, double i)
{
    int j;
    for(j = 0; j < MAXLINE; j++){
        if(var->varStatus[j] == true){
            if(strcmp(var->varName[j], s) == 0){
                var->varNum[j] = i;
                return true;
            }
        }
    }
    return false;
}

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*||||||||||||||   Turtle process and run   ||||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/*Deal with the outer curly braces*/
void parseInterProg(turtle *t, variablePool *var, sdlData *sdl)
{
    if(strcmp(t->words[t->blockcounter],"{")){
        myAlert("Not the right begin!\n");
    }
    t->linecounter++;
    t->blockcounter++;
    parseInterBody(t, var, sdl);
}

/*Parsing within the body*/
void parseInterBody(turtle *t, variablePool *var, sdlData *sdl)
{
    if(t->blockcounter == findMatchBrace(t, 0)){
        return;
    }
    parseInterLine(t, var, sdl);
    t->linecounter++;
    parseInterBody(t, var, sdl);
}

/*Parse through each line and execute them*/
void parseInterLine(turtle *t, variablePool *var, sdlData *sdl)
{
    char out[MAXLENGTH];
    char num[MAXLENGTH];
    myInt2String(t->linecounter+1, num);
    strcpy(out, "Wrong instruction: line");
    strcat(out, num);
    strcat(out, "\n");
    if(strcmp(t->words[t->blockcounter],"FD")==0){
        executeFDline(t, var, sdl);
        t->blockcounter++;
    }
    else if(strcmp(t->words[t->blockcounter],"RT")==0){
        executeRTline(t, var, sdl);
        t->blockcounter++;
    }
    else if(strcmp(t->words[t->blockcounter],"LT")==0){
        executeLTline(t, var, sdl);
        t->blockcounter++;
    }
    else if(strcmp(t->words[t->blockcounter],"DO")==0){
        executeLoop(t, var, sdl);
        t->blockcounter++;
    }
    else if(strcmp(t->words[t->blockcounter], "SET")==0){
        executeSet(t, var);
        t->blockcounter++;
    }
    else{
        myAlert(out);
    }
}

/*Loop parsing*/
void parseInterRange(turtle *t, variablePool *var, sdlData *sdl, int tail, bool cntline)
{
    if(t->blockcounter == tail){
        return;
    }
    parseInterLine(t, var, sdl);
    if(cntline){
        t->linecounter++;
    }
    parseInterRange(t, var, sdl, tail, cntline);
}

void executeLoop(turtle *t, variablePool *var, sdlData *sdl)
{
    int loopHeadBlock, loopTailBlock;
    int loopStart, loopEnd;
    int loopLength;
    int i;
    bool line;
    char name[MAXLENGTH];
    loopCheck(t);
    line = true;
    t->linecounter++;
    t->blockcounter++;
    loopHeadBlock = t->blockcounter;
    strcpy(name, t->words[loopHeadBlock]);
    initNewVar(var, name);
    loopTailBlock = findMatchBrace(t, loopHeadBlock+6);
    loopStart = myString2Int(t->words[loopHeadBlock+2]);
    loopEnd = myString2Int(t->words[loopHeadBlock+4]);
    loopLength = loopTailBlock - loopHeadBlock - 6;
    t->blockcounter += 6;
    for(i = loopStart; i <= loopEnd; i++){
        assignVar(var, name, (double)i);
        parseInterRange(t, var, sdl, loopTailBlock, line);
        t->blockcounter -= loopLength;
        line = false;
    }
    t->blockcounter += loopLength;
}

void loopCheck(turtle *t)
{
    int fromBlock, i;
    fromBlock = t->blockcounter + 2;
    if(strlen(t->words[fromBlock-1]) != 1||
        t->words[fromBlock][0] < 'A'||
        t->words[fromBlock][0] > 'Z'){
            myAlert("Wrong varname in loop!\n");
    }
    if(strcmp("FROM",t->words[fromBlock])){
        myAlert("Grammar Issue in loop!\n");
    }
    if(strcmp("TO",t->words[fromBlock+2])){
        myAlert("Grammar Issue in loop!\n");
    }
    i = 0;
    while(t->words[fromBlock+1][i]!='\0'){
        if( t->words[fromBlock+1][i] > '9'||
            t->words[fromBlock+1][i] < '0'){
                myAlert("Wrong number in loop!\n");
        }
        i++;
    }
    i = 0;
    while(t->words[fromBlock+3][i]!='\0'){
        if( t->words[fromBlock+3][i] > '9'||
            t->words[fromBlock+3][i] < '0'){
                myAlert("Wrong number in loop!\n");
        }
        i++;
    }
}

/*Set parsing*/
void executeSet(turtle *t, variablePool *var)
{
    int cnt;
    cnt = getSemiColumn(t);
    setCheck(t);
    initNewVar(var, t->words[t->blockcounter+1]);
    assignVar(var, t->words[t->blockcounter+1], executePolish(t, var, cnt));
    t->linecounter++;
    t->blockcounter += cnt;
}

void setCheck(turtle *t)
{
    int varBlock = t->blockcounter+1;
    if(strlen(t->words[varBlock]) != 1||
        t->words[varBlock][0] < 'A'||
        t->words[varBlock][0] > 'Z'){
            myAlert("Wrong varname in Set!\n");
    }
}

int getSemiColumn(turtle *t)
{
    int i = 0;
    while(strcmp(t->words[t->blockcounter+i], ";")!=0){
        i++;
    }
    return i;
}

double executePolish(turtle *t, variablePool *var, int cnt)
{
    int i, j;
    double op1, op2, ret;
    bool isdouble;
    stackSpace stack;
    stackInit(&stack);
    for(i = 3; i < cnt; i++){
        j = 0;
        isdouble = false;
        if( t->words[t->blockcounter+i][j] <= '9' &&
            t->words[t->blockcounter+i][j] >= '0'){
            while(t->words[t->blockcounter+i][j]!='\0'){
                if(t->words[t->blockcounter+i][j]=='.'){
                    isdouble = true;
                }
                j++;
            }
            if(isdouble){
                pushStack(&stack, myString2Double(t->words[t->blockcounter+i]));
            }
            else{
                pushStack(&stack, (double)myString2Int(t->words[t->blockcounter+i]));
            }
            isdouble = false;
        }
        else if(strcmp(t->words[t->blockcounter+i], "+")==0){
            op2 = popStack(&stack);
            op1 = popStack(&stack);
            ret = op1 + op2;
            pushStack(&stack, ret);
        }
        else if(strcmp(t->words[t->blockcounter+i], "-")==0){
            op2 = popStack(&stack);
            op1 = popStack(&stack);
            ret = op1 - op2;
            pushStack(&stack, ret);
        }
        else if(strcmp(t->words[t->blockcounter+i], "*")==0){
            op2 = popStack(&stack);
            op1 = popStack(&stack);
            ret = op1 * op2;
            pushStack(&stack, ret);
        }
        else if(strcmp(t->words[t->blockcounter+i], "/")==0){
            op2 = popStack(&stack);
            op1 = popStack(&stack);
            ret = op1 / op2;
            pushStack(&stack, ret);
        }
        else{
            returnVarNum(var, t->words[t->blockcounter+i], &ret);
            pushStack(&stack, ret);
        }
    }
    ret = popStack(&stack);
    return ret;
}

void pushStack(stackSpace *stack, double t)
{
    int i;
    for(i = stack->cnt; i > 0; i--){
        stack->operand[i] = stack->operand[i-1];
    }
    stack->operand[0] = t;
    stack->cnt++;
}

double popStack(stackSpace *stack)
{
    double ret;
    int i;
    ret = stack->operand[0];
    for(i = 0; i < stack->cnt; i++){
        stack->operand[i] = stack->operand[i+1];
    }
    stack->cnt--;
    return ret;
}

/*Movement execution*/
void executeFDline(turtle *t, variablePool *var, sdlData *sdl)
{
    t->blockcounter++;
    numberChecker(t, var, &(var->fd));
    executeFD(var->fd, sdl);
}

void executeFD(double fd, sdlData *sdl)
{
    sdl->v.length = fd;
    getCoordinate(&(sdl->l), &(sdl->v));
    SDL_RenderDrawLine(sdl->sw.renderer, sdl->l.cx1, sdl->l.cy1, sdl->l.cx2, sdl->l.cy2);
    refreshCoordinate(sdl);
    getUpdateScreen(&(sdl->sw));
    SDL_Delay(0.2);
    getUpdateScreen(&(sdl->sw));
    getSdlEvents(&(sdl->sw));
}

void executeLTline(turtle *t, variablePool *var, sdlData *sdl)
{
    t->blockcounter++;
    numberChecker(t, var, &(var->lt));
    executeLT(var->lt, sdl);
}

void executeLT(double lt, sdlData *sdl)
{
    sdl->v.angle -= (lt / 360.0 * 2.0 * PI);
    getUpdateScreen(&(sdl->sw));
    getSdlEvents(&(sdl->sw));
}

void executeRTline(turtle *t, variablePool *var, sdlData *sdl)
{
    t->blockcounter++;
    numberChecker(t, var, &(var->rt));
    executeRT(var->rt, sdl);
}

void executeRT(double rt, sdlData *sdl)
{
    sdl->v.angle += (rt / 360.0 * 2.0 * PI);
    getUpdateScreen(&(sdl->sw));
    getSdlEvents(&(sdl->sw));
}

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*|||||||||||||   Important Infrastructures   ||||||||||||*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/*Check if the block is number and if it's integer or double*/
void numberChecker(turtle *t, variablePool *var, double *num)
{
    int i;
    char out[MAXLENGTH];
    char tmp[MAXLENGTH];
    bool isdouble;
    isdouble = false;
    myInt2String(t->linecounter+1, tmp);
    strcpy(out, "Wrong Varnumber: line");
    strcat(out, tmp);
    strcat(out, "\n");
    i = 0;
    if( t->words[t->blockcounter][i] <= '9' &&
        t->words[t->blockcounter][i] >= '0'){
        while(t->words[t->blockcounter][i]!='\0'){
            if( t->words[t->blockcounter][i] > '9'||
                t->words[t->blockcounter][i] < '0'){
                    if(t->words[t->blockcounter][i]!='.'){
                        myAlert(out);
                    }
                    else{
                        isdouble = true;
                    }
            }
            i++;
        }
        if(isdouble){
            *num = myString2Double(t->words[t->blockcounter]);
        }
        else{
            *num = myString2Int(t->words[t->blockcounter]);
        }
        return;
    }
    else{
        if(returnVarNum(var, t->words[t->blockcounter], num)){
            return;
        }
    }
    myAlert(out);
}

/*Find the same layer of curly brace*/
int findMatchBrace(turtle *t, int head)
{
    int cntLeft, cntRight, i;
    cntLeft = 1;
    cntRight = 0;
    i = head + 1;
    while(t->words[i]){
        if(strcmp(t->words[i], "{") == 0){
            cntLeft++;
        }
        if(strcmp(t->words[i], "}") == 0){
            cntRight++;
        }
        if( cntLeft-cntRight == 0 &&
            strcmp(t->words[i], "}") == 0){
            return i;
        }
        i++;
    }
    myAlert("Unmatched curly braces!\n");
    return -1;
}

/*Set the end point to the new start*/
void refreshCoordinate(sdlData *sdl)
{
    sdl->l.cx1 = sdl->l.cx2;
    sdl->l.cy1 = sdl->l.cy2;
}

/*Retrieve variable and return value*/
bool returnVarNum(variablePool *var, char *s, double *num)
{
    int i;
    for(i = 0; i < MAXLINE; i++)
    {
        if( var->varStatus[i] && 
            !strcmp(s, var->varName[i])){
                *num = var->varNum[i];
                return true;
            }
    }
    return false;
}

void myInt2String(int a, char *s)
{
    char temp[MAXLENGTH];
    int i = 0;
    if(a == 0){
        temp[i++] = '0';
    }
    while(a > 0){
        temp[i++] = (a % 10) + '0';
        a /= 10;
    }
    temp[i] = '\0';
    for (i = 0; i < (int)strlen(temp); i++){
        s[i] = temp[strlen(temp) - i - 1];
    }
    s[i] = '\0';
}

int myString2Int(char *s)
{
    int i, out;
    i = 0;
    out = 0;
    while(s[i]!='\0'){
        out = out * 10 + s[i] - '0';
        i++;
    }
    return out;
}

double myString2Double(char *s)
{
    int i, cnt1, cnt2;
    double out;
    i = 0;
    cnt1 = 0;
    cnt2 = 0;
    out = 0;
    while(s[i]!='\0'){
        if(s[i]!='.'){
            out = out * 10 + s[i] - '0';
        }
        else{
            cnt1 = i;
        }
        i++;
    }
    cnt2 = i - 1;
    for(i = 0; i < (cnt2 - cnt1); i++){
        out /= 10;
    }
    return out;
}