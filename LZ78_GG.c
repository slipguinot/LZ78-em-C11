#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#define TAMD 400

enum{FALSE = 0, TRUE};

typedef struct arvore{
    unsigned short int codigo;
    char *entrada;
    struct arvore *esq;
    struct arvore *dir;
}Arvore;

Arvore *arv_vazia();

int arv_procurar_elemento(Arvore *a, char *entrada);
char *arv_procurar_elemento_por_codigo(Arvore *a, unsigned short int codigo);
int arv_procurar_codigo(Arvore *a, char *cadeia);
Arvore * arv_libera(Arvore *a);
Arvore *inserir_novo_elemento(Arvore *a, char *entrada, unsigned short int codigo);

char * junta_string(char *velha, char *nova);
char *copia_string(char *velha, char *nova);
void troca_ext(char *caminho, char *extensao_a_usar);

int compressao(FILE *arq, char *caminho);
int descompressao(FILE *arq, char *caminho, char *ext_original);

int main()
{
    setlocale(LC_ALL, "Portuguese");
    char caminho[100];
    int exit = FALSE;
    int decisao;
    FILE *arq, *intermedio, *saida;
    while(!exit)
    {
        printf("1. Comprimir um arquivo.");
        printf("\n2. Descomprimir um arquivo.");
        printf("\n3. Sair.\n");
        scanf("%d", &decisao);
        if(decisao < 1 || decisao > 3)
            exit = TRUE;

        switch(decisao)
        {
        case 1:
                system("cls");
                printf("Adicione o caminho do arquivo(com extensão):");
                scanf(" %99[^\n]", caminho);
                arq = fopen(caminho, "rb");
                if(arq == NULL)
                {
                    printf("Arquivo especificado não encontrado, por favor tente novamente.\n");
                    fclose(arq);
                    system("pause");
                    break;
                }
                ///CHAMADA DA FUNÇÃO DE COMPRIMIR
                compressao(arq, caminho);
                printf("\n");
                fclose(arq);
                system("pause");
            break;
        case 2:
            system("cls");
            char ext_original[11];
            printf("Adicione o caminho do arquivo comprimido(com extensão):");
            scanf(" %99[^\n]", caminho);
            arq = fopen(caminho, "rb");
            if(arq == NULL)
                {
                    printf("Arquivo especificado não encontrado, por favor tente novamente.\n");
                    system("pause");
                    fclose(arq);
                    break;
                }
            printf("Informe a extensão do arquivo original:");
            scanf(" %10[^\n]", ext_original);
            descompressao(arq, caminho, ext_original);
            printf("\n");
            fclose(arq);
            system("pause");
            break;
        case 3:
            exit = TRUE;
            break;
        default:
            decisao = '\0';
            break;
        }

    }
    return 0;
}

Arvore *arv_vazia()
{
    return NULL;
}

int arv_procurar_elemento(Arvore *a, char *entrada)
{
    if(a == NULL)
        return FALSE;
    if(strcmp(a->entrada, entrada)==0)
    {
        return TRUE;
    }
    else
        return arv_procurar_elemento(a->esq,entrada) || arv_procurar_elemento(a->dir, entrada);
}

char *arv_procurar_elemento_por_codigo(Arvore *a, unsigned short int codigo)
{
    if(a == NULL)
        return NULL;
    if(codigo == a->codigo)
    {
        return a->entrada;
    }
    else
    {
        char *se, *sd;
        if(codigo < a->codigo)
            {se = arv_procurar_elemento_por_codigo(a->esq, codigo);
            printf("fui para esquerda\n");}
        else
            sd = arv_procurar_elemento_por_codigo(a->dir, codigo);
    }
}


int arv_procurar_codigo(Arvore *a, char *cadeia)
{
    if(a == NULL)
    {
       return -1;
    }
    else if(strcmp(a->entrada, cadeia) == 0)
    {
        return a->codigo;
    }
   arv_procurar_codigo(a->esq, cadeia);
   arv_procurar_codigo(a->dir, cadeia);
}

Arvore * arv_libera(Arvore * a)
{
    if(a != NULL)
    {
        arv_libera(a->esq);
        arv_libera(a->dir);
        free(a);
    }
    return NULL;
}

Arvore *inserir_novo_elemento(Arvore *a, char *entrada, unsigned short int codigo)
{
    if (a == NULL)
    {
        a = (Arvore*)malloc(sizeof(Arvore));
        a->entrada = (char*)malloc(sizeof(entrada));
        a->entrada[0] = '\0';
        strcpy(a->entrada, entrada);
        a->codigo = codigo;
        a->dir = NULL;
        a->esq = NULL;
    }
    else if(codigo < a->codigo)
    {
        a->esq = inserir_novo_elemento(a->esq, entrada, codigo);
        printf("fui para esquerda\n");
    }
    else
        a->dir = inserir_novo_elemento(a->dir, entrada, codigo);
    return a;
}

char * junta_string(char *velha, char *nova)
{

    size_t len = strlen(velha) + strlen(nova) + 1;

    char *out = (char*)malloc(len);
    out[0] = '\0';

    strcpy (out, velha);
    strcat (out, nova);

    return out;
}

char *copia_string(char *velha, char *nova)
{
    size_t len = strlen(nova) + 1;
    char*out = (char*)malloc(len);
    out[0] = '\0';

    strcpy(out, nova);
    return out;
}

void troca_ext(char *caminho, char *extensao_a_usar)
{
    char *pFile;
    char *pExt;
    pFile = strrchr(caminho, '\\');
    pFile = pFile == NULL ? caminho : pFile+1;

    pExt = strrchr(pFile, '.');
    if (pExt != NULL)
        strcpy(pExt, extensao_a_usar);
    else
        strcat(pFile, extensao_a_usar);
}

int compressao(FILE *arq, char *caminho)
{
    FILE *saida, *inter;
    troca_ext(caminho, ".comp");
    inter = fopen("Dicionario_Compressao.txt", "w");
    saida = fopen(caminho, "wb");
    if(saida == NULL)
    {
        printf("O arquivo de saída não foi aberto. Abortando.");
        main();
    }

    char *cadeia, *cadeia_auxiliar, *entrada, *auxiliar;
    int procurador, qtde_leitura, teste, trigger;
    trigger = FALSE;
    entrada = (char*)malloc(sizeof(char)+2);
    unsigned short int codigo=1, codigo_auxiliar = 0;
    Arvore *dicionario = arv_vazia();
    cadeia = (char*)malloc(sizeof(char));
    cadeia[0] = '\0';
    dicionario = inserir_novo_elemento(dicionario, cadeia, 0);
    fprintf(inter, "[%s],[%hhu]\n", cadeia, 0);
    while(!trigger && !feof(arq))
    {
        if(codigo_auxiliar > TAMD)
        {
            printf("Triggered. Codigo Auxiliar:[%hhu]\n", codigo_auxiliar);
            trigger = TRUE;
        }

        if(!trigger)
        {
            entrada[0] = '\0';
            fread(entrada, sizeof(char), 1, arq);

            cadeia_auxiliar = copia_string(cadeia_auxiliar, cadeia);
            cadeia = junta_string(cadeia, entrada);
            procurador = arv_procurar_elemento(dicionario, cadeia);
            if(procurador == FALSE)
            {
                codigo_auxiliar = arv_procurar_codigo(dicionario, cadeia_auxiliar);
                fprintf(inter, "[%s],[%hhu]\n", cadeia, codigo);
                fwrite(&codigo_auxiliar, sizeof(unsigned short int), 1, saida);
                fwrite(&entrada[0], sizeof(char), 1, saida);
                dicionario = inserir_novo_elemento(dicionario, cadeia, codigo);
                codigo++;
                cadeia[0] = '\0';
                if(ferror(arq))
                {
                    perror("Erro na gravacao");
                    fclose(arq);
                    exit(1);
                }
            }
        }

    }
    qtde_leitura = 0;
    while(trigger && !feof(arq))
    {

        entrada[0] = '\0';
        teste = fread(entrada, sizeof(char), 1, arq);
        if(teste == 1)
            fwrite(entrada, sizeof(char), 1, saida);

        qtde_leitura++;
        if(ferror(arq))
        {
            perror("Erro na gravacao");
            fclose(arq);
            exit(1);
        }
    }
    if(feof(arq) && codigo < TAMD)
       {
            codigo_auxiliar = arv_procurar_codigo(dicionario, cadeia);
            fwrite(&codigo_auxiliar, sizeof(unsigned short int), 1, saida);
       }
    fclose(saida);
    fclose(inter);
    printf("\nQuantidade de leituras fora da compressão:[%d]", qtde_leitura);
    return 0;
}

int descompressao(FILE *arq, char *caminho, char *ext_original)
{
    FILE *saida, *inter;
    inter = fopen("Dicionario_descompressao.txt", "w");
    troca_ext(caminho, ext_original);
    saida = fopen(caminho, "wb");
    if(saida == NULL || inter == NULL)
    {
        printf("O arquivo de saída não foi aberto. Abortando.");
        main();
    }
    char *entrada, *cadeia, *cadeia_arvore, *cadeia_auxiliar;
    cadeia_auxiliar = (char*)malloc(sizeof(char));
    unsigned short int codigo =1, *codigo_auxiliar;
    int trigger = FALSE, procurador;
    int teste, qtd_leitura;
    entrada = (char*)malloc(sizeof(char)+2);
    cadeia = (char*)malloc(sizeof(char));
    cadeia[0] = '\0';
    cadeia_auxiliar[0] = '\0';

    codigo_auxiliar = (unsigned short int*)malloc(sizeof(unsigned short int));

    Arvore *dicionario = arv_vazia();
    dicionario = inserir_novo_elemento(dicionario, cadeia, 0);
    fprintf(inter, "[%s],[%hhu]\n", cadeia_auxiliar, 0);
    while(!trigger && !feof(arq))
    {
        fread(codigo_auxiliar, sizeof(unsigned short int), 1, arq);
        entrada[0] = '\0';
        fread(entrada, sizeof(char), 1, arq);
        cadeia_arvore = arv_procurar_elemento_por_codigo(dicionario, *codigo_auxiliar);

        cadeia_auxiliar = copia_string(cadeia_auxiliar, cadeia_arvore);
        cadeia_auxiliar = junta_string(cadeia_auxiliar, entrada);

        fwrite(cadeia_auxiliar, sizeof(char),strlen(cadeia_auxiliar), saida);
        procurador = arv_procurar_elemento(dicionario, cadeia_auxiliar);
        if(procurador == FALSE)
        {
            fprintf(inter, "[%s],[%hhu]\n", cadeia_auxiliar, codigo);
            dicionario = inserir_novo_elemento(dicionario, cadeia_auxiliar, codigo);
        }
        codigo++;

        if(*codigo_auxiliar >TAMD)
        {
            printf("Triggered. Codigo Auxiliar:[%hhu]", *codigo_auxiliar);
            trigger = TRUE;
        }
    }
    qtd_leitura = 0;
    while(trigger && !feof(arq))
    {
        entrada[0] = '\0';
        teste = fread(entrada, sizeof(char), 1, arq);
        if(teste == 1)
            fwrite(entrada, sizeof(char), 1, saida);
        qtd_leitura++;
    }
    printf("\nQuantidade de leituras fora da compressão:[%d]", qtd_leitura);
    fclose(saida);
    fclose(inter);
    return 0;
}
