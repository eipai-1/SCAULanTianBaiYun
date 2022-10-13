#include "Ycx_header.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

extern EntityAdt Global_entity;
extern int cxClient, cyClient, Monster1_wid, Game_status;
const int ENTITY_MAX = 100;
static const float Pi = 2*acos(0.0);
extern float Rad_of_gate, horCor_wid, horCor_lgh, verCor_wid, verCor_lgh, time_lag;
extern Map Game_map[MAP_SIZE][MAP_SIZE];
extern float map_wall_width;

void CharcMove(Charc_Info & s, float x, float y)
{
    int Col_id, Map_id;
    if(!MapEdgeDet(s.x+x, s.y+y, s.width)){
        Col_id = GlobalCollisionDet(Global_entity, s.x + x, s.y + y, s.width, s.id, NULL);
        if(Col_id == NO_COLLSION_TYPE || Col_id == MONSTER2_TYPE){
            s.x += x;
            s.y += y;
            if(Game_status == .x/cxClient)
        }
    }
}

double sinx(int cur_x, int cur_y)
{
    return ((cur_y - cyClient/2)/sqrt((cur_y - cyClient/2) * (cur_y - cyClient/2) + (cur_x - cxClient/2) * (cur_x - cxClient/2)));
}

double cosx(int cur_x, int cur_y)
{
    return ((cur_x - cxClient/2)/sqrt((cur_y - cyClient/2) * (cur_y - cyClient/2) + (cur_x - cxClient/2) * (cur_x - cxClient/2)));
}

float sin_x(float x1, float y1, float x2, float y2)
{
    return ((y2 - y1)/sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) *(y2 - y1))));
}

float cos_x(float x1, float y1, float x2, float y2)
{
    return ((x2 - x1)/sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) *(y2 - y1))));
}

int GetRnd(int seeds)
{
    srand(seeds);
    return rand();
}

void Monster1_Init(EntityAdt &entity, PCharc_Info p_charc)
{
    Entity_insert(entity, p_charc);
    entity.b[entity.p - 1]->cnt = 0;
    entity.b[entity.p - 1]->movemode = MOVEMODE_SLEEP;
    entity.b[entity.p - 1]->bullet_cnt = 0;
}

void MapInit()
{
    for(int i = 0; i < MAP_SIZE; i++){
        for(int j = 0; j < MAP_SIZE; j++){
            Game_map[i][j].types = NULL_MAPTYPE;
            Game_map[i][j].status = NULL_MAPSTATUS;
        }
    }
    Game_map[0][0].types = REWARD_MAPTYPE;
    Game_map[0][0].left = 0;
    Game_map[0][0].top = 0;
    Game_map[0][0].right = 1;
    Game_map[0][0].buttom = 0;
    EntityAry_init(Game_map[0][0].entity, ENTITY_MAX);
    Monster1_Init(Game_map[0][0].entity, Entity_Init_P(Monster1_wid, 200, 200, MONSTER_TYPE, 100));
    //Monster1_Init(Game_map[0][0].entity, Entity_Init_P(Monster1_wid, 200, 200, MONSTER2_TYPE, 100));

    Game_map[0][1].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[0][2].types = BATTLE_MAPTYPE;
    Game_map[0][2].status = BATTLING_MAPSTATUS;
    Game_map[0][2].left = 1;
    Game_map[0][2].top = 0;
    Game_map[0][2].right = 0;
    Game_map[0][2].buttom = 0;
    EntityAry_init(Game_map[0][2].entity, ENTITY_MAX);
    Monster1_Init(Game_map[0][2].entity, Entity_Init_P(Monster1_wid, 2*cxClient + 200, 200, MONSTER_TYPE, 100));
    Monster1_Init(Game_map[0][2].entity, Entity_Init_P(Monster1_wid, 2*cxClient + 400, 400, MONSTER_TYPE, 100));
}

Weapon WeaponInit(float damage, float bullet_cd, int types)
{
    Weapon w;
    w.damage = damage;
    w.bullet_cd = bullet_cd;
    w.types = types;
    return w;
}

int MapEdgeDet(float x, float y, int wid)//����1ʱ��ʾ����ײ 0��ʾ��ײ
{
    if(x < 0 || y < 0)return 1;

    bool flag = true;
    int x_map = (int)(x/1000), y_map = (int)(y/600);

    switch(Game_map[y_map][x_map].types){
    case BATTLE_MAPTYPE:
    case REWARD_MAPTYPE:
        if(x > cxClient*x_map + cxClient/2 - Rad_of_gate
           && x + wid < cxClient*x_map + cxClient/2 + Rad_of_gate){
            //printf("case1\n");
            if(y < cyClient*y_map + map_wall_width &&
               (!Game_map[y_map][x_map].top || y_map == 0)){
                return 0;
            }
            else if(y + wid > cyClient*(y_map + 1) - map_wall_width
                    && !Game_map[y_map][x_map].buttom){
                return 0;
            }
            flag = false;
        }

        if(y > cyClient*y_map + cyClient/2 - Rad_of_gate
           && y + wid < cyClient*y_map + cyClient/2 + Rad_of_gate){
            if(x < cxClient*x_map + map_wall_width &&
               (!Game_map[y_map][x_map].left || x_map == 0)){
                return 0;
            }
            else if(x + wid > cxClient*(x_map + 1) - map_wall_width
                    && !Game_map[y_map][x_map].right){
                return 0;
            }
            flag = false;
        }

        if(flag){
            if(x + wid > cxClient*(x_map+1) - map_wall_width
               || x < cxClient*x_map + map_wall_width
               || y + wid > cyClient*(y_map+1) - map_wall_width
               || y < cyClient*y_map + map_wall_width){
                   return 1;
               }
        }
        break;

    case CORRIDOR_HOR_MAPTYPE:
        if(y + wid < cyClient*y_map + cyClient/2 + horCor_wid/2 - map_wall_width
           && y > cyClient*y_map + cyClient/2 - horCor_wid/2 + map_wall_width)
            return 0;
        return 1;
        break;

    case CORRIDOR_VER_MAPTYPE:
        if(x + wid < cxClient*x_map + cxClient/2 + verCor_wid/2 - map_wall_width
           && x > cxClient*x_map + cxClient/2 - verCor_wid/2 + map_wall_width)
            return 0;
        return 1;
        break;
    }
    return Game_map[y_map][x_map].types;
}
