#ifndef MY_HEADER_H_
#define MY_HEADER_H_

#include "BulletList.h"
#include "le.h"
#include "chen.h"

#define MAP_DEPTH 5
#define MAP_SIZE 5

#define NO_MAPEDGE 0
#define MAPEDGE 1

#define NULL_MAPSTATUS 0
#define BATTLING_MAPSTATUS 1//战斗中房间状态

#define NULL_MAPTYPE 0      //无房间
#define BATTLE_MAPTYPE 1    //战斗房间
#define REWARD_MAPTYPE 2    //奖励房间
#define CORRIDOR_HOR_MAPTYPE 3  //走廊房间 横
#define CORRIDOR_VER_MAPTYPE 4  //走廊房间 竖

#define SHABBY_PISTOL_WEAPON 1
#define PREDATOR_PISTOL_WEAPON 2
#define SHOTGUN_WEAPON 3

#define START_GAMESTATUS 0
#define BATTLING_GAMESTATUS 1
#define RUNNING_GAMESTATUS 2
#define PAUSE_GAMESTATUS 3
#define CHOOSING_GAMESTATUS 4

#define NULL_TEXT 0
#define TELEPORT_TEXT 1

#define BLOOD_BUFF 1 //buff定义必须从0开始，0被定义为无buff被选中
#define RAGE_BUFF 2
#define RUSH_BUFF 3
#define DAMAGEUP_BUFF 4
#define BULLETSPEEDUP_BUFF 5
#define FIRINGRATEUP_BUFF 6
#define PIERCEBULLET_BUFF 7
#define MULTIBULLET_BUFF 8
#define DEFENSEUP_BUFF 9

typedef struct{
    int types;
    int status;
    int rooms_to_clear = 0;
    bool left, right, top, buttom;//0并表示不可用，即关门 1表示可用，即开门
    EntityAdt entity;
}Map;

void CharcMove(Charc_Info & s, float x, float y);//人物移动 x轴

void Monster1_Insert(EntityAdt &entity, PCharc_Info p_charc);

double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);
int GetRnd(int seeds);

void MapInit();
int MapEdgeDet(int depth, float x, float y, int wid);//返回1时表示碰撞 0表示无碰撞

#endif // MY_HEADER_H_
