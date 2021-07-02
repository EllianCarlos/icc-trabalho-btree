#include "dataFile.h"

void readEverythingFromFile(FILE *filePointer, int numberOfStudentsInFile)
{
    rewind(filePointer);
    student *st = (student *)calloc(numberOfStudentsInFile, sizeof(student));
    fread(st, sizeof(student), numberOfStudentsInFile, filePointer);
    printStudentArray(st, numberOfStudentsInFile);
    free(st);
}

int getNumberOfStudentsInFile(FILE *filePointer)
{
    fseek(filePointer, 0, SEEK_END);
    int fileSize = ftell(filePointer);
    int numberOfStudents = fileSize / sizeof(student);
    rewind(filePointer);
    return numberOfStudents;
}

void readFileFromRange(FILE *filePointer, int offset, int numberOfReads)
{
    rewind(filePointer);
    fseek(filePointer, offset * sizeof(student), SEEK_CUR);
    student *st = (student *)calloc(numberOfReads + 1, sizeof(student));
    fread(st, sizeof(student), numberOfReads, filePointer);
    printStudentArray(st, numberOfReads);
    free(st);
}

long appendAsFixedSize(FILE *fp, student *st)
{
    fseek(fp, 0, SEEK_END);
    long offset = ftell(fp);
    fwrite(&st->nUsp, sizeof(st->nUsp), 1, fp);
    fwrite(&st->name, sizeof(st->name), MAX_STRING_SIZE, fp);
    fwrite(&st->surname, sizeof(st->surname), MAX_STRING_SIZE, fp);
    fwrite(&st->course, sizeof(st->course), MAX_STRING_SIZE, fp);
    fwrite(&st->grade, sizeof(st->grade), 1, fp);
    fflush(fp);
    return offset;
}

void getByRNN(long RNN, FILE *fp)
{
    fseek(fp, RNN, SEEK_SET);
    student *st = malloc(sizeof(student));
    fread(&st->nUsp, sizeof(st->nUsp), 1, fp);
    fread(&st->name, sizeof(st->name), MAX_STRING_SIZE, fp);
    fread(&st->surname, sizeof(st->surname), MAX_STRING_SIZE, fp);
    fread(&st->course, sizeof(st->course), MAX_STRING_SIZE, fp);
    fread(&st->grade, sizeof(st->grade), 1, fp);
    printStudent(st);
    free(st);
    return;
}

void updateStudent(long RNN, FILE *fp, student *st)
{
    fseek(fp, RNN, SEEK_SET);
    fwrite(&st->nUsp, sizeof(st->nUsp), 1, fp);
    fwrite(&st->name, sizeof(st->name), MAX_STRING_SIZE, fp);
    fwrite(&st->surname, sizeof(st->surname), MAX_STRING_SIZE, fp);
    fwrite(&st->course, sizeof(st->course), MAX_STRING_SIZE, fp);
    fwrite(&st->grade, sizeof(st->grade), 1, fp);
    fflush(fp);
    fclose(fp);
    return;
}