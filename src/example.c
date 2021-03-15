#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAXNUMTOKENS 100
#define MAXTOKENSIZE 7
#define PROGNAME "01.no"
#define strsame(A,B) (strcmp(A, B)==0)
#define ERROR(PHRASE) {fprintf(stderr,"Fatal Error %s occured in \"01.no\", line %d\n",PHRASE, p->cw); exit(2);}

struct prog{
char wds[MAXNUMTOKENS][MAXTOKENSIZE];
int cw; /* Current Word */
};
typedef struct prog Program;

void Prog(Program *p);
void Code(Program *p);
void Statement(Program *p);

int main(void)
{
    int i;
    FILE *fp;

    /*Initialize prog structure*/
    Program prog;                           
    prog.cw = 0;
    for(i=0; i<MAXNUMTOKENS; i++)
    prog.wds[i][0] = '\0';

    /*Open file, if fail, print out*/
    if(!(fp = fopen(PROGNAME, "r"))){
        fprintf(stderr, "Cannot open %s\n",PROGNAME);
        exit(2);
    }

    /*Scan in the program*/
    i=0;
    while(fscanf(fp, "%s", prog.wds[i++])==1 && i<MAXNUMTOKENS);
    
    assert(i<MAXNUMTOKENS);

    Prog(&prog);

    printf("Parsed OK\n");
    return 0;
}

/*Test the first line and pass it to Code function*/
void Prog(Program *p)
{
    if(!strsame(p->wds[p->cw], "BEGIN"))
    ERROR("No BEGIN statement ?");
    p->cw = p->cw + 1;
    Code(p);
}

/*Recursively test the statement while it's not the "END"*/
void Code(Program *p)
{
if(strsame(p->wds[p->cw], "END"))
return;
Statement(p);
p->cw = p->cw + 1;
Code(p);
}

/*Test if the word is ONE or NOUGHT*/
void Statement(Program *p)
{
if(strsame(p->wds[p->cw], "ONE")){
return;
}
if(strsame(p->wds[p->cw], "NOUGHT")){
return;
}
ERROR("Expecting a ONE or NOUGHT ?");
}