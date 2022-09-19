#include <stdlib.h>
#include <stdio.h>
#include "BulletList.h"


bool bulletary_init(myadt_bullet &adt,int user_num)
{
    adt.num=user_num;
    adt.p=0;
    adt.b=(Bullet_Info *)malloc(sizeof(Bullet_Info)*adt.num);
    if(adt.b)
        return true;
    return false;
}
Bullet_Info bullet_init(float x, float y, int belongs, float vx, float vy)
{
    Bullet_Info new_node;
    new_node.x = x;
    new_node.y = y;
    new_node.belongs = belongs;
    new_node.vx = vx;
    new_node.vy = vy;

    return new_node;
}
bool bullet_insert(myadt_bullet &adt,Bullet_Info bullet)
{

    if(adt.p>=adt.num-1)
        return false;
    adt.b[adt.p++]=bullet;
    return true;

}
bool bullet_delete(myadt_bullet &adt,int now)
{
    if(adt.p<=0||now>=adt.p)
        return false;
    adt.b[now]=adt.b[--adt.p];
    return true;
}
void bullet_free(myadt_bullet &adt)
{
    free(adt.b);
}

//³ÂÖÇÁØ 20220919
