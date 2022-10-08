#include "le.h"
#include <stdlib.h>
#include <stdio.h>

Charc_Info Entity_Init(int wid, float x, float y, int types, int hp_max)
{
    Charc_Info ret;
    ret.width = wid;
    ret.x = x;
    ret.y = y;
    ret.types = types;
    ret.hp_now = ret.hp_max = hp_max;
    return ret;
}

PCharc_Info Entity_Init_P(int wid, float x, float y, int types, int hp_max)
{
    PCharc_Info ret = (PCharc_Info)malloc(sizeof(Charc_Info));
    ret->width = wid;
    ret->x = x;
    ret->y = y;
    ret->types = types;
    ret->hp_now = ret->hp_max = hp_max;
    return ret;
}

bool EntityAry_init(EntityAdt &adt,int user_num)
{
    adt.num=user_num;
    adt.p=0;
    adt.b=(PCharc_Info*)malloc(sizeof(PCharc_Info)*adt.num);
    if(adt.b)
        return true;
    return false;
}

bool Entity_insert(EntityAdt &adt, PCharc_Info Charc_insert)
{
    if(adt.p>=adt.num-1)
        return false;
    Charc_insert->id = adt.p;
    adt.b[adt.p++]=Charc_insert;
    return true;
}

bool Entity_delete(EntityAdt &adt,int now)
{
    if(adt.p<=0||now>=adt.p)
        return false;
    //PCharc_Info temp_free = adt.b[now];
    free(adt.b[now]);
    adt.b[adt.p - 1]->id = now;
    adt.b[now]=adt.b[--adt.p];
    //free(temp_free);
    return true;
}

void Entity_free(EntityAdt &adt)
{
    free(adt.b);
}


int GlobalCollisionDet(EntityAdt &Det, float x, float y, int wid, int id, int *ret_id)
{
    for(int i=0;i<Det.p;i++)          {
        //id相同表示同一实体，不用判断
        if(id != Det.b[i]->id){
            /*
            （一维坐标轴选取向右为正方向）
            不同实体或物体间作碰撞检测，分两个坐标轴处理，共三种情况发生区间重叠
            1.A的右端点小于B的右端点 且 大于B的左端点
            2.A的右端点大于B的右端点 且 A的左端点小于B的左端点
            3.A的左端点小于B的右端点 且 大于B的左端点

            返回碰到的实体的类型（宏定义）
            */
            if((Det.b[i]->x < x && x < Det.b[i]->x + Det.b[i]->width) ||
               (Det.b[i]->x < x + wid && x + wid < Det.b[i]->x + Det.b[i]->width) ||
               (Det.b[i]->x > x && x + wid > Det.b[i]->x + Det.b[i]->width)){
                if((Det.b[i]->y < y && y < Det.b[i]->y + Det.b[i]->width) ||
                   (Det.b[i]->y < y + wid && y + wid < Det.b[i]->y + Det.b[i]->width) ||
                   (Det.b[i]->y > y && y + wid > Det.b[i]->y + Det.b[i]->width)){
                    if(ret_id)*ret_id = i;
                    return Det.b[i]->types;
                }
            }
        }
    }
    return 0;
}

void ChangeAcc(PCharc_Info &s,float ax,float ay, float time_lap)
{
    s->vx+=ax*time_lap;
    s->vy+=ay*time_lap;
}
