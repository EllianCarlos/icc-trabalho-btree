#include "btreeInsertion.h"

// Toda lógica da inserção em btree tá aqui
// Ignorem a bagunça em algumas funções (nomes em português, outros em inglês), vou deixar bunitin depois kkkkk

// Cria uma estrutura de chave promovida com dados de um registro. Útil para as inserções e splits
promotedKey *criarChave(record *newRecord, long leftChild, long rightChild)
{
    promotedKey *chave = (promotedKey *)malloc(sizeof(promotedKey));
    chave->childs[0] = leftChild;
    chave->childs[1] = rightChild;
    chave->recordKey = (record *)malloc(sizeof(record));
    chave->recordKey->key = newRecord->key;
    chave->recordKey->recordRRN = newRecord->recordRRN;

    return chave;
}

/* Retorna o RRN de uma página filha para inserir uma chave.
   Faz esse processo até achar um nó folha */
long procuraPaginaPraInserir(record *newRecord, btPage *page)
{
    record *pageRecord = NULL;
    for (int i = 0; i < page->numberOfKeys; i++)
    {
        pageRecord = page->records[i];
        if (newRecord->key == pageRecord->key)
            return -1; // chaves iguais, erro!
        else if (newRecord->key < pageRecord->key)
            return page->childs[i];
    }

    return page->childs[page->numberOfKeys];
}

// Busca uma posição dentro do vetor de registros da página para inserir a nova chave
int buscaPosicaoNoNode(promotedKey *newRecord, btPage *page)
{
    int i = 0;
    for (i = 0; i < page->numberOfKeys; i++)
    {
        if (newRecord->recordKey->key < page->records[i]->key)
        {
            return i;
        }
        else if (newRecord->recordKey->key == page->records[i]->key)
        {
            return -1; // já foi inserido, ERRO
        }
    }

    return i;
}

// Insere uma chave de forma ordenada em uma página
Errors insercaoOrdenadaNoNode(promotedKey *newRecord, btPage *page)
{
    int posicao = buscaPosicaoNoNode(newRecord, page);
    if (posicao != -1)
    {
        for (int i = page->numberOfKeys-1; i > posicao; i--)
        {
            page->records[i + 1] = page->records[i];
            page->childs[i + 2] = page->childs[i + 1];
        }
        page->childs[posicao] = newRecord->childs[0];
        page->childs[posicao + 1] = newRecord->childs[1];
        page->records[posicao]->key = newRecord->recordKey->key;
        page->records[posicao]->recordRRN = newRecord->recordKey->recordRRN;

        return true;
    }
    else
    { // já foi inserido, ERRO
        return false;
    }
}

// "Reinicia" uma página splittada para receber novamente as chaves organizadas
void clearSplittedPage(btPage *page)
{
    for (int i = 0; i < MAXKEYS; i++)
    {
        page->records[i]->key = -1;
        page->records[i]->recordRRN = -1;
        page->childs[i] = -1;
    }
    page->childs[MAXKEYS] = -1;
    page->numberOfKeys = (MAXKEYS + 1) / 2;
}

// Organiza e distribui as chaves nos nós splittados
// (em construção)
promotedKey *organizeKeys(promotedKey **auxArray, btPage *originalPage, btPage *newPage)
{
    /*
    printf("\n --- Vetor auxiliar recebido: ---\n");
    for(int i = 0; i < (MAXKEYS+1); i++) {
        printf("Chave [%d]: %d\n", i, auxArray[i]->recordKey->key);
        printf("RRN do DF armazenado na chave [%d]: %ld\n", i, auxArray[i]->recordKey->recordRRN);
        printf("Filho esquerdo da chave [%d]: %ld\n", i, auxArray[i]->childs[0]);
        printf("Filho direito da chave [%d]: %ld\n", i, auxArray[i]->childs[1]);
    }
    */
    int meio = (MAXKEYS + 1) / 2;
    clearSplittedPage(originalPage); // limpa a página original antes de recolocar os elementos
    for (int i = 0; i < meio; i++)
    {
        originalPage->records[i]->key = auxArray[i]->recordKey->key;
        originalPage->records[i]->recordRRN = auxArray[i]->recordKey->recordRRN;
        originalPage->childs[i] = auxArray[i]->childs[0];
        originalPage->childs[i + 1] = auxArray[i]->childs[1];
    }
    int j = 0;
    for (int i = meio; i < (MAXKEYS + 1); i++)
    {
        newPage->records[j]->key = auxArray[i]->recordKey->key;
        newPage->records[j]->recordRRN = auxArray[i]->recordKey->recordRRN;
        newPage->childs[j] = auxArray[i]->childs[0];
        newPage->childs[j + 1] = auxArray[i]->childs[1];
        newPage->numberOfKeys++;
        j++;
    }

    return criarChave(newPage->records[0], newPage->childs[0], newPage->childs[1]); // retorna a primeira chave da nova página
}

// Cria um array auxiliar de chaves para ajudar no overflow e na promoção de uma chave
promotedKey **createArrayForKeys(btPage *page, promotedKey *newRecord)
{
    //printf("Entrou na funcao do array\n");
    promotedKey **auxArray = (promotedKey **)malloc((MAXKEYS + 1) * sizeof(promotedKey *));
    //printf("Alocou o array\n");

    int contadorPagina = 0;
    int contadorArray = 0;
    //printf("MAXKEYS = %d\n", MAXKEYS);
    while (contadorArray < (MAXKEYS + 1))
    {
        //printf("Contagem: %d\n", contadorPagina);
        if (contadorPagina < MAXKEYS && page->records[contadorPagina]->key < newRecord->recordKey->key)
        {
            auxArray[contadorArray] = criarChave(page->records[contadorPagina], page->childs[contadorPagina], page->childs[contadorPagina + 1]);
            //printf("--- Chave menor %d entrou\n", auxArray[contadorArray]->recordKey->key);
            contadorPagina++;
        }
        else if (contadorPagina >= MAXKEYS || page->records[contadorPagina]->key > newRecord->recordKey->key)
        {
            auxArray[contadorArray] = newRecord;
            //printf("--- Chave (%d) inserida colocada no array\n", auxArray[contadorArray]->recordKey->key);
        }
        else
        {
            return NULL;
        }

        contadorArray++;
    }

    return auxArray;
}

// Remove a chave promovida para o overflow
void removePromotedKeyFromPage(btPage *page)
{
    for (int i = 1; i < page->numberOfKeys; i++)
    {
        page->records[i - 1] = page->records[i];
        page->childs[i - 1] = page->childs[i];
        page->childs[i] = page->childs[i + 1];
    }
    page->numberOfKeys--;
}

// Função principal do split
// (em construção)
promotedKey *split(btPage *splitedPage, promotedKey *newRecord, FILE *fp)
{
    //printf(" === Entrou em um split por causa da chave %d\n", newRecord->recordKey->key);
    promotedKey **auxArray = createArrayForKeys(splitedPage, newRecord);
    if (auxArray == NULL)
        return NULL;
    //printf("Fez o vetor auxiliar\n");

    btPage *newPage = allocatePage();
    //printf("Alocou nova pagina\n");
    newPage->isLeaf = splitedPage->isLeaf;
    //printf("Setou se eh folha ou nao corretamente\n");
    fseek(fp, 0, SEEK_END);
    long rrnNewPage = ftell(fp);
    //printf("Deu o seek end\n");

    promotedKey *keyPromoted = organizeKeys(auxArray, splitedPage, newPage);
    //printf(" === Pegou a chave promovida %d\n", keyPromoted->recordKey->key);
    removePromotedKeyFromPage(newPage);
    //printf("Tirar a chave da pagina\n");

    // próx passo: definir o rrn dos filhos da chave promovida
    keyPromoted->childs[1] = rrnNewPage;
    //printf(" === Filho direito da chave promovida: %ld\n", rrnNewPage);

    writePageIntoFile(rrnNewPage, newPage, fp);

    free(auxArray);

    return keyPromoted;
}

// Cria uma nova página e seta ela como raiz caso houver overflow na antiga raiz
void createAndSetNewRoot(btPage *formerRoot, long rrnFormerRoot, promotedKey *promotedKey, FILE *fp)
{
    // Aloca e prepara os dados da nova raiz
    btPage *newRoot = allocatePage();
    //printf("Alocou a raiz nova\n");
    newRoot->isLeaf = false;
    promotedKey->childs[0] = rrnFormerRoot;
    //printf(" === Filho esquerdo da chave promovida: %ld\n\n", rrnFormerRoot);
    //printf("Pegou o primeiro filho\n");
    //printf("Chave: %d\nFilho1: %ld\nFilho2: %ld\n", promotedKey->recordKey->key, promotedKey->childs[0], promotedKey->childs[1]);
    insertInNode(promotedKey, newRoot, fp);
    //printf("Inseriu na nova raiz\n");

    // Escreve a raiz antiga no arquivo
    //printf("--- RRN a escrever %ld\n", rrnFormerRoot);
    //printf("--- Quantas chaves tem na raiz? %d\n", formerRoot->numberOfKeys);
    // for(int i = 0; i < formerRoot->numberOfKeys; i++) {
    //     //printf("--- Chave %d: %d\n", i, formerRoot->records[i]->key);
    //     //printf("--- Filho esquerdo: %ld\n", formerRoot->childs[i]);
    //     //printf("--- Filho direito: %ld\n", formerRoot->childs[i+1]);
    //     /*if(formerRoot->isLeaf)
    //         printf("--- Eh folha\n");
    //     else
    //         printf("--- Nao eh folha\n");*/
    // }
    writePageIntoFile(rrnFormerRoot, formerRoot, fp);
    //printf("Escreveu a raiz antiga no arquivo\n");

    // Calcula o rrn da nova raiz, escreve ela no arquivo e no cabeçalho
    fseek(fp, 0, SEEK_END);
    long rrnNewRoot = ftell(fp);
    writeTreeHeader(fp, rrnNewRoot);
    writePageIntoFile(rrnNewRoot, newRoot, fp);
}

// Faz uma inserção em um nó qualquer
promotedKey *insertInNode(promotedKey *newRecord, btPage *page, FILE *fp)
{
    if (page->numberOfKeys < MAXKEYS)
    { // Sem overflow
        // Checa se é possível inserir. Se sim, insere
        bool checkInsertion = insercaoOrdenadaNoNode(newRecord, page);
        if (checkInsertion)
        {
            page->numberOfKeys++;
            //printf("Insercao %d feita!\n", page->numberOfKeys);
            //printf("Insercao %d feita com sucesso\n", page->numberOfKeys);
        }
        else
            printf("O Registro ja existe!\n");

        return NULL;
    }
    else
    { // Com overflow
        // Faz o split e retorna a chave promovida
        promotedKey *promotedKey = split(page, newRecord, fp);
        if (promotedKey == NULL)
            printf("O Registro ja existe!\n");
        //printf("Split feito. Chave voltada: %d\n", promotedKey->recordKey->key);

        return promotedKey;
    }
}

// Inserção recursiva. Vai investigando os nós até achar uma folha
// (meio em construção ainda)
promotedKey *recInserirBTree(promotedKey *newRecord, btPage *page, FILE *fp)
{
    promotedKey *promotedKey = NULL;
    if (page->isLeaf)
    {
        //printf("Inserindo a chave %d em uma folha\n", newRecord->recordKey->key);
        promotedKey = insertInNode(newRecord, page, fp);
    }
    else
    {
        long rrnPageFilha = procuraPaginaPraInserir(newRecord->recordKey, page);
        //printf("RRN do filho que vai inserir a chave %d: %ld\n", newRecord->recordKey->key, rrnPageFilha);
        if (rrnPageFilha == -1)
        {
            printf("O Registro ja existe!\n");
            return NULL;
        }
        btPage *pageFilha = getPage(rrnPageFilha, fp);
        promotedKey = recInserirBTree(newRecord, pageFilha, fp);
        if (promotedKey != NULL)
        {
            promotedKey->childs[0] = rrnPageFilha;
            //printf(" === Filho esquerdo da chave promovida: %ld\n\n", rrnPageFilha);

            promotedKey = insertInNode(promotedKey, page, fp);
        }

        writePageIntoFile(rrnPageFilha, pageFilha, fp);
    }

    return promotedKey;
}

// Função principal para inserção
Errors bTreeInsert(record *newRecord, btPage *root, FILE *fp)
{
    //printf("Inserindo chave %d\n", newRecord->key);
    //printf("Inserindo a chave %d\n", newRecord->key);
    //FILE *fp = fopen("btree.bin", "r+");
    //btPage *root = getOrCreateRoot(fp);
    //printf("Entrou na funcao principal\n");
    // short test = root->numberOfKeys;
    long rrnRoot = getTreeHeader(fp);

    promotedKey *newRecordKey = criarChave(newRecord, -1, -1);
    promotedKey *promotedKey = recInserirBTree(newRecordKey, root, fp);
    if (promotedKey != NULL)
    {
        //printf("Entrou num if que nao devia\n");
        createAndSetNewRoot(root, rrnRoot, promotedKey, fp);
    }
    else
    {
        //printf("Escrevendo a raiz de volta\n");
        writePageIntoFile(rrnRoot, root, fp);
        // writeTreeHeader(fp, rrnRoot);
    }

    //fclose(fp);

    return true;
}