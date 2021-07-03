#include "btree.h"

btPage *allocatePage()
{
    btPage *page = (btPage *)malloc(sizeof(page));
    page->records = (record **)malloc(MAXKEYS * sizeof(record *));
    page->childs = malloc(sizeof(long) * (MAXKEYS + 1));
    for (int i = 0; i < MAXKEYS; i++)
    {
        page->records[i] = (record *)malloc(sizeof(record));
        page->records[i]->key = -1;
        page->records[i]->recordRRN = -1;
        page->childs[i] = -1;
    }
    page->childs[MAXKEYS] = -1;
    page->numberOfKeys = 0;
    page->isLeaf = true;

    return page;
}

void deallocatePage(btPage *page)
{
    for (int i = 0; i < MAXKEYS; i++)
    {
        if (page->records[i] != NULL) {
            free(page->records[i]);
        }
    }
    if(page->records != NULL)
    free(page->records);
    free(page->childs);
    free(page);
}

void writeTreeHeader(FILE *fp, long rootRRN)
{
    //Calcula espaço livre e escreve no cabeçalho da árvore, junto com o RRN do nó raíz
    fseek(fp, 0, SEEK_SET);
    fwrite(&rootRRN, sizeof(long), 1, fp);
    fflush(fp);
    while (ftell(fp) != PAGESIZE)
    {
        fwrite(FREESPACE, 1, 1, fp);
    }
    fflush(fp);
}

btPage *readPageFromFile(FILE *fp)
{
    //Aloca espaço para carregar página
    btPage *page = allocatePage();
    //Lê dados da página do arquivo
    //printf("Bytes lidos antes da leitura da pag: %ld\n", ftell(fp));
    for(int i = 0; i < MAXKEYS; i++) {
        fread(&page->records[i]->key, sizeof(int), 1, fp);
        fread(&page->records[i]->recordRRN, sizeof(long), 1, fp);
        fread(&page->childs[i], sizeof(long), 1, fp);
    }
    fread(&page->childs[MAXKEYS], sizeof(long), 1, fp);
    fread(&page->numberOfKeys, sizeof(short), 1, fp);
    fread(&page->isLeaf, sizeof(bool), 1, fp);
    //printf("Bytes lidos depois da leitura da pag: %ld\n\n", ftell(fp));

    /*
    printf("Informacoes da pag lida:\n");
    printf("Numero de chaves: %d\n", page->numberOfKeys);
    if(page->isLeaf)
        printf("Eh uma folha\n");

    printf("\n");
    */

    return page;
}

Errors writePageIntoFile(long rrn, btPage *page, FILE *fp)
{
    //printf("RRN a ser escrito: %ld\n", rrn);
    //Verifica se está tudo ok com os dados
    if (PAGESIZE <= rrn && page != NULL)
    {
        //printf("Entrou no primeiro if\n");
        //Encontra local para escrita baseado no RRN
        fseek(fp, rrn, SEEK_SET);
        //printf("Deu o fseek no rrn\n");
        //Escreve dados
        //printf("Bytes lidos antes da escrita na pag: %ld\n", ftell(fp));
        for(int i = 0; i < MAXKEYS; i++) {
            fwrite(&page->records[i]->key, sizeof(int), 1, fp);
            fwrite(&page->records[i]->recordRRN, sizeof(long), 1, fp);
            fwrite(&page->childs[i], sizeof(long), 1, fp);
            //printf("Escreveu a %d chave\n", i+1);
        }
        fwrite(&page->childs[MAXKEYS], sizeof(long), 1, fp);
        fwrite(&page->numberOfKeys, sizeof(short), 1, fp);
        fwrite(&page->isLeaf, sizeof(bool), 1, fp);
        //printf("Bytes lidos depois da escrita da pag: %ld\n\n", ftell(fp));
        fflush(fp);
        //printf("Escreveu tudo\n");
        //Atualiza valor de espaço livre na página
        for (int i = 0; i < FREESPACEONPAGE; i++)
        {
            fwrite(FREESPACE, 1, 1, fp);
        }
        fflush(fp);
        return true;
    }
    else
    {
        return false;
    }
    //Dica: você pode criar uma função que só lida com a escrita dos dados e chamar aqui
}

long getTreeHeader(FILE *fp)
{
    long rrnRoot = -1;
    //Carrega o cabeçalho da árvore, que está no início do arquivo
    fseek(fp, 0, SEEK_SET);
    fread(&rrnRoot, sizeof(long), 1, fp);
    return rrnRoot;
}

btPage *getPage(long RRN, FILE *fp)
{
    //Recupera uma página baseado no RRN
    fseek(fp, RRN, SEEK_SET);
    //Procura e carrega seus dados
    return readPageFromFile(fp);
}

btPage *createTree(FILE *fp)
{
    //Aloca espaço pra raiz
    btPage *root = allocatePage();
    //Escreve a raiz no cabeçalho
    writeTreeHeader(fp, PAGESIZE);
    //Escreve a pagina da raiz
    writePageIntoFile(PAGESIZE, root, fp);
    return root;
}

btPage *getOrCreateRoot(FILE *fp)
{
    //Verifica se a árvore já existe ou precisa criar uma nova
    long rrnRoot = getTreeHeader(fp);
    //Se a árvore não existir, cria ela
    if (rrnRoot == -1)
    {
        //printf("Nao tinha raiz\n");
        return createTree(fp);
    }
    //Se existir, só pega o RRN da raiz no cabeçalho e carrega sua página
    else
    {
        //printf("Tinha raiz\n");
        return getPage(rrnRoot, fp);
    }
    //Pode ser adaptada pra inserção e busca sem precisar de 2 funções
}

/*Returns rrn if key exist else return -1*/
long bTreeSelect(btPage *node, int key, FILE *fp)
{
    //printf("Procurando a chave %d...\n", key);
    long result;
    int i;
    if (!node)
    {
        //printf("Não tem node\n");
        return -1;
    }
    else
    {
        //printf("Numero de chaves no node: %d\n", node->numberOfKeys);
        for (i = 0; i < node->numberOfKeys; i++)
        {
            //Procura no nó atual se a chave existe
            //printf("Chave da pag analisada na iteracao %d: %d\n", i, node->records[i]->key);
            //printf("--- Verificando se eh igual a %d\n", node->records[i]->key);
            if (key == node->records[i]->key)
            {
                //printf("---EH IGUAL!\n");
                //Se encontrar a chave, retorna RRN dela
                //printf("Achei! Ela ta no RRN %ld\n", node->records[i]->recordRRN);
                return node->records[i]->recordRRN;
            }
            //Se não existir, tenta procurar no filho adequado, recursivamente
            else if (key < node->records[i]->key && !node->isLeaf)
            {
                //printf("---Eh menor\n");
                btPage *aux = getPage(node->childs[i], fp);
                result = bTreeSelect(aux, key, fp);
                deallocatePage(aux);
                return result;
            }
        }
        if (!node->isLeaf && (node->records[i-1] && key > node->records[i-1]->key))
        {
            //printf("---Eh maior que a chave %d\n", node->records[i-1]->key);
            btPage *aux = getPage(node->childs[i], fp);
            result = bTreeSelect(aux, key, fp);
            deallocatePage(aux);
            return result;
        }
    }
    //Se não encontrar (chegar num nó folha e não estiver lá), retorna -1
    //printf("Nao encontrei...\n");
    return -1;
}

record *createRecord(int key, long RNN)
{
    record *newRecord = (record *)malloc(sizeof(record));
    newRecord->key = key;
    newRecord->recordRRN = RNN;

    return newRecord;
}

void deleteRecord(record *recordToDelete)
{
    free(recordToDelete);
}