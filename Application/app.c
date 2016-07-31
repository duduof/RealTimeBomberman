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
//#include <stdlib.h>
//#include <sstream>
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

//TAMANHO DO LABIRINTO
#define XX 13
#define YY 17

//CODIGO DOS BICHOS
#define cod_inimigo1 3
#define cod_inimigo2 4
#define cod_inimigo3 5
#define cod_bomberman 7




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
// TAREFA Desenhar
static  OS_TCB   AppTaskDESENHARTCB;
static  CPU_STK  AppTaskDESENHARStk[APP_TASK_START_STK_SIZE];
// TAREFA DO BOMBERMAN
static  OS_TCB   AppTaskBOMBERMANTCB;
static  CPU_STK  AppTaskBOMBERMANStk[APP_TASK_START_STK_SIZE];

// IMAGENS UTILIZADAS NO PROGRAMA
HBITMAP *fundo, *tijolo, *bomba, *expcentro, *exphorizontal, *expvertical, *bomberman_cima, *bomberman_baixo, *bomberman_dir, *bomberman_esq, *img_inimigo1, *img_inimigo2, *img_inimigo3;
int bombaX, bombaY;

OS_SEM Mutex_MATRIZ;

// STRUCT PARA POSICAO
typedef struct{
  int x;
  int y;
}Posicao;


//ENUM DE ESTADO
typedef enum estado {VIVO,MORTO} Estado;

//ENUM DE DIRECAO
typedef enum direcao {BAIXO,CIMA,DIR,ESQ} Direcao;

//ARRAY DE ESPECIE
int codigo[4] = {cod_inimigo1,cod_inimigo2,cod_inimigo3,cod_bomberman}; //O NUMERO ATRIBUIDO AO INIMIGO EH REFERENTE AO SEU CODIGO

//STRUCT PARA BICHOS
typedef struct{
	Posicao posicao;
	Estado estado;
	Direcao direcao;
	int especie; //especie pode ser 1 para inimigo1,2 para 2, 3 para 3, e 4 para bomberman
}bicho;

//Cria bichos
bicho inimigo[3],bomberman;


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
9 - explosao bomba vertical
10 - explosao bomba centro
11 - explosao bomba horizonte
*********************************************************************************************************/
//Labirinto dos obstáculos
int LABIRINTO[XX][YY] = { 
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
static  void  App_TaskBOMBA (void  *p_arg);
static  void  App_TaskDESENHAR (void  *p_arg);
static  void  App_TaskBOMBERMAN (void  *p_arg);
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//DECLARACAO DAS FUNCOES CRIADAS
void atualizaPosicoes(bicho bbicho);
void anda(bicho *bbicho, Direcao ddirecao);

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
	Posicao posicao_aux;
	int aux=1;
	int i = 0, j = 0; // Variáveis responsáveis por percorrer toda a matriz LABIRINTO	
    int erroN;
	OS_ERR  err_os;
	OSSemCreate (&Mutex_MATRIZ, "mutex_matriz", 1, &err_os); //Cria mutex para acessar matriz
	
	//CRIAÇÃO DA TELA DE FUNDO DO JOGO
	fundo = GUI_CreateImage( "fundo.bmp", 612, 390);

	//IMPORTAÇÃO DA IMAGEM DA BOMBA
	bomba = GUI_CreateImage("bomba.bmp", 36, 30);

	//IMPORTAÇÃO DAS IMAGENS DO BOMBERMAN
	bomberman_esq = GUI_CreateImage("bomberman_esq.bmp", 36, 30);
	bomberman_baixo = GUI_CreateImage("bomberman_baixo.bmp", 36, 30);
	bomberman_cima = GUI_CreateImage("bomberman_cima.bmp", 36, 30);
	bomberman_dir = GUI_CreateImage("bomberman_dir.bmp", 36, 30);

	//IMPORTAÇÃO DAS IMAGENS RELATIVAS À EXPLOSÃO
	expcentro = GUI_CreateImage("expcentro.bmp", 36, 30);
	exphorizontal = GUI_CreateImage("exphorizont.bmp", 36, 30);
	expvertical = GUI_CreateImage("expvertical.bmp", 36, 30);

    //IMPORTACAO DOS TIJOLOS
	tijolo = GUI_CreateImage( "tijolo.bmp", 36, 30);

	//IMPORTACAO DOS INIMIGOS
	img_inimigo1 = GUI_CreateImage( "Inimigo1.bmp", 36, 30);
	img_inimigo2 = GUI_CreateImage( "Inimigo2.bmp", 36, 30);
	img_inimigo3 = GUI_CreateImage( "Inimigo3.bmp", 36, 30);


	//CRIA BOMBERMAN
	bomberman.posicao.x=1;bomberman.posicao.y=1;bomberman.direcao=DIR;bomberman.estado=VIVO;bomberman.especie=4;
	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);//wait
	atualizaPosicoes(bomberman);
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	//GERA POSICOES E CRIA INIMIGOS
	for (i=0;i<3;i++){
		while (aux){
			posicao_aux.x=rand()%XX;
			posicao_aux.y=rand()%YY;
			OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait 
			if ((LABIRINTO[posicao_aux.x][posicao_aux.y]==0)){		
				aux=0;
				//CRIA O INIMIGO I
				inimigo[i].posicao.x=posicao_aux.x;
				inimigo[i].posicao.y=posicao_aux.y;
				inimigo[i].direcao=DIR;
				inimigo[i].estado=VIVO;	
				inimigo[i].especie=i+1;
			}
			atualizaPosicoes(inimigo[i]);
			OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal			
		}
		aux=1;
	}

	erroN = GUI_Init(HandleGUIEvents);	// Inicializa a interface grafica

	if(erroN < 0 ) { // se falhou para carregar a Gui, retorna.
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
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

	// CRIAÇÃO DA TAREFA DESENHAR
	OSTaskCreate((OS_TCB     *)&AppTaskDESENHARTCB,               
                 (CPU_CHAR   *)"App StartDESENHAR",
                 (OS_TASK_PTR ) App_TaskDESENHAR,
                 (void       *) 0,
                 (OS_PRIO     ) 10,
                 (CPU_STK    *)&AppTaskDESENHARStk[0],
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


	//Considerando-se largura de cada coluna igual a 24 e altura igual a 20
	//
	////CRIAÇÃO DA BOMBA:
	//if (insereBomba){
	//	LABIRINTO[bombaY][bombaX] = 6;
	//	GUI_DrawImage(bomba, 24*bombaX*K, 20*bombaY*K, 36, 30, 1);

	//	OSTimeDly(3000, OS_OPT_TIME_DLY,&err_os);
	//	/*OSTimeDlyHMSM(0, 0, 10, 0, OS_OPT_TIME_DLY,&err_os);*/
	//	//CRIAÇÃO DAS EXPLOSÕES: 
	//	GUI_DrawImage(expcentro, 24*bombaX*K, 20*bombaY*K, 36, 30, 1);
	//	GUI_DrawImage(exphorizontal, 24*(bombaX-1)*K, 20*bombaY*K, 36, 30, 1);
	//	GUI_DrawImage(exphorizontal, 24*(bombaX+1)*K, 20*bombaY*K, 36, 30, 1);
	//	GUI_DrawImage(expvertical, 24*bombaX*K, 20*(bombaY-1)*K, 36, 30, 1);
	//	GUI_DrawImage(expvertical, 24*bombaX*K, 20*(bombaY+1)*K, 36, 30, 1);
	//}
	//insereBomba = FALSE;
}

static  void  App_TaskBOMBERMAN (void  *p_arg)
{
	OS_ERR  err_os;

}		

static  void  App_TaskDESENHAR (void  *p_arg)
{
	int i,j;
	OS_ERR  err_os;
	while(1){

	GUI_DrawImage(fundo, 0, 0, 1200, 800, 1); //DESENHA FUNDO

	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait 

	for (i = 0; i < 13; i++){
		for (j = 0; j < 17; j++){
			switch (LABIRINTO[i][j]){
			case 2 :GUI_DrawImage(tijolo, 24*j*K, 20*i*K, 36, 30, 1);
				break;
			case cod_bomberman:{
				//Verifica a direcao em que o bomberman esta andando, switch case deu problema ???
				if (bomberman.direcao==DIR){GUI_DrawImage(bomberman_dir, 24*j*K, 20*i*K, 36, 30, 1);}
				if (bomberman.direcao==ESQ){GUI_DrawImage(bomberman_esq, 24*j*K, 20*i*K, 36, 30, 1);}
				if (bomberman.direcao==BAIXO){GUI_DrawImage(bomberman_baixo, 24*j*K, 20*i*K, 36, 30, 1);}
				if (bomberman.direcao==CIMA){GUI_DrawImage(bomberman_cima, 24*j*K, 20*i*K, 36, 30, 1);}
				}
			   break;
			case cod_inimigo1: GUI_DrawImage(img_inimigo1, 24*j*K, 20*i*K, 36, 30, 1);
				break;
			case cod_inimigo2: GUI_DrawImage(img_inimigo2, 24*j*K, 20*i*K, 36, 30, 1);
				break;
			case cod_inimigo3: GUI_DrawImage(img_inimigo3, 24*j*K, 20*i*K, 36, 30, 1);
				break;
			case 6: GUI_DrawImage(bomba, 24*j*K, 20*i*K, 36, 30, 1);
				break;
			case 9: GUI_DrawImage(expvertical, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
				break;
			case 10: GUI_DrawImage(expcentro, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
				break;
		 case 11: GUI_DrawImage(exphorizontal, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
				break;
			}
		}
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err_os);
	}
	
	
}	



//FUNCOES 

//ATUALIZA O LABIRINTO DE ACORDO COM A POSICAO DOS BICHOS, LEMBRAR DE DAR WAIT NO MUTEX ANTES DE CHAMAR
void atualizaPosicoes(bicho bbicho){
	if (bbicho.estado==VIVO)
		LABIRINTO[bbicho.posicao.x][bbicho.posicao.y]=codigo[((bbicho.especie)-1)];
	else
		LABIRINTO[bbicho.posicao.x][bbicho.posicao.y]=0;
}

//FUNCAO ANDA, VERIFICA SE PODE MOVER NA POSICAO DESEJADA E CASO POSITIVO, MUDA A POSICAO
void anda(bicho *bbicho, Direcao ddirecao){
	Posicao nova_posicao=(*bbicho).posicao;
	OS_ERR  err_os;
	(*bbicho).direcao=ddirecao;
	switch (ddirecao){
	case DIR: nova_posicao.y++; break;
	case ESQ: nova_posicao.y--; break;
	case CIMA: nova_posicao.x--; break;
	case BAIXO: nova_posicao.x++; break;
	}
	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait
	if (LABIRINTO[nova_posicao.x][nova_posicao.y]==0){
		LABIRINTO[(*bbicho).posicao.x][(*bbicho).posicao.y]=0; //LIBERA NA MATRIZ A POSICAO ATUAL DO BICHO
		(*bbicho).posicao=nova_posicao; //ATUALIZA A POSICAO DO BICHO
		atualizaPosicoes(*bbicho); //OCUPA POSICAO CORRESPONDENTE DA MATRIZ
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os);

}


// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i,num_bombas;
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

					break;

				case VK_LEFT:
					anda(&bomberman, ESQ);
					break;

				case VK_RIGHT:
					anda(&bomberman, DIR);

					break;

				case VK_UP:
					anda(&bomberman, CIMA);
					break;

				case VK_DOWN:
					anda(&bomberman, BAIXO);
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

