#ifndef _BINARYTREE_H
#define _BINARYTREE_H



 typedef struct binaryTreeNode{
  int data;
  struct binaryTreeNode* left;
  struct binaryTreeNode* right;
}BinaryTreeNode;


 typedef struct {
    BinaryTreeNode* root;
}BinaryTree;

void insert(BinaryTree* tree, int value);
#endif

