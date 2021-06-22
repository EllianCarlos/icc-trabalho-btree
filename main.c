#include "btree.h"
#include "utils.h"
#include "student.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 256
#define MAX_WORD_SIZE 64

int main()
{
    char line[MAX_LINE_SIZE];
    fgets(line, sizeof(line), stdin);

    int opCode = getOperation(line);

    if (opCode == OP_INSERT)
    {
        student *st = getStudentFromLine(line);
        printStudent(st);
        deleteStudent(st);
    }

    return 0;
}