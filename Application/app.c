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
#include <time.h>
#include <stdlib.h>
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
static  OS_TCB   AppTaskBOMBATCB[3];
static  CPU_STK  AppTaskBOMBAStk[3][APP_TASK_START_STK_SIZE];
// TAREFA DOS INIMIGOS
static  OS_TCB   AppTaskINIMIGOTCB;
static  CPU_STK  AppTaskINIMIGOStk[APP_TASK_START_STK_SIZE];
// TAREFA Desenhar
static  OS_TCB   AppTaskDESENHARTCB;
static  CPU_STK  AppTaskDESENHARStk[APP_TASK_START_STK_SIZE];
// TAREFA DO BOMBERMAN
static  OS_TCB   AppTaskBOMBERMANTCB;
static  CPU_STK  AppTaskBOMBERMANStk[APP_TASK_START_STK_SIZE];

// IMAGENS UTILIZADAS NO PROGRAMA
HBITMAP *fundo, *tijolo, *bomba, *expcentro, *exphorizontal, *expvertical, *bomberman_bomba, *bomberman_cima,
	*bomberman_baixo, *bomberman_dir, *bomberman_esq, *img_inimigo1, *img_inimigo2, *img_inimigo3, *img_win, 
	*img_gameover, *img_inicio, *img_bonus;


// SEMAFOROS
OS_SEM Mutex_MATRIZ;
OS_SEM MORTE;
OS_SEM Mutex_bombas;

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

//NUMERO ATUAL DE BOMBAS E NUMERO MAXIMO DE BOMBAS
int num_bombas, max_bombas,jogo_rolando=0,bomba_ativa[3];


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
11 - explosao bomba horizontal
12 - bonus bombas
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
	,{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }};

int LABIRINTO2[XX][YY] = { //BACKUP DO LABIRINTO, PARA REINICIAR O JOGO
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
	,{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }};


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
static  void  App_TaskINIMIGO (void  *p_arg);
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//DECLARACAO DAS FUNCOES CRIADAS
void atualiza_posicoes(bicho bbicho);
void anda(bicho *bbicho, Direcao ddirecao);
void explosao(Posicao bbomba);
int testa_explosao(int pos, int aux);
void gameover();
void win();
void inicia_jogo();
Direcao melhor_caminho(bicho bbicho, Posicao anterior);
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
int  main (void){
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

	//INICIALIZA SEMAFOROS
	OSSemCreate (&Mutex_MATRIZ, "mutex_matriz", 1, &err_os); //Cria mutex para acessar matriz
	OSSemCreate (&Mutex_bombas, "mutex_bombas", 1, &err_os); //Cria mutex para acessar variavel de num de bombas
	OSSemCreate (&MORTE, "morte", 1, &err_os); //Cria semaforo para quando morre algum bicho


	erroN = GUI_Init(HandleGUIEvents);	// Inicializa a interface grafica

	if(erroN < 0 ) { // se falhou para carregar a Gui, retorna.
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
	}
	srand(time(NULL));



	//CRIAÇÃO DA TELA DE FUNDO DO JOGO
	fundo = GUI_CreateImage( "fundo.bmp", 612, 390);

	//IMAGENS INICIAIS E FINAIS
	img_gameover = GUI_CreateImage( "gameover.bmp", 612, 390);
	img_inicio = GUI_CreateImage( "inicio.bmp", 612, 390);
	img_win = GUI_CreateImage( "win.bmp", 612, 390);

	//IMPORTAÇÃO DA IMAGEM DA BOMBA
	bomba = GUI_CreateImage("bomba.bmp", 36, 30);
	bomberman_bomba = GUI_CreateImage("bomberman_bomba.bmp", 36, 30);
	img_bonus = GUI_CreateImage("bombas_bonus.bmp", 36, 30);

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

	GUI_DrawImage(img_inicio, 0, 0, 1200, 800, 1); //DESENHA TELA DE INICIO

		// CRIAÇÃO DA TAREFA DO BOMBERMAN
	OSTaskCreate((OS_TCB     *)&AppTaskBOMBERMANTCB,                
		(CPU_CHAR   *)"App StartBOMBERMAN",
		(OS_TASK_PTR ) App_TaskBOMBERMAN,
		(void       *) 0,
		(OS_PRIO     ) 6,
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

static  void  App_TaskINIMIGO (void  *p_arg)
{
	OS_ERR  err_os;
	int i,j=0;
	Posicao posicao_anterior[3];
	Direcao melhor_direcao;
	//GUARDA A POSICAO ANTERIOR
	for (i=0;i<3;i++){
		posicao_anterior[i].x=0;posicao_anterior[i].y=0;}
	while(1){
		for (i=0;i<3;i++){
			//USA A FUNCAO MELHOR_CAMINHO PARA DECIDIR PARA QUAL DIRECAO IR 
			melhor_direcao = melhor_caminho(inimigo[i],posicao_anterior[i]);
			posicao_anterior[i]=inimigo[i].posicao;
			anda(&inimigo[i],melhor_direcao);
		}
		OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_DLY, &err_os);
	}
}

static  void  App_TaskBOMBA (void  *p_arg)
{
	Posicao bbomba;
	int i, num_bomba_atual;
	OS_ERR  err_os;
	bbomba=bomberman.posicao;
	LABIRINTO[bbomba.x][bbomba.y] = 6;
	num_bomba_atual=num_bombas-1;
	bomba_ativa[num_bomba_atual]=1;

	OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_DLY, &err_os);

	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait 
	for (i=-1;i<2;i++){
		switch (LABIRINTO[bbomba.x+i][bbomba.y]){   //DESTRUIR PERSONAGENS E BLOCOS
		case cod_bomberman: bomberman.estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo1: inimigo[0].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo2: inimigo[1].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo3: inimigo[2].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case 2: LABIRINTO[bbomba.x+i][bbomba.y]=0;
			break;
		}
		if (LABIRINTO[bbomba.x+i][bbomba.y]!=1 && (LABIRINTO[bbomba.x][bbomba.y+i]!=6)){
			LABIRINTO[bbomba.x+i][bbomba.y]=9;}//ATRIBUI DESENHO DE EXPLOSAO VERTICAL A POSICAO
	}
	for (i=-1;i<2;i++){ //DESTRUIR PERSONAGENS E BLOCOS
		switch (LABIRINTO[bbomba.x][bbomba.y+i]){
		case cod_bomberman: bomberman.estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo1: inimigo[0].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo2: inimigo[1].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case cod_inimigo3: inimigo[2].estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
			break;
		case 2: LABIRINTO[bbomba.x][bbomba.y+i]=0;
			break;
		}
		if (LABIRINTO[bbomba.x][bbomba.y+i]!=1 && (LABIRINTO[bbomba.x][bbomba.y+i]!=6)){ //SE NAO FOR PAREDE E NAO FOR OUTRA BOMBA
			LABIRINTO[bbomba.x][bbomba.y+i]=11;} //ATRIBUI DESENHO DE EXPLOSAO HORIZONTAL A POSICAO
	}
	LABIRINTO[bbomba.x][bbomba.y]=10; //ATRIBUI DESENHO DE CENTRO DE EXPLOSAO A POSICAO
	if (bbomba.x==bomberman.posicao.x && bbomba.y == bomberman.posicao.y){
		bomberman.estado=MORTO;
		OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	//FINALIZA EXPLOSAO
	OSTimeDlyHMSM(0,0,0,700,OS_OPT_TIME_DLY, &err_os);
	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait 
	//LIMPA EXPLOSAO
	for (i=-1;i<2;i++){
		if (LABIRINTO[bbomba.x+i][bbomba.y]!=1){
			LABIRINTO[bbomba.x+i][bbomba.y]=0;}//ATRIBUI DESENHO DE EXPLOSAO VERTICAL A POSICAO
	}
	for (i=-1;i<2;i++){ //DESTRUIR PERSONAGENS E BLOCOS
		if (LABIRINTO[bbomba.x][bbomba.y+i]!=1){ //SE NAO FOR PAREDE
			LABIRINTO[bbomba.x][bbomba.y+i]=0;} //ATRIBUI DESENHO DE EXPLOSAO HORIZONTAL A POSICAO
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	num_bombas--;
	bomba_ativa[num_bomba_atual]=0;
	OSTaskDel(&AppTaskBOMBATCB[num_bomba_atual],&err_os);
	//OSTaskDel((OS_TCB *)0, &err_os);
}

static  void  App_TaskBOMBERMAN (void  *p_arg)
{
	OS_ERR  err_os;
	while (1){
		if (jogo_rolando){
			OSSemPend (&MORTE, 0, OS_OPT_PEND_BLOCKING, 0, &err_os); //wait
			if (bomberman.estado == MORTO){
				jogo_rolando=0;
				GUI_DrawImage(img_gameover, 0, 0, 1200, 800, 1); //DESENHA IMAGEM DE GAMEOVER
				gameover();	//ENCERRA TASKS
			}
			if (inimigo[0].estado == MORTO && inimigo[1].estado == MORTO && inimigo[2].estado == MORTO){
				jogo_rolando=0;
				GUI_DrawImage(img_win, 0, 0, 1200, 800, 1); //DESENHA IMAGEM DE VITORIA
				gameover();	//ENCERRA TASKS
			}
		}
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY, &err_os);
	}
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
				case 6:
					if (bomberman.posicao.x==i && bomberman.posicao.y==j)
						GUI_DrawImage(bomberman_bomba, 24*j*K, 20*i*K, 36, 30, 1);
					else
						GUI_DrawImage(bomba, 24*j*K, 20*i*K, 36, 30, 1);
					break;
				case 9: GUI_DrawImage(expvertical, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
					break;
				case 10: GUI_DrawImage(expcentro, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
					break;
				case 11: GUI_DrawImage(exphorizontal, 24*j*K, 20*i*K, 36, 30, 1);//CENTRO EXPLOSAO	
					break;
				case 12: GUI_DrawImage(img_bonus, 24*j*K, 20*i*K, 36, 30, 1);
					break;
				}
			}
		}
		OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err_os);
	}


}	


//######################## FUNCOES ########################

//ATUALIZA O LABIRINTO DE ACORDO COM A POSICAO DOS BICHOS, LEMBRAR DE DAR WAIT NO MUTEX ANTES DE CHAMAR
void atualiza_posicoes(bicho bbicho){
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
		if (LABIRINTO[(*bbicho).posicao.x][(*bbicho).posicao.y]!=6){ //CASO A BOMBA ESTEJA NA POSICAO, NAO A APAGA
			LABIRINTO[(*bbicho).posicao.x][(*bbicho).posicao.y]=0; //LIBERA NA MATRIZ A POSICAO ATUAL DO BICHO
		} 
		(*bbicho).posicao=nova_posicao; //ATUALIZA A POSICAO DO BICHO
		atualiza_posicoes(*bbicho); //OCUPA POSICAO CORRESPONDENTE DA MATRIZ
	}
	// SE ESTIVER EXPLOSAO NO LOCAL, MATA O BICHO
	else if (LABIRINTO[nova_posicao.x][nova_posicao.y]==9 || LABIRINTO[nova_posicao.x][nova_posicao.y]==10 || LABIRINTO[nova_posicao.x][nova_posicao.y]==11){
		(*bbicho).estado=MORTO; 
		OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal

	}
	//SE O INIMIGO ANDAR NA POSICAO DO BOMBERMAN, OU VICE-VERSA, O BOMBERMAN MORRE
	else if (LABIRINTO[nova_posicao.x][nova_posicao.y]==cod_bomberman 
		|| ((*bbicho).especie==4 && (LABIRINTO[nova_posicao.x][nova_posicao.y]==cod_inimigo1
		|| LABIRINTO[nova_posicao.x][nova_posicao.y]==cod_inimigo2 || LABIRINTO[nova_posicao.x][nova_posicao.y]==cod_inimigo3))){
			(*bbicho).posicao=nova_posicao; //ATUALIZA A POSICAO DO BICHO
			atualiza_posicoes(*bbicho);
			LABIRINTO[(*bbicho).posicao.x][(*bbicho).posicao.y]=0;
			bomberman.estado=MORTO;
			OSSemPost (&MORTE,OS_OPT_POST_1, &err_os); //signal
	}
	//SE O BICHO FOR O BOMBERMAN E A PROXIMA POSICAO FOR A DO BONUS, AUMENTA O MAXI DE BOMBAS E OCUPA POSICAO
	else if ((*bbicho).especie==4 && LABIRINTO[nova_posicao.x][nova_posicao.y]==12){
		LABIRINTO[(*bbicho).posicao.x][(*bbicho).posicao.y]=0; //LIBERA NA MATRIZ A POSICAO ATUAL DO BICHO
		(*bbicho).posicao=nova_posicao; //ATUALIZA A POSICAO DO BICHO
		atualiza_posicoes(*bbicho); //OCUPA POSICAO CORRESPONDENTE DA MATRIZ
		max_bombas=3;
	}

	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal

}


//FUNCAO QUE BUSCA O MELHOR CAMINHO PARA O INIMIGO
Direcao melhor_caminho(bicho bbicho, Posicao anterior){
	float opcao[4],melhor;
	int i,melhor_i;
	OS_ERR err_os;
	Posicao posicao[4];
	srand(time(NULL));

	//POSICOES POSSIVEIS
	posicao[0].x = (bbicho.posicao.x+1); posicao[0].y = bbicho.posicao.y; //BAIXO
	posicao[1].x = (bbicho.posicao.x-1); posicao[1].y = bbicho.posicao.y; //CIMA
	posicao[2].x = (bbicho.posicao.x); posicao[2].y = bbicho.posicao.y+1; //DIR
	posicao[3].x = (bbicho.posicao.x); posicao[3].y = bbicho.posicao.y-1; //ESQ

	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);//wait

	// CALCULA A PONTUACAO DAS DIRECOES POSSIVEIS
	for (i=0;i<4;i++){
		if (LABIRINTO[posicao[i].x][posicao[i].y] == 0){
			//O ((rand()%5*0.05+1) ENCONTRA UM VALOR ENTRE 1 E 1.2, PARA CRIAR UMA CERTA ALEATORIEDADE NO MOVIMENTO
			opcao[i]=(rand()%5*0.05+1)*((posicao[i].x-bomberman.posicao.x)^2 + (posicao[i].y-bomberman.posicao.y)^2);} //CALCULA DISTANCIA ECLIDIANA, QUANTO MENOR, MELHOR
		else opcao[i]=100; //PARA NAO TENTAR ANDAR ONDE NAO PODE
		if ((posicao[i].x == anterior.x) && (posicao[i].y == anterior.y)){
			opcao[i]+= 30;} // PUNICAO PARA NAO VOLTAR A POSICAO ANTERIOR
		if ((posicao[i].x == bomberman.posicao.x) && (posicao[i].y == bomberman.posicao.y)){
			opcao[i]=0;} //MELHOR PONTUACAO PARA CASO O BOMBERMAN ESTEJA NA POSICAO DESEJADA
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	melhor=opcao[0];
	melhor_i=0;
	// VERIFICA A MELHOR DIRECAO
	for (i=1;i<4;i++){
		if (opcao[i]<melhor){
			melhor = opcao[i];
			melhor_i=i;
		}
	}
	// RETORNA A MELHOR DIRECAO
	switch (melhor_i){
	case 0: return BAIXO;
	case 1: return CIMA;
	case 2: return DIR;
	case 3: return ESQ;
	}

}

//FINALIZA TASKS
void gameover(){
	int i;
	OS_ERR err_os;

	for (i=0;i<max_bombas;i++){
		if (bomba_ativa[i]==1){
			OSTaskDel(&AppTaskBOMBATCB[i],&err_os);
		}
	}

	//DELETA TASKS	
	OSTaskDel(&AppTaskDESENHARTCB,&err_os);
	OSTaskDel(&AppTaskINIMIGOTCB,&err_os);

	//DELETA SEMAFOROS
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal
	OSSemPost (&Mutex_bombas,OS_OPT_POST_1, &err_os); //signal


}


//INICIALIZA TASKS E VARIAVEIS DO JOGO
void inicia_jogo(){
	//Parametros iniciais da bomba
	int i,aux=1;
	Posicao posicao_aux;
	OS_ERR err_os;

	bomba_ativa[0]=0;bomba_ativa[1]=0;bomba_ativa[2]=0; //ZERA BOMBAS ATIVAS



	memcpy(LABIRINTO, LABIRINTO2, sizeof(LABIRINTO)); //VOLTA AO ESTADO ORIGINAL DE LABIRINTO
	num_bombas = 0; //NUM DE BOMBAS ATUAL
	max_bombas = 1; //NUM MAXIMO DE BOMBAS, PODE IR ATE 3 (PELA TASK BOMBA)

	//CRIA BOMBERMAN
	bomberman.posicao.x=1;bomberman.posicao.y=1;bomberman.direcao=DIR;bomberman.estado=VIVO;bomberman.especie=4;
	OSSemPend (&Mutex_MATRIZ, 0, OS_OPT_PEND_BLOCKING, 0, &err_os);//wait
	atualiza_posicoes(bomberman);
	//COLOCA BONUS EM ESPACO LIVRE
	while (aux){
		posicao_aux.x=rand()%XX;
		posicao_aux.y=rand()%YY;
		if ((LABIRINTO[posicao_aux.x][posicao_aux.y]==0)){		
			aux=0;
			LABIRINTO[posicao_aux.x][posicao_aux.y]=12; //RECEBE IMG DO BONUS;
		}						
	}
	aux=1;


	//GERA POSICOES E CRIA INIMIGOS
	for (i=0;i<3;i++){
		while (aux){
			posicao_aux.x=rand()%(XX-4)+4;
			posicao_aux.y=rand()%(YY-4)+4;
			if ((LABIRINTO[posicao_aux.x][posicao_aux.y]==0)){		
				aux=0;
				//CRIA O INIMIGO I
				inimigo[i].posicao.x=posicao_aux.x;
				inimigo[i].posicao.y=posicao_aux.y;
				inimigo[i].direcao=DIR;
				inimigo[i].estado=VIVO;	
				inimigo[i].especie=i+1;
				atualiza_posicoes(inimigo[i]);
			}			
		}
		aux=1;
	}
	OSSemPost (&Mutex_MATRIZ,OS_OPT_POST_1, &err_os); //signal		

	// CRIAÇÃO DA TAREFA INIMIGO
	OSTaskCreate((OS_TCB     *)&AppTaskINIMIGOTCB,               
		(CPU_CHAR   *)"App StartINIMIGO",
		(OS_TASK_PTR ) App_TaskINIMIGO,
		(void       *) 0,
		(OS_PRIO     ) 1,
		(CPU_STK    *)&AppTaskINIMIGOStk[0],
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
		(OS_PRIO     ) 2,
		(CPU_STK    *)&AppTaskDESENHARStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);


}

//##########################################################


// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OS_ERR  err_os;
	int i,k;
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
			if (jogo_rolando==0){
				jogo_rolando=1; //VARIAVEL DE ESTADO DO JOGO
				inicia_jogo();}
			break; 

		case VK_SPACE:
			if (jogo_rolando==1){	
				if (num_bombas<max_bombas){
					num_bombas++;
					k=num_bombas-1;
					if (bomba_ativa[k]==0){
					// CRIAÇÃO DA TAREFA DA BOMBA
					OSTaskCreate((OS_TCB     *)&AppTaskBOMBATCB[k],               
						(CPU_CHAR   *)"App StartBOMBA",
						(OS_TASK_PTR ) App_TaskBOMBA,
						(void       *) &k,
						(OS_PRIO     ) (15+k),
						(CPU_STK    *)&AppTaskBOMBAStk[k][0],
						(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
						(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
						(OS_MSG_QTY  ) 0u,
						(OS_TICK     ) 0u,
						(void       *) 0,
						(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
						(OS_ERR     *)&err_os);
					}else
					{
						num_bombas--;
					}
				}
			}
			break;

		case VK_LEFT:
			if (jogo_rolando==1){
				anda(&bomberman, ESQ);
			}
			break;
		case VK_RIGHT:
			if (jogo_rolando==1){
				anda(&bomberman, DIR);
			}
			break;
		case VK_UP:
			if (jogo_rolando==1){
				anda(&bomberman, CIMA);
			}
			break;
		case VK_DOWN:
			if (jogo_rolando==1){
				anda(&bomberman, BAIXO);
			}
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

