#include <cstdio>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../common/differentiation.h"
#include "../common/print_tree.h"
#include "../common/log_funcs.h"
#include "../common/write_in_file.h"

#include "build_tree.h"

void PrintA(Node* node, FILE* file, Variables* arrayVar);
void PrintAsm(char* filename, Node* node, Variables* arrayVar);
void PrintIntA(Node* node, FILE* file, Variables* arrayVar);
void PrintParentA(Node* node, FILE* file, Variables* arrayVar);

void Print(Node* node, FILE* file, Variables* arrayVar);
void PrintAs(char* filename, Node* node, Variables* arrayVar);
void PrintInt(Node* node, FILE* file, Variables* arrayVar);
void PrintParent(Node* node, FILE* file, Variables* arrayVar);

char* IssuesOp(Node* node);
char* IssuesOpA(Node* node);

void TransformAndEvaluate(Differ* differ)
{
    int changeCount = 0;
    do {
        changeCount = 0;
        TransformationNode(&differ->tree->rootTree, &changeCount, differ->variables, differ->tree);
        GenerateImage(differ);
    } while (changeCount > 0);
}


int main()
{
    OpenLogFile("LOGE1111.txt", "w");
    Differ differ_before = {};
    Tree tree = {};
    Variables array = {};
    differ_before.tree = &tree;
    differ_before.variables = &array;

    CtorRootAndVariebles(&differ_before);
    BuildTreeMiddleEnd("../file/code2.txt", &differ_before);
    printf("qwery\n");
    GenerateImage(&differ_before);
    SetParentPointers(differ_before.tree->rootTree, NULL);
    TransformAndEvaluate(&differ_before);
    PrintTreeToFileWithoutBrackets("../file/code3.txt", differ_before.tree->rootTree, differ_before.variables);
    GenerateImage(&differ_before);
    PrintAsm("../file/defInf.s", differ_before.tree->rootTree, differ_before.variables);
    // PrintAs("../file/defInffff.txt", differ_before.tree->rootTree, differ_before.variables);
}

void PrintAsm(char* filename, Node* node, Variables* arrayVar)
{
    assert(node);

    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Ошибка при открытии файла.\n");
        return;
    }
    fprintf(file, "section .text\n\tglobal _start\n");
    fprintf(file, "_start:\n");
    PrintA(node, file, arrayVar);

    fprintf(file, "\tmov rax, 0x2000001\n\txor rdi, rdi\n\tsyscall\n");

    fprintf(file, "\n%%include \"myPrintf.s\"\n");
    fclose(file);
}

void PrintA(Node* node, FILE* file, Variables* arrayVar)
{
    if (node == NULL) return;
    if (node->type == OPERAT && node->value == SEMICOLON)
    {
        PrintA(node->left, file, arrayVar);
        PrintA(node->right, file, arrayVar);
    }
    else if (node->type == INT || node->type == VAR)
        PrintIntA(node, file, arrayVar);
    else {
        PrintParentA(node, file, arrayVar);
    }
}

void PrintIntA(Node* node, FILE* file, Variables* arrayVar)
{
    if (node->type == VAR) {
        char* token = "";
        if (node->value == 0) token = "r9";
        else if (node->value == 1) token = "rbx";
        else if (node->value == 2) token = "rcx";
        else if (node->value == 3) token = "r11";
        // if (node->parent->type == OPERAT && node->parent->value == EQ)
        if (node->flagDirection == RIGHT)
            fprintf(file, "%s", token);
        else
            fprintf(file, "%s, ", token);
        // else
        //     fprintf(file, "PUSH %s\n", token);

        //fprintf(file, "mov %s, %d\n", token, node->value);
    }
    else {
        fprintf(file, "%d", node->value);
    }
        //fprintf(file, "mov %s, %d\n", "rax", node->value);
}



int lableA[100];
int CNTA = 1;

void PrintParentA(Node* node, FILE* file, Variables* arrayVar)
{
    if (node->type == OPERAT && node->value == EQ) {
        if (node->right->type != OPERAT) {
            fprintf(file, "\tmov ");
            PrintA(node->left, file, arrayVar);
        }
        node->right->flagDirection = RIGHT;
        PrintA(node->right, file, arrayVar);
        fprintf(file, "\n");
    }
    else if (node->type == OPERAT && (node->value == LESS || node->value == MORE)) {
        lableA[node->value] = CNTA;
        CNTA++;
        PrintA(node->left, file, arrayVar);
        PrintA(node->right, file, arrayVar);
        char* token = "";
        if (node->value == MORE) token = "\n\tjl";
        else if (node->value == LESS) token = "\n\tjg ";
        fprintf(file, "%s metka_%d\n", token, lableA[node->value]);
    }
    else if (node->value == PRINTF) {
        if (node->right != NULL && node->right->type == VAR) {
            fprintf(file, "\tmov rdi, ");
            node->right->flagDirection = RIGHT;
            PrintA(node->right, file, arrayVar);
        }
        fprintf(file, "\tcall myPrintf\n");
    }
    else if (node->value == WHILE || node->value == IF)
    {
        lableA[node->value] = CNTA;
        CNTA++;
        fprintf(file, "metka_%d:\n", lableA[node->value]);

        fprintf(file, "\tcmp ");
        PrintA(node->left, file, arrayVar);
        node->flagDirection = RIGHT;
        PrintA(node->right, file, arrayVar);
        fprintf(file, "\tjmp metka_%d\n", lableA[node->value]);
        fprintf(file, "metka_%d:\n", lableA[node->left->value]);
    }
    else
    {
        char* operation = "";
        operation = IssuesOpA(node);
        fprintf(file, "\t%s ", operation);

        if (strcmp(operation, "mul") == 0) {
            node->flagDirection = RIGHT;
            node->right->flagDirection = RIGHT;
            PrintA(node->right, file, arrayVar);
        } else {
            node->flagDirection = LEFT;
            node->right->flagDirection = LEFT;
            PrintA(node->left, file, arrayVar);

            node->flagDirection = RIGHT;
            node->right->flagDirection = RIGHT;
            PrintA(node->right, file, arrayVar);
        }
        fprintf(file, "\n");
    }
}

void PrintAs(char* filename, Node* node, Variables* arrayVar)
{
    assert(node);

    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Ошибка при открытии файла.\n");
        return;
    }

    Print(node, file, arrayVar);
    fprintf(file, "OUT\n");
    fprintf(file, "HLT\n");
    fclose(file);
}

void Print(Node* node, FILE* file, Variables* arrayVar)
{
    if (node == NULL) return;
    if (node->type == OPERAT && node->value == SEMICOLON)
    {
        Print(node->left, file, arrayVar);
        Print(node->right, file, arrayVar);
    }
    else if (node->type == INT || node->type == VAR)
        PrintInt(node, file, arrayVar);
    else {
        PrintParent(node, file, arrayVar);
    }
}

void PrintInt(Node* node, FILE* file, Variables* arrayVar)
{
    if (node->type == VAR) {
        char* token = "";
        if (node->value == 0) token = "RAX";
        else if (node->value == 1) token = "RBX";
        else if (node->value == 2) token = "RCX";
        else if (node->value == 3) token = "RDX";
        if (node->parent->type == OPERAT && node->parent->value == EQ)
            fprintf(file, "POP %s\n", token);
        else
            fprintf(file, "PUSH %s\n", token);
    }
    else {
        fprintf(file, "PUSH %d\n", node->value);
    }
}

int lable[100];
int CNT = 1;

void PrintParent(Node* node, FILE* file, Variables* arrayVar)
{
    if (node->type == OPERAT && node->value == EQ) {
        Print(node->right, file, arrayVar);
        Print(node->left, file, arrayVar);
    }
    else if (node->type == OPERAT && (node->value == LESS || node->value == MORE)) {
        lable[node->value] = CNT;
        CNT++;
        Print(node->right, file, arrayVar);
        Print(node->left, file, arrayVar);
        char* token = "";
        if (node->value == MORE) token = "JA";
        else if (node->value == LESS) token = "JB";
        fprintf(file, "%s :%d\n", token, lable[node->value]);
    }
    else if (node->value == WHILE || node->value == IF)
    {
        lable[node->value] = CNT;
        CNT++;
        fprintf(file, ":%d\n", lable[node->value]);
        node->flagDirection = LEFT;
        Print(node->left, file, arrayVar);
        node->flagDirection = RIGHT;
        Print(node->right, file, arrayVar);
        fprintf(file, "JMP :%d\n", lable[node->value]);
        fprintf(file, ":%d\n", lable[node->left->value]);
    }
    else
    {
        node->flagDirection = LEFT;
        Print(node->left, file, arrayVar);

        char* operation = "";
        operation = IssuesOp(node);

        node->flagDirection = RIGHT;
        Print(node->right, file, arrayVar);
        fprintf(file, "%s\n", operation);
    }
}

char* IssuesOp(Node* node)
{
    if (node->value == ADD) return "ADD";
    if (node->value == SUB) return "SUB";
    if (node->value == DIV) return "DIV";
    if (node->value == MUL) return "MUL";
    if (node->value == POW) return "POW";
    else return "BBBB";
}

char* IssuesOpA(Node* node)
{
    if (node->value == ADD) return "add";
    if (node->value == SUB) return "sub";
    if (node->value == DIV) return "div";
    if (node->value == MUL) return "mul";
    if (node->value == POW) return "pow";
    else return "BBBB";
}
