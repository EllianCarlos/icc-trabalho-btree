#include "btree.h"
#include "utils.h"
#include "datafile.h"
#include "student.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 256
#define MAX_WORD_SIZE 64
#define datafilename "datafile.bin"
#define btreefilename "btree.bin"

void closeAndFreeAll(FILE *bTreeFile, btPage *pageToDeallocate)
{
    fclose(bTreeFile);
    deallocatePage(pageToDeallocate);
}

void throwErrorAndClose(const char *ErrorMessage, FILE *bTreeFile, btPage *pageToDeallocate)
{
    fprintf(stderr, ErrorMessage);
    closeAndFreeAll(bTreeFile, pageToDeallocate);
    exitSystem();
}

int main()
{
    FILE *bTreeFile = fopen(btreefilename, "ab+");
    btPage *bTreePage = getOrCreateRoot(bTreeFile);

    char line[MAX_LINE_SIZE];
    fgets(line, sizeof(line), stdin);

    int opCode = getOperation(line);

    if (OP_INSERT == opCode)
    {

        student *st = getStudentFromLine(line);
        long RNN = appendAsFixedSize(datafilename, st);
        nodeKey *newRecord = createRecord(st->nUsp, RNN);
        bool opResult = bTreeInsert(newRecord, bTreePage, bTreeFile);
        deleteStudent(st);
        if (opResult == false)
        {
            throwErrorAndClose("Erro ao adicionar registro", bTreeFile, bTreePage);
        }
    }
    else if (OP_UPDATE == opCode)
    {
        student *st = getStudentFromLine(line);
        long RNN = bTreeSelect(bTreePage, st->nUsp, bTreeFile);
        if (RNN == -1)
        {
            fprint(stdout, "Registro nao encontrado!");
        }
        else
        {
            updateStudent(RNN, datafilename, st);
        }
        deleteStudent(st);
    }
    else if (OP_SEARCH == opCode)
    {
        int nUsp = getNUspFromLine(line);
        long RNN = bTreeSelect(bTreePage, nUsp, bTreeFile);
        if (RNN == -1)
        {
            fprint(stdout, "Registro nao encontrado!");
        }
        else
        {
            student st;
            getByRNN(RNN, datafilename, &st);
            printStudent(&st);
        }
    }
    else
    {
        closeAndFreeAll(bTreeFile, bTreePage);
        exitSystem();
    }
    return 0;
}