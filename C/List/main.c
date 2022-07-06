#include"Double_List.h"
#include<stdio.h>
void main()
{

MyDouble_List *head;

head = CreateDoubleList();

print(head);

int num[20]={};

printf("输入插入的位置：");



printf("\n输入插入的值：");

CreateRandom(num,0,20);
for(int i=0;i<20;i++)
{
    printf("随机后的值为nums[i]=%d\n",num[i]);
}
for(int i=0;i<20;i++)
{
    InsertDoubleListAtIndex(head, i, num[i]);
}


print(head);

printf("\n");

 

printf("\n\n====删除双向链表的某位置上的一个值====\n");

int j=17;

myDoubleListDeleteAtIndex(head, j);

print(head);



printf("\n");

 

}