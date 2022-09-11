#include "My_header.h"
#include <cmath>

extern int cxClient, cyClient;

void CharcMove_x(Charc_Info & s,int x)
{
    if(s.x + x + s.width < cxClient && s.x + x > 0 )
        s.x += x;
}

void CharcMove_y(Charc_Info & s, int y)
{
    if(s.y + y + s.width < cyClient && s.y + y > 0)
        s.y += y;
}

double sinx(Charc_Info &s, int cur_x, int cur_y)
{
    return (cur_y - s.y)/sqrt((cur_y - s.y * cur_y - s.y) + (cur_x - s.x * cur_x - s.x));
}

double cosx(Charc_Info &s, int cur_x, int cur_y)
{
    return (cur_x - s.x)/sqrt((cur_y - s.y * cur_y - s.y) + (cur_x - s.x * cur_x - s.x));
}
