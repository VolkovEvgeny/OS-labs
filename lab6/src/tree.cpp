#include "tree.h"
#include <iostream>
#include <stdio.h>

using namespace std;

Tree *createTree(int value)
{
    Tree *tree = (Tree *)malloc(sizeof(Tree));

    tree->id = value;
    tree->right = NULL;
    tree->left = NULL;

    return tree;
}

Tree *createNode(Tree *root, int value)
{
    Tree *res = createTree(value);
    if (value == root->id)
    {
        return root;
    }
    if (value > (root->id) && (root->right) == NULL)
    {
        root->right = createTree(value);
        return root;
    }
    if (value < (root->id) && (root->left) == NULL)
    {
        root->left = createTree(value);
        return root;
    }
    if (value > (root->id) && (root->right) != NULL)
    {
        root->right = createNode(root->right, value);
    }
    if (value < (root->id) && (root->left) != NULL)
    {
        root->left = createNode(root->left, value);
    }
    return root;
}

void printTree(Tree *root, int n)
{
    if (root != NULL)
    {
        printTree(root->right, n + 1);
        for (int i = 0; i < n; i++)
            printf("\t");
        printf("%d\n", root->id);
        printTree(root->left, n + 1);
    }
}

bool existNode(Tree *root, int id)
{
    if (root == NULL)
    {
        return false;
    }
    if (root->id == id)
    {
        return true;
    }
    return existNode(root->left, id) || existNode(root->right, id);
}

Tree *deleteNode(Tree *root, int id)
{
    if (root == NULL)
        return root;
    if (id < root->id)
    {
        root->left = deleteNode(root->left, id);
        return root;
    }
    if (id > root->id)
    {
        root->right = deleteNode(root->right, id);
        return root;
    }
    free(root);
    root = NULL;
    return root;
}
