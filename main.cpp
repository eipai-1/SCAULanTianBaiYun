#include <windows.h>
#include <stdio.h>
#include "include/My_header.h"
#include "include/BulletList.h"

#define AMMO_MAX 10
#define ID_TIMER_1 1
//内部链接-仅作用于该源代码及其包含的头文件(即翻译单元)
//弹幕变量
static myadt_bullet Ammo;

//人物消息
static Charc_Info CharcInfo = {50, 1, 1, 10, 10};

//其他参数
static int bullet_cd = 5;
static int bullet_cd_cnt = bullet_cd;
static float steps = 0.2, time_lag, bullet_speed = 1.2;

//外部链接-适用于多文件程序
int cxClient = 1000, cyClient = 600;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GameMain();


void GameMain()
{
    extern myadt_bullet Ammo;
    extern Charc_Info CharcInfo;
    extern int bullet_cd;
    extern int bullet_cd_cnt;
    extern int cxClient, cyClient;
    extern float steps, time_lag;

    if(GetAsyncKeyState('W'))CharcMove_y(CharcInfo, -steps*time_lag);
    if(GetAsyncKeyState('A'))CharcMove_x(CharcInfo, -steps*time_lag);
    if(GetAsyncKeyState('S'))CharcMove_y(CharcInfo, steps*time_lag);
    if(GetAsyncKeyState('D'))CharcMove_x(CharcInfo, steps*time_lag);
    if(bullet_cd_cnt < bullet_cd){
        bullet_cd_cnt++;
    }

    for(int i = 0; i < Ammo.p; i++){
        if(Ammo.b[i].x + Ammo.b[i].vx < cxClient && Ammo.b[i].y + Ammo.b[i].vy < cyClient && Ammo.b[i].x > 0 && Ammo.b[i].y > 0){
            Ammo.b[i].x += (Ammo.b[i].vx*time_lag);
            Ammo.b[i].y += (Ammo.b[i].vy*time_lag);
        }
        else{
            bullet_delete(Ammo, i);
        }
    }
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("Window");
    HWND         hwnd ;
    MSG          msg ;
    WNDCLASS     wndclass ;
    LARGE_INTEGER fre, pre, next;
    extern float time_lag;

    wndclass.style         = 0;//注意 窗口重设大小不会重绘
    wndclass.lpfnWndProc   = WndProc;//窗口过程为WndProc
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;


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
    Sleep(10);
    QueryPerformanceCounter(&next);

    timeBeginPeriod(1);

    //游戏主循环
    while (msg.message != WM_QUIT)
    {
        Sleep(1);
        if(PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)){
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
        GameMain();
        //tNow = GetTickCount();
        time_lag = (((next.QuadPart - pre.QuadPart)*1000.0)/fre.QuadPart);
        {//固定帧率
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            pre = next;
        }
        QueryPerformanceCounter(&next);
    }
    return msg.wParam;

}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, hdcBmp, hdcBuf, hdcBmp2;
    PAINTSTRUCT ps;
    HBITMAP hBmpBuf;
    int cur_x, cur_y;
    static HBITMAP hBmp_character, hBmp_background, hBmp_character2, hBmp_bullet,
    hBmp_bullet2;
    extern float bullet_speed;
    static HINSTANCE hInstance;
    extern Charc_Info CharcInfo;
    extern myadt_bullet Ammo;
    extern int cxClient, cyClient, bullet_cd_cnt;
    RECT rect;

    switch (message)
    {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        hBmp_character = LoadBitmap(hInstance, TEXT("character"));
        hBmp_background = LoadBitmap(hInstance, TEXT("background"));
        hBmp_character2 = LoadBitmap(hInstance, TEXT("character2"));
        hBmp_bullet = LoadBitmap(hInstance, TEXT("bullet"));
        hBmp_bullet2 = LoadBitmap(hInstance, TEXT("bullet2"));
        //SetTimer(hwnd, ID_TIMER_1, 10, NULL);
        CharcInfo.width = 64;
        bulletary_init(Ammo, AMMO_MAX);
        return 0;

//*
    case WM_LBUTTONDOWN:
        cur_x = LOWORD(lParam);
        cur_y = HIWORD(lParam);
        if(bullet_cd_cnt == bullet_cd){//到cd才能发射
            //printf("shoot!\n");
            if(bullet_insert(Ammo, bullet_init(CharcInfo.x, CharcInfo.y, 0,
                        (bullet_speed*cosx(cur_x, cur_y)), (bullet_speed*sinx(cur_x, cur_y))))){
                //printf("Insert Success\n");
                //printf("sinx=%f cosx=%f\n",sinx(cur_x, cur_y), cosx(cur_x, cur_y));
            }
            else{
                //printf("Insert Failed\n");
            }
            bullet_cd_cnt = 0;
        }

        return 0;

    case WM_RBUTTONDOWN:

        return 0;

    case WM_TIMER://游戏主循环
        //GameMain(Ammo);
        /*
        {
            if(!BulletList_IsEmpty(Blist_head)){
                BulletList loop = Blist_head->next;
                while(loop){
                    if(loop->x + loop->v_x + loop->wid < cxClient && loop->x > 0
                       && loop->y + loop->v_y + loop->wid < cyClient && loop->y > 0){
                        loop->x += loop->v_x;
                        loop->y += loop->v_y;
                    }
                    loop = loop->next;
                }
            }
        }
        //*/
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        hdcBuf = CreateCompatibleDC(hdc);
        hdcBmp = CreateCompatibleDC(hdc);
        hdcBmp2 = CreateCompatibleDC(hdc);
        hBmpBuf = CreateCompatibleBitmap(hdc, cxClient, cyClient);
        SelectObject(hdcBuf, hBmpBuf);
        GetClientRect(hwnd, &rect);
        FillRect(hdcBuf, &rect, WHITE_BRUSH);

        //加载背景
        SelectObject(hdcBmp, hBmp_background);
        StretchBlt(hdcBuf, cxClient/2 - CharcInfo.x, cyClient/2 - CharcInfo.y, cxClient, cyClient, hdcBmp, 0, 0, 100, 100, SRCCOPY);

        SelectObject(hdcBmp2, hBmp_character2);
        StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp2, 0, 0, 64, 64, SRCAND);
        SelectObject(hdcBmp, hBmp_character);
        StretchBlt(hdcBuf, cxClient/2, cyClient/2, CharcInfo.width, CharcInfo.width, hdcBmp, 0, 0, 64, 64, SRCPAINT);
        SelectObject(hdcBmp2, hBmp_bullet2);
        SelectObject(hdcBmp, hBmp_bullet);
        //printf("ammo.p=%d\n",Ammo.p);
        for(int i = 0; i < Ammo.p; i++){
            //printf("%d %d\n",cxClient/2 - CharcInfo.x + Ammo.b[i].x, cyClient/2 - CharcInfo.y + Ammo.b[i].y);
            StretchBlt(hdcBuf, cxClient/2 - CharcInfo.x + Ammo.b[i].x, cyClient/2 - CharcInfo.y + Ammo.b[i].y, 15, 15, hdcBmp2, 0, 0, 15, 15, SRCAND);
            StretchBlt(hdcBuf, cxClient/2 - CharcInfo.x + Ammo.b[i].x, cyClient/2 - CharcInfo.y + Ammo.b[i].y, 15, 15, hdcBmp, 0, 0, 15, 15, SRCPAINT);
        }


        //将画布复制到屏幕
        BitBlt(hdc, 0, 0, cxClient, cyClient, hdcBuf, 0, 0, SRCCOPY);//将画布复制到屏幕上
        DeleteObject(hBmpBuf);
        DeleteDC(hdcBuf);
        DeleteDC(hdcBmp);
        DeleteDC(hdcBmp2);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        bullet_free(Ammo);
        DeleteObject(hBmp_character);
        DeleteObject(hBmp_background);
        //KillTimer(hwnd, ID_TIMER_1);
        PostQuitMessage (0) ;
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
