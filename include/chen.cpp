#include "le.h"
#include "chen.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

float Monster_rush_cnt = 0, Monster_rush_time_cnt = 0;
static bool is_valid = true;

extern EntityAdt Global_entity;
extern int cxClient, cyClient;
extern float Monster1_steps, Monster1_movetime, Monster1_direction;
static const float Pi = 2*acos(0.0);
extern float Alert_range_squared, Monster2_damage;
extern myadt_bullet Ammo;
extern int Map_depth_now;

extern float Monster1_bullet_movetime;
extern float Monster1_bullet_speed;

void MonsterMoveType1(Charc_Info &Monster, Charc_Info main_char, float time_lag)//普通远程怪物行为
{
    float x = Monster.vx * time_lag, y = Monster.vy * time_lag;
    if(Monster.movemode != MOVEMODE_SLEEP){

        if(Monster.cnt < Monster1_movetime){

            Monster.cnt += time_lag;
            switch(Monster.movemode){

              case MOVEMODE_SHIFT://移动
                  if(!GlobalCollisionDet(Global_entity, Monster.x + x,
                                       Monster.y + y, Monster.width, Monster.id, NULL)
                        && !MapEdgeDet(Map_depth_now, Monster.x + x, Monster.y + y, Monster.width)){

                    Monster.x += x;
                    Monster.y += y;
                }

                break;
            case MOVEMODE_STATIC://不动
                break;
            }
            if(Monster.bullet_cnt > Monster1_bullet_movetime){

                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                cos_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_bullet_speed,
                                                sin_x(Monster.x, Monster.y, main_char.x, main_char.y) * Monster1_bullet_speed,
                                                10));
                Monster.bullet_cnt = 0;
            }
            else{

                Monster.bullet_cnt += time_lag;
            }
        }
        else{
            Monster.cnt = 0;
            Monster.movemode ^= 1;//位与操作 1、0来回切换
            srand((int)(time_lag*1000));
            Monster1_direction = (rand()%1000)/500.0*Pi;
            //printf("NewDIr=%f\n", Monster1_direction);
            Monster.vx = cos(Monster1_direction) * Monster1_steps;
            Monster.vy = sin(Monster1_direction) * Monster1_steps;

            /*
            如果两向量数量积小于0，则夹角大于90°，此时翻转速度矢量
            */
            if((cos_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * cos(Monster1_direction) +
                    sin_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * sin(Monster1_direction)) < 0){

                Monster.vx *= -1;
                Monster.vy *= -1;
            }
        }
    }
    else{
        if((Monster.x - main_char.x) * (Monster.x - main_char.x) +
                (Monster.y - main_char.y) * (Monster.y - main_char.y) < Alert_range_squared)
            Monster.movemode = MOVEMODE_SHIFT;
    }

}

void MonsterMoveType2(Charc_Info &Monster, Charc_Info &main_char, float time_lag)//普通近战怪物行为
{
    float Monster_rush_cd, Monster_rush_time;
    if(Monster.types == BOSS_MONSTER_TYPE){
        Monster_rush_cd = 750;
        Monster_rush_time = 150;
    }
    else{
        Monster_rush_cd = 1500;
        Monster_rush_time = 150;
    }
    float x = Monster.vx * time_lag, y = Monster.vy * time_lag;
    int type = GlobalCollisionDet(Global_entity, Monster.x + x, Monster.y + y, Monster.width, Monster.id, NULL);
    static int rnd_type = 1;
     if(Monster.movemode != MOVEMODE_SLEEP){
        if(Monster_rush_cnt >= Monster_rush_cd){
            if(Monster_rush_time_cnt < Monster_rush_time){
                switch(type){
                    case MAIN_CHARC_TYPE:
                        printf("main %d\n", is_valid);
                        if(is_valid){
                            main_char.hp_now -= Monster2_damage;
                            is_valid = false;
                        }
                        Monster.x += x;
                        Monster.y += y;
                        break;

                    case MONSTER2_TYPE:
                        break;

                    default:
                        if(!MapEdgeDet(Map_depth_now, Monster.x + x, Monster.y + y, Monster.width)){
                            Monster.x += x;
                            Monster.y += y;
                        }
                }
                Monster_rush_time_cnt += time_lag;
            }
            else if(Monster_rush_time_cnt >= Monster_rush_time){
                    /*
                if(rnd_type){
                    rnd_type = 0;
                    srand((int)(time_lag*1000) % 500);
                }
                else{
                    rnd_type = 1;
                    srand((unsigned)time(NULL));
                }
                Monster_rush_cd = (rand()%150)*10 + Monster_rush_cd;
                */
                Monster_rush_cnt = 0;
                Monster_rush_time_cnt = 0;
                is_valid = true;
            }
        }
        else{
             Monster.vx = cos_x(Monster.x, Monster.y, main_char.x, main_char.y) * 3;
             Monster.vy = sin_x(Monster.x, Monster.y, main_char.x, main_char.y) * 3;
             Monster_rush_cnt += time_lag;
        }
     }
     else{
        if((Monster.x - main_char.x) * (Monster.x - main_char.x) +
            (Monster.y - main_char.y) * (Monster.y - main_char.y) < Alert_range_squared){
                Monster.movemode = MOVEMODE_SHIFT;
        }

     }
}

void EntiInit1(float x, float y)
{
    Entity_insert(Global_entity, Entity_Init_P(30, x, y, LONGRANG_MONSTER_TYPE, 100));
    Global_entity.b[Global_entity.p - 1]->cnt = 0;
    Global_entity.b[Global_entity.p - 1]->movemode = MOVEMODE_SLEEP;
    Global_entity.b[Global_entity.p - 1]->bullet_cnt = 0;
}

void Bossstage1(Charc_Info &Monster, Charc_Info main_char, float time_lag)//boss第一阶段行为模式
{
    float static m = 0;
    float cnt = 250;
    if(Monster.movemode != MOVEMODE_SLEEP){

        if(Monster.cnt > Monster1_movetime && Monster.boss_summon > 0){


            switch(Monster.movemode){

            case MOVEMODE_SHIFT://召唤小怪，次数为boss_summon
                EntiInit1(Monster.x + 100, Monster.y - 100);
                EntiInit1(Monster.x + 100, Monster.y + 100);
                EntiInit1(Monster.x - 100, Monster.y + 100);
                EntiInit1(Monster.x - 100, Monster.y - 100);
                Monster.boss_summon -= 1;
                Monster.cnt = 0;
                break;
            case MOVEMODE_STATIC://不动
                break;
            }
            Monster.cnt = 0;
        }
        else{

            Monster.cnt += time_lag;
            Monster.movemode ^= 1;

            if(Monster.bullet_cnt >= cnt){


                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(m) * Monster1_bullet_speed,
                                                cos(m) * Monster1_bullet_speed,
                                                20));

                m -= 0.1;
                Monster.bullet_cnt = 0;
            }
            else{

                Monster.bullet_cnt += time_lag;
            }
        }
    }
    else{

            if((Monster.x - main_char.x) * (Monster.x - main_char.x) +
                    (Monster.y - main_char.y) * (Monster.y - main_char.y) < Alert_range_squared)
                Monster.movemode = MOVEMODE_SHIFT;
    }

}

void Bossstage2(Charc_Info &Monster, Charc_Info main_char, float time_lag)//boss第二阶段行为模式
{
    static float m = 1.5 * 3.14, d = 2.25 * 3.14, n = 2.75 * 3.14;//调好角度，使得子出弹从90，210，330度方向射出
    static float m1 = 2.5 * 3.14, d1 = 1.25 * 3.14, n1 = 1.75 * 3.14;
    float cnt = 300;//使得子弹先后射出，而不是同时射出
    float x = Monster.vx * time_lag, y = Monster.vy * time_lag;
    if(Monster.cnt > Monster1_movetime && Monster.cnt < 4 * Monster1_movetime){
            if(Monster.bullet_cnt >= cnt && ((int)((m1 / 3.14) - 1.5) % 2) == 0){//每一条子弹都绕圆周射出


                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(m) * Monster1_bullet_speed,
                                                cos(m) * Monster1_bullet_speed,
                                                20));

                m -= 0.2;
                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(n) * Monster1_bullet_speed,
                                                cos(n) * Monster1_bullet_speed,
                                                20));

                n -= 0.2;
                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(d) * Monster1_bullet_speed,
                                                cos(d) * Monster1_bullet_speed,
                                                20));

                d -= 0.2;
                Monster.bullet_cnt = 0;
        }

        else{

            Monster.bullet_cnt += time_lag;
        }
        /*if(Monster.bullet_cnt >= Monster1_bullet_movetime){
            bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin_x(Ammo.b->x, Ammo.b->y, main_char.x, main_char.y)* Monster1_bullet_speed,
                                                cos_x(Ammo.b->x, Ammo.b->y, main_char.x, main_char.y)* Monster1_bullet_speed,
                                                20));
            Monster.bullet_cnt = 0;
        }
        else{
            Monster.bullet_cnt += time_lag;
        }*/
      if(Monster.bullet_cnt >= cnt && ((int)((m / 3.14) - 0.5) % 2) == 0){//每一条子弹都绕圆周射出


                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(m1) * Monster1_bullet_speed,
                                                cos(m1) * Monster1_bullet_speed,
                                                20));

                m1 += 0.2;
                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(n1) * Monster1_bullet_speed,
                                                cos(n1) * Monster1_bullet_speed,
                                                20));

                n1 += 0.2;
                bullet_insert(Ammo, bullet_init(Monster.x, Monster.y, MONSTER_BELONGS,
                                                sin(d1) * Monster1_bullet_speed,
                                                cos(d1) * Monster1_bullet_speed,
                                                20));

                d1 += 0.2;
                Monster.bullet_cnt = 0;
        }

        else{

            Monster.bullet_cnt += time_lag;
        }
        Monster.cnt += time_lag;
    }
    else if(Monster.cnt <= Monster1_movetime){//boss移动
            Monster.cnt += time_lag;
            srand((int)(time_lag*1000));
            Monster1_direction = (rand()%1000)/500.0*Pi;
            Monster.vx = cos(Monster1_direction) * Monster1_steps;
            Monster.vy = sin(Monster1_direction) * Monster1_steps;
            /*
            如果两向量数量积小于0，则夹角大于90°，此时翻转速度矢量
            */
            if((cos_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * cos(Monster1_direction) +
                    sin_x(Monster.x, Monster.y, main_char.x, main_char.y)
                    * sin(Monster1_direction)) < 0){

                Monster.vx *= -1;
                Monster.vy *= -1;
            }
            if(!GlobalCollisionDet(Global_entity, Monster.x + x,
                                       Monster.y + y, Monster.width, Monster.id, NULL)
                        && !MapEdgeDet(Map_depth_now, Monster.x + x, Monster.y + y, Monster.width)){

                Monster.x += x;
                Monster.y += y;
            }
    }
    else if(Monster.cnt >= 4 * Monster1_movetime){
        Monster.cnt = 0;
    }

}

void MonsterMoveType3(Charc_Info &Monster, Charc_Info &main_char, float time_lag)//boss行为
{
    int Hp = (Monster.hp_now * 100) / Monster.hp_max;//获取当前Boss血量百分比
    if(Hp > 70){//大于70%，第一阶段

        Bossstage1(Monster, main_char, time_lag);
    }
    if(Hp <= 70 && Hp >30){//大于30%，小于70%第二阶段

        Bossstage2(Monster, main_char, time_lag);
    }
    if(Hp <= 30){//小于30%第三阶段
            MonsterMoveType2(Monster, main_char, time_lag);
            /*
        //printf("%f %f\n",Monster.cnt, Monster1_movetime);
        if(Monster.cnt < 2 * Monster1_movetime && Monster.cnt > Monster1_movetime){

            //printf("YES\n");
            Monster.cnt += time_lag;
        }
        else if(Monster.cnt <= Monster1_movetime){

            //printf("NO\n");
            Bossstage2(Monster, main_char, time_lag);
            Monster.cnt += time_lag;
        }
        else if(Monster.cnt >= 2 * Monster1_movetime){
            Monster.cnt = 0;
        }
        */
    }
}


