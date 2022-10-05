#ifndef MY_HEADER_H_
#define MY_HEADER_H_
#include "le.h"

void CharcMove(Charc_Info & s, float x, float y);//»ÀŒÔ“∆∂Ø x÷·

double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);
int GetRnd(int seeds);

void MonsterMoveType1(Charc_Info &Monster, Charc_Info main_char, float time_lag);
void MonsterMoveType2(Charc_Info &Monster, Charc_Info main_char, float time_lag);

#endif // MY_HEADER_H_
