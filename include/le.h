#ifndef LE_H_
#define LE_H_

#define OBSTACLE_TYPE 1
#define OBSTACLE_UNBREAKABLE_TYPE 2
#define MONSTER_TYPE 3
#define MAIN_CHARC_TYPE 4

typedef struct{
    int width = 50;
    float x, y;
    float vx,vy;
    int hp_now, hp_max;
    int types,id;
    float cnt;
    int movemode;
    float bullet_cnt;
}Charc_Info,*PCharc_Info;

typedef struct{
    PCharc_Info *b;//���鴢��
    int p;//ջ��
    int num;//�ṹ�������С
}EntityAdt;

Charc_Info Entity_Init(int wid, float x, float y, int types, int hp_max);
PCharc_Info Entity_Init_P(int wid, float x, float y, int types, int hp_max);
bool EntityAry_init(EntityAdt &adt,int user_num);
bool Entity_insert(EntityAdt &adt,PCharc_Info Charc_insert);//����
bool Entity_delete(EntityAdt &adt,int now);//ɾ��
void Entity_free(EntityAdt &adt);

void ChangeAcc(PCharc_Info &s,int ax,int ay);//���ٶ�

/*
2ά������ײ���

DetΪʵ����Ϣ
x,y,widΪ��Ҫ��������������
��ʵ���봫��idΪ-1
������ײʵ�������
*/
int GlobalCollisionDet(EntityAdt &Det, float x, float y, int wid, int id, int *ret_id);

#endif
/*
�¼��� 2022.9.24
��ײ���
�ٶȱ仯
*/

/*
Ҷ���� 2022.10.2
�޸�����ײ���
�����һЩע��
*/
