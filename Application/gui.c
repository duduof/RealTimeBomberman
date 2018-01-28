/*
************************************************************************************************************************
*
*                                                  BIBLIOTECA GRAFICA
*
* Arquivo    : GUI.H
* Autor      : Eduardo Luz
* Versao	 : V0.01
* Data		 : 01/06/2013
*
* Baseado em : http://www.winprog.org/tutorial/simple_window.html
*
* Para mais sobre Animacao ver : http://www.winprog.org/tutorial/animation.html
*
* Para mais sobre textos e fontes : http://www.winprog.org/tutorial/fonts.html
*
* Para mais sobre botoes : http://stackoverflow.com/questions/2522029/how-to-handle-click-event-in-win32-api
************************************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                          APPLICATION CODE
*
*                                          Microsoft Windows
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JBL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  "gui.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

//HWND hwnd;
//HDC hdc;
//MSG Msg;


/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//void GUI_DisplayError(DWORD dw, const char* szFuncName);
//int InitGui();
//HBITMAP * PutImage(int x, int y,char *path);
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//void DisplayError(DWORD dw, const char* szFuncName);
//void PutPixel(int x, int y, COLORREF color);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



// BIBLIOTECA Grafica ------

int GUI_Init_Default()
{
	WNDCLASSEX wc;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    //wc.hInstance     = hInstance;
	wc.hInstance     = NULL;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "Trabalho BCC722";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "Trabalho BCC722",
        "The title of my window",
        WS_OVERLAPPEDWINDOW | SS_BITMAP,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, NULL, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -2;
    }

    ShowWindow(hwnd, 1);
    UpdateWindow(hwnd);

	hdc = GetDC(hwnd);

	return 1;
}

int GUI_Init(WNDPROC customWinProc)
{
	WNDCLASSEX wc;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = customWinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    //wc.hInstance     = hInstance;
	wc.hInstance     = NULL;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "Trabalho BCC722";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "Trabalho BCC722",
        "BOMBERMAN",
        WS_OVERLAPPEDWINDOW | SS_BITMAP,
        CW_USEDEFAULT, CW_USEDEFAULT, 680 + 20, 540 + 20,
        NULL, NULL, NULL, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -2;
    }

    ShowWindow(hwnd, 1);
    UpdateWindow(hwnd);

	hdc = GetDC(hwnd);

	return 1;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch(msg)
    {
		// Handles all Windows Messages 
		/*
        case WM_COMMAND:

            {
              if(((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED))
              {
                int iMID;
                iMID = LOWORD(wParam);
                switch(iMID)
                {
                  case button_id:
                      {
                       MessageBox(hwnd, (LPCTSTR)"You just pushed me!",  (LPCTSTR) "My Program!", MB_OK|MB_ICONEXCLAMATION);
                       break;
                       }
                  default:
                       break;
                }
              }
              break;
            }*/

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

		case WM_PAINT: 
			{
				// redesenha as imagens da tela
				//GUI_PutImage(0, 0, "C:\\Users\\eduardoluz\\r2d2.bmp");

			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

		case WM_LBUTTONDOWN: // The user has clicked the mouse button. Capture the mouse the window still receives mouse events when the cursor is moved out.
		//SetCapture( hWnd );
		//BeginMousing();
			return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	   case WM_MOUSEMOVE:
		   if( wParam & MK_LBUTTON )
		   {
			  // The user is moving the mouse while LMB is down. Do rotation/whatever.
			  //OnMousing();
		   }
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

	   case WM_LBUTTONUP:
	   //ReleaseCapture(); // User released mouse button, so no need to keep track of global mouse events.
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

	   case WM_CAPTURECHANGED: // The mouse capture window has changed. If the new capture window is not this window, then we need to stop rotation/whatever.
	   //if( (HWND)lParam != hWnd )
	   //{
		//   EndMousing();
	   //}
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;

}

void GUI_DisplayError(DWORD dw, const char* szFuncName)
{
     LPSTR buff;
     FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
              NULL,
              dw,
              LANG_NEUTRAL,
              (LPSTR)&buff,
              0,
              NULL);
     MessageBox(NULL, buff, szFuncName, MB_OK);
}

void GUI_PutPixel(int x, int y, COLORREF color)
{
	SetPixel(hdc, x, y, color);
}
 
HBITMAP * GUI_PutImage(int xPos,int yPos,int width,int height, const char *path)
{
	RECT prc;
	HBITMAP g_hbitmap = NULL;
	// para imagem -------
	HDC memdc;
    HGDIOBJ old;
    PAINTSTRUCT ps;
    BITMAP bm;
	// ------------------

	GetWindowRect(hwnd,&prc);

	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = yPos+height;


	g_hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                                //"C:\\Users\\eduardoluz\\Koala.bmp",
								//(LPCWSTR)".\\Koala.bmp",
								//"C:\\Users\\eduardoluz\\r2d2.bmp",
								path,
                                IMAGE_BITMAP,
                                500,
                                400,
                                LR_LOADFROMFILE);
    if(!g_hbitmap) {
        GUI_DisplayError(GetLastError(), "Error loading bitmap");
		printf("Error loading bitmap");
    }

	//hdc = BeginPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "BeginPaint");
    memdc = CreateCompatibleDC(hdc);
    //DisplayError(GetLastError(), "CreateCompatibleDC");
    old = SelectObject(memdc, g_hbitmap);
   
	// DisplayError(GetLastError(), "SelectObject");
    GetObject(g_hbitmap, sizeof(bm), &bm);
    
	//DisplayError(GetLastError(), "GetObject");
     BitBlt(hdc, xPos, yPos, width, height, memdc, 0, 0, SRCCOPY);
	//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memdc, 0, 0, SRCCOPY);
    
	//DisplayError(GetLastError(), "BitBlt");
    SelectObject(memdc, old);
    //DisplayError(GetLastError(), "SelectObject");
    
	DeleteDC(memdc);
    //DisplayError(GetLastError(), "DeleteDC");
    
	//EndPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "EndPaint");

	return &g_hbitmap;

	/*
	HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, prc.right, prc.bottom);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = SelectObject(hdcMem, g_hbmMask);

    FillRect(hdcBuffer, prc, GetStockObject(WHITE_BRUSH));

    BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCAND);

    SelectObject(hdcMem, g_hbmBall);
    BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCPAINT);

    BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);

	return NULL;*/
}

HBITMAP * GUI_PutImage2(int xPos,int yPos,int width,int height, const char *path)
{
	RECT prc;
	HBITMAP g_hbitmap = NULL;
	// para imagem -------
	HDC memdc;
    HGDIOBJ old;
    PAINTSTRUCT ps;
    BITMAP bm;
	// ------------------

	GetWindowRect(hwnd,&prc);

	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = yPos+height;


	g_hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                                //"C:\\Users\\eduardoluz\\Koala.bmp",
								//(LPCWSTR)".\\Koala.bmp",
								//"C:\\Users\\eduardoluz\\r2d2.bmp",
								path,
                                IMAGE_BITMAP,
                                30,
                                30,
                                LR_LOADFROMFILE);
    if(!g_hbitmap) {
        GUI_DisplayError(GetLastError(), "Error loading bitmap");
		printf("Error loading bitmap");
    }

	//hdc = BeginPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "BeginPaint");
    memdc = CreateCompatibleDC(hdc);
    //DisplayError(GetLastError(), "CreateCompatibleDC");
    old = SelectObject(memdc, g_hbitmap);
   
	// DisplayError(GetLastError(), "SelectObject");
    GetObject(g_hbitmap, sizeof(bm), &bm);
    
	//DisplayError(GetLastError(), "GetObject");
     BitBlt(hdc, xPos, yPos, width, height, memdc, 0, 0, SRCCOPY);
	//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memdc, 0, 0, SRCCOPY);
    
	//DisplayError(GetLastError(), "BitBlt");
    SelectObject(memdc, old);
    //DisplayError(GetLastError(), "SelectObject");
    
	DeleteDC(memdc);
    //DisplayError(GetLastError(), "DeleteDC");
    
	//EndPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "EndPaint");

	return &g_hbitmap;

	/*
	HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, prc.right, prc.bottom);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = SelectObject(hdcMem, g_hbmMask);

    FillRect(hdcBuffer, prc, GetStockObject(WHITE_BRUSH));

    BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCAND);

    SelectObject(hdcMem, g_hbmBall);
    BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCPAINT);

    BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);

	return NULL;*/
}


HBITMAP * GUI_CreateImage(const char *path, int width, int height)
{
	RECT prc;
	HBITMAP g_hbitmap = NULL;
	// para imagem -------
	HDC memdc;
    HGDIOBJ old;
    PAINTSTRUCT ps;
    BITMAP bm;
	// ------------------

	GetWindowRect(hwnd,&prc);

	/*prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = yPos+height;*/


	g_hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
                                path,
                                IMAGE_BITMAP,
                                width,
                                height,
                                LR_LOADFROMFILE);
    if(!g_hbitmap) {
        GUI_DisplayError(GetLastError(), "Error loading bitmap");
		printf("Error loading bitmap");
    }

	
}

void GUI_DrawImage(HBITMAP * hbitmap, int xPos,int yPos,int width,int height)
{
	RECT prc;
	//HBITMAP g_hbitmap = NULL;
	// para imagem -------
	HDC memdc;
    HGDIOBJ old;
    PAINTSTRUCT ps;
    BITMAP bm;
	// ------------------

	GetWindowRect(hwnd,&prc);

	
	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width ;
	prc.bottom = yPos+height;
	



	//hdc = BeginPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "BeginPaint");
    memdc = CreateCompatibleDC(hdc);
	
    //DisplayError(GetLastError(), "CreateCompatibleDC");
    old = SelectObject(memdc, hbitmap);
   
	// DisplayError(GetLastError(), "SelectObject");
    GetObject(hbitmap, sizeof(bm), &bm);
    
	FillRect(hdc, &prc, (HBRUSH)(WHITE_BRUSH));
	//FillRect(hdc, &prc, (HBRUSH)(HOLLOW_BRUSH));

	//DisplayError(GetLastError(), "GetObject");
     BitBlt(hdc, xPos, yPos, width, height, memdc, 0, 0, SRCCOPY);
	//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memdc, 0, 0, SRCCOPY);
    
	//DisplayError(GetLastError(), "BitBlt");
    SelectObject(memdc, old);
    //DisplayError(GetLastError(), "SelectObject");
    
	DeleteDC(memdc);
    //DisplayError(GetLastError(), "DeleteDC");
    
	//REMOVI END PAINT
	//EndPaint(hwnd, &ps);

    //DisplayError(GetLastError(), "EndPaint");

	return;

}

/*
void GUI_DrawImage(HBITMAP * hbitmap, int xPos,int yPos,int width,int height)
{
	RECT prc;
	//HBITMAP g_hbitmap = NULL;
	// para imagem -------
	HDC memdc;
    HGDIOBJ old;
    PAINTSTRUCT ps;
    BITMAP bm;
	// ------------------

	GetWindowRect(hwnd,&prc);

	prc.left = xPos-10;
	prc.top  = yPos-10;
	prc.right = xPos + width ;
	prc.bottom = yPos+height;

	hdc = BeginPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "BeginPaint");
    memdc = CreateCompatibleDC(hdc);
    //DisplayError(GetLastError(), "CreateCompatibleDC");
    old = SelectObject(memdc, hbitmap);
   
	// DisplayError(GetLastError(), "SelectObject");
    GetObject(hbitmap, sizeof(bm), &bm);
    
	FillRect(hdc, &prc, (HBRUSH)(WHITE_BRUSH));
	//FillRect(hdc, &prc, (HBRUSH)(HOLLOW_BRUSH));

	//DisplayError(GetLastError(), "GetObject");
     BitBlt(hdc, xPos, yPos, width, height, memdc, 0, 0, SRCCOPY);
	//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memdc, 0, 0, SRCCOPY);
    
	//DisplayError(GetLastError(), "BitBlt");
    SelectObject(memdc, old);
    //DisplayError(GetLastError(), "SelectObject");
    
	DeleteDC(memdc);
    //DisplayError(GetLastError(), "DeleteDC");
    
	EndPaint(hwnd, &ps);
    //DisplayError(GetLastError(), "EndPaint");

	return;

}*/

/*
void ReDrawV2()
{
	RECT prc;
	HDC hdcBuffer;
	HBITMAP hbmBuffer ;
	HBITMAP hbmOldBuffer;
	HDC hdcMem;
	HBITMAP hbmOld;
	
	GetWindowRect(hwnd,&prc);

    hdcBuffer = CreateCompatibleDC(hdc);
	hbmBuffer = CreateCompatibleBitmap(hdc, prc.left, prc.top);
	hbmBuffer = CreateCompatibleBitmap(hdc, 0,0);
    hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    hdcMem = CreateCompatibleDC(hdc);
 //   hbmOld = SelectObject(hdcMem, image);

    FillRect(hdcBuffer, &prc, GetStockObject(WHITE_BRUSH));

	//BitBlt(hdcBuffer, prc.left, prc.top, prc.right-prc.left, prc.bottom-prc.top, hdcMem, 0, 0, SRCAND);
	BitBlt(hdcBuffer, 0, 0,800, 600, hdcMem, 0, 0, SRCAND);

    //SelectObject(hdcMem, image);
    //BitBlt(hdcBuffer, prc.left, prc.top, prc.right-prc.left, prc.bottom-prc.top, hdcMem, 0, 0, SRCPAINT);
	BitBlt(hdcBuffer, 0, 0,800, 600, hdcMem, 0, 0, SRCPAINT);

    BitBlt(hdc, 0, 0, 800, 600, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcBuffer, hbmOldBuffer);

    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}*/

void ReDrawV2(HBITMAP* image, int xPos,int yPos,int width,int height)
{
	RECT prc;
	HDC hdcBuffer;
	HBITMAP hbmBuffer ;
	HBITMAP hbmOldBuffer;
	HDC hdcMem;
	HBITMAP hbmOld;
	
	GetWindowRect(hwnd,&prc);

    hdcBuffer = CreateCompatibleDC(hdc);
	hbmBuffer = CreateCompatibleBitmap(hdc, prc.left, prc.top);
	//hbmBuffer = CreateCompatibleBitmap(hdc, 0,0);
    hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    hdcMem = CreateCompatibleDC(hdc);
    hbmOld = SelectObject(hdcMem, image);

    FillRect(hdcBuffer, &prc, GetStockObject(WHITE_BRUSH));

	//BitBlt(hdcBuffer, prc.left, prc.top, prc.right-prc.left, prc.bottom-prc.top, hdcMem, 0, 0, SRCAND);
	BitBlt(hdcBuffer, 0, 0,800, 600, hdcMem, 0, 0, SRCAND);

    SelectObject(hdcMem, image);
    //BitBlt(hdcBuffer, prc.left, prc.top, prc.right-prc.left, prc.bottom-prc.top, hdcMem, 0, 0, SRCPAINT);

	BitBlt(hdcBuffer, 0, 0,800, 600, hdcMem, 0, 0, SRCPAINT);

    BitBlt(hdc, 0, 0, 800, 600, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcBuffer, hbmOldBuffer);

    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}

void ReDraw(HBITMAP* image, int xPos,int yPos,int width,int height)
{
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, xPos+width, yPos+height);
    HBITMAP hbmOldBuffer = SelectObject(hdcBuffer, hbmBuffer);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = SelectObject(hdcMem, image);

	RECT prc;
	prc.bottom = yPos+height;
	prc.right = xPos+width;
	prc.left = xPos;
	prc.top = yPos;
		
    FillRect(hdcBuffer, &prc, GetStockObject(WHITE_BRUSH));

    BitBlt(hdcBuffer, xPos, yPos, width, height, hdcMem, 0, 0, SRCAND);

    SelectObject(hdcMem, image);
    BitBlt(hdcBuffer, xPos, yPos, width, height, hdcMem, 0, 0, SRCPAINT);

    BitBlt(hdc, 0, 0, prc.right, prc.bottom, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}

void GUI_DrawText(char * text, int fontSize, int xPos, int yPos, COLORREF textColor, COLORREF background )
{
	HFONT hf;
    long lfHeight;
    char szSize[100];
    char szTitle[] = "These are the dimensions of your client area:";
    //HFONT hfOld;

	// ESCOLHE O TAMANHO DA FONTE
    lfHeight = -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	 
     //hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
	 hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, "Arial");

    if(hf)
    {
		
		RECT prc;
		//char szSize[100];
		//char szTitle[] = "These are the dimensions of your client area:";
		
		SetBkColor(hdc, background);
		SetTextColor(hdc, textColor);

		/*
		if(g_bOpaque)
		{
			SetBkMode(hdc, OPAQUE);
		}
		else
		{
			SetBkMode(hdc, TRANSPARENT);
		}*/

		
		GetWindowRect(hwnd,&prc);

		prc.left = xPos;
		prc.top  = yPos;

		//prc.left += xPos;
		//prc.top += yPos;
		//prc.right += xPos + 200;
		//prc.bottom += yPos + 100;

		SelectObject(hdc, hf);

		DrawText(hdc, text, -1, &prc, DT_WORDBREAK);

		//wsprintf(szSize, "{%d, %d, %d, %d}", prc.left, prc.top, prc.right, prc.bottom);
		//DrawText(hdc, szSize, -1, &prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        DeleteObject(hf);
       
    }
    else
    {
        MessageBox(hwnd, "Font creation failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
    } 

	
}

//#define IDC_MAIN_EDIT	101
// O ID do edit deve ser 'unico e deve-se manter o ID rastrado.
// Deve-se pegar um ponteiro para o edit pelo ID
void GUI_CreateEditLine(int xPos, int yPos, int width, int editID)
{
	HWND hEdit;
	RECT prc;

	//hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    //    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
    //    0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
        WS_CHILD | WS_VISIBLE , 
        0, 0, 100, 100, hwnd, (HMENU)editID, GetModuleHandle(NULL), NULL);
    if(hEdit == NULL)
        MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

	GetWindowRect(hwnd,&prc);

	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = yPos+25;

    //hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
    SetWindowPos(hEdit, NULL, prc.left, prc.top, width, 100, SWP_NOZORDER);

}

HWND GUI_GetEditLine(int editID)
{
	return GetDlgItem(hwnd, editID);
}

char * GUI_GetTextEditLine(int editId)
{
	int len;
	
	//SetDlgItemText(hwnd, 1, "ttesteLE"); // para depuracao de teste

	len = GetWindowTextLength(GetDlgItem(hwnd, editId));
    if(len > 0)
    {
        int i;
        char* buf;

        buf = (char*)GlobalAlloc(GPTR, len + 1);
        GetDlgItemText(hwnd, editId, buf, len + 1);

        //... do stuff with text ...
		//printf("%s", buf);

        //GlobalFree((HANDLE)buf);

		return buf;
    }

	return NULL;
}

void GUI_SetTextEditLine(int editID, char * text)
{
	SetDlgItemText(hwnd, editID, text); 
}


void GUI_CreateLabel(int xPos, int yPos, int width, int height, char * text, int labelID)
{
	HWND hlabel;
	RECT prc;

	hlabel = CreateWindowEx(NULL,"Static",text, WS_CHILD|WS_VISIBLE,
                    10,25,300,20,hwnd,(HMENU)labelID,NULL,NULL);

    if(hlabel == NULL)
        MessageBox(hwnd, "Could not create Label box.", "Error", MB_OK | MB_ICONERROR);

	GetWindowRect(hwnd,&prc);

	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = 25;

    //hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
    SetWindowPos(hlabel, NULL, prc.left, prc.top, width, height, SWP_NOZORDER);
}

HWND GUI_GetLabel(int labelID)
{
	return GetDlgItem(hwnd, labelID);
}

char * GUI_GetTextLabel(int labelID)
{
	int len;
	
	//SetDlgItemText(hwnd, 1, "ttesteLE"); // para depuracao de teste

	len = GetWindowTextLength(GetDlgItem(hwnd, labelID));
    if(len > 0)
    {
        int i;
        char* buf;

        buf = (char*)GlobalAlloc(GPTR, len + 1);
        GetDlgItemText(hwnd, labelID, buf, len + 1);

        //... do stuff with text ...
		//printf("%s", buf);

        //GlobalFree((HANDLE)buf);

		return buf;
    }

	return NULL;
}

void GUI_SetTextLabel(int labelID, char * text)
{
	SetDlgItemText(hwnd, labelID, text); 
}

void GUI_CreateButton(int xPos, int yPos, int width, int height, char * text, int buttonID)
{
	HWND button;
	RECT prc;

	//create button
    button = CreateWindowEx(WS_EX_CLIENTEDGE,"Button",text,WS_CHILD|WS_VISIBLE,
                    10,100,100,25,hwnd,(HMENU)buttonID,NULL,NULL);

    if(button == NULL)
        MessageBox(hwnd, "Could not create Button.", "Error", MB_OK | MB_ICONERROR);

	GetWindowRect(hwnd,&prc);

	prc.left = xPos;
	prc.top  = yPos;
	prc.right = xPos + width;
	prc.bottom = 25;

    //hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
    SetWindowPos(button, NULL, prc.left, prc.top, width, height, SWP_NOZORDER);
}

HWND GUI_GetButton(int buttonID)
{
	return GetDlgItem(hwnd, buttonID);
}

