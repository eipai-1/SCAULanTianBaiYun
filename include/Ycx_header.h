#ifndef MY_HEADER_H_
#define MY_HEADER_H_

#include "le.h"
#include "chen.h"

#define MAP_SIZE 4

#define NULL_MAPSTATUS 0
#define BATTLING_MAPSTATUS 1
#define SLEEP_MAPSTATUS 2

#define NULL_MAPTYPE 0      //�޷���
#define BATTLE_MAPTYPE 1    //ս������
#define REWARD_MAPTYPE 2    //��������
#define CORRIDOR_HOR_MAPTYPE 3  //���ȷ��� ��
#define CORRIDOR_VER_MAPTYPE 4  //���ȷ��� ��

#define SHABBY_PISTOL_WEAPON 1
#define PREDATOR_PISTOL_WEAPON 2
#define SHOTGUN_WEAPON 3

#define START_GAMESTATUS 0
#define BATTLING_GAMESTATUS 1
#define RUNNING_GAMESTATUS 2

typedef struct{
    int status;
    int types;
    bool left, right, top, buttom;
    EntityAdt entity;
}Map;

typedef struct{
    float damage;
    float bullet_cd;
    int types;
}Weapon;

void CharcMove(Charc_Info & s, float x, float y);//�����ƶ� x��

double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);
int GetRnd(int seeds);

Weapon WeaponInit(float damage, float bullet_cd, int types);

void MapInit();
int MapEdgeDet(float x, float y, int wid);//����1ʱ��ʾ��ײ 0��ʾ����ײ

#endif // MY_HEADER_H_
