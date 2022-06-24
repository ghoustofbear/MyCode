#include"BinaryTree.h"
#include <stdio.h>
void insert(BinaryTree* tree, int value)
{
  
    BinaryTreeNode* node=(BinaryTreeNode*)malloc(sizeof(BinaryTreeNode));
    node->data=value;
    node->left=NULL;
    node->right=NULL;
    if(tree->root==NULL)//判断树是不是空树
    {
        tree->root=tree;
    }
    else
    {
        BinaryTreeNode* temp=tree->root;//从树根开始
        while(temp!=NULL)
        {
            if(value<temp->data)//小于就进左节点
            {
                if(temp->left==NULL)
                {
                    temp->left=node;
                    free(node);
                    return ;
                }
                else
                {
                    
                    temp=temp->left;
                }
                
            }
            else//否则进入右节点
            {
                if(temp->right==NULL)
                {
                    temp->right=temp;
                    free(node);
                    return ;
                }
                else
                {
                    temp=temp->left;                 
                }              
            }
        }

    }
    return ;
}
