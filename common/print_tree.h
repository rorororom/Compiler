#ifndef PRINT
#define PRINT

#include <stdlib.h>
#include "differentiation.h"

void Preamble();
void EndOfDocument();
void PrintStartProekt();

void PrintParentANorNullTex(Node* node, FILE* file, Variables* arrayVar);
void PrintNodeTex(Node* node, FILE* file, Variables* arrayVar);
void PrintParentANullTex(Node* node, FILE* file, Variables* arrayVar);
void PrintIntANodeTex(Node* node, FILE* file, Variables* arrayVar);

void PrintTreeLaTex(const char* expression, Node* node, Variables* arrayVar, Lines* text);

void PrintInFileInfForm(Node* node, Variables* arrayVar);
void PrintNodeInfForm(Node* node, FILE* file, Variables* arrayVar);

void PrintTreeToFileWithoutBrackets(Node* node, Variables* arrayVar);
void PrintNode(Node* node, FILE* file, Variables* arrayVar);
void PrintIntANode(Node* node, FILE* file, Variables* arrayVar);
void PrintParentANorNull(Node* node, FILE* file, Variables* arrayVar);
void PrintParentANull(Node* node, FILE* file, Variables* arrayVar);

void EndTex();
#endif
