#include "Ycx_header.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

extern EntityAdt Global_entity;
extern int cxClient, cyClient, Monster1_wid, Game_status, map_temp_left,
map_temp_top, map_temp_right, map_temp_buttom, Map_depth_now, Map_rooms_to_clear[MAP_DEPTH];
const int ENTITY_MAX = 100;
//static const float Pi = 2*acos(0.0);
extern float Rad_of_gate, horCor_wid, horCor_lgh, verCor_wid, verCor_lgh, time_lag,
map_wall_width;
extern Map Game_map[MAP_DEPTH][MAP_SIZE][MAP_SIZE];
int LockDoorCnt = 0;

void CharcMove(Charc_Info & s, float x, float y)
{
    int Col_id, temp_x, temp_y;
    if(!MapEdgeDet(Map_depth_now, s.x + x, s.y + y, s.width)){
        Col_id = GlobalCollisionDet(Global_entity, s.x + x, s.y + y, s.width, s.id, NULL);
        if(Col_id != MONSTER_TYPE && Col_id != OBSTACLE_TYPE && Col_id != OBSTACLE_UNBREAKABLE_TYPE){
            s.x += x;
            s.y += y;
//*
            temp_x = s.x/cxClient, temp_y = s.y/cyClient;
            //printf("%d %d %d %d\n", Game_map[temp_y][temp_x].left, Game_map[temp_y][temp_x].top,
            //       Game_map[temp_y][temp_x].right, Game_map[temp_y][temp_x].buttom);
            //printf("%d %d %d\n",Map_depth_now, temp_x, temp_y);
            if(Game_status == RUNNING_GAMESTATUS
               && Game_map[Map_depth_now][temp_y][temp_x].status == BATTLING_MAPSTATUS){
                //printf("1\n");
                if(LockDoorCnt < 18){
                    LockDoorCnt++;
                }
                else{
                    if(LockDoorCnt)LockDoorCnt = 0;
                    //printf("switch\n");
                    Game_status = BATTLING_GAMESTATUS;
                    map_temp_left = Game_map[Map_depth_now][temp_y][temp_x].left;
                    map_temp_top = Game_map[Map_depth_now][temp_y][temp_x].top;
                    map_temp_right = Game_map[Map_depth_now][temp_y][temp_x].right;
                    map_temp_buttom = Game_map[Map_depth_now][temp_y][temp_x].buttom;
                    Game_map[Map_depth_now][temp_y][temp_x].left = 0;
                    Game_map[Map_depth_now][temp_y][temp_x].top = 0;
                    Game_map[Map_depth_now][temp_y][temp_x].right = 0;
                    Game_map[Map_depth_now][temp_y][temp_x].buttom = 0;
                }
            }
//*/
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

void Monster_Insert(EntityAdt &entity, PCharc_Info p_charc)
{
    Entity_insert(entity, p_charc);
    switch(p_charc->types){
    case MONSTER_TYPE:
    case MONSTER2_TYPE:
        entity.b[entity.p - 1]->cnt = 0;
        entity.b[entity.p - 1]->movemode = MOVEMODE_SLEEP;
        entity.b[entity.p - 1]->bullet_cnt = 0;
        break;

    case BOSS_MONSTER_TYPE:
        entity.b[entity.p - 1]->boss_summon = 1;
        entity.b[entity.p - 1]->cnt = 0;
        entity.b[entity.p - 1]->movemode = MOVEMODE_SLEEP;
        entity.b[entity.p - 1]->bullet_cnt = 0;
        entity.b[entity.p - 1]->vx = entity.b[entity.p - 1]->vy = 1.5;
        break;
    }
/*
    switch(p_charc->types){
    case MONSTER_TYPE:
    case MONSTER2_TYPE:
        entity.b[entity.p - 1]->cnt = 0;
        entity.b[entity.p - 1]->movemode = MOVEMODE_SLEEP;
        entity.b[entity.p - 1]->bullet_cnt = 0;
        break;
    case BOSS_MONSTER_TYPE:
        entity.b[entity.p - 1]->cnt = 0;
        entity.b[entity.p - 1]->movemode = MOVEMODE_SLEEP;
        entity.b[entity.p - 1]->bullet_cnt = 0;
        entity.b[entity.p - 1]->boss_summon = 1;
        break;
    }
    Entity_insert(entity, p_charc);
    */
}

void MapInit()
{
    //d = 1
    Map_rooms_to_clear[0] = 1;
    Game_map[0][0][0].left = 0;
    Game_map[0][0][0].top = 0;
    Game_map[0][0][0].right = 1;
    Game_map[0][0][0].buttom = 0;
    Game_map[0][0][0].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[0][0][0].entity, ENTITY_MAX);
    //Entity_insert(Game_map[0][0][0].entity, Entity_Init_P(50, 200, 200, MONSTER_TYPE, 30));

    Game_map[0][0][1].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[0][0][2].left = 1;
    Game_map[0][0][2].top = 0;
    Game_map[0][0][2].right = 1;
    Game_map[0][0][2].buttom = 0;
    Game_map[0][0][2].types = BATTLE_MAPTYPE;
    Game_map[0][0][2].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[0][0][2].entity, ENTITY_MAX);
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(64, cxClient*2 + cxClient/2 - 25, cyClient/2 - 25, MONSTER_TYPE, 100));
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 200, 200, OBSTACLE_TYPE, 30));
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 240, 200, OBSTACLE_TYPE, 30));
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 280, 200, OBSTACLE_TYPE, 30));

    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 200, 400, OBSTACLE_TYPE, 30));
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 240, 400, OBSTACLE_TYPE, 30));
    Monster_Insert(Game_map[0][0][2].entity, Entity_Init_P(40, cxClient*2 + 280, 400, OBSTACLE_TYPE, 30));

    Game_map[0][0][3].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[0][0][4].left = 1;
    Game_map[0][0][4].top = 0;
    Game_map[0][0][4].right = 1;
    Game_map[0][0][4].buttom = 0;
    Game_map[0][0][4].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[0][0][4].entity, ENTITY_MAX);
    Entity_insert(Game_map[0][0][4].entity, Entity_Init_P(150, cxClient*4 + 125, cyClient/2 - 75, TELEPORT_TYPE, 100));
    //-----

    //d = 2
    Map_rooms_to_clear[1] = 2;
    Game_map[1][0][0].left = 0;
    Game_map[1][0][0].top = 0;
    Game_map[1][0][0].right = 1;
    Game_map[1][0][0].buttom = 1;
    Game_map[1][0][0].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[1][0][0].entity, ENTITY_MAX);

    Game_map[1][0][1].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[1][0][2].left = 1;
    Game_map[1][0][2].top = 0;
    Game_map[1][0][2].right = 0;
    Game_map[1][0][2].buttom = 0;
    Game_map[1][0][2].types = BATTLE_MAPTYPE;
    Game_map[1][0][2].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[1][0][2].entity, ENTITY_MAX);
    Monster_Insert(Game_map[1][0][2].entity, Entity_Init_P(64, cxClient*2 + 200, 200, MONSTER_TYPE, 100));
    Monster_Insert(Game_map[1][0][2].entity, Entity_Init_P(64, cxClient*2 + 300, 300, MONSTER2_TYPE, 100));
    Monster_Insert(Game_map[1][0][2].entity, Entity_Init_P(64, cxClient*2 + 200, 400, MONSTER_TYPE, 100));

    Game_map[1][1][0].types = CORRIDOR_VER_MAPTYPE;

    Game_map[1][2][0].left = 0;
    Game_map[1][2][0].top = 1;
    Game_map[1][2][0].right = 1;
    Game_map[1][2][0].buttom = 0;
    Game_map[1][2][0].types = BATTLE_MAPTYPE;
    Game_map[1][2][0].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[1][2][0].entity, ENTITY_MAX);
    Monster_Insert(Game_map[1][2][0].entity, Entity_Init_P(64, 500, cyClient*2 + 300, MONSTER2_TYPE, 100));
    Monster_Insert(Game_map[1][2][0].entity, Entity_Init_P(64, 500, cyClient*2 + 400, MONSTER_TYPE, 100));

    Game_map[1][2][1].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[1][2][2].left = 1;
    Game_map[1][2][2].top = 0;
    Game_map[1][2][2].right = 0;
    Game_map[1][2][2].buttom = 0;
    Game_map[1][2][2].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[1][2][2].entity, ENTITY_MAX);
    Entity_insert(Game_map[1][2][2].entity, Entity_Init_P(150, cxClient*2 + 200, cyClient*2 + cyClient/2 - 25, TELEPORT_TYPE, 100));
    //-----

    //d = 3
    Map_rooms_to_clear[2] = 3;

    Game_map[2][0][0].left = 0;
    Game_map[2][0][0].top = 0;
    Game_map[2][0][0].right = 0;
    Game_map[2][0][0].buttom = 1;
    Game_map[2][0][0].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[1][0][0].entity, ENTITY_MAX);

    Game_map[2][1][0].types = CORRIDOR_VER_MAPTYPE;

    Game_map[2][2][0].left = 0;
    Game_map[2][2][0].top = 1;
    Game_map[2][2][0].right = 0;
    Game_map[2][2][0].buttom = 1;
    Game_map[2][2][0].types = BATTLE_MAPTYPE;
    Game_map[2][2][0].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[2][2][0].entity, ENTITY_MAX);
    Entity_insert(Game_map[2][2][0].entity, Entity_Init_P(50, 500, cyClient*2 + 400, MONSTER2_TYPE, 200));
    Entity_insert(Game_map[2][2][0].entity, Entity_Init_P(50, 400, cyClient*2 + 500, MONSTER_TYPE, 150));
    Entity_insert(Game_map[2][2][0].entity, Entity_Init_P(50, 600, cyClient*2 + 500, MONSTER_TYPE, 150));

    Game_map[2][3][0].types = CORRIDOR_VER_MAPTYPE;

    Game_map[2][4][0].left = 0;
    Game_map[2][4][0].top = 1;
    Game_map[2][4][0].right = 1;
    Game_map[2][4][0].buttom = 0;
    Game_map[2][4][0].types = BATTLE_MAPTYPE;
    Game_map[2][4][0].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[2][4][0].entity, ENTITY_MAX);
    Entity_insert(Game_map[2][4][0].entity, Entity_Init_P(50, 250, cyClient*4 + 300, MONSTER_TYPE, 150));
    Entity_insert(Game_map[2][4][0].entity, Entity_Init_P(50, 500, cyClient*4 + 300, MONSTER2_TYPE, 200));
    Entity_insert(Game_map[2][4][0].entity, Entity_Init_P(50, 250, cyClient*4 + 410, MONSTER_TYPE, 150));
    Entity_insert(Game_map[2][4][0].entity, Entity_Init_P(50, 500, cyClient*4 + 410, MONSTER_TYPE, 150));

    Game_map[2][4][1].types = CORRIDOR_VER_MAPTYPE;
    Game_map[2][4][2].left = 1;
    Game_map[2][4][2].top = 0;
    Game_map[2][4][2].right = 0;
    Game_map[2][4][2].buttom = 0;
    Game_map[2][4][2].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[2][4][2].entity, ENTITY_MAX);
    Entity_insert(Game_map[2][4][2].entity, Entity_Init_P(50, cxClient*2 + 200, cyClient * 4 + cyClient/2 - 25, TELEPORT_TYPE, 100));
    //-----

    //d = 4
    Game_map[3][0][0].left = 0;
    Game_map[3][0][0].top = 0;
    Game_map[3][0][0].right = 1;
    Game_map[3][0][0].buttom = 0;
    Game_map[3][0][0].types = REWARD_MAPTYPE;
    EntityAry_init(Game_map[3][0][0].entity, ENTITY_MAX);

    Game_map[3][0][1].types = CORRIDOR_HOR_MAPTYPE;

    Game_map[3][0][2].left = 0;
    Game_map[3][0][2].top = 0;
    Game_map[3][0][2].right = 1;
    Game_map[3][0][2].buttom = 0;
    Game_map[3][0][2].types = BATTLE_MAPTYPE;
    Game_map[3][0][2].status = BATTLING_MAPSTATUS;
    EntityAry_init(Game_map[3][0][2].entity, ENTITY_MAX);
    Entity_insert(Game_map[3][0][2].entity, Entity_Init_P(50, cxClient*2 + cxClient/2, cyClient/2, BOSS_MONSTER_TYPE, 500));
    //-----

    //Monster_Insert(Game_map[1][0][2].entity, Entity_Init_P(64, cxClient*2 + 200, 400, MONSTER_TYPE, 100));
    //Monster_Insert(Game_map[1][0][2].entity, Entity_Init_P(64, cxClient*2 + 400, 400, MONSTER_TYPE, 100));
}

int MapEdgeDet(int depth, float x, float y, int wid)//返回1时表示碰撞 0表示无碰撞
{
    if(x < 0 || y < 0)return MAPEDGE;

    bool flag = true;
    int x_map = (int)(x/1000), y_map = (int)(y/600);

    switch(Game_map[depth][y_map][x_map].types){
    case BATTLE_MAPTYPE:
    case REWARD_MAPTYPE:
        //在竖直方向区间内
        if(x > cxClient*x_map + cxClient/2 - Rad_of_gate
           && x + wid < cxClient*x_map + cxClient/2 + Rad_of_gate){
            //printf("case1\n");
            //且没有开门
            if(y < cyClient*y_map + map_wall_width &&
               (!Game_map[depth][y_map][x_map].top || y_map == 0)){
                return MAPEDGE;
            }
            else if(y + wid > cyClient*(y_map + 1) - map_wall_width
                    && !Game_map[depth][y_map][x_map].buttom){
                return MAPEDGE;
            }
            flag = false;
        }

        //在水平区间内
        if(y > cyClient*y_map + cyClient/2 - Rad_of_gate
           && y + wid < cyClient*y_map + cyClient/2 + Rad_of_gate){
            //且没有开门
            if(x < cxClient*x_map + map_wall_width &&
               (!Game_map[depth][y_map][x_map].left || x_map == 0)){
                return MAPEDGE;
            }
            else if(x + wid > cxClient*(x_map + 1) - map_wall_width
                    && !Game_map[depth][y_map][x_map].right){
                return MAPEDGE;
            }
            flag = false;
        }

        if(flag){
            if(x + wid > cxClient*(x_map+1) - map_wall_width
               || x < cxClient*x_map + map_wall_width
               || y + wid > cyClient*(y_map+1) - map_wall_width
               || y < cyClient*y_map + map_wall_width){
                   return MAPEDGE;
               }
        }
        break;

    case CORRIDOR_HOR_MAPTYPE:
        if(y + wid < cyClient*y_map + cyClient/2 + horCor_wid/2 - map_wall_width
           && y > cyClient*y_map + cyClient/2 - horCor_wid/2 + map_wall_width)
            return NO_MAPEDGE;
        return MAPEDGE;
        break;

    case CORRIDOR_VER_MAPTYPE:
        if(x + wid < cxClient*x_map + cxClient/2 + verCor_wid/2 - map_wall_width
           && x > cxClient*x_map + cxClient/2 - verCor_wid/2 + map_wall_width)
            return NO_MAPEDGE;
        return MAPEDGE;
        break;
    }
    return NO_MAPEDGE;
}
