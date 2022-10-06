#ifndef MY_HEADER_H_
#define MY_HEADER_H_
#include "le.h"

#define MAP_SIZE 3

#define NULL_MAPSTATUS 0
#define BATTLING_MAPSTATUS 1
#define SLEEP_MAPSTATUS 2

#define NULL_MAPTYPE 0      //�޷���
#define BATTLE_MAPTYPE 1    //ս������
#define REWARD_MAPTYPE 2    //��������
#define CORRIDOR_MAPTYPE 3  //���ȷ���

typedef struct{
    int status;
    int types;
    bool left, right, top, buttom;
    EntityAdt entity;
}Map;

void CharcMove(Charc_Info & s, float x, float y);//�����ƶ� x��

double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);
int GetRnd(int seeds);

void MonsterMoveType2(Charc_Info &Monster, Charc_Info main_char, float time_lag);

#endif // MY_HEADER_H_
