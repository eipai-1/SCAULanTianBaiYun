#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "include/Ycx_header.h"
#include "include/BulletList.h"
#include "include/le.h"
#include "include/chen.h"

/*-----游戏相关参数-----*/
//游戏引擎相关参数
static LARGE_INTEGER fre;
static float time_lag = 5;//帧生成时间
int cxClient = 1000, cyClient = 600;//窗口总大小
static HBITMAP hBmp_character, hBmp_background, hBmp_character2, hBmp_bullet,
hBmp_bullet2, hBmpBuf, hBmp_tree, hBmp_tree2;//游戏所有的位图句柄
static HINSTANCE hInstance;
static HDC hdc, hdcBmp, hdcBuf, hdcBmp2;//游戏所有的设备环境句柄
static int cur_x, cur_y;//鼠标位置
static HBRUSH hBrush;
static HFONT hFont;
int Game_status = RUNNING_GAMESTATUS;
//-----

//地图相关参数
Map Game_map[MAP_SIZE][MAP_SIZE];
float map_wall_width = 2;
float Rad_of_gate = 40, horCor_wid = 80, horCor_lgh = cxClient,
verCor_wid = 80, verCor_lgh = cyClient;
int x_map = 0, y_map = 0, map_temp_left, map_temp_top, map_temp_right, map_temp_buttom;
bool Monster_clear = true;
//-----

//弹幕相关参数
static const int AMMO_MAX = 10;//全局最大弹幕数
myadt_bullet Ammo;//全局弹幕
float bullet_cd = 500, bullet_speed = 1.2;//弹幕cd与弹幕射速
static float bullet_cd_cnt = bullet_cd;//弹幕cd计时器
//-----

//实体相关参数
static Charc_Info CharcInfo = Entity_Init(50, cxClient/2, cyClient/2, MAIN_CHARC_TYPE, 100);//主角信息
static float steps = 0.3;//主角移动速度
const int ENTITY_MAX = 100;//最大实体数
EntityAdt Global_entity;//全局实体

int Monster1_wid = 64;
float Monster1_steps = 0.1, Monster1_movetime = 500, Monster1_direction;
int Monster1_movemode = 1;
float Alert_range_squared = 9e4;
float  Monster1_bullet_speed = 0.5;
float Monster1_bullet_movetime = 1000;

int hp_paint_width = 5;
//-----

//游戏道具参数
int weapon_now;
//-----

//其他参数
static int rnd_flag = 1;
static float rnd_num;
const float Pi = 2*acos(0.0);
//-----



/*-----游戏主要函数声明-----*/
//游戏引擎声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GameMain();
void GameInit();
void GamePaint(HWND);
void GameQuit();
void GameEventLeftClick(int cur_x, int cur_y);
//-----

//地图相关函数声明
void MapPaint();
//-----

//弹幕函数声明
void BulletUpdate();
void BulletPaint();
//-----

//实体函数声明
void EntityUpdate();
void EntityPaint();
void EntityInit();
void EntityHpPaint(Charc_Info &s);
void MainCharcHpPaint();
//-----

//游戏道具函数声明
void Weapon_Load();
//-----

//其他函数声明
void RndPaint();
//-----



/*-----游戏主要函数定义-----*/
//游戏引擎定义
void GameMain()
{
    if(GetAsyncKeyState('W'))CharcMove(CharcInfo, 0, -steps*time_lag);
    if(GetAsyncKeyState('A'))CharcMove(CharcInfo, -steps*time_lag, 0);
    if(GetAsyncKeyState('S'))CharcMove(CharcInfo, 0, steps*time_lag);
    if(GetAsyncKeyState('D'))CharcMove(CharcInfo, steps*time_lag, 0);

    if(GetAsyncKeyState(VK_LBUTTON))GameEventLeftClick(cur_x, cur_y);

    if(CharcInfo.x/cxClient != x_map || CharcInfo.y/cyClient != y_map){//全局实体更新
        int temp_x, temp_y;
        if(CharcInfo.x / cxClient > x_map){
            temp_x = (CharcInfo.x + cxClient/2)/cxClient;
        }
        else{
            temp_x = (CharcInfo.x - cxClient/2)/cxClient;
        }
        if(CharcInfo.y / cyClient > y_map){
            temp_y = (CharcInfo.y + cyClient/2)/cyClient;
        }
        else{
            temp_y = (CharcInfo.y - cyClient/2)/cyClient;
        }

        if(temp_x == x_map - 2 || temp_x == x_map + 2){
            x_map = temp_x;
            Global_entity = Game_map[y_map][x_map].entity;
            Entity_insert(Global_entity, &CharcInfo);
        }
        else if(temp_y == y_map - 2 || temp_y == y_map + 2){
            y_map = temp_y;
            Global_entity = Game_map[y_map][x_map].entity;
            Entity_insert(Global_entity, &CharcInfo);
        }
    }
//*
    if(Game_status == BATTLING_GAMESTATUS && Monster_clear){
        Game_status = RUNNING_GAMESTATUS;
            Game_map[y_map][x_map].left = map_temp_left;
            Game_map[y_map][x_map].top = map_temp_top;
            Game_map[y_map][x_map].right = map_temp_right;
            Game_map[y_map][x_map].buttom = map_temp_buttom;
    }
//*/

    BulletUpdate();
    EntityUpdate();
}
void GameInit()
{
    hBmp_character = LoadBitmap(hInstance, TEXT("character"));
    hBmp_background = LoadBitmap(hInstance, TEXT("background"));
    hBmp_character2 = LoadBitmap(hInstance, TEXT("character2"));
    hBmp_bullet = LoadBitmap(hInstance, TEXT("bullet"));
    hBmp_bullet2 = LoadBitmap(hInstance, TEXT("bullet2"));
    hBmp_tree = LoadBitmap(hInstance, TEXT("tree"));
    hBmp_tree2 = LoadBitmap(hInstance, TEXT("tree2"));
    bulletary_init(Ammo, AMMO_MAX);

    EntityAry_init(Global_entity, ENTITY_MAX);
    //EntityAry_init(Global_entity, ENTITY_MAX);
    Entity_insert(Global_entity, &CharcInfo);
    EntityInit();

    Game_map[0][0].entity = Global_entity;

    printf("%d",Global_entity.p);

    hFont = CreateFont(24, 18, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, TEXT("MyFONT"));

    MapInit();
}
void GamePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    RECT rect;
    char szbuffer[100];

    hdc = BeginPaint(hwnd, &ps);

    hdcBuf = CreateCompatibleDC(hdc);
    hdcBmp = CreateCompatibleDC(hdc);
    hdcBmp2 = CreateCompatibleDC(hdc);
    hBmpBuf = CreateCompatibleBitmap(hdc, cxClient, cyClient);
    SelectObject(hdcBuf, hBmpBuf);
    GetClientRect(hwnd, &rect);
    FillRect(hdcBuf, &rect, WHITE_BRUSH);

    SetBkMode(hdcBuf, TRANSPARENT);

    //加载背景
    MapPaint();
    //SelectObject(hdcBmp, hBmp_background);
    //StretchBlt(hdcBuf, 0 + cxClient/2 - CharcInfo.x, 0 + cyClient/2 - CharcInfo.y, cxClient, cyClient, hdcBmp, 0, 0, 100, 100, SRCCOPY);

    SelectObject(hdcBmp2, hBmp_character2);
    StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp2, 0, 0, 64, 64, SRCAND);

    SelectObject(hdcBmp, hBmp_character);
    StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp, 0, 0, 64, 64, SRCPAINT);

    SetTextColor(hdcBuf, RGB(40, 20, 50));
    SelectObject(hdcBuf, (HFONT)hFont);
    TextOut(hdcBuf, 0, 0, szbuffer, wsprintf(szbuffer, "x_map:%d y_map:%d", x_map, y_map));
    TextOut(hdcBuf, cxClient - 200, 0, szbuffer, wsprintf(szbuffer, "FPS:%d", (int)(1000.0/time_lag)));
    //TextOut(hdcBuf, 0, 20, szbuffer, wsprintf(szbuffer, "%d,%d", cur_x, cur_y));
    //TextOut(hdcBuf, 0, 40, szbuffer, wsprintf(szbuffer, "CharcInfo.width=%d", CharcInfo.width));

    BulletPaint();
    EntityPaint();
    //RndPaint();

    MainCharcHpPaint();
    //将画布复制到屏幕
    BitBlt(hdc, 0, 0, cxClient, cyClient, hdcBuf, 0, 0, SRCCOPY);//将画布复制到屏幕上
    DeleteObject(hBmpBuf);
    DeleteDC(hdcBuf);
    DeleteDC(hdcBmp);
    DeleteDC(hdcBmp2);
    EndPaint(hwnd, &ps);
}
void GameQuit()
{
    bullet_free(Ammo);
    DeleteObject(hBrush);
    DeleteObject(hBmp_character);
    DeleteObject(hBmp_background);
    PostQuitMessage (0) ;
}
void GameEventLeftClick(int cur_x, int cur_y)
{
    if(bullet_cd_cnt >= bullet_cd){//到cd才能发射
        if(bullet_insert(Ammo, bullet_init(CharcInfo.x, CharcInfo.y, MAIN_CHARC_BELONGS,
                    (bullet_speed*cosx(cur_x, cur_y)), (bullet_speed*sinx(cur_x, cur_y)), 10))){
        }
        bullet_cd_cnt = 0;
    }
}
//-----

//地图相关函数定义
void MapPaint()
{
    float tempx, tempy;

    SelectObject(hdcBmp, hBmp_background);
    for(int i = 0; i < MAP_SIZE; i++){
        for(int j = 0; j < MAP_SIZE; j++){
            switch(Game_map[i][j].types){
            case NULL_MAPTYPE:
                break;

            case CORRIDOR_HOR_MAPTYPE:
                tempx = j*cxClient + cxClient/2 - CharcInfo.x;
                tempy = i*cyClient + cyClient/2 - CharcInfo.y + (cyClient/2 - horCor_wid/2);
                //printf("%lf %lf\n", tempx, tempy);
                StretchBlt(hdcBuf, tempx,
                           tempy,
                           horCor_lgh, horCor_wid, hdcBmp, 0, 0, 100, 100, SRCCOPY);
                break;

            case CORRIDOR_VER_MAPTYPE:
                tempx = j*cxClient + cxClient/2 - CharcInfo.x + (cxClient/2 - verCor_wid/2);
                tempy = i*cyClient + cyClient/2 - CharcInfo.y;
                //printf("%lf %lf\n", tempx, tempy);
                StretchBlt(hdcBuf, tempx,
                           tempy,
                           verCor_wid, verCor_lgh, hdcBmp, 0, 0, 100, 100, SRCCOPY);
                break;

            case BATTLE_MAPTYPE:
            case REWARD_MAPTYPE:
                StretchBlt(hdcBuf, j*cxClient + cxClient/2 - CharcInfo.x,
                           i*cyClient + cyClient/2 - CharcInfo.y,
                           cxClient, cyClient, hdcBmp, 0, 0, 100, 100, SRCCOPY);
                break;
            }
        }
    }
}
//-----

//弹幕函数定义
void BulletUpdate()
{
    int Col_id, ret_id;
    if(bullet_cd_cnt < bullet_cd){
        bullet_cd_cnt += time_lag;
    }
    for(int i = 0; i < Ammo.p; i++){
        if(MapEdgeDet(Ammo.b[i].x + Ammo.b[i].vx*time_lag, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 10)){
            bullet_delete(Ammo, i);
        }

        Col_id = GlobalCollisionDet(Global_entity, Ammo.b[i].x + Ammo.b[i].vx*time_lag, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 15, -1, &ret_id);
        switch(Col_id){
        case NO_COLLSION_TYPE:
            Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
            Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            break;

        case MAIN_CHARC_TYPE:
            if(Ammo.b[i].belongs == MONSTER_BELONGS){
                CharcInfo.hp_now -= Ammo.b[i].damage;
                bullet_delete(Ammo, i);
                return;
            }
            else{
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            }
            break;

        case OBSTACLE_TYPE:
        case OBSTACLE_UNBREAKABLE_TYPE:
            bullet_delete(Ammo, i);
            break;

        case MONSTER_TYPE:
            if(Ammo.b[i].belongs == MAIN_CHARC_BELONGS){
                Global_entity.b[ret_id]->hp_now -= Ammo.b[i].damage;
                bullet_delete(Ammo, i);
                if(Global_entity.b[ret_id]->movemode == MOVEMODE_SLEEP){
                    Global_entity.b[ret_id]->movemode = MOVEMODE_SHIFT;
                }
            }
            else{
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            }
            break;
        }
    }
}
void BulletUpdate2()
{
    int Col_id, ret_id;
    if(bullet_cd_cnt < bullet_cd){
        bullet_cd_cnt += time_lag;
    }
    for(int i = 0; i < Ammo.p; i++){
        Col_id = GlobalCollisionDet(Global_entity, Ammo.b[i].x + Ammo.b[i].vx*time_lag, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 15, -1, &ret_id);
        switch(Col_id){
        case 0:
            if(!MapEdgeDet(Ammo.b[i].x + Ammo.b[i].vx*time_lag, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 10)){
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            }
            else{
                bullet_delete(Ammo, i);
            }
            break;

        case 4:
            if(Ammo.b[i].belongs == MONSTER_BELONGS){
                CharcInfo.hp_now -= Ammo.b[i].damage;
                bullet_delete(Ammo, i);
            }
            else if(!MapEdgeDet(Ammo.b[i].x + Ammo.b[i].vx*time_lag, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 10)){
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            }
            else{
                bullet_delete(Ammo, i);
            }
            break;

        case 1:
        case 2:
            bullet_delete(Ammo, i);
            break;

        case 3:
            if(Ammo.b[i].belongs == MAIN_CHARC_BELONGS){
                Global_entity.b[ret_id]->hp_now -= Ammo.b[i].damage;
                bullet_delete(Ammo, i);
            }
            else if(Ammo.b[i].x + Ammo.b[i].vx < cxClient
               && Ammo.b[i].y + Ammo.b[i].vy < cyClient
               && Ammo.b[i].x > 0 && Ammo.b[i].y > 0){
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
            }
            break;
        }
    }
}
void BulletPaint()
{
    SelectObject(hdcBmp2, hBmp_bullet2);
    SelectObject(hdcBmp, hBmp_bullet);
    for(int i = 0; i < Ammo.p; i++){
        StretchBlt(hdcBuf, cxClient/2 - CharcInfo.x + Ammo.b[i].x, cyClient/2 - CharcInfo.y + Ammo.b[i].y, 15, 15, hdcBmp2, 0, 0, 15, 15, SRCAND);
        StretchBlt(hdcBuf, cxClient/2 - CharcInfo.x + Ammo.b[i].x, cyClient/2 - CharcInfo.y + Ammo.b[i].y, 15, 15, hdcBmp, 0, 0, 15, 15, SRCPAINT);
    }
}
//-----

//实体函数定义
void EntityPaint()
{
    for(int i = 0; i < Global_entity.p; i++){
        switch(Global_entity.b[i]->types){
        case OBSTACLE_UNBREAKABLE_TYPE:
            SelectObject(hdcBmp, hBmp_tree);
            SelectObject(hdcBmp2, hBmp_tree2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 20, 20, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 20, 20, SRCPAINT);
            break;

        case MONSTER_TYPE:
        case MONSTER2_TYPE:
            SelectObject(hdcBmp, hBmp_character);
            SelectObject(hdcBmp2, hBmp_character2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 64, 64, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 64, 64, SRCPAINT);
            EntityHpPaint(*(Global_entity.b[i]));
            break;

        }
    }
}
void EntityUpdate()
{
    Monster_clear = true;
    for(int i = 0; i < Global_entity.p; i++){
        if(Global_entity.b[i]->types == MONSTER_TYPE){
            if(Monster_clear){
                Monster_clear = false;
            }

            if(Global_entity.b[i]->hp_now < 0){
                Entity_delete(Global_entity, i);//删除操作
            }
            else{
                MonsterMoveType1(*Global_entity.b[i], CharcInfo, time_lag);
            }
        }
    }
}
void EntityInit()
{
    Entity_insert(Global_entity, Entity_Init_P(Monster1_wid, 100, 100, MONSTER_TYPE, 100));
    Global_entity.b[Global_entity.p - 1]->cnt = 0;
    Global_entity.b[Global_entity.p - 1]->movemode = MOVEMODE_SLEEP;
    Global_entity.b[Global_entity.p - 1]->bullet_cnt = 0;
}
void EntityHpPaint(Charc_Info &s)
{
    hBrush = CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    SelectObject(hdcBuf, hBrush);
    RoundRect(hdcBuf, s.x + cxClient/2 - CharcInfo.x, s.y + cyClient/2 - CharcInfo.y,
              s.x + s.width + cxClient/2 - CharcInfo.x, s.y + hp_paint_width + cyClient/2 - CharcInfo.y, 6, 6);
    DeleteObject(hBrush);

    hBrush = CreateSolidBrush(RGB(244, 40, 20));
    SelectObject(hdcBuf, hBrush);
    RoundRect(hdcBuf, s.x + cxClient/2 - CharcInfo.x, s.y + cyClient/2 - CharcInfo.y,
              s.x + s.width*((float)s.hp_now/s.hp_max) + cxClient/2 - CharcInfo.x, s.y + hp_paint_width + cyClient/2 - CharcInfo.y, 6, 6);
    DeleteObject(hBrush);

}
void MainCharcHpPaint()
{
    hBrush = CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    SelectObject(hdcBuf, hBrush);
    RoundRect(hdcBuf, 60, 20, 160, 30, 5, 5);
    DeleteObject(hBrush);

    hBrush = CreateSolidBrush(RGB(244, 40, 20));
    SelectObject(hdcBuf, hBrush);
    RoundRect(hdcBuf, 60, 20, 60 + (100)*((float)CharcInfo.hp_now/CharcInfo.hp_max), 30, 5, 5);
    DeleteObject(hBrush);

    char szbuffer[100];
    TextOut(hdcBuf, 170, 20, szbuffer, wsprintf(szbuffer, "%d/%d", CharcInfo.hp_now, CharcInfo.hp_max));
}
//-----

//游戏道具函数定义
void Weapon_Shoot(int cur_x, int cur_y)
{
    switch(weapon_now){
    case SHABBY_PISTOL_WEAPON:

        break;

    case SHOTGUN_WEAPON:

        break;

    case PREDATOR_PISTOL_WEAPON:

        break;
    };
}
//-----

//其他函数定义
void RndPaint()
{
    static char rnd_szbfr[100];
    if(rnd_flag){
        srand((int)(time_lag*1000) % 100);
        rnd_num = cos((rand() % 1000) / 500.0 * Pi);
        rnd_flag = 0;
    }
    TextOut(hdcBuf, 0, 40, rnd_szbfr, wsprintf(rnd_szbfr, "rnd=%d", (int)(rnd_num*1000)));
}
//-----



/*-----主函数与窗口过程-----*/
int WINAPI WinMain (HINSTANCE hIns, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("Window");
    HWND         hwnd ;
    MSG          msg ;
    WNDCLASS     wndclass ;
    LARGE_INTEGER pre, next;
    //float frame_lag;

    wndclass.style         = 0;//注意 窗口重设大小不会重绘
    wndclass.lpfnWndProc   = WndProc;//窗口过程为WndProc
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hIns;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;

    hInstance = hIns;

    if (!RegisterClass (&wndclass))
    {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
                  szAppName, MB_ICONERROR) ;
        return 0 ;
    }

    hwnd = CreateWindow (szAppName, TEXT ("TypicalWindow"),
                      WS_TILED | WS_MINIMIZEBOX | WS_SYSMENU,
                      0, 0,
                      cxClient, cyClient,
                      NULL, NULL, hInstance, NULL) ;

    ShowWindow (hwnd, iCmdShow) ;
    UpdateWindow (hwnd) ;

    QueryPerformanceFrequency(&fre);
    QueryPerformanceCounter(&pre);
    Sleep(1);
    QueryPerformanceCounter(&next);
    timeBeginPeriod(1);

    //游戏主循环
    while (msg.message != WM_QUIT)
    {
        //Sleep(40);
        if(PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)){
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
        GameMain();
        {
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            pre = next;
        }
        QueryPerformanceCounter(&next);
        time_lag = (((next.QuadPart - pre.QuadPart)*1000.0)/fre.QuadPart);
    }

    return msg.wParam;
}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Charc_Info temp_charcInfo;
    switch (message)
    {
    case WM_CREATE:
        GameInit();
        return 0;

    case WM_LBUTTONDBLCLK:
        GameEventLeftClick(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_MOUSEMOVE:
        cur_x = LOWORD(lParam);
        cur_y = HIWORD(lParam);
        return 0;

    case WM_RBUTTONDOWN:
        cur_x = LOWORD(lParam);
        cur_y = HIWORD(lParam);
        EntityInit();
        return 0;

    case WM_PAINT:
        GamePaint(hwnd);
        return 0;

    case WM_DESTROY:
        GameQuit();
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
