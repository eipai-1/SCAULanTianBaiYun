#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
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
hBmp_bullet2, hBmpBuf, hBmp_tree, hBmp_tree2, hBmp_teleport, hBmp_teleport2,
hBmp_dialog, hBmp_blood_buff, hBmp_rage_buff, hBmp_rush_buff, hBmp_damageup_buff,
hBmp_bulletspeedup_buff, hBmp_firingrateup_buff, hBmp_piercebullet_buff,
hBmp_multibullet_buff, hBmp_defenseup_buff, hBmp_timedrop, hBmp_curedrop,
hBmp_timedrop2, hBmp_curedrop2, hBmp_monster, hBmp_monster2, hBmp_box,
hBmp_box2, hBmp_title, hBmp_title1, hBmp_title2;//游戏所有的位图句柄
static HINSTANCE hInstance;
static HDC hdc, hdcBmp, hdcBuf, hdcBmp2;//游戏所有的设备环境句柄
static int cur_x, cur_y;//鼠标位置
static HBRUSH hBrush;
static HFONT hFont, hFont_title;
int Game_status = START_GAMESTATUS, prev_Game_status;
int Game_text_now = 0;
static int Font_height = 24, Font_width = 12;
static float key_down_cnt, key_down_time = 300;
int game_start_selecting;
//-----

//地图相关参数
int Map_depth_now = 0;
Map Game_map[MAP_DEPTH][MAP_SIZE][MAP_SIZE];
int Map_rooms_to_clear[MAP_DEPTH];
float map_wall_width = 2;
float Rad_of_gate = 40, horCor_wid = 80, horCor_lgh = cxClient,
verCor_wid = 80, verCor_lgh = cyClient;
int x_map = 0, y_map = 0, map_temp_left, map_temp_top, map_temp_right, map_temp_buttom;
bool Monster_clear = true;
int rooms_cleared = 0;
//-----

//弹幕相关参数
static int bullet_damage = 10;
static const int AMMO_MAX = 1000;//全局最大弹幕数
myadt_bullet Ammo;//全局弹幕
float bullet_cd = 500, bullet_speed = 1.2;//弹幕cd与弹幕射速
static float bullet_cd_cnt = bullet_cd;//弹幕cd计时器
static float bullet_disperce = 18;//多发子弹时，子弹的散布
//-----

//实体相关参数
static Charc_Info CharcInfo =
Entity_Init(50, cxClient/2, cyClient/2, MAIN_CHARC_TYPE, 100);//主角信息
static float steps = 0.3;//主角移动速度
const int ENTITY_MAX = 100;//最大实体数
EntityAdt Global_entity;//全局实体

int Monster1_wid = 64;
float Monster1_steps = 0.27, Monster1_movetime = 500, Monster1_direction;
int Monster1_movemode = 1;
float Alert_range_squared = 9e4;
float  Monster1_bullet_speed = 0.5;
float Monster1_bullet_movetime = 1000;

float Monster2_damage = 7;

int hp_paint_width = 5;
//-----

//游戏道具参数
const int MAX_BUFF = 9;
int weapon_now;
int buff_choose[3];
int Game_buff[MAX_BUFF + 1];//全部初始值设为0，代表buff未选，1表示buff已选
int cor_buff_x = 350, cor_buff_y = 350, cor_buff_wid = 50;
static int chosen_buff;

static int cure_item_num = 0, time_item_num = 0;
static int cure_hp_recover = 10;
float bloodsucking_rate = 0.05;
float damage_factor = 1.0;
float speed_factor = 1.0;
float bulletspeedup_buff_factor = 0.3, damageup_buff_factor = 0.4, firingrateup_buff_factor = 0.3;
float rush_cnt = 80, rush_time = 80, rush_speed = 1.5, rush_cd_cnt = 1800, rush_cd_time = 1800, rushing_status = 0;
float time_item_cd = 5000, time_item_cnt = 5000, cure_item_cd = 3000, cure_item_cnt = 3000;

float time_factor = 1.0;//时间修正系数
float time_slow_cnt = 0, time_slow_duration = 2000;

int time_drop_prob = 3, cure_drop_prob = 3;
//-----

//其他参数
static int rnd_flag = 1;
static float rnd_num;
const float Pi = 2*acos(0.0);
//-----



/*-----游戏主要函数声明-----*/
//游戏引擎声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GameMain();//游戏更新函数
void GameInit();//游戏初始化函数
void GamePaint(HWND);//游戏绘制函数
void GameQuit();
void GameEventLeftClick(int cur_x, int cur_y);//游戏左键单击事件函数
void GameInteract(int event, int id);//游戏互动函数
void GameTextPaintDown();
//-----

//地图相关函数声明
void MapPaint();//地图绘制函数
//-----

//弹幕函数声明
void BulletUpdate();//弹幕更新函数
void BulletPaint();//弹幕绘制函数
//-----

//实体函数声明
void EntityUpdate();//实体更新
void EntityPaint();//实体绘制
void EntityInit();
void EntityHpPaint(Charc_Info &s);
void MainCharcHpPaint();
//-----

//游戏道具函数声明
//-----

//其他函数声明
//-----



/*-----游戏主要函数定义-----*/
//游戏引擎定义
void GameMain()
{
    int Col_id, *entity_ret_id = (int*)malloc(sizeof(int));
    static int vx = 0, vy = 0;

    switch(Game_status){
    case START_GAMESTATUS:
        if(cxClient*2.0/5.0 < cur_x && cur_x < cxClient*3.0/5
           && cyClient*2.0/5 < cur_y && cur_y < cyClient*3.0/5){
            game_start_selecting = 1;
        }
        else if(cxClient*2.0/5.0 < cur_x && cur_x < cxClient*3.0/5
           && cyClient*3.0/5 < cur_y && cur_y < cyClient*4.0/5){
            game_start_selecting = 2;
        }
        else{
            game_start_selecting = 0;
        }
        if(GetAsyncKeyState(VK_LBUTTON)){
            switch(game_start_selecting){
            case 1:
                Game_status = RUNNING_GAMESTATUS;
                break;

            case 2:
                PostQuitMessage(0);
                break;
            }
        }
        break;

    case END_GAMESTATUS:
        if(GetAsyncKeyState(VK_ESCAPE))Game_status = START_GAMESTATUS;
        break;

    case RUNNING_GAMESTATUS:
    case BATTLING_GAMESTATUS:
        //检测是否按下暂停
        if(key_down_cnt < key_down_time){
            key_down_cnt += time_lag;
        }
        else if(GetAsyncKeyState(VK_ESCAPE)){
            prev_Game_status = Game_status;
            Game_status = PAUSE_GAMESTATUS;
            key_down_cnt = 0;
            return;
        }
        //-----

        //检测是否按下移动键
        if(rushing_status == 0){
            vx = vy = 0;//未冲刺时
            if(GetAsyncKeyState('W')){
                CharcMove(CharcInfo, 0, -steps*time_lag*speed_factor);
                vy = -1;
            }
            if(GetAsyncKeyState('A')){
                CharcMove(CharcInfo, -steps*time_lag*speed_factor, 0);
                vx = -1;
            }
            if(GetAsyncKeyState('S')){
                CharcMove(CharcInfo, 0, steps*time_lag*speed_factor);
                vy = 1;
            }
            if(GetAsyncKeyState('D')){
                CharcMove(CharcInfo, steps*time_lag*speed_factor, 0);
                vx = 1;
            }
        }
        //-----

        //检测是否发射子弹
        if(GetAsyncKeyState(VK_LBUTTON)){
            GameEventLeftClick(cur_x, cur_y);
        }
        //-----

        //检测是否使用了道具
        if(time_item_cnt >= time_item_cd){
            if(time_item_num > 0){
                if(GetAsyncKeyState('T')){
                    time_item_num--;
                    time_item_cnt = 0;
                    time_factor = 0.25;
                }
            }
        }
        else{
            time_item_cnt += time_lag;
        }

        if(cure_item_cnt >= cure_item_cd){
            if(cure_item_num > 0){
                if(GetAsyncKeyState('H')){
                    if(CharcInfo.hp_now + cure_hp_recover <= CharcInfo.hp_max){
                        CharcInfo.hp_now += cure_hp_recover;
                        cure_item_num--;
                        cure_item_cnt = 0;
                    }
                }
            }
        }
        else{
            cure_item_cnt += time_lag;
        }

        if(time_factor != 1.0){
            if(time_slow_cnt >= time_slow_duration){
                time_factor = 1.0;
                time_slow_cnt = 0;
            }
            else{
                time_slow_cnt += time_lag;
            }
        }
        //-----

        //互动部分 包括文本显示与实际互动效果
        Col_id = GlobalCollisionDet(Global_entity, CharcInfo.x, CharcInfo.y, CharcInfo.width, CharcInfo.id, entity_ret_id);
        switch(Col_id){
        case TELEPORT_TYPE:
            Game_text_now = TELEPORT_TEXT;
            break;

        case CUREDROP_TYPE:
            Game_text_now = CUREDROP_TYPE;
            break;

        case TIMEDROP_TYPE:
            Game_text_now = TIMEDROP_TYPE;
            break;

        default:
            if(Game_text_now)Game_text_now = NULL_TEXT;
        }

        if(GetAsyncKeyState(VK_SPACE))GameInteract(Col_id, *entity_ret_id);
        //-----

        //全局实体变量更新
        if(CharcInfo.x/cxClient != x_map || CharcInfo.y/cyClient != y_map){
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
                Global_entity = Game_map[Map_depth_now][y_map][x_map].entity;
                Entity_insert(Global_entity, &CharcInfo);
            }
            else if(temp_y == y_map - 2 || temp_y == y_map + 2){
                y_map = temp_y;
                Global_entity = Game_map[Map_depth_now][y_map][x_map].entity;
                Entity_insert(Global_entity, &CharcInfo);
            }
        }
        //-----

        //战斗结束，开门
        if(Game_status == BATTLING_GAMESTATUS && Monster_clear){
            Game_status = RUNNING_GAMESTATUS;
            rooms_cleared++;
            Game_map[Map_depth_now][y_map][x_map].status = NULL_MAPSTATUS;
            Game_map[Map_depth_now][y_map][x_map].left = map_temp_left;
            Game_map[Map_depth_now][y_map][x_map].top = map_temp_top;
            Game_map[Map_depth_now][y_map][x_map].right = map_temp_right;
            Game_map[Map_depth_now][y_map][x_map].buttom = map_temp_buttom;
        }
        //-----

        //根据buff更新数值
        if(Game_buff[RAGE_BUFF]){
            float rate = 1 - (CharcInfo.hp_now/CharcInfo.hp_max);
            speed_factor = (1 + rate);
            damage_factor = (damage_factor + rate);
        }
        //-----

        //判断是否拥有并使用了技能
        if(Game_buff[RUSH_BUFF]){//如果有冲刺技能
            if(rush_cd_cnt >= rush_cd_time){
                //printf("CD!\n");
                if(rushing_status == 0){//处于未冲刺状态
                    if(GetAsyncKeyState(VK_SHIFT) && (vx || vy)){//启动冲刺状态
                        rushing_status = 1;
                    }
                }
                else if(rush_cnt > 0){//已处于冲刺状态 且正在持续
                    CharcMove(CharcInfo, vx*rush_speed*time_lag, vy*rush_speed*time_lag);
                    rush_cnt -= time_lag;
                }
                else{//冲刺结束
                    rush_cnt = rush_time;//恢复持续时间
                    rushing_status = 0;//关闭冲刺状态
                    rush_cd_cnt = 0;
                    vx = vy = 0;
                }
            }
            else{
                if(GetAsyncKeyState(VK_SHIFT));
                rush_cd_cnt += time_lag;
            }
        }
        //-----

        //其他更新
        BulletUpdate();
        EntityUpdate();
        //-----
        break;


    //选择增益buff阶段
    case CHOOSING_GAMESTATUS:
        if(cor_buff_x < cur_x && cur_x < cor_buff_x + cor_buff_wid
           && cor_buff_y < cur_y && cur_y < cor_buff_y + cor_buff_wid){
            chosen_buff = buff_choose[0];
        }
        else if(cor_buff_x + cor_buff_wid * 2 < cur_x && cur_x < cor_buff_x + cor_buff_wid * 3
           && cor_buff_y < cur_y && cur_y < cor_buff_y + cor_buff_wid){
            chosen_buff = buff_choose[1];
        }
        else if(cor_buff_x + cor_buff_wid * 4 < cur_x && cur_x < cor_buff_x + cor_buff_wid * 5
           && cor_buff_y < cur_y && cur_y < cor_buff_y + cor_buff_wid){
            chosen_buff = buff_choose[2];
        }
        else{
            chosen_buff = 0;
        }

        //按下左键
        if(GetAsyncKeyState((VK_LBUTTON))){//选中操作
            if(chosen_buff != 0){
                Game_status = RUNNING_GAMESTATUS;
                Game_buff[chosen_buff] = 1;

                switch(chosen_buff){
                case DAMAGEUP_BUFF:
                    damage_factor += damageup_buff_factor;
                    break;

                case BULLETSPEEDUP_BUFF:
                    bullet_speed *= (1 + bulletspeedup_buff_factor);
                    break;

                case FIRINGRATEUP_BUFF:
                    bullet_cd *= (1 - firingrateup_buff_factor);
                    break;

                case PIERCEBULLET_BUFF:
                    damage_factor -= 0.3;
                    if(damage_factor < 0)damage_factor = 0.01;
                    break;

                case MULTIBULLET_BUFF:
                    damage_factor -= 0.4;
                    if(damage_factor < 0)damage_factor = 0.01;
                    break;
                }
            }
        }
        //-----
        break;
    //-----


    //暂停状态
    case PAUSE_GAMESTATUS:
        //检测是否取消了暂停
        if(key_down_cnt < key_down_time){
            key_down_cnt += time_lag;
        }
        else if(GetAsyncKeyState(VK_ESCAPE)){
            Game_status = prev_Game_status;
            key_down_cnt = 0;
        }
        //-----
        break;
    //-----

    }
}
void GameInit()
{
    //printf("init start\n");
    hBmp_dialog = LoadBitmap(hInstance, TEXT("dialog"));
    hBmp_character = LoadBitmap(hInstance, TEXT("knight"));
    hBmp_character2 = LoadBitmap(hInstance, TEXT("knight2"));
    hBmp_monster = LoadBitmap(hInstance, TEXT("monster"));
    hBmp_background = LoadBitmap(hInstance, TEXT("background"));
    hBmp_monster2 = LoadBitmap(hInstance, TEXT("monster2"));
    hBmp_bullet = LoadBitmap(hInstance, TEXT("bullet"));
    hBmp_bullet2 = LoadBitmap(hInstance, TEXT("bullet2"));
    hBmp_tree = LoadBitmap(hInstance, TEXT("tree"));
    hBmp_tree2 = LoadBitmap(hInstance, TEXT("tree2"));
    hBmp_teleport = LoadBitmap((hInstance), TEXT("teleport"));
    hBmp_teleport2 = LoadBitmap((hInstance), TEXT("teleport2"));

    hBmp_blood_buff = LoadBitmap(hInstance, TEXT("blood_buff"));
    hBmp_rage_buff = LoadBitmap(hInstance, TEXT("rage_buff"));
    hBmp_rush_buff = LoadBitmap(hInstance, TEXT("rush_buff"));
    hBmp_damageup_buff = LoadBitmap(hInstance, TEXT("damageup_buff"));
    hBmp_bulletspeedup_buff = LoadBitmap(hInstance, TEXT("bulletspeedup_buff"));
    hBmp_firingrateup_buff = LoadBitmap(hInstance, TEXT("firingrateup_buff"));
    hBmp_piercebullet_buff = LoadBitmap(hInstance, TEXT("piercebullet_buff"));
    hBmp_multibullet_buff = LoadBitmap(hInstance, TEXT("multibullet_buff"));
    hBmp_defenseup_buff = LoadBitmap(hInstance, TEXT("defenseup_buff"));

    hBmp_timedrop = LoadBitmap(hInstance, TEXT("timedrop"));
    hBmp_timedrop2 = LoadBitmap(hInstance, TEXT("timedrop2"));
    hBmp_curedrop = LoadBitmap(hInstance, TEXT("curedrop"));
    hBmp_curedrop2 = LoadBitmap(hInstance, TEXT("curedrop2"));

    hBmp_box = LoadBitmap(hInstance, TEXT("box"));
    hBmp_box2 = LoadBitmap(hInstance, TEXT("box2"));

    hBmp_title = LoadBitmap(hInstance, TEXT("title"));
    hBmp_title1 = LoadBitmap(hInstance, TEXT("title1"));
    hBmp_title2 = LoadBitmap(hInstance, TEXT("title2"));
    //printf("Map start\n");
    MapInit();
    //printf("map finished\n");

    bulletary_init(Ammo, AMMO_MAX);

    //EntityAry_init(Global_entity, ENTITY_MAX);
    Entity_insert(Game_map[Map_depth_now][0][0].entity, &CharcInfo);
    //EntityInit();

    Global_entity = Game_map[Map_depth_now][0][0].entity;

    //printf("%d\n",Global_entity.p);

    hFont = CreateFont(Font_height, Font_width, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, TEXT("MyFONT"));

    for(int i = 0; i < MAX_BUFF; i++){
        Game_buff[i] = 0;
    }
    //Game_buff[RUSH_BUFF] = 1;
    //printf("INIT\n");
}
void GamePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    RECT rect;
    char szbuffer[100];
    int num;
    int cor_text_x = 200, cor_text_y = cor_buff_wid, cor_timedrop_x = 250,
    cor_timedrop_y = 5, cor_curedrop_x = 400, cor_curedrop_y = 5, cor_drop_wid = 50,
    cor_cure_cd_x = 390, cor_cure_cd_y = 5, cor_time_cd_x = 240, cor_time_cd_y = 5,
    cor_cd_wid = 8, cor_cd_lgh = 50;

    int cor_rush_skill_x = 80, cor_rush_skill_y = 500, cor_rush_skill_wid = 100,
    cor_rush_cd_x = 72, cor_rush_cd_y = 500, cor_rush_cd_lgh = 50, cor_rush_cd_wid = 8;

    hdc = BeginPaint(hwnd, &ps);

    hdcBuf = CreateCompatibleDC(hdc);
    hdcBmp = CreateCompatibleDC(hdc);
    hdcBmp2 = CreateCompatibleDC(hdc);
    hBmpBuf = CreateCompatibleBitmap(hdc, cxClient, cyClient);
    SelectObject(hdcBuf, hBmpBuf);
    GetClientRect(hwnd, &rect);
    FillRect(hdcBuf, &rect, WHITE_BRUSH);


    switch(Game_status){
    case START_GAMESTATUS:
        SelectObject(hdcBmp, hBmp_title);
        StretchBlt(hdcBuf, 0, 0, cxClient, cyClient, hdcBmp, 0, 0, 3564, 2000, SRCCOPY);

        hFont_title = CreateFont(cyClient/5, cxClient/40, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, TEXT("MyFONT"));

        SelectObject(hdcBmp, hBmp_title1);
        SelectObject(hdcBmp2, hBmp_title2);
        StretchBlt(hdcBuf, cxClient/5, 0, cxClient*3/5, cyClient/5, hdcBmp2, 0, 0, 160, 40, SRCAND);
        StretchBlt(hdcBuf, cxClient/5, 0, cxClient*3/5, cyClient/5, hdcBmp, 0, 0, 160, 40, SRCPAINT);

        SetBkMode(hdcBuf, TRANSPARENT);
        SelectObject(hdcBuf, hFont_title);
        if(game_start_selecting == 1){
            SetTextColor(hdcBuf, RGB(20, 200, 200));
        }
        TextOut(hdcBuf, cxClient*2/5.0, cyClient*2/5.0, TEXT("开始游戏"),8);
        SetTextColor(hdcBuf, RGB(20, 20, 20));

        if(game_start_selecting == 2){
            SetTextColor(hdcBuf, RGB(20, 200, 200));
        }
        TextOut(hdcBuf, cxClient*2/5.0, cyClient*3/5.0, TEXT("退出游戏"),8);
        SetTextColor(hdcBuf, RGB(20, 20, 20));
        DeleteObject(hFont_title);
        break;

    case END_GAMESTATUS:
        SelectObject(hdcBuf, hFont);
        num = wsprintf(szbuffer, "游戏结束");
        TextOut(hdcBuf, cxClient/2 - num/2*Font_width, 20, szbuffer, num);
        num = wsprintf(szbuffer, "按Esc返回");
        TextOut(hdcBuf, cxClient/2 - num/2*Font_width, cyClient - Font_height * 3, szbuffer, num);
        DeleteObject(hFont);
        break;

    case RUNNING_GAMESTATUS:
    case BATTLING_GAMESTATUS:
        SetBkMode(hdcBuf, TRANSPARENT);

        //加载背景
        MapPaint();
        EntityPaint();
        BulletPaint();

        SelectObject(hdcBmp2, hBmp_character2);
        StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp2, 0, 0, 25, 25, SRCAND);

        SelectObject(hdcBmp, hBmp_character);
        StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp, 0, 0, 25, 25, SRCPAINT);

        //DeleteObject(hdcBmp);
        //DeleteObject(hdcBmp2);

        SetTextColor(hdcBuf, RGB(40, 20, 50));
        SelectObject(hdcBuf, (HFONT)hFont);
        TextOut(hdcBuf, cxClient - 200, 0, szbuffer, wsprintf(szbuffer, "FPS:%d", (int)(1000.0/time_lag)));
        //TextOut(hdcBuf, 0, 40, szbuffer, wsprintf(szbuffer, "x_map:%d y_map:%d",x_map, y_map));
        //TextOut(hdcBuf, 0, Font_height, szbuffer, wsprintf(szbuffer, "%d", (int)(damage_factor*100)));

        if(cure_item_num > 0){
            SelectObject(hdcBuf, GetStockObject(NULL_BRUSH));
            Rectangle(hdcBuf, cor_curedrop_x, cor_curedrop_y,
                      cor_curedrop_x + cor_drop_wid, cor_curedrop_y + cor_drop_wid);
            SelectObject(hdcBmp, hBmp_curedrop);
            SelectObject(hdcBmp2, hBmp_curedrop2);
            StretchBlt(hdcBuf, cor_curedrop_x, cor_curedrop_y,
                       cor_drop_wid, cor_drop_wid, hdcBmp2,
                       0, 0, 50, 50, SRCAND);
            StretchBlt(hdcBuf, cor_curedrop_x, cor_curedrop_y,
                       cor_drop_wid, cor_drop_wid, hdcBmp,
                       0, 0, 50, 50, SRCPAINT);
            TextOut(hdcBuf, 450, 50, szbuffer, wsprintf(szbuffer, "%d", cure_item_num));

            if(cure_item_cnt < cure_item_cd){
                hBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdcBuf, hBrush);
                RoundRect(hdcBuf, cor_cure_cd_x, cor_cure_cd_y,
                          cor_cure_cd_x + cor_cd_wid, cor_cure_cd_y + cor_cd_lgh, 4, 4);
                DeleteObject(hBrush);

                hBrush = CreateSolidBrush(RGB(25, 25, 255));
                SelectObject(hdcBuf, hBrush);
                RoundRect(hdcBuf, cor_cure_cd_x,
                          (cor_cure_cd_y) + (cor_cd_lgh * ((cure_item_cnt)/cure_item_cd)),
                          cor_cure_cd_x + cor_cd_wid,
                          (cor_cure_cd_y + cor_cd_lgh), 4, 4);

                DeleteObject(hBrush);

            }
        }

        if(time_item_num > 0){
            SelectObject(hdcBuf, GetStockObject(NULL_BRUSH));
            Rectangle(hdcBuf, cor_timedrop_x, cor_timedrop_y,
                      cor_timedrop_x + cor_drop_wid, cor_timedrop_y + cor_drop_wid);
            //DeleteObject(hdcBuf);

            SelectObject(hdcBmp, hBmp_timedrop);
            SelectObject(hdcBmp2, hBmp_timedrop2);

            StretchBlt(hdcBuf, cor_timedrop_x, cor_timedrop_y,
                       cor_drop_wid, cor_drop_wid, hdcBmp2,
                       0, 0, 50, 50, SRCAND);
            StretchBlt(hdcBuf, cor_timedrop_x, cor_timedrop_y,
                       cor_drop_wid, cor_drop_wid, hdcBmp,
                       0, 0, 50, 50, SRCPAINT);
            TextOut(hdcBuf, 300, 50, szbuffer, wsprintf(szbuffer, "%d", time_item_num));

            if(time_item_cnt < time_item_cd){
                hBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdcBuf, hBrush);
                RoundRect(hdcBuf, cor_time_cd_x, cor_time_cd_y,
                          cor_time_cd_x + cor_cd_wid, cor_time_cd_y + cor_cd_lgh, 4, 4);
                DeleteObject(hBrush);

                hBrush = CreateSolidBrush(RGB(25, 25, 255));
                SelectObject(hdcBuf, hBrush);
                RoundRect(hdcBuf, cor_time_cd_x,
                          (cor_time_cd_y) + (cor_cd_lgh * ((time_item_cnt)/time_item_cd)),
                          cor_time_cd_x + cor_cd_wid,
                          (cor_time_cd_y + cor_cd_lgh), 4, 4);

                DeleteObject(hBrush);

            }
        }

        if(Game_buff[RUSH_BUFF]){
            SelectObject(hdcBmp, hBmp_rush_buff);
            StretchBlt(hdcBuf, cor_rush_skill_x, cor_rush_skill_y, cor_rush_skill_wid,
                       cor_rush_skill_wid, hdcBmp, 0, 0, 50, 50, SRCCOPY);

            if(rush_cd_cnt < rush_cd_time){
                SelectObject(hdcBuf, GetStockObject(WHITE_BRUSH));
                RoundRect(hdcBuf, cor_rush_cd_x, cor_rush_cd_y,
                          cor_rush_cd_x + cor_rush_cd_wid, cor_rush_cd_y + cor_rush_cd_lgh, 4, 4);

                hBrush = CreateSolidBrush(RGB(25, 25, 255));
                SelectObject(hdcBuf, hBrush);
                RoundRect(hdcBuf, cor_rush_cd_x, cor_rush_cd_y + cor_rush_cd_lgh*(rush_cd_cnt/rush_cd_time),
                          cor_rush_cd_x + cor_rush_cd_wid, cor_rush_cd_y + cor_rush_cd_lgh, 4, 4);
                DeleteObject(hBrush);
            }
        }

        GameTextPaintDown();

        MainCharcHpPaint();
        break;



    case CHOOSING_GAMESTATUS:
        for(int i = 0; i < 3; i++){
            switch(buff_choose[i]){
            case BLOOD_BUFF:
                SelectObject(hdcBmp, hBmp_blood_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case RAGE_BUFF:
                SelectObject(hdcBmp, hBmp_rage_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case RUSH_BUFF:
                SelectObject(hdcBmp, hBmp_rush_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case DAMAGEUP_BUFF:
                SelectObject(hdcBmp, hBmp_damageup_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case BULLETSPEEDUP_BUFF:
                SelectObject(hdcBmp, hBmp_bulletspeedup_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;


            case FIRINGRATEUP_BUFF:
                SelectObject(hdcBmp, hBmp_firingrateup_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case PIERCEBULLET_BUFF:
                SelectObject(hdcBmp, hBmp_piercebullet_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case MULTIBULLET_BUFF:
                SelectObject(hdcBmp, hBmp_multibullet_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;

            case DEFENSEUP_BUFF:
                SelectObject(hdcBmp, hBmp_defenseup_buff);
                StretchBlt(hdcBuf, cor_buff_x + cor_buff_wid*i*2, cor_buff_y, cor_buff_wid,
                           cor_buff_wid, hdcBmp, 0, 0, 25, 25, SRCCOPY);
                break;
            }
        }

        SelectObject(hdcBuf, hFont);
        num = wsprintf(szbuffer, "请选择一项增益");
        TextOut(hdcBuf, cxClient/2 - num * Font_width / 2, cyClient/2 - 50, szbuffer, num);
        switch(chosen_buff){
        case BLOOD_BUFF:
            num = wsprintf(szbuffer, "获取造成伤害50的生命值");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("获取造成伤害5%的生命值"), num);
            break;

        case RAGE_BUFF:
            num = wsprintf(szbuffer, "生命值每减少100，攻击力和移速加100");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("生命值每减少10%，攻击力和移速加10%"), num);
            break;

        case RUSH_BUFF:
            num = wsprintf(szbuffer, "按SHIFT键冲刺");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, szbuffer, num);
            break;

        case DAMAGEUP_BUFF:
            num = wsprintf(szbuffer, "你的子弹伤害加300");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("你的子弹伤害加30%"), num);
            break;

        case BULLETSPEEDUP_BUFF:
            num = wsprintf(szbuffer, "子弹速度增加300");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("子弹速度增加30%"), num);
            break;

        case FIRINGRATEUP_BUFF:
            num = wsprintf(szbuffer, "开火速度增加300");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("开火速度增加30%"), num);
            break;

        case PIERCEBULLET_BUFF:
            num = wsprintf(szbuffer, "子弹伤害减少300，可以穿透敌人");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("子弹伤害减少30%，可以穿透敌人"), num);
            break;

        case MULTIBULLET_BUFF:
            num = wsprintf(szbuffer, "子弹伤害减少400，每次齐射三发子弹");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, TEXT("子弹伤害减少40%，每次齐射三发子弹"), num);
            break;

        case DEFENSEUP_BUFF:
            num = wsprintf(szbuffer, "减5受到的伤害");
            TextOut(hdcBuf, cxClient/2 - num/2 * Font_width, cyClient - 100, szbuffer, num);
            break;
        }
        break;



    case PAUSE_GAMESTATUS:
        SelectObject(hdcBuf, hFont);
        for(int i = 1; i <= MAX_BUFF; i++){
            if(Game_buff[i]){
                switch(i){
                case BLOOD_BUFF:
                    num = wsprintf(szbuffer, "获取造成伤害50的生命值");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("获取造成伤害5%的生命值"), num);
                    break;

                case RAGE_BUFF:
                    num = wsprintf(szbuffer, "生命值每减少100，攻击力和移速加100");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("生命值每减少10%，攻击力和移速加10%"), num);
                    break;

                case RUSH_BUFF:
                    num = wsprintf(szbuffer, "按SHIFT键冲刺");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, szbuffer, num);
                    break;

                case DAMAGEUP_BUFF:
                    num = wsprintf(szbuffer, "你的子弹伤害加300");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("你的子弹伤害加30%"), num);
                    break;

                case BULLETSPEEDUP_BUFF:
                    num = wsprintf(szbuffer, "子弹速度增加300");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("子弹速度增加30%"), num);
                    break;

                case FIRINGRATEUP_BUFF:
                    num = wsprintf(szbuffer, "开火速度增加300");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("开火速度增加30%"), num);
                    break;

                case PIERCEBULLET_BUFF:
                    num = wsprintf(szbuffer, "子弹伤害减少300，可以穿透敌人");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("子弹伤害减少30%，可以穿透敌人"), num);
                    break;

                case MULTIBULLET_BUFF:
                    num = wsprintf(szbuffer, "子弹伤害减少400，每次齐射三发子弹");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, TEXT("子弹伤害减少40%，每次齐射三发子弹"), num);
                    break;

                case DEFENSEUP_BUFF:
                    num = wsprintf(szbuffer, "减5受到的伤害");
                    TextOut(hdcBuf, cor_text_x, cor_text_y, szbuffer, num);
                    break;
                }
                cor_text_y += cor_buff_wid;
            }
        }
        //DeleteObject(hFont);
        break;

    }


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
        bullet_insert(Ammo, bullet_init(CharcInfo.x + 10, CharcInfo.y + 10, MAIN_CHARC_BELONGS,
                    (bullet_speed*cosx(cur_x, cur_y)), (bullet_speed*sinx(cur_x, cur_y)), bullet_damage));
        if(Game_buff[MULTIBULLET_BUFF]){
        bullet_insert(Ammo, bullet_init(CharcInfo.x + 10, CharcInfo.y + 10, MAIN_CHARC_BELONGS,
                    (bullet_speed*cosx(cur_x + bullet_disperce, cur_y + bullet_disperce)),
                    (bullet_speed*sinx(cur_x + bullet_disperce, cur_y + bullet_disperce)), bullet_damage));
        bullet_insert(Ammo, bullet_init(CharcInfo.x + 10, CharcInfo.y + 10, MAIN_CHARC_BELONGS,
                    (bullet_speed*cosx(cur_x - bullet_disperce, cur_y - bullet_disperce)),
                    (bullet_speed*sinx(cur_x - bullet_disperce, cur_y - bullet_disperce)), bullet_damage));
        }
        bullet_cd_cnt = 0;
    }
}
void GameInteract(int event, int id)
{
    int Game_buff_to_choose[MAX_BUFF + 1];
    for(int i = 1; i <= MAX_BUFF + 1; i++)Game_buff_to_choose[i] = 0;
    int rnd;
    bool flag = true;
    switch(event){
    case TELEPORT_TYPE:
        if(rooms_cleared < Map_rooms_to_clear[Map_depth_now]){
            break;
        }
        Map_depth_now++;
        rooms_cleared = 0;
        CharcInfo.x = cxClient/2, CharcInfo.y = cyClient/2;
        x_map = 0, y_map = 0;
        srand((int)(time_lag * 1000 + 500)%1000);
        rnd = rand() % MAX_BUFF + 1;
        for(int i = 0; i < 3;){
            if(Game_buff[rnd] == 0 && Game_buff_to_choose[rnd] == 0){
                buff_choose[i++] = rnd;
                Game_buff_to_choose[rnd] = 1;
                //printf("buff_choose[%d] = %d\n", i-1, buff_choose[i-1]);
            }
            else{
                if(flag){
                    flag = false;
                    srand((unsigned)time(NULL));
                }
                rnd = rand() % MAX_BUFF + 1;
            }
        }
        Game_status = CHOOSING_GAMESTATUS;
        break;

    case CUREDROP_TYPE:
        Entity_delete(Global_entity, id);
        cure_item_num++;
        break;


    case TIMEDROP_TYPE:
        Entity_delete(Global_entity, id);
        time_item_num++;
        break;
    };
}
void GameTextPaintDown()
{
    char szbuffer[100];
    int num;
    switch(Game_text_now){
    case TELEPORT_TEXT:
        num = wsprintf(szbuffer, "按空格键进入下一关");
        TextOut(hdcBuf, cxClient/2 - num/2*18, cyClient - 80, szbuffer, num);
        break;

    case CUREDROP_TYPE:
    case TIMEDROP_TYPE:
        num = wsprintf(szbuffer, "按空格键捡起");
        TextOut(hdcBuf, cxClient/2 - num/2*18, cyClient - 80, szbuffer, num);
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
            switch(Game_map[Map_depth_now][i][j].types){
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
    float factor = 1.0, blood_sucked;
    if(bullet_cd_cnt < bullet_cd){
        bullet_cd_cnt += time_lag*factor;
    }
    for(int i = 0; i < Ammo.p; i++){
        if(Ammo.b[i].belongs != MAIN_CHARC_BELONGS){
            factor = time_factor;
        }
        else{
            factor = 1.0;
        }

        if(MapEdgeDet(Map_depth_now, Ammo.b[i].x + Ammo.b[i].vx*time_lag*factor, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 10)){
            bullet_delete(Ammo, i);
        }

        Col_id = GlobalCollisionDet(Global_entity, Ammo.b[i].x + Ammo.b[i].vx*time_lag*factor, Ammo.b[i].y + Ammo.b[i].vy*time_lag, 15, -1, &ret_id);
        switch(Col_id){
        case NOCOLLISION_TYPE:
            Ammo.b[i].x += (Ammo.b[i].vx*time_lag*factor);
            Ammo.b[i].y += (Ammo.b[i].vy*time_lag*factor);
            break;

        case MAIN_CHARC_TYPE:
            if(Ammo.b[i].belongs == MONSTER_BELONGS && rushing_status == 0){
                CharcInfo.hp_now -= ((Ammo.b[i].damage - 5 * Game_buff[DEFENSEUP_BUFF]) > 0 ?
                                     (Ammo.b[i].damage - 5 * Game_buff[DEFENSEUP_BUFF]) : 0);
                bullet_delete(Ammo, i);
                if(CharcInfo.hp_now <= 0)Game_status = END_GAMESTATUS;
                return;
            }
            else{
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag*factor);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag*factor);
            }
            break;

        case OBSTACLE_TYPE:
            Global_entity.b[ret_id]->hp_now -= Ammo.b[i].damage;
            bullet_delete(Ammo, i);
            break;

        case MONSTER_TYPE:
        case MONSTER2_TYPE:
        case BOSS_MONSTER_TYPE:
            if(Ammo.b[i].belongs == MAIN_CHARC_BELONGS){
                if(Ammo.b[i].pre_id != ret_id){
                    Global_entity.b[ret_id]->hp_now -= (Ammo.b[i].damage * damage_factor);
                    Ammo.b[i].pre_id = ret_id;
                    if(Game_buff[BLOOD_BUFF]){
                        blood_sucked = (Ammo.b[i].damage * damage_factor * bloodsucking_rate);
                        if(CharcInfo.hp_now + blood_sucked <= CharcInfo.hp_max){
                            CharcInfo.hp_now += blood_sucked;
                        }
                    }
                }

                if(Game_buff[PIERCEBULLET_BUFF]){
                    Ammo.b[i].x += (Ammo.b[i].vx*time_lag*factor);
                    Ammo.b[i].y += (Ammo.b[i].vy*time_lag*factor);
                }
                else{
                    bullet_delete(Ammo, i);
                }


                if(Global_entity.b[ret_id]->movemode == MOVEMODE_SLEEP){
                    Global_entity.b[ret_id]->movemode = MOVEMODE_SHIFT;
                }
            }
            else{
                Ammo.b[i].x += (Ammo.b[i].vx*time_lag*factor);
                Ammo.b[i].y += (Ammo.b[i].vy*time_lag*factor);
            }
            break;

        default:
            Ammo.b[i].x += (Ammo.b[i].vx*time_lag*factor);
            Ammo.b[i].y += (Ammo.b[i].vy*time_lag*factor);
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
        //*
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
        case BOSS_MONSTER_TYPE:
            SelectObject(hdcBmp, hBmp_monster);
            SelectObject(hdcBmp2, hBmp_monster2);
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

        case TELEPORT_TYPE:
            SelectObject(hdcBmp, hBmp_teleport);
            SelectObject(hdcBmp2, hBmp_teleport2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 75, 75, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 75, 75, SRCPAINT);
            break;

        case TIMEDROP_TYPE:
            SelectObject(hdcBmp, hBmp_timedrop);
            SelectObject(hdcBmp2, hBmp_timedrop2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 50, 50, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 50, 50, SRCPAINT);
            break;

        case CUREDROP_TYPE:
            SelectObject(hdcBmp, hBmp_curedrop);
            SelectObject(hdcBmp2, hBmp_curedrop2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 50, 50, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 50, 50, SRCPAINT);
            break;

        case OBSTACLE_TYPE:
            //printf("obstacle\n");
            SelectObject(hdcBmp, hBmp_box);
            SelectObject(hdcBmp2, hBmp_box2);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp2,
                       0, 0, 25, 25, SRCAND);
            StretchBlt(hdcBuf, Global_entity.b[i]->x - CharcInfo.x + cxClient/2,
                       Global_entity.b[i]->y - CharcInfo.y + cyClient/2,
                       Global_entity.b[i]->width, Global_entity.b[i]->width, hdcBmp,
                       0, 0, 25, 25, SRCPAINT);
            break;
        }
    }
}
void EntityUpdate()
{
    Monster_clear = true;
    for(int i = 0; i < Global_entity.p; i++){
        switch(Global_entity.b[i]->types)
        {
        case MONSTER_TYPE:
            if(Monster_clear){//检测是否还有怪物
                Monster_clear = false;
            }

            if(Global_entity.b[i]->hp_now < 0){
                srand((int)(time_lag * 1000) % 100);
                if(rand() % 10 == 2){//掉落治疗药水
                    Entity_insert(Global_entity, Entity_Init_P(25, Global_entity.b[i]->x, Global_entity.b[i]->y, CUREDROP_TYPE, 100));
                }
                else if(rand() % 15 == 3){//掉落时间宝石
                    Entity_insert(Global_entity, Entity_Init_P(25, Global_entity.b[i]->x, Global_entity.b[i]->y, TIMEDROP_TYPE, 100));
                }
                Entity_delete(Global_entity, i);//删除操作
            }
            else{
                MonsterMoveType1(*Global_entity.b[i], CharcInfo, time_lag*time_factor);
            }
            break;

        case MONSTER2_TYPE:
            if(Monster_clear){//检测是否还有怪物
                Monster_clear = false;
            }

            if(Global_entity.b[i]->hp_now < 0){
                srand((int)(time_lag * 1000) % 100);
                if(rand() % cure_drop_prob == 0){//掉落治疗药水
                    Entity_insert(Global_entity, Entity_Init_P(25, Global_entity.b[i]->x, Global_entity.b[i]->y, CUREDROP_TYPE, 100));
                }
                else if(rand() % time_drop_prob == 0){//掉落时间宝石
                    Entity_insert(Global_entity, Entity_Init_P(25, Global_entity.b[i]->x, Global_entity.b[i]->y, TIMEDROP_TYPE, 100));
                }
                Entity_delete(Global_entity, i);//删除操作
            }
            else{
                MonsterMoveType2(*Global_entity.b[i], CharcInfo, time_lag*time_factor);
            }
            break;

        case BOSS_MONSTER_TYPE:
            if(Monster_clear){//检测是否还有怪物
                Monster_clear = false;
            }
            if(Global_entity.b[i]->hp_now < 0){
                Entity_delete(Global_entity, i);//删除操作
            }
            else{
                MonsterMoveType3(*Global_entity.b[i], CharcInfo, time_lag*time_factor);
            }
            break;

        case OBSTACLE_TYPE:
            if(Global_entity.b[i]->hp_now <= 0){
                Entity_delete(Global_entity, i);//删除操作
            }
        }
    }
}
void EntityInit(float x, float y, int types)
{
    Entity_insert(Global_entity, Entity_Init_P(Monster1_wid, x, y, types, 100));
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
    RoundRect(hdcBuf, 10, 20, 140, 30, 5, 5);
    DeleteObject(hBrush);

    hBrush = CreateSolidBrush(RGB(244, 40, 20));
    SelectObject(hdcBuf, hBrush);
    RoundRect(hdcBuf, 10, 20, 10 + (130)*((float)CharcInfo.hp_now/CharcInfo.hp_max), 30, 5, 5);
    DeleteObject(hBrush);

    char szbuffer[100];
    TextOut(hdcBuf, 145, 20, szbuffer, wsprintf(szbuffer, "%d/%d", (int)CharcInfo.hp_now, (int)CharcInfo.hp_max));
}
//-----

//游戏道具函数定义
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

    hwnd = CreateWindow (szAppName, TEXT ("PixelKnight"),
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

    case WM_PAINT:
        GamePaint(hwnd);
        return 0;

    case WM_DESTROY:
        GameQuit();
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
