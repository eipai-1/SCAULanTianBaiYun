#include "le.h"
#include "chen.h"
#include <stdlib.h>
#include <math.h>

extern EntityAdt Global_entity;
extern int cxClient, cyClient;
extern float Monster1_steps, Monster1_movetime, Monster1_direction;
static const float Pi = 2*acos(0.0);
extern float Alert_range_squared;
extern myadt_bullet Ammo;

extern float Monster1_bullet_movetime;
extern float Monster1_bullet_speed;

void MonsterMoveType1(Charc_Info &Monster, Charc_Info main_char, float time_lag)
{
    float x = Monster.vx * time_lag, y = Monster.vy * time_lag;
    if(Monster.movemode != MOVEMODE_SLEEP)
    {
        if(Monster.cnt < Monster1_movetime)
        {
            Monster.cnt += time_lag;
            switch(Monster.movemode)
            {
            case MOVEMODE_SHIFT://移动
                if(!GlobalCollisionDet(Global_entity, Monster.x + x,
                                       Monster.y + y, Monster.width, Monster.id, NULL)
                        && Monster.x + x > 0 && Monster.y + y> 0
                        && Monster.x + Monster.width + x < cxClient
                        && Monster.y + Monster.width + y < cyClient)
                {
                    Monster.x += x;
                    Monster.y += y;
                }

                break;
            case MOVEMODE_STATIC://不动
                break;
            }
            if(Monster.bullet_cnt > Monster1_bullet_movetime)
            {
                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                cos_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_bullet_speed,
                                                sin_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_bullet_speed,
                                                10));
                Monster.bullet_cnt = 0;
            }
            else
            {
                Monster.bullet_cnt += time_lag;
            }
        }
        else
        {
            Monster.cnt = 0;
            Monster.movemode ^= 1;//位与操作 1、0来回切换
            srand((int)(time_lag*1000));
            Monster1_direction = (rand()%1000)/500.0*Pi;
            //printf("NewDIr=%f\n", Monster1_direction);
            Monster.vx = cos(Monster1_direction) * Monster1_steps;
            Monster.vy = sin(Monster1_direction) * Monster1_steps;

            /*
            如果两向量数量积小于0，则夹角大于90°，此时翻转速度矢量
            */
            if((cos_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * cos(Monster1_direction) +
                    sin_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * sin(Monster1_direction)) < 0)
            {
                Monster.vx *= -1;
                Monster.vy *= -1;
            }
        }
    }
    else
    {
        if((Monster.x - main_char.x) * (Monster.x - main_char.x) +
                (Monster.y - main_char.y) * (Monster.y - main_char.y) < Alert_range_squared)
            Monster.movemode = MOVEMODE_SHIFT;
    }
}
