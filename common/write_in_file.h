#ifndef WRITE
#define WRITE

#include <stdlib.h>
#include "differentiation.h"

void PrintTreeToFileWithoutBrackets(char* filename, Node* node, Variables* arrayVar);
void PrintNode(Node* node, FILE* file, Variables* arrayVar);
void PrintIntANode(Node* node, FILE* file, Variables* arrayVar);
void PrintParentANorNull(Node* node, FILE* file, Variables* arrayVar);
void PrintParentANull(Node* node, FILE* file, Variables* arrayVar);

#endif
