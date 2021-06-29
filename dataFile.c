#include "datafile.h"

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

long appendAsFixedSize(char *fileName, student s)
{
    FILE *filePointer = fopen(fileName, "a+");
    fseek(filePointer, 0, SEEK_END);
    long offset = ftell(filePointer);
    fwrite(&s, sizeof(student), 1, filePointer);
    fflush(filePointer);
    fclose(filePointer);
    return offset;
}

void getByRNN(long RNN, char *filename, student *st, int nUsp)
{
    FILE *studentfile = fopen(filename, "ab+");
    rewind(studentfile);
    fseek(studentfile, RNN, SEEK_CUR);
    fread(st, sizeof(student), 1, studentfile);
    fclose(studentfile);
    return;
}

void updateStudent(long RNN, char *filename, student *st)
{
    FILE *studentFile = fopen(filename, "w+");
    rewind(studentFile);
    fseek(studentFile, RNN, SEEK_CUR);
    fwrite(st, sizeof(student), 1, studentFile);
    fclose(studentFile);
    return;
}