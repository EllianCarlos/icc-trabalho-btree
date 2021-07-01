#include "btree.h"

nodeKey *criarChave(nodeKey *newRecord);
long procuraPaginaPraInserir(nodeKey *newRecord, btPage *page);
int buscaPosicaoNoNode(nodeKey *newRecord, btPage *page);
Errors insercaoOrdenadaNoNode(nodeKey *newRecord, btPage *page);
void clearSplittedPage(btPage *page);
nodeKey *organizeKeys(nodeKey **auxArray, btPage *originalPage, btPage *newPage);
nodeKey **createArrayForKeys(btPage *page, nodeKey *newRecord);
void removePromotedKeyFromPage(btPage *page);
nodeKey *split(btPage *splitedPage, nodeKey *newRecord, FILE *fp);
void createAndSetNewRoot(btPage *formerRoot, long rrnFormerRoot, nodeKey *promotedKey, FILE *fp);
nodeKey *insertInNode(nodeKey *newRecord, btPage *page, FILE *fp);
nodeKey *recInserirBTree(nodeKey *newRecord, btPage *page, FILE *fp);
Errors bTreeInsert(nodeKey *newRecord);