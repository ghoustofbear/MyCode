#ifndef DOUBULE_H
#define DOUBULE_H
typedef struct MyDouble_List
{
    /* data */
    double data;
    struct MyDouble_List* next;
    struct MyDouble_List* prev;
}MyDouble_List;

// 创建双向链表
struct MyDouble_List* CreateDoubleList(void);
//打印一个链表
void print(MyDouble_List* head);
//插入一个节点
void InsertDoubleListAtIndex(MyDouble_List *obj, int Index, int Value);
//创造一个随机数
void CreateRandom(int * nums,int n,int m);
//获取第Index的值
double myDoubleListGet(MyDouble_List* head,int Index);
//从头部增加
void myDoubleListAddAtHead(MyDouble_List *obj, int val);
//从尾部增加
void myDoubleListAddAtTail(MyDouble_List *obj, int val);
//删除第Index个节点
void myDoubleListDeleteAtIndex(MyDouble_List *obj, int index);
//删除整个链表
void myLinkedListFree(MyDouble_List *obj);

#endif