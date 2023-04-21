#include <iostream>

using namespace std;

struct Tree
{
    int id;
    Tree *left;
    Tree *right;
};

Tree *createNode(Tree *root, int id);
bool existNode(Tree *root, int id);
Tree *deleteNode(Tree *root, int id);
Tree *createTree(int id);
void printTree(Tree *root, int n);