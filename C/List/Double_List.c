#include<stdio.h>
#include<malloc.h>
#include"Double_List.h"
#include<time.h>
#include<stdlib.h>
void CreateRandom(int * nums,int n,int m)
{
    srand((unsigned int)time(NULL));
// int ret1 = rand() % 10 + 1;//生成1~10的随机数
// int ret2 = rand() % 100 + 1;//生成1~100的随机数
// int ret3 = rand() % 34 + 66;//生成66~99的随机数
 int ret4 = rand() % (m - n + 1) + n;//生成n~m的随机数
 for(int i=0;i<m-n;i++)
 {
    nums[i]=rand() % (m - n + 1) + n;
    printf("nums[i]=%d\n",nums[i]);
 }

}
// 创建双向链表
struct MyDouble_List* CreateDoubleList(void)
{
   struct MyDouble_List* temp=(struct MyDouble_List*)malloc(sizeof(struct MyDouble_List));
   temp->data=-1.11111111;
   temp->next=NULL;
   temp->prev=NULL;
   return temp;
}


//打印一个链表

void print(MyDouble_List* head)
{
    MyDouble_List* temp=head->next;
    if(temp==NULL)
    {
        printf("该链表为空链表\n");
    }
    while(temp!=NULL)
    {
        printf("链表的值为:%lf\n",(temp->data));
        temp=temp->next;
    }
}

//插入一个节点
void InsertDoubleListAtIndex(MyDouble_List *obj, int Index, int value)
{
  // 过滤非法 index < 0, 使用头插法, 题目要求,正常情况过滤负数,特判头节点
    if (Index <= 0)
        return myDoubleListAddAtHead(obj, value);

    // 找到第 index-1 个节点
    int i = 0;
    MyDouble_List *head = obj->next;
    for (i = 0; head->next && i < Index - 1; i++)
    { // 和获取第 index 值 类似, 只不过这里需要获取 前一个节点
        head = head->next;
    }

    // 判断退出条件
    if (Index - 1 != i)
    {
        return;
    }

    // 初始化节点, 避免浪费空间
    MyDouble_List *node = (MyDouble_List *)malloc(sizeof(MyDouble_List));
    node->data = value;

    // 插入节点, 道理同 myLinkedListAddAtHead 部分的注释
    // node 链接 到 head 和 head->next
    node->next = head->next; // 链接到 head
    node->prev = head;       // 链接到 head->next

    // head 和 head->next 链接到 node
    head->next = node;
    // 插入尾节点特判
    if (NULL == node->next)
        return;
    node->next->prev = node; // head->next =
   

}
//删除一个节点
void myDoubleListDeleteAtIndex(MyDouble_List *obj, int index)
{
     // 异常拦截
    if (!(obj->next) || index < 0)
        return;

    // 分类讨论
    MyDouble_List *temp = NULL;
    // 1. 删除头节点
    if (0 == index)
    {
        // 存下要销毁的节点
        temp = obj->next;
        // 头节点指向第二个节点
        obj->next = obj->next->next;
        // 第二个节点链接上头节点
        // 考虑所删除的链表只有一个节点
        if (NULL == obj->next)
            return;
        obj->next->prev = obj;

        temp->next = temp;
        free(temp);
        return;
    }

    // 找到 第 index - 1 个节点
    int i = 0;
    MyDouble_List *head = obj->next;
    for (i = 0; head->next && i < index - 1; i++)
    {
        head = head->next;
    }

    if (i != index - 1 || (NULL == head->next))
        return;

    // 删除节点
    // 保留节点指针
    temp = head->next;
    // index-1节点的 链接到  index+1节点
    head->next = head->next->next;
    // 特判删除最后一个节点
    if (NULL == head->next)
        return;
    // index+1节点的 链接到  index-1节点
    head->next->prev = head;
    // 释放节点
    temp->next = temp;
    free(temp);
}


double myDoubleListGet(MyDouble_List* head,int Index)
{
       // 1. index < 0
       // 2. 链表为空
       // 3. index 过大
       if((Index<0)&&!(head->next))
       {
            return -1;
       }
       MyDouble_List* obj=head->next;
       // 查找 第 index 个节点, 可能存在两种情况
       //  1.存在 第 index 个节点
      //  2.不存在 第 index 个节点
    for (int i = 0; obj && i < Index; i++)
    {
         obj = obj->next;
    }

     // obj 非空代表存在整个值 返回 -1 ,
     // 否则返回 val
    return (obj ? obj->data : -1);
}
//从头部增加
void myDoubleListAddAtHead(MyDouble_List *obj, int val)
{
      // 初始化新节点
     MyDouble_List *node = (MyDouble_List *)malloc(sizeof(MyDouble_List));
     node->data = val;

//     // 指向 head 后面的节点
//     // 如果为空,也会指向空, 所以无需next指向空
//     // obj 为头节点, obj->next 为第一个节点
     // 1. node->next 指向 obj->next 表示 新节点指向原本的第一个节点, 从此之后 新节点 就可以访问到旧节点
     node->next = obj->next;
//     // 2. node->prev = obj 表示 node 的前驱指针 链接上 头节点 node, 这个时候就可以断开, obj->next 和 obj之间的链接了
     node->prev = obj;
//     // 3. obj->next 指针 指向 node , node 成为链表的第一个节点
     obj->next = node;
//     // 这里考虑第一次插入节点
     if (NULL == node->next)
         return;
//     // 4. 因为 node 在 1 出获得了 原本第一个节点的地址, 所以next->prev为原本第一个节点的前驱指针, 指向了 node, node 的 前驱链接合
      node->next->prev = node;
}
void myDoubleListAddAtTail(MyDouble_List *obj, int val)
{
     // 当链表为空, 尾插变头插
     if (NULL == obj->next)
    {
        myDoubleListDeleteAtIndex(obj, val);
        return;
    }
    // 初始化节点
    MyDouble_List *node = (MyDouble_List *)malloc(sizeof(MyDouble_List));
    node->data = val;
    node->next = NULL;

    // 查找最后一个节点
    MyDouble_List *tail = obj->next;
    while (tail->next)
    { //obj为空 无法获取到前一个节点
        tail = tail->next;
    }

    // 尾节点指向新节点
    tail->next = node;
    node->prev = tail;
}
void myLinkedListFree(MyDouble_List *obj)
{
     // 异常拦截
    if (!obj)
        return;

    MyDouble_List *head = obj->next;
    MyDouble_List *temp = NULL;
    while (head)
    {
        temp = head;
        head = head->next;
        temp->next = temp->prev = NULL;
        free(temp);
    }
    obj = NULL;
}