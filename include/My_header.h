#ifndef MY_HEADER_H_
#define MY_HEADER_H_

#include <windows.h>
typedef struct{
    int width = 50;
    int x, y;
    int hp_now, hp_max;
}Charc_Info;

void CharcMove_x(Charc_Info & s, int x);//�����ƶ� x��
void CharcMove_y(Charc_Info & s, int y);//�����ƶ� y��
double sinx(Charc_Info &s, int cur_x, int cur_y);
double cosx(Charc_Info &s, int cur_x, int cur_y);

#endif // MY_HEADER_H_
