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
ʹ��.cpp�ļ�ʵ�ֽӿ�
�����������Ըģ�ֻҪ����ʵ����
*/

void BulletList_Init(BulletList &new_node);//��ʼ���ڵ�new_node
void BulletList_Insert(BulletList &pre, BulletList &new_node);//�������ڵ�pre ������½ڵ�new_node
void BulletList_Delete(BulletList &pre);//ɾ�������ڵ�ĺ�һλ�ڵ�
int BulletList_IsEmpty(BulletList &head);//�ж��Ƿ�Ϊ������

//BulletList_init
//BulletList_Insert  ����Ľ�����Ҫ�ȳ�ʼ��
//
//BulletList_IsEmpty �ǿ��򷵻�1

#endif

//������ 20220911
