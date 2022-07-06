#include"BinaryTree.h"
#include<stdio.h>
#include<stdlib.h>
int main(int argc, char const *argv[])
{
    int arrays[]={};
    for(int i=0;i<20;i++)
    {
        arrays[i]=(rand()%20);
       printf("nums[i]=%d",arrays[i]);
    }
    BinaryTree tree;
    for(int i=0;i<20;i++)
    {
        insert(&tree,arrays[i]);
    }
    
    return 0;
}