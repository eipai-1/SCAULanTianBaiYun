#ifndef BULLETLIST_H_
#define BULLETLIST_H_

//��Ļ�ṹ��
typedef struct Bullet_Info{
	float x, y;
	float vx, vy;
	int belongs;
	float damage;
}Bullet_Info, *BulletList;

//
typedef struct myadt_bullet{
	Bullet_Info *b;
    int p;//ջ��
    int num;//�ṹ�������С
}myadt_bullet;

//bool����ֵ ˵����ǰ�����Ƿ�ɹ�
bool bulletary_init(myadt_bullet &adt,int user_num);//��Ļ�ṹ�����鶯̬�����ڴ�ռ� user_num�����С
Bullet_Info bullet_init(float x, float y, int belongs, float vx, float vy, float damage);//��ʼ����Ļ�ṹ�岢����
bool bullet_insert(myadt_bullet &adt,Bullet_Info bullet);//���뵯Ļ
bool bullet_delete(myadt_bullet &adt,int now);//ɾ����Ļ now�ǵ�ǰ��Ҫɾ����λ��
void bullet_free(myadt_bullet &adt);//�ͷ��ڴ�ռ�

#endif

//������ 20220919
/*
Ҷ���� 20220919
�޸���bullet_init�������޸�x��y��vx��vyΪ����ֵ
*/
