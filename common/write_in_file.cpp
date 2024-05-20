#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "differentiation.h"
#include "write_in_file.h"

void PrintTreeToFileWithoutBrackets(char* filename, Node* node, Variables* arrayVar)
{
    assert(node);

    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Ошибка при открытии файла.\n");
        return;
    }

    PrintNode(node, file, arrayVar);
    fclose(file);
}

void PrintNode(Node* node, FILE* file, Variables* arrayVar)
{
    if (node == NULL) return;
    if (node->type == OPERAT && node->value == SEMICOLON)
    {
        PrintNode(node->left, file, arrayVar);
        if (!(node->left && (node->left->type == OPERAT && (node->left->value == WHILE || node->left->value == IF))))
        {
            fprintf(file, ";\n");  // Print semicolon only if the left node is not while or if
        }
        PrintNode(node->right, file, arrayVar);
    }
    else if (node->type == INT || node->type == VAR)
        PrintIntANode(node, file, arrayVar);
    else {
        PrintParentANull(node, file, arrayVar);
    }
}

void PrintIntANode(Node* node, FILE* file, Variables* arrayVar)
{
    PrintNode(node->left, file, arrayVar);
    if (node->type ==VAR) {
        int CNTA = 0;
        while (arrayVar->data[CNTA].type == OPERAT) CNTA++;
        char* nameValue = strdup(arrayVar->data[node->value + CNTA].name);
        fprintf(file, "%s ", nameValue);
    }
    else {
        fprintf(file, "%d ", node->value);
    }
    PrintNode(node->right, file, arrayVar);
}

void PrintParentANorNull(Node* node, FILE* file, Variables* arrayVar)
{
    if (node->value == SEMICOLON && node->type == OPERAT)
        return;
    int operationParent = CheckingPriorityOperation(node->parent->value);
    if (operationParent == ERROR_OP) printf("error operation\n");
    int operationNowNode = CheckingPriorityOperation(node->value);
    if (operationNowNode == ERROR_OP) printf("error operation\n");

    if (operationParent > operationNowNode   || ((node->parent->flagDirection == RIGHT) &&
       (operationParent == operationNowNode) &&  (node->value == DIV)))
        fprintf(file, "( ");
    else if ((node->parent->flagDirection == RIGHT) && (operationParent == operationNowNode)  &&
             (node->value == SUB))
        node->value = ADD;

    if (SIN <= node->value && node->value <= ARCCOT)
    {
        char* operation = IssuesOperation(node);
        fprintf(file, "%s( ", operation);

        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);
        fprintf(file, ") ");
    }
    if (node->value == WHILE || node->value == IF)
    {
        char* operation = IssuesOperation(node);
        fprintf(file, "%s( ", operation);
        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);
        fprintf(file, ") ");
        node->flagDirection = RIGHT;
        fprintf(file, "{ ");
        PrintNode(node->right, file, arrayVar);
        fprintf(file, "} ");
    }
    else
    {
        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);
        char* operation = "";
        if (node->value == POW)
            operation = IssuesOperation1(node);
        else
            operation = IssuesOperation(node);

        fprintf(file, "%s ", operation);

        node->flagDirection = RIGHT;
        PrintNode(node->right, file, arrayVar);
    }

    if (operationParent > operationNowNode || ((node->parent->flagDirection == RIGHT) &&
       (operationParent == operationNowNode) && (node->value == DIV)))
    {
        fprintf(file, ") ");
    }
}

void PrintParentANull(Node* node, FILE* file, Variables* arrayVar)
{
    if (5 <= node->value && node->value <= 13)
    {
        char* operation = IssuesOperation(node);
        fprintf(file, "%s( ", operation);

        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);
        fprintf(file, ") ");
    }
    if (node->value == WHILE || node->value == IF)
    {
        char* operation = IssuesOperation(node);
        fprintf(file, "%s ( ", operation);
        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);
        fprintf(file, ")\n");
        node->flagDirection = RIGHT;
        fprintf(file, "{ ");
        PrintNode(node->right, file, arrayVar);
        fprintf(file, "}\n");
    }
    else
    {
        node->flagDirection = LEFT;
        PrintNode(node->left, file, arrayVar);

        char* operation = "";
        if (node->value == POW)
            operation = IssuesOperation1(node);
        else
            operation = IssuesOperation(node);

        node->flagDirection = RIGHT;
        fprintf(file, "%s ", operation);
        PrintNode(node->right, file, arrayVar);
    }
}
