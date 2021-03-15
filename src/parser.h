#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define MAXLENGTH 50
#define MAXLINE  200

struct turtle{
    char words[MAXLINE][MAXLENGTH];
    int linecounter;
    int blockcounter;
};
typedef struct turtle turtle;

#ifndef _INS_
#define _INS_
struct instruction{
    char i[MAXLENGTH][MAXLINE];
    int num[MAXLINE];
    int cnt;
};
typedef struct instruction instruction;
#endif

void checkArgc(int i);
void turtleInit(turtle *t);
void insInit(instruction *ins);
void openFile(FILE *fp, char *s, turtle *t);
void scanTurtle(FILE *fp, turtle *t);
void parseProgram(turtle *t, instruction *ins);
void parseBody(turtle *t, instruction *ins);
void parseLine(turtle *t, instruction *ins);
void instructionChecker(turtle *t, instruction *ins);
void numberChecker(turtle *t, instruction *ins);


void myInt2String(int a, char *s);
int myString2Int(char *s);
void testSimple(char *s);
void myAlert(char *s);
void testParser(void);

void checkArgc(int i)
{
    if(i == 2){
        return;
    }
    else myAlert("Wrong Input Format!\n");
}
void turtleInit(turtle *t)
{
    int i;
    for(i = 0; i < MAXLINE; i++){
        strcpy(t->words[i], "\0");
    }
    t->linecounter = 0;
    t->blockcounter = 0;
}
void insInit(instruction *ins)
{
    int i;
    for(i = 0; i < MAXLINE; i++){
        ins->num[i] = 0;
    }
    ins->cnt = 0;
}
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
void parseProgram(turtle *t, instruction *ins)
{
    if(strcmp(t->words[t->blockcounter],"{")){
        myAlert("Not the right begin!\n");
    }
    t->linecounter += 1;
    t->blockcounter += 1;
    parseBody(t, ins);
}
void parseBody(turtle *t, instruction *ins)
{
    if(strcmp(t->words[t->blockcounter],"}")==0){
        return;
    }
    parseLine(t, ins);
    t->linecounter += 1;
    ins->cnt += 1;
    parseBody(t, ins);
}
void parseLine(turtle *t, instruction *ins)
{
    instructionChecker(t, ins);
    t->blockcounter += 1;
    numberChecker(t, ins);
    t->blockcounter += 1;
}
void instructionChecker(turtle *t, instruction *ins)
{
    char out[MAXLENGTH];
    char num[MAXLENGTH];
    myInt2String(t->linecounter+1, num);
    strcpy(out, "Wrong instruction: line");
    strcat(out, num);
    strcat(out, "\n");
    if(strcmp(t->words[t->blockcounter],"FD")==0||
       strcmp(t->words[t->blockcounter],"LT")==0||
       strcmp(t->words[t->blockcounter],"RT")==0 ){
        strcpy(ins->i[ins->cnt], t->words[t->blockcounter]);       
        return;
    }
    myAlert(out);
}
void numberChecker(turtle *t, instruction *ins)
{
    int i;
    char out[MAXLENGTH];
    char num[MAXLENGTH];
    myInt2String(t->linecounter+1, num);
    strcpy(out, "Wrong Varnumber: line");
    strcat(out, num);
    strcat(out, "\n");
    i = 0;
    while(t->words[t->blockcounter][i]!='\0'){
        if( t->words[t->blockcounter][i] > '9'||
            t->words[t->blockcounter][i] < '0'){
                myAlert(out);
            }
        i++;
    }
    ins->num[ins->cnt] = myString2Int(t->words[t->blockcounter]);
    return;
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
    for (i = 0; i < strlen(temp); i++){
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
void myAlert(char *s)
{
    fprintf(stderr,"%s",s);
    exit(EXIT_FAILURE);
}
#endif