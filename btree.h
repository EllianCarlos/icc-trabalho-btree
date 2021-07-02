#ifndef BTREE
#define BTREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

#define PAGESIZE 4096
#define TREEHEADER PAGESIZE
// #define MAXKEYS 200
#define MAXKEYS 204
#define AUXFIELDSSIZEONPAGE (2 + 1) // number of keys and ”is leaf” bool
#define PAGELENGTH ((MAXKEYS * 4) + (MAXKEYS * 8) + ((MAXKEYS + 1) * 8) + 3)
#define FREESPACEONPAGE (PAGESIZE - PAGELENGTH)
#define RECORDLENGTH ((MAXKEYS * 4) + (MAXKEYS * 8))
#define FREESPACE ""

typedef struct record
{
    int key;
    long recordRRN;
} record;

typedef struct promotedKey
{
    record *recordKey;
    long childs[2];
} promotedKey;

typedef struct page
{
    record **records;
    long *childs;
    short numberOfKeys;
    bool isLeaf;
} btPage;

btPage *allocatePage();
void deallocatePage(btPage *page);
void writeTreeHeader(FILE *fp, long rootRRN);
btPage *readPageFromFile(FILE *fp);
Errors writePageIntoFile(long rrn, btPage *page, FILE *fp);
long getTreeHeader(FILE *fp);
btPage *getPage(long RRN, FILE *fp);
btPage *createTree(FILE *fp);
btPage *getOrCreateRoot(FILE *);
long bTreeSelect(btPage *, int, FILE *);
record *createRecord(int key, long RNN);
btPage *getRoot(FILE);

#endif