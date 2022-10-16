#ifndef LE_H_
#define LE_H_

typedef struct{
    int width = 50;
    float x, y;
    float vx,vy;
    float hp_now, hp_max;
    int types,id;
    float cnt;
    int movemode;
    float bullet_cnt;
    int boss_summon;
}Charc_Info,*PCharc_Info;

typedef struct{
    PCharc_Info *b;//数组储存
    int p;//栈顶
    int num;//结构体数组大小
}EntityAdt;

Charc_Info Entity_Init(int wid, float x, float y, int types, int hp_max);//人物初始化
PCharc_Info Entity_Init_P(int wid, float x, float y, int types, int hp_max);//人物初始化 返回指针
bool EntityAry_init(EntityAdt &adt,int user_num);//数据结构中初始化
bool Entity_insert(EntityAdt &adt,PCharc_Info Charc_insert);//插入
bool Entity_delete(EntityAdt &adt,int now);//删除
void Entity_free(EntityAdt &adt);//释放空间

int GlobalCollisionDet(EntityAdt &Det, float x, float y, int wid,int id, int *ret_id);//矩形碰撞检测，返回碰撞类型
void ChangeAcc(PCharc_Info &s,int ax,int ay);//改变速度

/*
2维矩形碰撞检测

Det为实体信息
x,y,wid为需要检测的正方形坐标
非实体请传入id为-1
返回碰撞实体的类型
*/
#endif
/*
陈家乐 2022.9.24
碰撞监测
速度变化
*/

/*
叶财兴 2022.10.2
修改了碰撞监测
添加了一些注释
*/
