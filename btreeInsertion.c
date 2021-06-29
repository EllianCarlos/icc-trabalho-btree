#include "btreeInsertion.h"

// Toda lógica da inserção em btree tá aqui
// Ignorem a bagunça em algumas funções (nomes em português, outros em inglês), vou deixar bunitin depois kkkkk

// Cria uma chave com dados de um registro
promotedKey *criarChave(record *newRecord) {
    promotedKey *chave = (promotedKey *)malloc(sizeof(promotedKey));
    chave->childs[0] = -1;
    chave->childs[1] = -1;
    chave->key = newRecord->key;
    chave->recordRRN = newRecord->recordRRN;

    return chave;
}

/* Retorna o RRN de uma página filha para inserir uma chave.
   Faz esse processo até achar um nó folha */
long procuraPaginaPraInserir(record *newRecord, btPage *page) {
    record *pageRecord = NULL;
    for(int i = 0; i < page->numberOfKeys; i++) {
        pageRecord = page->records[i];
        if(newRecord->key == pageRecord->key)
            return -1; // chaves iguais, erro!
        else if(newRecord->key < pageRecord->key)
            return page->childs[i];
    }

    return page->childs[page->numberOfKeys];
}

// Busca uma posição dentro do vetor de registros da página para inserir a nova chave
int buscaPosicaoNoNode(record *newRecord, btPage *page) {
    int i = 0;
    for(i = 0; i < page->numberOfKeys; i++) {
        if(newRecord->key < page->records[i]->key)
            break;
        else if(newRecord->key == page->records[i]->key)
            return -1; // já foi inserido, ERRO
    }

    return i;
}

// Insere um registro de forma ordenada em uma página
Errors insercaoOrdenadaNoNode(record *newRecord, btPage *page) {
    int posicao = buscaPosicaoNoNode(newRecord, page);
    if(posicao != -1) {
        for(int i = page->numberOfKeys-1; i >= posicao; i--) {
            page->records[i+1] = page->records[i];
        }
        page->records[posicao] = newRecord;

        return true;
    }
    else { // já foi inserido, ERRO
        return false;
    }
}

// Cria uma estrutura de chave promovida com uma chave e os ponteiros de filho (à esquerda e à direita da chave)
promotedKey *convertRecordToKey(record *r, long child1, long child2) {
    promotedKey *keyPromoted = (promotedKey *)malloc(sizeof(promotedKey));

    keyPromoted->key = r->key;
    keyPromoted->recordRRN = r->recordRRN;
    keyPromoted->childs[0] = child1;
    keyPromoted->childs[1] = child2;

    return keyPromoted;
}

// Organiza e distribui as chaves nos nós splittados
// (em construção)
void organizeKeys(promotedKey **auxArray, btPage *originalPage, btPage *newPage) {
    int meio = (MAXKEYS+1)/2;
    for(int i = 0; i < meio; i++) {
        originalPage->records[i] = auxArray[i];
    }
    int j = 0;
    for(int i = meio; i < (MAXKEYS+1); i++) {
        newPage->records[j] = auxArray[i];
    }
}

/*
promotedKey *distributeKeys(record **auxArray, btPage *originalPage, btPage *newPage) {
    promotedKey *selectedKey = (promotedKey *)malloc(sizeof(promotedKey));

    // ...

    return selectedKey;
}
*/

// Cria um array auxiliar de chaves para ajudar no overflow e na promoção de uma chave
promotedKey **createArrayForKeys(btPage *page, record *newRecord) {
    promotedKey **auxArray = (promotedKey **)malloc((MAXKEYS+1)*sizeof(promotedKey *));

    int contadorPagina = 0;
    int contadorArray = 0;
    while(contadorArray < (MAXKEYS+1)) {
        if(page->records[contadorPagina]->key < newRecord->key) {
            auxArray[contadorArray] = convertRecordToKey(page->records[contadorPagina], page->childs[contadorPagina], page->childs[contadorPagina+1]);
            contadorPagina++;
        }
        else if(page->records[contadorPagina]->key > newRecord->key) {
            auxArray[contadorArray] = convertRecordToKey(newRecord, -1, -1);
        }
        else {
            return NULL;
        }

        contadorArray++;
    }

    return auxArray;
}

// Libera a memória do array acima
void freeArrayForKeys(promotedKey **auxArray) {
    for(int i = 0; i < (MAXKEYS+1); i++) {
        free(auxArray[i]);
    }
    free(auxArray);
}

// Função principal do split
// (em construção)
void split(btPage *splitedPage, record *newRecord, FILE *fp) {
    // Reunir as chaves da página + a chave a ser inserida em um array auxiliar
    promotedKey **auxArray = createArrayForKeys(splitedPage, newRecord);
    // Criar nova página
    btPage *newPage = allocatePage();
    fseek(fp, 0, SEEK_END);
    long rrnNewPage = ftell(fp);
    // Distribuir chaves e promover uma delas
    promotedKey *keyPromoted;

    writePageIntoFile(rrnNewPage, newPage, fp);

    freeArrayForKeys(auxArray);
}

// Faz uma inserção em um nó folha
promotedKey *inserirEmFolha(record *newRecord, btPage *page, FILE *fp) {
    if(page->numberOfKeys < MAXKEYS) {
        bool checkInsertion = insercaoOrdenadaNoNode(newRecord, page);
        if(checkInsertion) {
            page->numberOfKeys++;
        }
        else
            printf("O Registro ja existe!\n");
        
        return NULL;
    }
    else {
        // inserção com split
    }
}

// Inserção recursiva. Vai investigando os nós até achar uma folha
// (meio em construção ainda)
promotedKey *recInserirBTree(record *newRecord, btPage *page, FILE *fp) {
    promotedKey *possivelChavePromovida = NULL;
    if(page->isLeaf) {
        possivelChavePromovida = inserirEmFolha(newRecord, page, fp);
    }
    else {
        long rrnPageFilha = procuraPaginaPraInserir(newRecord, page);
        if(rrnPageFilha == -1) {
            printf("O Registro ja existe!\n");
            return NULL;
        }
        btPage *pageFilha = getPage(rrnPageFilha, fp);
        possivelChavePromovida = recInserirBTree(newRecord, pageFilha, fp);

        writePageIntoFile(rrnPageFilha, pageFilha, fp);
    }

    return NULL;
}

// Função principal para inserção
Errors inserirBTree(record *newRecord) {
    FILE *fp = fopen("arquivo.dat", "r+");
    btPage *root = getOrCreateRoot(fp);
    long rrnRoot = getTreeHeader(fp);

    promotedKey *possivelChavePromovida = recInserirBTree(newRecord, root, fp);

    writePageIntoFile(rrnRoot, root, fp);
    writeTreeHeader(fp, rrnRoot);

    fclose(fp);
}