#ifndef BULLETLIST_H_
#define BULLETLIST_H_

typedef struct Bullet_Info{
	int x, y;
	int belongs;
	int v_x, v_y, vel;
	int wid = 15;
	struct Bullet_Info *next;
}Bullet_Info, *BulletList;

/*
使用.cpp文件实现接口
函数参数可以改，只要功能实现了
*/

void BulletList_Init(BulletList &new_node);//初始化节点new_node
void BulletList_Insert(BulletList &pre, BulletList &new_node);//在所给节点pre 后插入新节点new_node
void BulletList_Delete(BulletList &pre);//删除所给节点的后一位节点
int BulletList_IsEmpty(BulletList &head);//判断是否为空链表

//BulletList_init
//BulletList_Insert  插入的结点参数要先初始化
//
//BulletList_IsEmpty 是空则返回1

#endif

//陈智霖 20220911
