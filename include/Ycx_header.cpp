#include "Ycx_header.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

extern EntityAdt Global_entity;
extern int cxClient, cyClient;
extern float Monster1_steps, Monster1_movecnt, Monster1_movetime, Monster1_direction;
extern float Monster1_vx, Monster1_vy;
static const float Pi = 2*acos(0.0);
extern int Monster1_movemode;
extern int Rad_of_gate;
extern Map Game_map[MAP_SIZE][MAP_SIZE];

void CharcMove(Charc_Info & s, float x, float y)
{
    if(s.x + x > 0 && s.y + y > 0 && s.x + s.width + x < cxClient && s.y + y + s.width < cyClient){
        if(!GlobalCollisionDet(Global_entity, s.x + x, s.y + y, s.width, s.id, NULL)){
            s.x += x;
            s.y += y;
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

void MapInit()
{
    for(int i = 0; i < MAP_SIZE; i++){
        for(int j = 0; j < MAP_SIZE; j++){
            Game_map[i][j].types = NULL_MAPTYPE;
            Game_map[i][j].status = NULL_MAPSTATUS;
        }
    }
    Game_map[0][0].types = REWARD_MAPTYPE;
    Game_map[0][1].types = CORRIDOR_MAPTYPE;
    Game_map[0][2].types = BATTLE_MAPTYPE;
    Game_map[0][2].status = SLEEP_MAPSTATUS;
}

int MapEdgeDet(float x, float y, int wid)//返回1时表示碰撞 0表示无碰撞
{
    int x_map = (int)x/1000, y_map = (int)y/600;
    switch(Game_map[x_map][y_map].types){
    case REWARD_MAPTYPE:
        if(y < 0 || x < 0)return 1;
        else{
            if((x < cxClient*x_map && x + wid < cxClient*x_map + cxClient/2 - Rad_of_gate)
               || (x > cxClient*x_map + cxClient/2 + Rad_of_gate && x + wid < cxClient*(x_map+1))){
                if(y + wid >= cyClient*(y_map+1))
                    return 1;
                return 0;
            }
        }
        break;
    }
}

void MonsterMoveType2(Charc_Info &Monster, Charc_Info main_char, float time_lag)
{
    float x = Monster.vx * time_lag, y = Monster.vy * time_lag;
    if(Monster1_movecnt < Monster1_movetime){
        Monster1_movecnt += time_lag;
        if(!GlobalCollisionDet(Global_entity, Monster.x + x,
            Monster.y + y, Monster.width, Monster.id, NULL)
            && Monster.x + x > 0 && Monster.y + y> 0
            && Monster.x + Monster.width + x < cxClient
            && Monster.y + Monster.width + y < cyClient){
                Monster.x += x;
                Monster.y += y;
            }
    }
    else{
        Monster1_movecnt = 0;
        srand((int)(time_lag*1000));
        Monster1_direction = (rand()%1000)/500.0*Pi;
        //printf("NewDIr=%f\n", Monster1_direction);
        Monster.vx = cos(Monster1_direction) * Monster1_steps;
        Monster.vy = sin(Monster1_direction) * Monster1_steps;

        //加上玩家方向的矢量
        Monster.vx += cos_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_steps;
        Monster.vy += sin_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_steps;
    }
}
