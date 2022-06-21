template<class T>

class BinaryTreeNode
{
public:
    T data;
    BinaryTreeNode<T> *leftchild;
    BinaryTreeNode<T> *rightchild;
    BinaryTreeNode()
    {
        leftchild=rightchild=NULL;
    }
    BinaryTreeNode(T element)
    {
        data=element;
        leftchild=rightchild=NULL;
    }
    BinaryTreeNode(T element,BinaryTreeNode *left,BinaryTreeNode *right)
    {
        data=element;
        leftchild=left;
        rightchild=right;
    }
};
template<class T>
class binarytree
{
public:
    binarytreenode<T> *root;
    binarytree(binarytreenode<T> *r=NULL)
    {
        root=r;
    }
    bool isempty()
    {
        return root==NULL;
    }
    void maketree(const T ele,binarytree<T> left,binarytree<T> right)   //建树
    {
        root=new binarytreenode<T>(ele,left.root,right.root);
        left.root=right.root=0;
    }
    void visit(binarytreenode<T> *t)   //访问结点中元素
    {
        cout<<t->data<<" ";
    }
    void levelorder()       //广度优先遍历二叉树
    {
        queue<binarytreenode<T> *> nodequeue;
        binarytreenode<T> *pointer=root;
        if(pointer)
            nodequeue.push(pointer);
        while(!nodequeue.empty())
        {
            pointer=nodequeue.frontT();
            visit(pointer);
            nodequeue.pop();
            if(pointer->leftchild)
                nodequeue.push(pointer->leftchild);
            if(pointer->rightchild)
                nodequeue.push(pointer->rightchild);
        }
    }
};

