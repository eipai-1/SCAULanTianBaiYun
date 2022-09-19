#ifndef MY_HEADER_H_
#define MY_HEADER_H_

#include <windows.h>
typedef struct{
    int width = 50;
    float x, y;
    int hp_now, hp_max;
}Charc_Info;

void CharcMove_x(Charc_Info & s, float x);//人物移动 x轴
void CharcMove_y(Charc_Info & s, float y);//人物移动 y轴
double sinx(int cur_x, int cur_y);
double cosx(int cur_x, int cur_y);

#endif // MY_HEADER_H_
