
/*
*********************************************************************************************************
*
*                                              TRABALHO PRÁTICO - BCC722
*
*                                                  JOGO BOMBERMAN
*
* Arquivo			: app.c
* Versao			: 1.0
* Aluno(s)			: Diego, Eduardo, Hugo
* Data				: 
* Descricao			: *****
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

// biblioteca 
#include "gui.h"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define SIZE_TIJOLO_DEFAULT 29
  
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

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   AppStartTaskTCB;
static  CPU_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   AppTask1TCB;
static  CPU_STK  AppTask1Stk[APP_TASK_START_STK_SIZE];

// imagens usadas no programa
HBITMAP * img;
HBITMAP * img_fundo, *img_tijolo,*img_bomba, *img_bomberman, *img_explosao_centro,*img_explosao_horizoantal, *img_explosao_vertical, *img_fogo_vertical, * img_inimigo;
int imgXPos, imgYPos;


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
int LABIRINTO[13][17] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
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
static  void  App_Task1 (void  *p_arg);
void Criar_Figuras(void);
void Monta_Tijolos(void);

LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*******************************************************************************************************/

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

    OSInit(&err_os);                                            /* inicializa uC/OS-III.                                */
    APP_TEST_FAULT(err_os, OS_ERR_NONE);

    
	OSTaskCreate((OS_TCB     *)&AppStartTaskTCB,                /* Cria a tarefa inicial.                             */
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


}

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

static  void  App_Task1 (void  *p_arg)
{
	int i=0;
	OS_ERR  err_os;

}

static  void  App_TaskStart (void  *p_arg)
{
	
	int i=0;
	int erroN;
	OS_ERR  err_os;

	erroN = GUI_Init(HandleGUIEvents);	// inicializa a interface grafica

	if( erroN < 0 ) { // se falhou para carregar a Gui, retorna.
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
	}
	
	Criar_Figuras();

	GUI_DrawImage(img_fundo, 0, 0, 680, 520, 1); // Coloca o Fundo
	Monta_Tijolos();

	// cria outra tarefa
	OSTaskCreate((OS_TCB     *)&AppTask1TCB,                /* Cria a tarefa inicial.                             */
                 (CPU_CHAR   *)"App Start1",
                 (OS_TASK_PTR ) App_Task1,
                 (void       *) 0,
                 (OS_PRIO     ) 10,
                 (CPU_STK    *)&AppTask1Stk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err_os);
    APP_TEST_FAULT(err_os, OS_ERR_NONE);

    // Loop de mensagens para interface grafica
    while (1)
   		 {
			PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);

			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY, &err_os);
    }

	printf("\n fim do loop de msg");

}

void Criar_Figuras(void)
{
		img_fundo = GUI_CreateImage("fundo.bmp",612, 390);
}

void Monta_Tijolos(void)
{
		img_tijolo = GUI_CreateImage("tijolo.bmp", 36, 30);
		GUI_DrawImage(img_tijolo, (40), (40), 36, 30, 1);

}

// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch(msg)
    {
		case WM_KEYDOWN:
		  switch (wParam)
		  {
			case VK_HOME:
			  // Insert code here to process the HOME key
			  // ...
			  break;

			case VK_END:
			  // Insert code here to process the END key
			  // ...
			  break;

			case VK_INSERT:
			  // Insert code here to process the INS key
			  // ...
			  break;

			case VK_F2:
			  // Insert code here to process the F2 key
			  // ...
			  break;

			case VK_LEFT:
			  // Insert code here to process the LEFT ARROW key
			  // ...
			  break;

			case VK_RIGHT:
			  // Insert code here to process the RIGHT ARROW key
			  // ...
			  break;

			case VK_UP:
			  // Insert code here to process the UP ARROW key
			  // ...
			  break;

			case VK_DOWN:
			  // Insert code here to process the DOWN ARROW key
			  // ...
			  break;

			case VK_DELETE:
			  // Insert code here to process the DELETE key
			  // ...
			  break;
        
			default:
			  // Insert code here to process other noncharacter keystrokes
			  // ...
			  break;
		  } 

		// Handles all Windows Messages 
		
        case WM_COMMAND:

            {
              if(((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED))
              {
                int iMID;
                iMID = LOWORD(wParam);
                switch(iMID)
                {
                  case ID_BUTTON_1:
                      {
                       MessageBox(hwnd, (LPCTSTR)"Botao Pressionado!!",  (LPCTSTR) "Teste de Botao!", MB_OK|MB_ICONEXCLAMATION);
                       break;
                       }
                  default:
                       break;
                }
              }
			  else if(HIWORD(wParam)==EN_CHANGE && LOWORD(wParam)==ID_EDIT_LINE_1) 
				{
					//text in the textbox has been modified
					//do your coding here
					 MessageBox(hwnd, (LPCTSTR)"Line edit modificado!!",  (LPCTSTR) "Teste edit!", MB_OK|MB_ICONEXCLAMATION);
				}
              break;
            }

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

		
		case WM_PAINT: 
			{
			/*
				// Exemplo
				for(i=0;i<500;i++)
					GUI_PutPixel(50+i,200,RGB(0,255,0));

				// redesenha as imagens da tela
				GUI_DrawImage(img, imgXPos, imgYPos, 100, 100,1);
			*/
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
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	   break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
