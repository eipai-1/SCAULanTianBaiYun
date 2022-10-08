#ifndef MY_HEADER_H_
#define MY_HEADER_H_
#include "le.h"

#define MAP_SIZE 4

#define NULL_MAPSTATUS 0
#define BATTLING_MAPSTATUS 1
#define SLEEP_MAPSTATUS 2

#define NULL_MAPTYPE 0      //无房间
#define BATTLE_MAPTYPE 1    //战斗房间
#define REWARD_MAPTYPE 2    //奖励房间
#define CORRIDOR_HOR_MAPTYPE 3  //走廊房间 横
#define CORRIDOR_VER_MAPTYPE 4  //走廊房间 竖

typedef struct{
    int status;
    int types;
    bool left, right, top, buttom;
    EntityAdt entity;
}Map;

void CharcMove(Charc_Info & s, float x, float y);//人物移动 x轴

double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);
int GetRnd(int seeds);

void MapInit();
int MapEdgeDet(float x, float y, int wid);//返回1时表示碰撞 0表示无碰撞

#endif // MY_HEADER_H_
