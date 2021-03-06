#include "btree.h"
#include "utils.h"
#include "dataFile.h"
#include "student.h"
#include "btreeInsertion.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 256
#define MAX_WORD_SIZE 64
#define datafilename "datafile.bin"
#define btreefilename "btree.bin"

#define DBG                         \
    fprintf(stdout, "AAAAAAAAA\n"); \
    fflush(stdout)

void closeAndFreeAll(FILE *bTreeFile, FILE *dataFile)
{
    fclose(bTreeFile);
    fclose(dataFile);
}

void throwErrorAndClose(const char *ErrorMessage, FILE *bTreeFile, FILE *dataFile)
{
    fprintf(stderr, "%s\n", ErrorMessage);
    closeAndFreeAll(bTreeFile, dataFile);
    exitSystem();
}

int main()
{
    FILE *bTreeFile = fopen(btreefilename, "wb+");

    FILE *dataFile = fopen(datafilename, "wb+");

    char line[MAX_LINE_SIZE];
    while (true)
    {
        fgets(line, sizeof(line), stdin);

        int opCode = getOperation(line);

        if (OP_INSERT == opCode)
        {
            student *st = getStudentFromLine(line);
            btPage *bTreePage = getOrCreateRoot(bTreeFile);
            long RNNisFound = bTreeSelect(bTreePage, st->nUsp, bTreeFile);
            if (RNNisFound != -1)
            {
                fprintf(stdout, "O Registro ja existe!\n");
                continue;
            }
            long RNN = appendAsFixedSize(dataFile, st);
            record *newRecord = createRecord(st->nUsp, RNN);
            bool opResult = bTreeInsert(newRecord, bTreePage, bTreeFile);
            //printf("Fez uma insercao\n");
            deleteStudent(st);
            if (opResult == false)
            {
                deallocatePage(bTreePage);
                throwErrorAndClose("Erro ao adicionar registro", bTreeFile, dataFile);
            }
        }
        else if (OP_UPDATE == opCode)
        {
            btPage *bTreePage = getOrCreateRoot(bTreeFile);
            student *st = getStudentFromLine(line);
            long RNN = bTreeSelect(bTreePage, st->nUsp, bTreeFile);
            if (RNN == -1)
            {
                fprintf(stdout, "Registro nao encontrado!\n");
            }
            else
            {
                updateStudent(RNN, dataFile, st);
            }
            deleteStudent(st);
        }
        else if (OP_SEARCH == opCode)
        {
            btPage *bTreePage = getOrCreateRoot(bTreeFile);
            int nUsp = getNUspFromLine(line);
            long RNN = bTreeSelect(bTreePage, nUsp, bTreeFile);
            //printf("Resultado da busca: %ld\n", RNN);
            // deallocatePage(bTreePage);
            if (RNN == -1)
            {
                fprintf(stdout, "Registro nao encontrado!\n");
            }
            else
            {
                getByRNN(RNN, dataFile);
            }
        }
        else
        {
            closeAndFreeAll(bTreeFile, dataFile);
            exitSystem();
        }
    }
    return 0;
}