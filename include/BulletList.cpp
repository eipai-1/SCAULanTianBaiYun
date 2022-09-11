
#include <stdio.h>
#include "BulletList.h"
/*
typedef struct Bullet_Info{
	int x, y;
	int belongs;
	struct Bullet_Info *next;
}Bullet_Info, *BulletList;
*/
void BulletList_Init(BulletList &new_node)//初始化节点new_node
{
    new_node = new Bullet_Info;//new
    new_node->x=new_node->y=0;
    new_node->v_x=new_node->v_y=5;
    new_node->belongs=0;
    new_node->wid=15;
    new_node->vel=20;
    new_node->next=NULL;
}
void BulletList_Insert(BulletList &pre, BulletList &new_node)//在所给节点pre 后插入新节点new_node
{
    BulletList p=pre->next;//保存后一个结点

    pre->next=new_node;
    new_node->next=p;
}

void BulletList_Delete(BulletList &pre)//删除所给节点的后一位节点
{
    BulletList p1,p2;//保存后一个及后后一个
    p1=pre->next;
    p2=p1->next;
    pre->next=p2;
    delete p1;//释放空间
}
int BulletList_IsEmpty(BulletList &head)//判断是否为空链表
{
    if(head->next==NULL)
        return 1;
    else
        return 0;
}

/*int main()
{
    BulletList head,t1,t2,t3;
    BulletList_Init(t1);
    printf("%d %d\n",t1->x,t1->y);
    BulletList_Init(t2);
    t2->x=2;
    BulletList_Insert(t1,t2);
    printf("%d %d\n",t1->next->x,t2->y);
    BulletList_Init(t3);
    t3->y=3;
    BulletList_Insert(t2,t3);
    printf("%d ",t1->next->next->y);
    BulletList_Delete(t1);
    printf("%d ",t1->next->y);
    if(!BulletList_IsEmpty(t1))
        printf("no empty");
    return 0;
}
*/ //测试
//陈智霖 20220911
