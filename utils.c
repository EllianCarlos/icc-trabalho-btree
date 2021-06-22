#include "utils.h"

int getOperation(char *line)
{
    char *operation = (char *)malloc(MAX_STRING_SIZE * sizeof(char));
    if (1 == sscanf(line, "%s", operation))
    {
        int opCode = getOperationNumber(operation);
        free(operation);
        return opCode;
    }
    else
    {
        fprintf(stderr, "Impossível ler a entrada.");
        exitSystem();
    }
}

int getOperationNumber(char *operation)
{
    if (strcmp(operation, STRING_INSERT) == 0)
    {
        return OP_INSERT;
    }
    else if (strcmp(operation, STRING_SEARCH) == 0)
    {
        return OP_SEARCH;
    }
    else if (strcmp(operation, STRING_UPDATE) == 0)
    {
        return OP_UPDATE;
    }
    else if (strcmp(operation, STRING_EXIT) == 0)
    {
        return OP_EXIT;
    }
    else
    {
        fprintf(stderr, "Operação inválida fechando sistema.");
        exitSystem();
    }
}

void exitSystem()
{
    exit(0);
}

student *getStudentFromLine(char *line)
{
    char *word = (char *)malloc(sizeof(char) * MAX_STRING_SIZE);
    if (1 == sscanf(line, "insert %s", word))
    {
        student *st = getStudentFromWord(word);
        free(word);
        return st;
    }
    else
    {
        free(word);
        fprintf(stderr, "Impossível ler a entrada.");
        exitSystem();
    }
}

student *getStudentFromWord(char *word)
{
    char *tok = (char *)malloc(sizeof(char) * MAX_STRING_SIZE);

    tok = strtok(word, ",");
    int nUsp = atoi(tok);

    tok = strtok(NULL, ",\"");
    char *name = malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(name, tok);

    tok = strtok(NULL, ",\"");
    char *surname = malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(surname, tok);

    tok = strtok(NULL, ",\"");
    char *course = malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(course, tok);

    tok = strtok(NULL, ",\n");
    float grade = atof(tok);

    student *st = createStudent(nUsp, name, surname, course, grade);
    free(name);
    free(surname);
    free(course);
    return st;
}