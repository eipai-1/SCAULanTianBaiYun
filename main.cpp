#include <windows.h>
#include "include/My_header.h"
#include "include/BulletList.h"

#define ID_TIMER_1 1

BulletList Blist_head, Bl_next;

int cxClient = 1000, cyClient = 600;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("Window");
    HWND         hwnd ;
    MSG          msg ;
    WNDCLASS     wndclass ;
    LARGE_INTEGER fre, pre, next;

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

    //DWORD tPre = GetTickCount();
    //DWORD tNow = GetTickCount();
    QueryPerformanceFrequency(&fre);
    QueryPerformanceCounter(&pre);

    BulletList_Init(Blist_head);
    BulletList_Init(Bl_next);

    while (msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)){
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
        //tNow = GetTickCount();
        QueryPerformanceCounter(&next);
        if(((next.QuadPart - pre.QuadPart)*1000/fre.QuadPart) >= 16){//设定帧率为100
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            pre = next;
        }
    }
    return msg.wParam ;
}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, hdcBmp, hdcBuf, hdcBmp2;
    PAINTSTRUCT ps;
    HBITMAP hBmpBuf;
    int cur_x, cur_y;
    static HBITMAP hBmp_character, hBmp_background, hBmp_character2, hBmp_bullet,
    hBmp_bullet2;
    static int steps = 4, bullet_cd = 5;
    static HINSTANCE hInstance;
    static Charc_Info CharcInfo;
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
        SetTimer(hwnd, ID_TIMER_1, 10, NULL);
        CharcInfo.width = 64;
        return 0;

//*
    case WM_LBUTTONDOWN:
        cur_x = LOWORD(lParam);
        cur_y = HIWORD(lParam);

/*
        if(bullet_cd == 5){//TEST FINISHIED LOOKS_OK
            bullet_cd = 0;
            BulletList new_node;
            BulletList_Init(new_node);
            new_node->x = CharcInfo.x;
            new_node->y = CharcInfo.y;

            new_node->v_x = new_node->vel * sinx(CharcInfo, cur_x, cur_y);
            new_node->v_y = new_node->vel * cosx(CharcInfo, cur_x, cur_y);
            BulletList_Insert(Blist_head, new_node);
        }
//*/
        return 0;

    case WM_RBUTTONDOWN:

        return 0;

    case WM_TIMER:
        if(GetAsyncKeyState('W'))CharcMove_y(CharcInfo, -steps);
        if(GetAsyncKeyState('A'))CharcMove_x(CharcInfo, -steps);
        if(GetAsyncKeyState('S'))CharcMove_y(CharcInfo, steps);
        if(GetAsyncKeyState('D'))CharcMove_x(CharcInfo, steps);
        if(bullet_cd < 5){
            bullet_cd++;
        }
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
//*
        {
            if((Blist_head)){
                TextOut(hdcBuf, 0, 0, TEXT("!NULL"), 5);
            }
            else
                TextOut(hdcBuf, 0, 0, TEXT("NULL"), 4);
        }
//*/
        //TextOut(hdcBuf, 0, 0, TEXT("???"), 3);

        BitBlt(hdc, 0, 0, cxClient, cyClient, hdcBuf, 0, 0, SRCCOPY);//将画布复制到屏幕上
        DeleteObject(hBmpBuf);
        DeleteDC(hdcBuf);
        DeleteDC(hdcBmp);
        DeleteDC(hdcBmp2);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        DeleteObject(hBmp_character);
        DeleteObject(hBmp_background);
        KillTimer(hwnd, ID_TIMER_1);
        PostQuitMessage (0) ;
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
