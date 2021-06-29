#include "btree.h"

// Allocate page pointer
btPage *allocatePage()
{
    btPage *page = malloc(sizeof(PAGELENGTH));
    page->records = malloc(sizeof(RECORDLENGTH) * MAXKEYS);
    page->childs = malloc(sizeof(long) * (MAXKEYS + 1));
}

// Desallocate page pointer
void deallocatePage(btPage *page)
{
    free(page->records);
    free(page->childs);
    free(page);
}

/*Retrives page from file pointer*/
btPage *readPageFromFile(FILE *fp)
{
    //Aloca espaço para carregar página
    btPage *page = allocatePage();
    //Lê dados da página do arquivo
    fread(page->records, RECORDLENGTH, MAXKEYS, fp);
    fread(page->childs, sizeof(long), (MAXKEYS + 1), fp);
    fread(page->numberOfKeys, sizeof(short), 1, fp);
    fread(page->isLeaf, sizeof(bool), 1, fp);
    return page;
}

/*Writes page into file in certain rrn position*/
Errors writePageIntoFile(long rrn, btPage *page, FILE *fp)
{
    //Verifica se está tudo ok com os dados
    if (PAGESIZE <= rrn && !page)
    {
        //Encontra local para escrita baseado no RRN
        fseek(fp, rrn, SEEK_SET);
        //Escreve dados
        fwrite(page->records, RECORDLENGTH, MAXKEYS, fp);
        fwrite(page->childs, sizeof(long), (MAXKEYS + 1), fp);
        fwrite(page->numberOfKeys, sizeof(short), 1, fp);
        fwrite(page->isLeaf, sizeof(bool), 1, fp);
        //Atualiza valor de espaço livre na página
        for (int i = 0; i <= FREESPACEONPAGE; i++)
        {
            fwrite(FREESPACE, 1, 1, fp);
        }
        return true;
    }
    else
    {
        return false;
    }
    //Dica: você pode criar uma função que só lida com a escrita dos dados e chamar aqui
}

/*Get page by rrn*/
btPage *getPage(long RRN, FILE *fp)
{
    //Recupera uma página baseado no RRN
    fseek(fp, RRN, SEEK_SET);
    //Procura e carrega seus dados
    return readPageFromFile(fp);
}

/*Get root RRN from header*/
long getTreeHeader(FILE *fp)
{
    long rrnRoot = 0;
    //Carrega o cabeçalho da árvore, que está no início do arquivo
    fseek(fp, 0, SEEK_SET);
    fread(&rrnRoot, sizeof(long), 1, fp);
    return rrnRoot;
}

/*Writes root RRN in header*/
void writeTreeHeader(FILE *fp, long rootRRN)
{
    //Calcula espaço livre e escreve no cabeçalho da árvore, junto com o RRN do nó raíz
    fseek(fp, 0, SEEK_SET);
    fwrite(&rootRRN, sizeof(long), 1, fp);
    while (ftell(fp) != PAGESIZE)
    {
        fwrite(FREESPACE, 1, 1, fp);
    }
}

btPage *createTree(FILE *fp)
{
    //Aloca espaço pra raiz
    btPage *root = allocatePage();
    //Inicializa os valores
    root->numberOfKeys = 0;
    root->isLeaf = true;
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
    if (rrnRoot == 0)
    {
        return createTree(fp);
    }
    //Se existir, só pega o RRN da raiz no cabeçalho e carrega sua página
    else
    {
        return getPage(rrnRoot, fp);
    }
    //Pode ser adaptada pra inserção e busca sem precisar de 2 funções
}

promotedKey *insertIntoNode(btPage *page, promotedKey *newKey, FILE *fp)
{
    //Procura local pra inserir nova chave na página
    //Se não couber, splitta ele
    //Escreve dados na página
}

/*Insert some key on page*/
btPage *searchPositionOnPageAndInsert(btPage *page, promotedKey *newKey)
{
    //Encontra a posição para inserir a chave
    //Se não existir espaço, precisa criar uma nova página (usem uma função para criar)
    //Salvar dados da nova chave na página
}
/*
    If page size is odd the return is biggest slice
*/
btPage *splitAndCreateNewNode(btPage **page)
{
    //Encontra a posição do meio das chaves
    //Cria espaço pra nova página
    //Copia metade das chaves pra página nova
    //Limpa elas do nó antigo
    //Atualiza os filhos de ambas as páginas
    //Atualiza o número de chaves de ambas
}

/*Extract a promoted key from page*/
promotedKey *extractpromotedKey(btPage *page)
{
    //Aloca espaço pra chave
    //Tira ela da página
    //Atualiza dados da página (filho, número de chaves, etc)
}

/*Split node and writes into file*/
promotedKey *_split(btPage *page, FILE *fp, promotedKey *newKey)
{
    //Divide a página, cria o novo nó (faça numa função auxiliar pois é complexo)
    //Extrai a chave promovida e atualiza os filhos da chave
    //Escreve a página nova e a que foi dividida (com suas atualizações) no arquivo
}

/*Used if promotions reaches root*/
btPage *createNodeWithPromotedKey(promotedKey *promoKey)
{
    //Se promoção cria estrutura para nova raiz,
    //Aloca espaço para ela
    //Salva dados da chave promovida na nova raiz
    //atualiza os filhos deste novo nó/página
    //Pode reusar função que cria página nova e adicionar somente especificidades
}

Errors setNodeAsRoot(btPage *page, FILE *fp)
{
    //Escreve página nova e atualiza o cabeçalho para conter ela como raiz
    //Deveria ser chamada junto com criação de novo nó quando promoção cria uma nova raiz
}

/*Recursive insertion*/
promotedKey *_bTreeInsert(btPage *node, promotedKey *key, FILE *fp)
{
    //Se nó a ser inserido a chave é folha, tenta inserir
    //Caso a inserção crie uma promoção, precisa retornar a chave promovida para a recursão
    //Se não for nó folha, procura qual sub-árvore seguir para inserir numa folha
    //Encontrar a posição correta e descer para filho à esquerda se a chave for menor
    //E descer à direita se for maior
    //Chamar a inserção recursiva pro filho escolhido
    //Se a inserção recursiva retornar uma chave promovida, precisa tentar inserir essa chave promovida no nó atual
    //Retornar chave promovida ou um valor NULL se não houve promoção
}

Errors bTreeInsert(record *newRecord, btPage *root, FILE *fp)
{
    //Função mais abstrata de inserção
    //Prepara os dados da nova chave
    //Tenta inserir recursivamente
    //Se tiver chave promovida no final da recursão, é que existe nova raiz
    //Chama as funções pra criar nova raiz e atualizar o cabeçalho
}

/*Returns rrn if key exist else return -1*/
long bTreeSelect(btPage *node, int key, FILE *fp)
{
    long result;
    int i;
    if (!node)
    {
        return -1;
    }
    else
    {
        for (i = 0; i < node->numberOfKeys; i++)
        {
            //Procura no nó atual se a chave existe
            if (key == node->records[i].key)
            {
                //Se encontrar a chave, retorna RRN dela
                return node->records[i].recordRRN;
            }
            //Se não existir, tenta procurar no filho adequado, recursivamente
            else if (key < node->records[i].key && !node->isLeaf)
            {
                btPage *aux = getPage(node->childs[i], fp);
                result = bTreeSelect(aux, key, fp);
                deallocatePage(aux);
                return result;
            }
        }
        //Procura no ultimo filho
        if (key < node->records[i].key && !node->isLeaf)
        {
            btPage *aux = getPage(node->childs[i], fp);
            result = bTreeSelect(aux, key, fp);
            deallocatePage(aux);
            return result;
        }
    }
    //Se não encontrar (chegar num nó folha e não estiver lá), retorna -1
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