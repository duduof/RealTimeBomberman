/*
*********************************************************************************************************
*
*                                              TRABALHO PRÁTICO - BCC722
*
*                                                  JOGO BOMBERMAN
*
* Arquivo			: app.c
* Versao			: 1.1
* Aluno(s)			: Arthur Viana, Gabriel Garcia, Jéssica Soares, Rafael Alves
* Data				: 18/07/2016
* Descricao			: Aplicação de conceitos de Programação em Tempo Real no desenvolvimento 
*					  do Jogo Bomberman
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/
#include  <app_cfg.h>
#include  <os.h>
#include  <os_cfg_app.h>
// biblioteca GUI
#include "gui.h"
/* Foi realizada uma alteração na função GUI_DrawImage presente no gui.c
*  Na função citada, alterou-se o seguinte código
*		prc.left = xPos-10;
*		prc.top  = yPos-10;
*  Para
*		prc.left = xPos;
*		prc.top  = yPos;
*  Removendo-se a ateração nas posições xPos e yPos, a fim de eliminar partes brancas nas imagens
*  inseridas durante a programação deste trabalho.
*/
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
  
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/
enum GRAPHIC_OBJS{
	ID_BUTTON_1 = 0,
	ID_EDIT_LINE_1,
	ID_LABEL_1,
};
// Proporção de ampliação das imagens inseridas
#define K 1.5

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
// TAREFA INICIAL
static  OS_TCB   AppStartTaskTCB;
static  CPU_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];
// TAREFA DA BOMBA
static  OS_TCB   AppTaskBOMBATCB;
static  CPU_STK  AppTaskBOMBAStk[APP_TASK_START_STK_SIZE];
// TAREFA DO BOMBERMAN
static  OS_TCB   AppTaskBOMBERMANTCB;
static  CPU_STK  AppTaskBOMBERMANStk[APP_TASK_START_STK_SIZE];

// IMAGENS UTILIZADAS NO PROGRAMA
HBITMAP * fundo, *tijolo, *bomba, *expcentro, *exphorizontal, *expvertical, *bomberman;
int bombermanX, bombermanY, bombaX, bombaY;
BOOL insereBomba;

/*
*********************************************************************************************************
							LABIRINTOS - CODIFICAÇÃO DOS OBJETOS
0 - vazio
1 - parede
2 - tijolo
3 - inimigo 1
4 - inimigo 2
5 - inimigo 3
6 - bomba
7 - bomberman
8 - *
*********************************************************************************************************/
//Labirinto dos obstáculos
int LABIRINTO[13][17] = { 
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
   ,{ 1, 0, 0, 0, 2, 0, 2, 2, 0, 2, 2, 0, 0, 2, 0, 0, 1 }
   ,{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1 }
   ,{ 1, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1 }
   ,{ 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
   ,{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 1 }
   ,{ 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1 }
   ,{ 1, 0, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1 }
   ,{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 2, 1 }
   ,{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1 }
   ,{ 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1 }
   ,{ 1, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
   ,{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
   };

/*
*********************************************************************************************************
*                                       EXTERN VARIABLES
*********************************************************************************************************
*/
// Variáveis declaradas do módulo da GUI
extern HWND hwnd; 
extern HDC hdc;
extern MSG Msg;

/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/
#define  APP_TASK_STOP();                             { while (DEF_ON) { \
                                                            ;            \
                                                        }                \
                                                      }

#define  APP_TEST_FAULT(err_var, err_code)            { if ((err_var) != (err_code)) {   \
                                                            APP_TASK_STOP();             \
                                                        }                                \
                                                      }

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*									Declaração das Tarefas Criadas
*********************************************************************************************************
*/
static  void  App_TaskStart (void  *p_arg);
//static  void  App_Task1 (void  *p_arg);
static  void  App_TaskBOMBA (void  *p_arg);
static  void  App_TaskBOMBERMAN (void  *p_arg);
void verificaPosBomberman(int x, int y);
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
*********************************************************************************************************
*                                               main()
*
* Description : Funcao principal do programa.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
int  main (void)
{
    OS_ERR  err_os;
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

    OSInit(&err_os);                                            /* Inicializa uC/OS-III.*/
    APP_TEST_FAULT(err_os, OS_ERR_NONE);
    
	OSTaskCreate((OS_TCB     *)&AppStartTaskTCB,                /* Cria a tarefa inicial.*/
                 (CPU_CHAR   *)"App Start Task",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppStartTaskStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);
    APP_TEST_FAULT(err_os, OS_ERR_NONE);
	OSStart(&err_os);                                         /* Inicia o funcionamento do escalonador. */
    APP_TEST_FAULT(err_os, OS_ERR_NONE);
	insereBomba = FALSE;
}
/*******************************************************************************************************/

/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Exemplo de tarefa Inicial do sistema.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Exemplo de tarefa Inicial do sistema.
*
* Arguments   : p_arg       Argumento passado a 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
*********************************************************************************************************
*/
static  void  App_TaskStart (void  *p_arg)
{
	int i = 0, j = 0; // Variáveis responsáveis por percorrer toda a matriz LABIRINTO
	int erroN;
	OS_ERR  err_os;

	erroN = GUI_Init(HandleGUIEvents);	// Inicializa a interface grafica

	if(erroN < 0 ) { // se falhou para carregar a Gui, retorna.
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
	}

	//Considerando-se largura de cada coluna igual a 24 e altura igual a 20
	//CRIAÇÃO DA TELA DE FUNDO DO JOGO
	fundo = GUI_CreateImage( "fundo.bmp", 612, 390);
	GUI_DrawImage(fundo, 0, 0, 1200, 800, 1);

	//CRIAÇÃO DOS TIJOLOS
	tijolo = GUI_CreateImage( "tijolo.bmp", 36, 30);
	for (i = 0; i < 13; i++){
		for (j = 0; j < 17; j++){
			if (LABIRINTO[i][j] == 2){
				GUI_DrawImage(tijolo, 24*j*K, 20*i*K, 36, 30, 1);
			}
		}
	}

	// CRIAÇÃO DA TAREFA DA BOMBA
	OSTaskCreate((OS_TCB     *)&AppTaskBOMBATCB,               
                 (CPU_CHAR   *)"App StartBOMBA",
                 (OS_TASK_PTR ) App_TaskBOMBA,
                 (void       *) 0,
                 (OS_PRIO     ) 10,
                 (CPU_STK    *)&AppTaskBOMBAStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	// CRIAÇÃO DA TAREFA DO BOMBERMAN
	OSTaskCreate((OS_TCB     *)&AppTaskBOMBERMANTCB,                
                 (CPU_CHAR   *)"App StartBOMBERMAN",
                 (OS_TASK_PTR ) App_TaskBOMBERMAN,
                 (void       *) 0,
                 (OS_PRIO     ) 10,
                 (CPU_STK    *)&AppTaskBOMBERMANStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	// POSIÇÃO INICIAL DO BOMBERMAN
	bombermanX = 1;
	bombermanY = 1;

	printf("\n Inicio do loop de msg");
    // Loop de mensagens para interface grafica
    while (1){
		PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY, &err_os);
    }
	printf("\n fim do loop de msg");
}

static  void  App_TaskBOMBA (void  *p_arg)
{
	OS_ERR  err_os;
	//IMPORTAÇÃO DA IMAGEM DA BOMBA
	bomba = GUI_CreateImage("bomba.bmp", 36, 30);

	//IMPORTAÇÃO DAS IMAGENS RELATIVAS À EXPLOSÃO
	expcentro = GUI_CreateImage("expcentro.bmp", 36, 30);
	exphorizontal = GUI_CreateImage("exphorizontal.bmp", 36, 30);
	expvertical = GUI_CreateImage("expvertical.bmp", 36, 30);
	
	//CRIAÇÃO DA BOMBA:
	if (insereBomba){
		LABIRINTO[bombaY][bombaX] = 6;
		GUI_DrawImage(bomba, 24*bombaX*K, 20*bombaY*K, 36, 30, 1);

		OSTimeDly(3000, OS_OPT_TIME_DLY,&err_os);
		/*OSTimeDlyHMSM(0, 0, 10, 0, OS_OPT_TIME_DLY,&err_os);*/
		//CRIAÇÃO DAS EXPLOSÕES: 
		GUI_DrawImage(expcentro, 24*bombaX*K, 20*bombaY*K, 36, 30, 1);
		GUI_DrawImage(exphorizontal, 24*(bombaX-1)*K, 20*bombaY*K, 36, 30, 1);
		GUI_DrawImage(exphorizontal, 24*(bombaX+1)*K, 20*bombaY*K, 36, 30, 1);
		GUI_DrawImage(expvertical, 24*bombaX*K, 20*(bombaY-1)*K, 36, 30, 1);
		GUI_DrawImage(expvertical, 24*bombaX*K, 20*(bombaY+1)*K, 36, 30, 1);
	}
	insereBomba = FALSE;
}

static  void  App_TaskBOMBERMAN (void  *p_arg)
{
	OS_ERR  err_os;
	//while (1) {
	//IMPORTAÇÃO DA IMAGEM DO BOMBERMAN
	bomberman = GUI_CreateImage("bomberman.bmp", 36, 30);

	//CRIAÇÃO DO BOMBERMAN
	LABIRINTO[bombermanY][bombermanX] = 7;
	if (LABIRINTO[bombermanY][bombermanX] == 7){
		GUI_DrawImage(bomberman, 24*bombermanX*K, 20*bombermanY*K, 36, 30, 1);
	}
	printf("Bomberman(x,y)=(%d,%d)\n",bombermanX,bombermanY);
}		

void verificaPosBomberman(int x, int y){
	OS_ERR  err_os;

	if (LABIRINTO[y][x] == 0){
		//OSTaskDel(&AppTaskBOMBERMANTCB, &err_os);
		// Se na posição atual foi inserida uma bomba,  
		if (LABIRINTO[bombermanY][bombermanX] == 6){
			LABIRINTO[bombermanY][bombermanX] = 6;
		}
		else{
			LABIRINTO[bombermanY][bombermanX] = 0;
		}
		LABIRINTO[y][x] = 7;
		bombermanX = x;
		bombermanY = y;
		App_TaskBOMBERMAN(0);
	}
	else{
		LABIRINTO[bombermanY][bombermanX] = 7;
		//App_TaskBOMBERMAN(0);
	}
}

// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;
	int k, l;
	int x = bombermanX;
	int y = bombermanY;

	switch(msg)
    {
		case WM_KEYDOWN:
			switch (wParam){
				case VK_HOME:
					// Insert code here to process the HOME key
					break;

				case VK_END:
					// Insert code here to process the END key
					break;

				case VK_INSERT:
					// Insert code here to process the INS key
					break;

				case VK_F2:
					// Insert code here to process the F2 key
					break; 

				case VK_SPACE:
					//Inserir bomba;
					bombaX = bombermanX;
					bombaY = bombermanY;
					LABIRINTO[bombaY][bombaX] = 6;
					for (k = 0; k < 13; k++){
						for (l = 0; l < 17; l++){
							printf("%d \t", LABIRINTO[k][l]);
						}
						printf("\n");
					}
					insereBomba = TRUE;
					App_TaskBOMBA(0);
					break;

				case VK_LEFT:
					// Insert code here to process the LEFT ARROW key
					printf("Left\n");
					x--;
					printf("Bomberman(x,y)=(%d,%d)\n\n",bombermanX,bombermanY);
					//App_TaskBOMBERMAN (0);
					verificaPosBomberman(x,y);
					for (k = 0; k < 13; k++){
						for (l = 0; l < 17; l++){
							printf("%d \t", LABIRINTO[k][l]);
						}
						printf("\n");
					}
					break;

				case VK_RIGHT:
					// Insert code here to process the RIGHT ARROW key
					printf("Right\n");
					x++;
					printf("Bomberman(x,y)=(%d,%d)\n\n",bombermanX,bombermanY);
					//App_TaskBOMBERMAN (0);
					verificaPosBomberman(x,y);
					break;

				case VK_UP:
					// Insert code here to process the UP ARROW key
					printf("Up\n");
					y--;
					printf("Bomberman(x,y)=(%d,%d)\n\n",bombermanX,bombermanY);
					//App_TaskBOMBERMAN (0);
					verificaPosBomberman(x,y);
					break;

				case VK_DOWN:
					// Insert code here to process the DOWN ARROW key
					printf("Down\n");
					y++;
					printf("Bomberman(x,y)=(%d,%d)\n\n",bombermanX,bombermanY);
					//App_TaskBOMBERMAN (0);
					verificaPosBomberman(x,y);
					break;

				case VK_DELETE:
					// Insert code here to process the DELETE key
					break;
        
				default:
				  // Insert code here to process other noncharacter keystrokes
				  break;
			}
		// Handles all Windows Messages 
		case WM_COMMAND:{
			if(((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED)){
				int iMID;
				iMID = LOWORD(wParam);
				switch(iMID){
					case ID_BUTTON_1:{
						MessageBox(hwnd, (LPCTSTR)"Botao Pressionado!!",  (LPCTSTR) "Teste de Botao!", MB_OK|MB_ICONEXCLAMATION);
						break;
					}
					default:
						break;
                }
			}
			else if(HIWORD(wParam)==EN_CHANGE && LOWORD(wParam)==ID_EDIT_LINE_1){
				//text in the textbox has been modified
				//do your coding here
				MessageBox(hwnd, (LPCTSTR)"Line edit modificado!!",  (LPCTSTR) "Teste edit!", MB_OK|MB_ICONEXCLAMATION);
			}
            break;
		}

        case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
					
		case WM_PAINT:{
			// Exemplo
			for(i=0;i<500;i++)
				GUI_PutPixel(50+i,200,RGB(0,255,0));

			// redesenha as imagens da tela
			//GUI_DrawImage(img, imgXPos, imgYPos, 100, 100,1);
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
			if( wParam & MK_LBUTTON ){
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
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

        default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
    return 0;
}