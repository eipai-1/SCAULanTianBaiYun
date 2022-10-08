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
        //id��ͬ��ʾͬһʵ�壬�����ж�
        if(id != Det.b[i]->id){
            /*
            ��һά������ѡȡ����Ϊ������
            ��ͬʵ������������ײ��⣬�����������ᴦ��������������������ص�
            1.A���Ҷ˵�С��B���Ҷ˵� �� ����B����˵�
            2.A���Ҷ˵����B���Ҷ˵� �� A����˵�С��B����˵�
            3.A����˵�С��B���Ҷ˵� �� ����B����˵�

            ����������ʵ������ͣ��궨�壩
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
