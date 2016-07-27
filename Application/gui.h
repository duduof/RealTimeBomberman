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

#ifndef GUI_H
#define GUI_H

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
*                                       GLOBAL VARIABLES - Handle para as janelas/filas msg
*********************************************************************************************************
*/
HWND hwnd;
HDC hdc;
MSG Msg;

/*
*********************************************************************************************************
*                                       Prototipo das funcoes
*********************************************************************************************************
*/

// Rotinas de inicializacao
int GUI_Init_Default();	//usa funcao callback padrao
int GUI_Init(WNDPROC customWinProc);	// recebe como parametro uma funcao callback

// funcao callback padrao da biblioteca
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//exibe msg de erro em uma caixa de dialogo
void GUI_DisplayError(DWORD dw, const char* szFuncName);

// Coloca um pixel na tela
// parametro x: posicao X na tela
// parametro y: posicao Y na tela
// parametro color: cor do pixel (pode-se usar RGB(XX,YY,ZZ))
// retorno: nulo
void GUI_PutPixel(int x, int y, COLORREF color);

// Coloca uma imagem na tela
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro width: largura da imagem
// parametro height: altura da imagem
// parametro path: caminho da imagem no sistema de arquivos (ex. c:\imagem.bmp)
// retorno: ponteiro para handle da imagem
HBITMAP * GUI_PutImage(int xPos,int yPos,int width,int height,const char *path);

// Coloca uma imagem na tela
// hbitmap ponteiro para o handle do bitmap
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro width: largura da imagem
// parametro height: altura da imagem
void GUI_DrawImage(HBITMAP * hbitmap,int xPos,int yPos,int width,int height, int index);

// Coloca uma imagem na tela
// parametro width: largura da imagem
// parametro height: altura da imagem
// parametro path: caminho da imagem no sistema de arquivos (ex. c:\imagem.bmp)
// retorno: ponteiro para handle da imagem
HBITMAP * GUI_CreateImage(const char *path,int width, int height);

void ReDraw(HBITMAP* image, int xPos,int yPos,int width,int height);


// Coloca um pixel na tela
// parametro  text: texto a ser colocado na tela
// parametro fontSize: tamanho da fonte do texto
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro textColor: cor do texto (preto = RGB(0,0,0))
// parametro backColor: cor do fundo do texto (branco = RGB(255,255,255))
// retorno: nulo
void GUI_DrawText(char * text, int fontSize, int xPos, int yPos, COLORREF textColor, COLORREF background );

// Coloca um line edit na tela
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro width: Largura do line edit
// parametro editID: identificador UNICO do line edit
// retorno: nulo
void GUI_CreateEditLine(int xPos, int yPos, int width, int editID);

// retorna ponteiro para o line edit
// parametro editID: identificador UNICO do line edit
HWND GUI_GetEditLine(int editID);

// retorna conteudo de texto do line edit
// parametro editID: identificador UNICO do line edit
char * GUI_GetTextEditLine(int editId);

// altera conteudo de texto do line edit
// parametro editID: identificador UNICO do line edit
void GUI_SetTextEditLine(int editID, char * text);

// Coloca um rotulo (label) na tela
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro width: Largura do label
// parametro height: Altura do label
// parametro text: texto inicial do label
// parametro labelID: identificador UNICO do label
// retorno: nulo
void GUI_CreateLabel(int xPos, int yPos, int width, int height, char * text, int labelID);

// Retorna um handle para o label
// parametro labelID: identificador UNICO do label
HWND GUI_GetLabel(int labelID);

// retorna o texto do label
// parametro labelID: identificador UNICO do label
char * GUI_GetTextLabel(int labelID);

// altera o texto do label
// parametro labelID: identificador UNICO do label
void GUI_SetTextLabel(int labelID, char * text);

// Coloca um botao na tela
// parametro xPos: posicao X na tela
// parametro yPos: posicao Y na tela
// parametro width: Largura do botao
// parametro height: Altura do botao
// parametro text: texto do botao
// parametro labelID: identificador UNICO do botao
// retorno: nulo
void GUI_CreateButton(int xPos, int yPos, int width, int height, char * text, int buttonID);

// Retorna um handle para o botao
// parametro labelID: identificador UNICO do botao
HWND GUI_GetButton(int buttonID);


#endif
