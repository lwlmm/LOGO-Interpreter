#include "parser.h"

int main(int argc, char* argv[])
{
    FILE *fp;
    turtle t;
    instruction ins;

    checkArgc(argc);
    turtleInit(&t);
    insInit(&ins);
    openFile(fp, argv[1], &t);
    parseProgram(&t, &ins);

    printf("Parsed ok!\n");

    return 0;
}