#ifndef BULLETLIST_H_
#define BULLETLIST_H_

//弹幕结构体
typedef struct Bullet_Info{
	float x, y;
	float vx, vy;
	int belongs;
	float damage;
}Bullet_Info, *BulletList;

//
typedef struct myadt_bullet{
	Bullet_Info *b;
    int p;//栈顶
    int num;//结构体数组大小
}myadt_bullet;

//bool返回值 说明当前操作是否成功
bool bulletary_init(myadt_bullet &adt,int user_num);//弹幕结构体数组动态分配内存空间 user_num数组大小
Bullet_Info bullet_init(float x, float y, int belongs, float vx, float vy, float damage);//初始化弹幕结构体并返回
bool bullet_insert(myadt_bullet &adt,Bullet_Info bullet);//插入弹幕
bool bullet_delete(myadt_bullet &adt,int now);//删除弹幕 now是当前需要删除的位置
void bullet_free(myadt_bullet &adt);//释放内存空间

#endif

//陈智霖 20220919
/*
叶财兴 20220919
修改了bullet_init函数，修改x，y，vx，vy为浮点值
*/
