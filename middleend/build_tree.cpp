#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/differentiation.h"
#include "../common/log_funcs.h"
#include "build_tree.h"


#define BUF_V tokens[pBuf].value
#define BUF_T tokens[pBuf].type
#define BUF_L tokens[old_p].left
#define BUF_R tokens[old_p].right
#define BUF tokens[old_p]

int LINE_COUNT = 0;

char* s = NULL;
int p = 0;
Variables arrayVar;
int size = 0;
int pBuf = 0;

void AddNewId(int type, int val, char* name)
{
    arrayVar.data[arrayVar.size].type = type;
    arrayVar.data[arrayVar.size].value = val;
    arrayVar.data[arrayVar.size].name = name;
    arrayVar.size++;
}

void CreatId()
{
    AddNewId(OPERAT, SIN, "sin");
    AddNewId(OPERAT, COS, "cos");
    AddNewId(OPERAT, TAN, "tg");
    AddNewId(OPERAT, COT, "ctg");
    AddNewId(OPERAT, LN, "ln");
    AddNewId(OPERAT, ARCSIN, "arcsin");
    AddNewId(OPERAT, ARCCOS, "arccos");
    AddNewId(OPERAT, ARCCOT, "arctg");
    AddNewId(OPERAT, ARCTAN, "arcctg");
    AddNewId(OPERAT, IF, "if");
    AddNewId(OPERAT, WHILE, "while");
    AddNewId(OPERAT, PRINTF, "printf");
}

int BuildTreeMiddleEnd(char* filename, Differ* differ_before) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка при открытии файла.\n");
    }

    Buffer array = {NULL, 0};

    GetFileSize(file, &array);
    array.buffer = (char*)calloc(array.size + 1, sizeof(char));
    ReadFileInBuffer(file, &array);
    FillText(&array);
    s = array.buffer;
    p = 0;

    size = array.size;
    arrayVar.capacity = CAPACITY;
    arrayVar.size = ZERO;
    arrayVar.data = (VariableData*)calloc(arrayVar.capacity, sizeof(VariableData));
    CreatId();

    Node* tokens = (Node*)calloc(1000, sizeof(Node));
    TokenInizial(tokens);

    for (int i = 0; i < pBuf; i++)
    {
        printf("%d: type = %d, values = %d\n", i, tokens[i].type, tokens[i].value);
    }
    pBuf = 0;
    Node* node = ParseInputTokens(tokens);

    differ_before->tree->rootTree = node;
    differ_before->variables = &arrayVar;
    GenerateImage(differ_before);
    return 0;
}


Node* ParseInputTokens(Node* tokens) {
    Node* node = NULL;
    fprintf(LOG_FILE, "я нахожусь в ParseInputTokens, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);

    node = ExtractStatementBody(tokens);

    if (BUF_V != END) printf("ошибкаG\n");
    return node;
}

Node* GetIf(Node* tokens) {
    Node* ifNode = &tokens[pBuf++];
    if (BUF_V != BRACKET) {
        printf("Ошибка: ожидается '}' после блока While\n");
    }
    pBuf++;
    Node* condition = GetAddition(tokens);
    if (BUF_V != BRACKET) {
        printf("Ошибка: ожидается '}' после блока While\n");
    }
    pBuf++;
    if (BUF_V != OPEN_BRACE) {
        printf("Ошибка: ожидается '{' после условия IF\n");
        return NULL;
    }

    pBuf++;  // Пропустить '{'

    ifNode->left = condition;
    ifNode->right = ExtractStatementBody(tokens);

    return ifNode;
}

Node* ExtractStatementBody(Node* tokens)
{
    CREAT_NODE(whileBodyNode);
    InitializeNode(whileBodyNode, OPERAT, SEMICOLON, NULL, NULL, NULL);

    Node* currentNode = whileBodyNode;

    while (BUF_V != CLOSE_BRACE && BUF_V != END) {
        if (BUF_V == WHILE) {
            Node* nestedWhile = GetExpressionWithPowerhile(tokens);
            currentNode->left = nestedWhile;
        } else if (BUF_V == PRINTF) {
            currentNode->left = &tokens[pBuf++];
            printf("pBuf = %d\n", pBuf);
            currentNode->left->right = &tokens[pBuf];
            if (tokens[pBuf + 1].type == VAR) {
                pBuf++;
                printf("pBuf = %d\n", pBuf);
                currentNode->left->right->right = &tokens[pBuf];
            }
            printf("pBuf = %d\n", pBuf);
            pBuf++;
        } else if (BUF_V == IF) {
            Node* nestedIf = GetIf(tokens);
            currentNode->left = nestedIf;
        } else {
            Node* statement = GetAddition(tokens);
            currentNode->left = statement;
        }

        pBuf++;

        if (BUF_V != CLOSE_BRACE && BUF_V != END) {
            CREAT_NODE(nextStatement);
            InitializeNode(nextStatement, OPERAT, SEMICOLON, NULL, NULL, NULL);
            currentNode->right = nextStatement;
            currentNode = nextStatement;
        }
    }
    return whileBodyNode;
}

// Node* GetExpressionWithPowerhile(Node* tokens) {
//     fprintf(LOG_FILE, "я нахожусь в GetExpressionWithPowerhile, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);
//     Node* whileNode = &tokens[pBuf++];
//     fprintf(LOG_FILE, "я собираюсь вызвать GetExpression, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);
//
//     fprintf(LOG_FILE, "я после вызова GetExpression, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);
//
//     if (BUF_V != OPEN_BRACE) {
//         printf("Ошибка: ожидается '{' после условия While\n");
//         return NULL;
//     }
//     pBuf++;  // Пропустить '{'
//
//     Node* uslovie = ExtractStatementBody(tokens);
//
//     if (BUF_V != CLOSE_BRACE) {
//         printf("Ошибка: ожидается '{' после условия While\n");
//         return NULL;
//     }
//     //pBuf++;
//
//      if (BUF_V != BRACKET) {
//         printf("Ошибка: ожидается '(' после блока While\n");
//     }
//     pBuf++;
//
//     Node* condition = GetAddition(tokens);
//
//     if (BUF_V != BRACKET) {
//         printf("Ошибка: ожидается ')' после блока While\n");
//     }
//     pBuf++;
//
//     whileNode->left = condition;
//     whileNode->right = uslovie;
//
//     return whileNode;
// }

Node* GetExpressionWithPowerhile(Node* tokens) {
    fprintf(LOG_FILE, "я нахожусь в GetExpressionWithPowerhile, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);
    Node* whileNode = &tokens[pBuf++];
    fprintf(LOG_FILE, "я собираюсь вызвать GetExpression, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);

    if (BUF_V != BRACKET) {
        printf("Ошибка: ожидается '(' после блока While\n");
    }
    pBuf++;

    Node* condition = GetAddition(tokens);

    if (BUF_V != BRACKET) {
        printf("Ошибка: ожидается ')' после блока While\n");
    }
    pBuf++;

    fprintf(LOG_FILE, "я после вызова GetExpression, pbuf = %d, token = %d\n", pBuf, tokens[pBuf]);

    if (BUF_V != OPEN_BRACE) {
        printf("Ошибка: ожидается '{' после условия While\n");
        return NULL;
    }

    pBuf++;  // Пропустить '{'

    whileNode->left = condition;
    whileNode->right = ExtractStatementBody(tokens);

    return whileNode;
}


Node* GetId(Node* tokens)
{
    if (tokens[pBuf].type == VAR) {
        return &tokens[pBuf++];
    }
    else return NULL;
}

Node* GetAddition(Node* tokens)
{
    int old_p = pBuf;
    Node* node = GetId(tokens);
    if (node == NULL)
    {
        node = GetExpression(tokens);
    }
    else if (tokens[pBuf].value != EQ && tokens[pBuf].value != MORE && tokens[pBuf].value != LESS)
    {
        pBuf = old_p;
        node = GetExpression(tokens);
    }
    else {
        Node* main;
        if ((tokens[pBuf].value == EQ   ||
            tokens[pBuf].value == MORE  ||
            tokens[pBuf].value == LESS) &&
            tokens[pBuf].type == OPERAT)
        {
            main = &tokens[pBuf];
            pBuf++;
        }
        Node* node2 = GetExpression(tokens);
        Node* nodeL = node;
        node = main;
        node->left = nodeL;
        node->right = node2;
    }
    return node;
}


Node* GetExpression(Node* tokens)
{
    Node* node = GetTerm(tokens);

    while (BUF_V == ADD || BUF_V == SUB) {
        int old_p = pBuf;
        pBuf++;
        Node* node2 = GetTerm(tokens);

        BUF_L = node;
        BUF_R = node2;
        node = &BUF;
    }
    return node;
}

Node* GetTerm(Node* tokens)
{
    Node* node = GetExpressionWithPower(tokens);

    while (BUF_V == MUL || BUF_V == DIV || BUF_V == POW) {
        int old_p = pBuf;
        pBuf++;
        Node* node2 = GetExpressionWithPower(tokens);

        BUF_L = node;
        BUF_R = node2;
        node = &BUF;
    }
    return node;
}

Node* GetExpressionWithPower(Node* tokens)
{
    Node* node = GetP(tokens);

    while (BUF_V == POW) {
        int old_p = pBuf;
        pBuf++;
        Node* node2 = GetP(tokens);

        BUF_L = node;
        BUF_R = node2;
        node = &BUF;
    }
    return node;
}

Node* GetP(Node* tokens)
{
    Node* node = NULL;

    if (BUF_V == BRACKET) {
        pBuf++;
        node = GetExpression(tokens);
        if (BUF_V != BRACKET) printf("ошибкаP\n");
        pBuf++;
    }
    else if (BUF_T == OPERAT || BUF_T == VAR) {
        node = GetOparat(tokens);
        if (node->type == OPERAT)
            node->left = GetP(tokens);
    }
    else {
        node = GetNumber(tokens);
    }
    return node;
}

Node* GetNumber(Node* tokens)
{
    return &tokens[pBuf++];
}

Node* GetOparat(Node* tokens)
{
    for (int i = 0; i < arrayVar.size; i++) {
        if (arrayVar.data[i].type == OPERAT) {
            if (arrayVar.data[i].value == BUF_V) {
                return &tokens[pBuf++];
            }
        }
    }
    return &tokens[pBuf++];
}

int GetNumFromMyLanguage()
{
    int val = 0;
    while ('0' <= s[p] && s[p] <= '9') {
        val = val * 10 + s[p] - '0';
        p++;
    }
    return val;
}

void AddVar(char* name, Node* tokens)
{
    int CNTA = 0;
    for (int i = 0; i < arrayVar.size; i++)
    {
        if (arrayVar.data[i].type == VAR)
        {
            CNTA++;
        }
    }
    AddNewId(VAR, CNTA + 1, name);
    InitializeNode(&tokens[pBuf], VAR, CNTA+1, NULL, NULL, NULL);
    pBuf++;
}

void AddVariableVar(Variables* arrayVar, int type, int value, const char* name) {
    if (arrayVar->size >= arrayVar->capacity) {
        // Увеличиваем емкость массива, если необходимо
        arrayVar->capacity *= 2;
        arrayVar->data = (VariableData*)realloc(arrayVar->data, arrayVar->capacity * sizeof(VariableData));
    }
    arrayVar->data[arrayVar->size].type = type;
    arrayVar->data[arrayVar->size].value = value;
    arrayVar->data[arrayVar->size].name = strdup(name);
    arrayVar->size++;
}


#define INIT(type, val) InitializeNode(&tokens[pBuf++], type, val, NULL, NULL, NULL);

void GetOparatperatFromMyLanguage(Node* tokens)
{
    int flagMatch = 0;
    char token[OP_LEN] = "";
    int i = 0;
    while ('a' <= s[p] && s[p] <= 'z') {
        token[i] = s[p];
        i++;
        p++;
    }

    for (int j = 0; j < arrayVar.size; j++) {
        if (strcmp(token, arrayVar.data[j].name) == 0) {
            flagMatch++;
            int val = arrayVar.data[j].value;
            if (arrayVar.data[j].type == OPERAT) {
                InitializeNode(&tokens[pBuf++], OPERAT, val, NULL, NULL, NULL);
            } else if (arrayVar.data[j].type == VAR) {
                InitializeNode(&tokens[pBuf++], VAR, val, NULL, NULL, NULL);
            }
        }
    }

    if (flagMatch == 0) {
        int CNTA = 0;
        while (arrayVar.data[CNTA].type == OPERAT) CNTA++;
        int val = arrayVar.size - CNTA;
        AddVariableVar(&arrayVar, VAR, val, token);
        InitializeNode(&tokens[pBuf++], VAR, val, NULL, NULL, NULL);
    }
}

void SkipSpaces()
{
    while (s[p] == '\t' || s[p] == '\n' || s[p] == ' ')
        p++;
}

void TokenInizial(Node* tokens)
{
    int num = 0;
    while(p <= size)
    {
        switch(s[p])
        {
            case '(':
                INIT(OPERAT, BRACKET);
                p++;
                break;
            case ')':
                INIT(OPERAT, BRACKET);
                p++;
                break;
            case '{':
                INIT(OPERAT, OPEN_BRACE);
                p++;
                break;
            case '}':
                INIT(OPERAT, CLOSE_BRACE);
                p++;
                break;
            case '\0':
                if (p < size) {
                    p++;
                    break;
                }
                else {
                    INIT(OPERAT, END);
                    p++;
                    break;
                }
            // case '\n':
            //     LINE_COUNT++;
            //     p++;
            //     printf("asdf\n");
            //     if (s[p] == '\0') p++;
            //     break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                num = GetNumFromMyLanguage();
                INIT(INT, num);
                break;
            case '+':
                INIT(OPERAT, ADD);
                p++;
                break;
            case '!':
                INIT(OPERAT, MORE);
                p++;
                break;
            case '@':
                INIT(OPERAT, LESS);
                p++;
                break;
            case '-':
                INIT(OPERAT, SUB);
                p++;
                break;
            case ';':
                INIT(OPERAT, SEMICOLON);
                p++;
                break;
            case '*':
                INIT(OPERAT, MUL);
                p++;
                break;
            case '^':
                INIT(OPERAT, POW);
                p++;
                break;
            case '/':
                INIT(OPERAT, DIV);
                p++;
                break;
            case '=':
                INIT(OPERAT, EQ);
                p++;
                break;
            default:
            {
                if ('a' <= s[p] && s[p] <= 'z') {
                    GetOparatperatFromMyLanguage(tokens);
                }
                else SkipSpaces();
                break;
            }
        }
    }
}
