#include "My_header.h"
#include <cmath>

extern int cxClient, cyClient;

void CharcMove_x(Charc_Info & s,float x)
{
    if(s.x + x + s.width < cxClient && s.x + x > 0 )
        s.x += x;
}

void CharcMove_y(Charc_Info & s, float y)
{
    if(s.y + y + s.width < cyClient && s.y + y > 0)
        s.y += y;
}

double sinx(int cur_x, int cur_y)
{
    return ((cur_y - cyClient/2)/sqrt((cur_y - cyClient/2) * (cur_y - cyClient/2) + (cur_x - cxClient/2) * (cur_x - cxClient/2)));
}

double cosx(int cur_x, int cur_y)
{
    return ((cur_x - cxClient/2)/sqrt((cur_y - cyClient/2) * (cur_y - cyClient/2) + (cur_x - cxClient/2) * (cur_x - cxClient/2)));
}
