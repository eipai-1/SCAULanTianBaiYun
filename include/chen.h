#ifndef CHEN_H_
#define CHEN_H_

#include "le.h"
#include "BulletList.h"
#include "Ycx_header.h"

#define MOVEMODE_SLEEP 2
#define MOVEMODE_STATIC 0
#define MOVEMODE_SHIFT 1
#define MONSTER_BELONGS 0
#define MAIN_CHARC_BELONGS 1

typedef struct{
    int Level;
    float speed;
    float power;
}Weapon, *Arms;

void MonsterMoveType1(Charc_Info &Monster, Charc_Info main_char, float time_lag);
void MonsterMoveType2(Charc_Info &Monster, Charc_Info &main_char, float tome_lag);
void MonsterMoveType3(Charc_Info &Monster, Charc_Info &main_char, float tome_lag);
void EntiInit1(float x, float y);
void Bossstage1(Charc_Info &Monster, Charc_Info main_char, float time_lag);
void Bossstage2(Charc_Info &Monster, Charc_Info main_char, float time_lag);
float sin_x(float x1, float y1, float x2, float y2);
float cos_x(float x1, float y1, float x2, float y2);

#endif
