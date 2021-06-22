#include "readFromFixedSizeFile.h"

void readEverythingFromFile(FILE * filePointer, int numberOfStudentsInFile) {
    rewind(filePointer);
    student* st = (student*)calloc(numberOfStudentsInFile, sizeof(student));
    fread(st, sizeof(student), numberOfStudentsInFile, filePointer);
    printStudentArray(st, numberOfStudentsInFile);
    free(st);
}

int getNumberOfStudentsInFile(FILE *filePointer) {
    fseek(filePointer, 0, SEEK_END);
    int fileSize = ftell(filePointer);
    int numberOfStudents = fileSize/sizeof(student);
    rewind(filePointer);
    return numberOfStudents;
}

void readFileFromRange(FILE *filePointer, int offset, int numberOfReads) {
    rewind(filePointer);
    fseek(filePointer, offset*sizeof(student), SEEK_CUR);
    student* st = (student*)calloc(numberOfReads+1, sizeof(student));
    fread(st, sizeof(student), numberOfReads, filePointer);
    printStudentArray(st, numberOfReads);
    free(st);
}