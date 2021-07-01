#ifndef BTREE
#define BTREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

#define PAGESIZE 4096
#define TREEHEADER PAGESIZE
#define MAXKEYS 200
//#define MAXKEYS 4
#define AUXFIELDSSIZEONPAGE (2 + 1) // number of keys and ”is leaf” bool
#define FREESPACEONPAGE (PAGESIZE - ((MAXKEYS * 4) + (MAXKEYS * 8) + ((MAXKEYS + 1) * 8) + 3))
#define PAGELENGTH ((MAXKEYS * 4) + (MAXKEYS * 8) + ((MAXKEYS + 1) * 8) + 3)
#define RECORDLENGTH ((MAXKEYS * 4) + (MAXKEYS * 8))
#define FREESPACE ""

/*
typedef struct record
{
    int key;
    long recordRRN;
} record;
*/

typedef struct nodeKey {
    int key;
    long recordRRN;
    long childs[2];
} nodeKey;

typedef struct page {
    nodeKey **keys;
    long *childs;
    short numberOfKeys;
    bool isLeaf;
} btPage;

btPage *allocatePage();
void desallocatePage(btPage *page);
void writeTreeHeader(FILE *fp, long rootRRN);
btPage *readPageFromFile(FILE *fp);
Errors writePageIntoFile(long rrn, btPage *page, FILE *fp);
long getTreeHeader(FILE *fp);
btPage *getPage(long RRN, FILE *fp);
btPage *createTree(FILE *fp);
btPage *getOrCreateRoot(FILE *);
long bTreeSelect(btPage *, int, FILE *);
nodeKey *createRecord(int key, long RNN);
btPage *getRoot(FILE);

#endif