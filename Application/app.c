
/*
*********************************************************************************************************
*
*                                              TRABALHO PR�TICO - BCC722
*
*                                                  JOGO BOMBERMAN
*
* Arquivo			: app.c
* Versao			: 1.0
* Aluno(s)			: Hugo Azevedo - Diego - Eduardo
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
#define SIZE_TIJOLO_DEFAULT 40

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

int GOD_MODE = 1; // Em desenvolvimento = 1. Produ��o = 0;

enum GRAPHIC_OBJS{
	ID_BUTTON_1 = 0,
	ID_EDIT_LINE_1,
	ID_LABEL_1,
};

static int BACKGROUND_IMAGE_WIDTH = 680; // Largura da tela. 680 = 17 * 40.
static int BACKGROUND_IMAGE_HEIGTH = 520; // Altura da tela. 520 = 13 * 40.

static int MAP_WIDTH = 17; // Largura da tela em pontos da matrix.
static int MAP_HEIGTH = 13; // Altura da tela em pontos da matrix.

int BLOCK_SIZE = 40; // Tamanho b�sico de cada bloco.

int MAX_BOMBS = 100; // N�mero m�ximo de bombas que podem ser inserids.
int BOMBS_DELAY = 200; // Delay de explos�o das 

/*
*********************************************************************************************************
*                                            GLOBAL TASKS
*********************************************************************************************************
*/

static  OS_TCB   AppStartTaskTCB;
static  CPU_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Player_TaskTCB;
static  CPU_STK  Player_TaskStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Bombs_TaskTCB;
static  CPU_STK  Bombs_TaskStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Explosion_TaskTCB;
static  CPU_STK  Explosion_TaskStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Enemy_1TCB;
static  CPU_STK  Enemy_1Stk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Enemy_2TCB;
static  CPU_STK  Enemy_2Stk[APP_TASK_START_STK_SIZE];

static  OS_TCB   Enemy_3TCB;
static  CPU_STK  Enemy_3Stk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


// HBITMAP * img;
HBITMAP * img_back, *img_block,*img_bomb, *img_player, *img_bomberman, *img_bomberman_reverse, *img_explosion_center,*img_explosion_horizontal, *img_explosion_vertical, *img_fogo_vertical, *img_enemy1, *img_enemy2, *img_enemy3;


/*
*********************************************************************************************************
LABIRINTOS - CODIFICA��O DOS OBJETOS
0 - vazio
1 - parede
2 - tijolo
3 - inimigo 1
4 - inimigo 2
5 - inimigo 3
6 - bomba
7 - *
8 - rastro da explos�o
*********************************************************************************************************/


/*
* Labirinto dos obst�culos
* O acesso � matrix 'LABIRINTO' deve ser feito com posi��es y e x invertidas.
* Ex.: LABIRINTO[Y][X];
*/
int LABIRINTO[13][17] = 
{ { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
,{ 1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
,{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1 }
,{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1 }
,{ 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }
,{ 1, 2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
,{ 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1 }
,{ 1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 1 }
,{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1 }
,{ 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
,{ 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1 }
,{ 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
,{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

// Vari�veis declaradas do m�dulo da GUI
extern HWND hwnd; 
extern HDC hdc;
extern MSG Msg;

int BOMBERMAN_POS_X = 1; // Posi��o x do bomberbam na matrix LABIRINTO. 1 = posi��o inicial.
int BOMBERMAN_POS_Y = 1; // Posi��o y do bomberbam na matrix LABIRINTO. 1 = posi��o inicial.

int BOMBERMAN_X = 40; // Posi��o x do bomberbam em rela��o � tela, ou seja, a posi��o em px. 40 = posi��o inicial.
int BOMBERMAN_Y = 40; // Posi��o x do bomberbam em rela��o � tela, ou seja, a posi��o em px. 40 = posi��o inicial.

int WAITING_CLICK = 0; // Flag que bloquei o multi click na mesma tecla.
int BOMB_ON = 0; // Flag que sinaliza que uma bomba foi plantada.
int POWER_ON = 1; // Flag que sinaliza se o bomberman adquiriu algum poder. 

int num_bombs = 0; // N�mero total de bombas.
int placed_bombs = 0; // N�mero total de bombas colocadas.

int enemy_count  = 3; // N�mero de inimigos.

/*
* Matrix de bombas. Ser� inicializada na fun��o Initiate_Bombs_Matrix(void).
* A primeira coluna de cada entrada representa a posi��o em X, a segunda em Y e a terceira o tempo de detona��o.
*/
int BOMBS[100][3];

/*
* Matrix de posi��o dos inimigos.
* A primeira coluna de cada entrada representa a posi��o em X, a segunda em Y.
*/
int ENEMYS_POS[3][2] = 
{ { 15 ,  1 }
, {  1 , 11 }
, { 15 , 11 }
};

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
*									Declara��o das Tarefas Criadas
*********************************************************************************************************
*/
static  void  App_TaskStart (void  *p_arg);
static  void  Player_Task (void  *p_arg);
static  void  Bombs_Task (void  *p_arg);
static  void  Explosion_Task (void  *p_arg);

static  void Enemy_1 (void *p_arg);
static  void Enemy_2 (void *p_arg);
static  void Enemy_3 (void *p_arg);

static void Draw_Background(void);
static void Draw_Player(void);
static void Draw_Blocks(void);
static void Draw_Bombs(int, int);
static void Draw_Explosion(HBITMAP*, int, int);
static void Draw_Enemys(void);
static void Draw_Enemy(int, HBITMAP*, int, int);

static void Kill_Enemy(int);
static void Create_Enemys_Tasks(void);
static void Criar_Figuras(void);
static void Initiate_Bombs_Matrix(void);
static void Make_Move(int);
static void Put_Bomb(void);
static void Explode(int[]);
static void Verifications(void);
static void Finish_Game(void);

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

	if (GOD_MODE == 1) 
	{
		printf("GOD MODE ON.");
		BOMBS_DELAY = 50;
	}

	Initiate_Bombs_Matrix();

	OSTaskCreate((OS_TCB     *)&AppStartTaskTCB,                /* Cria a tarefa inicial.                             */
		(CPU_CHAR   *)"App Start Task",
		(OS_TASK_PTR ) App_TaskStart,
		(void       *) 0,
		(OS_PRIO     ) 10,
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

// Cria as imagens
static void Criar_Figuras(void)
{
	img_back = GUI_CreateImage("fundo.bmp", BACKGROUND_IMAGE_WIDTH, BACKGROUND_IMAGE_HEIGTH);
	img_block = GUI_CreateImage("tijolo.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_bomberman = GUI_CreateImage("bomberman.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_bomberman_reverse = GUI_CreateImage("bomberman_inverso.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_enemy1 = GUI_CreateImage("enemy.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_enemy2 = GUI_CreateImage("enemy2.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_enemy3 = GUI_CreateImage("enemy3.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_bomb = GUI_CreateImage("bomba.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_explosion_center = GUI_CreateImage("expcentro.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_explosion_horizontal = GUI_CreateImage("exphorizont.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_explosion_vertical = GUI_CreateImage("expvertical.bmp", BLOCK_SIZE, BLOCK_SIZE);
	img_player = img_bomberman;
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

static  void  App_TaskStart (void  *p_arg)
{

	int i=0;
	int erroN;
	OS_ERR  err_os;

	erroN = GUI_Init(HandleGUIEvents);

	if( erroN < 0 ) 
	{ 
		printf("\n Erro ao iniciar a Gui (%d)",erroN);
	}

	Criar_Figuras();

	OSTaskCreate((OS_TCB     *)&Player_TaskTCB,                
		(CPU_CHAR   *)"PLAYER TASK",
		(OS_TASK_PTR ) Player_Task,
		(void       *) 0,
		(OS_PRIO     ) 9,
		(CPU_STK    *)&Player_TaskStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSTaskCreate((OS_TCB     *)&Bombs_TaskTCB,                
		(CPU_CHAR   *)"Bombs TASK",
		(OS_TASK_PTR ) Bombs_Task,
		(void       *) 0,
		(OS_PRIO     ) 8,
		(CPU_STK    *)&Bombs_TaskStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	Create_Enemys_Tasks();

	// Loop de mensagens para interface grafica
	while (1)
	{

		PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		Draw_Background();
		Draw_Blocks();
		Draw_Player();
		Draw_Enemys();

		Verifications();

		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_DLY, &err_os);

	}

	printf("\n fim do loop de msg");

}

// Verifica se o player se chocou com um inimigo ou com a explos�o de uma bomba.
static void Verifications(void)
{
	int i;
	int j;
	int k;

	//for (i = 1 ; i < 12; i++) 
	//{
	//	if (BOMBERMAN_POS_Y == i) 
	//	{
	//		for (j = 1; j < 16; j++) 
	//		{
	//			if (LABIRINTO[i][j] == 8) 
	//			{
	//				if (BOMBERMAN_POS_X == j && BOMBERMAN_POS_Y == i) // Caso o bomberbam se encontre com o rastro da explos�o.
	//				{
	//					Finish_Game();
	//				}
	//			}
	//		}
	//	}
	//}
	for (k = 0; k < 3; k++) 
	{
		if (BOMBERMAN_POS_X == ENEMYS_POS[k][0] && BOMBERMAN_POS_Y == ENEMYS_POS[k][1]) // Caso o bomberbam se encontre com um inimigo.
		{
			Finish_Game();
		}
	}
}

// Task do player n�o utilizada no momento.
static  void  Player_Task (void  *p_arg)
{
	OS_ERR  err_os;

	while (1) 
	{
		if (LABIRINTO[BOMBERMAN_POS_Y][BOMBERMAN_POS_X] == 8) 
		{
			Finish_Game();
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_DLY, &err_os);
	}
}

// Task respons�vel pro verificar, desenhar e desencadear a a��o de explodir bombas.
static  void  Bombs_Task (int  p_arg[])
{
	OS_ERR  err_os;

	int i;
	while (1) 
	{			
		for (i = 0; i < num_bombs; i++) 
		{
			if (BOMBS[i][2] > 0) { // Verifica se est� na hora de explodir a bomba.
				BOMBS[i][2]--; // Decrementa quanto tempo falta para a bomba explodir.
				Draw_Bombs(BOMBS[i][0], BOMBS[i][1]); // Desenha a bomba na posi��o dada.
			} 
			else
			{
				Explode(BOMBS[i]);
				// Define como 0 a posi��o da bomba explodida, ou seja, fora do mapa alcan��vel.
				BOMBS[i][0] = 0;
				BOMBS[i][1] = 0;
				BOMBS[i][2] = 0;
			}

		}
		OSTimeDlyHMSM(0,0,0,30,OS_OPT_TIME_DLY, &err_os);
	}
}

// Task respons�vel por gerar as explos�es e desenha-las.
static  void  Explosion_Task (int  p_arg[])
{
	OS_ERR        err_os;

	int x = p_arg[0];
	int y = p_arg[1];

	int i;
	int k;
	int j;

	int explosion_time = 80; // Tempo que a explos�o continuar� evidente no mapa.

	BOMB_ON = 0;

	if (GOD_MODE == 1) explosion_time = 30;

	for (i = 0; i < explosion_time; i++) 
	{
		Draw_Explosion(img_explosion_center, x, y);
		if (LABIRINTO[y][x-1] != 1) 
		{
			Draw_Explosion(img_explosion_horizontal, x-1, y);

			if (POWER_ON == 1 && x > 1 && LABIRINTO[y][x-2] != 1)
			{
				Draw_Explosion(img_explosion_horizontal, x-2, y);
			}
		}
		if (LABIRINTO[y][x+1] != 1) 
		{
			Draw_Explosion(img_explosion_horizontal, x+1, y);

			if (POWER_ON == 1 && x < 14 && LABIRINTO[y][x+2] != 1) 
			{
				Draw_Explosion(img_explosion_horizontal, x+2, y);
			}
		}
		if (LABIRINTO[y-1][x] != 1) 
		{
			Draw_Explosion(img_explosion_vertical, x, y-1);

			if (POWER_ON == 1 && y > 1 && LABIRINTO[y-2][x] != 1) 
			{
				Draw_Explosion(img_explosion_vertical, x, y-2);
			}
		}
		if (LABIRINTO[y+1][x] != 1) 
		{
			Draw_Explosion(img_explosion_vertical, x, y+1);

			if (POWER_ON == 1 && y < 10 && LABIRINTO[y+2][x] != 1) 
			{
				Draw_Explosion(img_explosion_vertical, x, y+2);
			}
		}

		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_DLY, &err_os);
	}

	// Remove os rastros de explos�o ap�s passar o tempo definido por explosion_time.
	for (k = 1; k < 12; k++) 
	{
		for (j = 1; j < 16; j++) 
		{
			if (LABIRINTO[k][j] == 8) 
			{
				LABIRINTO[k][j] = 0;
			}
		} 
	}

}

// Task inimigo 1
static  void Enemy_1 (void *p_arg)
{
	OS_ERR  err_os;

	while (1) 
	{
		int x = ENEMYS_POS[0][0];
		int y = ENEMYS_POS[0][1];

		//Draw_Enemy(1, img_enemy1,x, y);

		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err_os);
	}
}

// Task inimigo 2
static  void Enemy_2 (void *p_arg)
{
	OS_ERR  err_os;

	while (1) 
	{
		int x = ENEMYS_POS[1][0];
		int y = ENEMYS_POS[1][1];

		//Draw_Enemy(2, img_enemy2, x, y);

		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err_os);
	}
}

// Task inimigo 3
static  void Enemy_3 (void *p_arg)
{
	OS_ERR  err_os;

	while (1) 
	{
		int x = ENEMYS_POS[2][0];
		int y = ENEMYS_POS[2][1];

		//Draw_Enemy(3, img_enemy3, x, y);

		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY, &err_os);
	}
}

static void Draw_Enemys()
{
	int x = ENEMYS_POS[0][0];
	int y = ENEMYS_POS[0][1];
	if (x != 0)
		Draw_Enemy(1, img_enemy1,x, y);

	x = ENEMYS_POS[1][0];
	y = ENEMYS_POS[1][1];
	if (x != 0)
		Draw_Enemy(2, img_enemy2, x, y);

	x = ENEMYS_POS[2][0];
	y = ENEMYS_POS[2][1];
	if (x != 0)
		Draw_Enemy(3, img_enemy3, x, y);
}

// Desenha um inimigo qualquer na tela
static void Draw_Enemy(int enemy, HBITMAP *img, int x, int y)
{
	GUI_DrawImage(img, // *img
		x * BLOCK_SIZE, // posx
		y * BLOCK_SIZE, // posy
		BLOCK_SIZE, // width
		BLOCK_SIZE, // height
		2); // index

	LABIRINTO[y][x] = enemy + 2; // O c�digo do inimigo � o seu n�mero +2
}

// Re-desenha o background do jogo.
static void Draw_Background(void)
{
	GUI_DrawImage(img_back, 4, 0, BACKGROUND_IMAGE_WIDTH, BACKGROUND_IMAGE_HEIGTH, 0); // Coloca o Fundo com offset proposital de 4px em x.
}

// Desenha o player na posi��o atual
static void Draw_Player(void)
{
	GUI_DrawImage(img_player, // *img
		BOMBERMAN_X, // posx
		BOMBERMAN_Y, // posy
		BLOCK_SIZE, // width
		BLOCK_SIZE, // height
		2); // index
}

// Desenha os blocos com base na matrix LABIRINTO (pode ser otimizado)
static void Draw_Blocks(void)
{

	int i;
	int j;

	for (i = 1 ; i < 12; i++) 
	{
		for (j = 1; j < 16; j++) 
		{

			if (LABIRINTO[i][j] == 2) 
			{
				GUI_DrawImage(img_block, // *img
					j * BLOCK_SIZE, // posx
					i * BLOCK_SIZE, // posy
					BLOCK_SIZE, // width
					BLOCK_SIZE, // height
					3); // index
			}
		}
	}
}

// Desenha as bombas.
static void Draw_Bombs(int x, int y)
{
	if (x == 0 && y == 0) return;
	GUI_DrawImage(img_bomb, // *img
		x * BLOCK_SIZE, // posx
		y * BLOCK_SIZE, // posy
		BLOCK_SIZE, // width
		BLOCK_SIZE, // height
		3); // index
}

// Desenha de fato as explos�es e mata as tasks dos inimigos, caso sejam atingidos.
static void Draw_Explosion(HBITMAP *img, int x, int y)
{
	GUI_DrawImage(img, // *img
		x * BLOCK_SIZE, // posx
		y * BLOCK_SIZE, // posy
		BLOCK_SIZE, // width
		BLOCK_SIZE, // height
		3); // index

	if (LABIRINTO[y][x] == 3) 
	{
		Kill_Enemy(1);
	}

	if (LABIRINTO[y][x] == 4) 
	{
		Kill_Enemy(2);
	}

	if (LABIRINTO[y][x] == 5) 
	{
		Kill_Enemy(3);
	}

	LABIRINTO[y][x] = 8;
}

// Fun��o respons�vel por matar as tasks dos inimigos quando s�o atingidos por explos�es.
// Quando um inimigo � morto, � importante zerar sua matrix de posi��es;
static void Kill_Enemy(int i)
{
	OS_ERR os_err;

	if (i==1)
	{
		ENEMYS_POS[i-1][0] = 0;
		ENEMYS_POS[i-1][1] = 0;
		enemy_count--;
		OSTaskDel(&Enemy_1TCB, &os_err);

	}
	if (i==2)
	{
		ENEMYS_POS[i-1][0] = 0;
		ENEMYS_POS[i-1][1] = 0;
		enemy_count--;
		OSTaskDel(&Enemy_2TCB, &os_err);
	}
	if (i==3)
	{
		ENEMYS_POS[i-1][0] = 0;
		ENEMYS_POS[i-1][1] = 0;
		enemy_count--;
		OSTaskDel(&Enemy_3TCB, &os_err);
	}
}

// Cria as tasks dos 3 inimigos
static void Create_Enemys_Tasks () 
{
	OS_ERR  err_os;

	OSTaskCreate((OS_TCB     *)&Enemy_1TCB,                
		(CPU_CHAR   *)"Enemy_1",
		(OS_TASK_PTR ) Enemy_1,
		(void       *) 0,
		(OS_PRIO     ) 7,
		(CPU_STK    *)&Enemy_1Stk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSTaskCreate((OS_TCB     *)&Enemy_2TCB,                
		(CPU_CHAR   *)"Enemy_2",
		(OS_TASK_PTR ) Enemy_2,
		(void       *) 0,
		(OS_PRIO     ) 7,
		(CPU_STK    *)&Enemy_2Stk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	OSTaskCreate((OS_TCB     *)&Enemy_3TCB,                
		(CPU_CHAR   *)"Enemy_3",
		(OS_TASK_PTR ) Enemy_3,
		(void       *) 0,
		(OS_PRIO     ) 7,
		(CPU_STK    *)&Enemy_3Stk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
}

// Fun��o que recebe o c�digo do movimento ou a��o a ser realizado a o trata, criando um delay tamb�m para funcionar como debounce.
static void Make_Move(int opt)
{
	/* O acesso � matrix 'LABIRINTO' deve ser feito com posi��es y e x invertidas.
	*  Ex.: LABIRINTO[Y][X];
	*/
	OS_ERR  err_os;

	if (WAITING_CLICK == 1) return; // Flag que bloquei o multiplo clique.

	if (opt == 1) // LEFT
	{
		int nextMove = LABIRINTO[BOMBERMAN_POS_Y][BOMBERMAN_POS_X - 1];
		if (nextMove == 0 || nextMove == 8)
		{
			img_player = img_bomberman_reverse;
			BOMBERMAN_POS_X--;
			BOMBERMAN_X -=  BLOCK_SIZE;
		}
	} else if (opt == 2) // UP
	{
		int nextMove = LABIRINTO[BOMBERMAN_POS_Y - 1][BOMBERMAN_POS_X];
		if (nextMove == 0 || nextMove == 8)
		{
			BOMBERMAN_POS_Y--;
			BOMBERMAN_Y -=  BLOCK_SIZE;
		}
	} else if (opt == 3) // RIGHT
	{
		int nextMove = LABIRINTO[BOMBERMAN_POS_Y][BOMBERMAN_POS_X + 1];
		if (nextMove == 0 || nextMove == 8)
		{
			img_player = img_bomberman;
			BOMBERMAN_POS_X++;
			BOMBERMAN_X +=  BLOCK_SIZE;
		}
	} else if (opt == 4) // DOWN
	{
		int nextMove = LABIRINTO[BOMBERMAN_POS_Y + 1][BOMBERMAN_POS_X];
		if (nextMove == 0 || nextMove == 8)
		{
			BOMBERMAN_POS_Y++;
			BOMBERMAN_Y +=  BLOCK_SIZE;
		}
	} else if (opt == 5) // PLANT BOMM (SPACE)
	{
		if (BOMB_ON == 0) // Caso n�o haja bombas no mapa, � permitido colocar uma.
			Put_Bomb();
	}
	WAITING_CLICK = 1;
	OSTimeDlyHMSM(0,0,0,80,OS_OPT_TIME_DLY, &err_os);
	WAITING_CLICK = 0;

}

// Planta de fato a bomba na posi��o atual do bomberman;
static void Put_Bomb(void) 
{
	BOMB_ON = 1; // Flag que indica uma bomba no mapa

	if (placed_bombs == MAX_BOMBS) // Caso o player tente colocar mais bombas que o permitido, perde o jogo.
	{
		Finish_Game();
		return;
	};

	BOMBS[num_bombs][0] = BOMBERMAN_POS_X;
	BOMBS[num_bombs][1] = BOMBERMAN_POS_Y;
	BOMBS[num_bombs][2] = BOMBS_DELAY;

	num_bombs++;
	placed_bombs++;

	LABIRINTO[BOMBERMAN_POS_Y][BOMBERMAN_POS_X] = 6; // Define na matrix de posi��es a posi��o da bomba
}

// Cria a task respons�vel pela explos�o da bomba na posi��o x = bomb[0], y = bomb[1]
static void Explode(int bomb[2])
{
	OS_ERR  err_os;
	int x = bomb[0];
	int y = bomb[1];

	OSTaskCreate((OS_TCB     *)&Explosion_TaskTCB,                
		(CPU_CHAR   *)"Explosion TASK",
		(OS_TASK_PTR ) Explosion_Task,
		bomb, // array contendo a posi��o central da explos�o passada para a task Explosion_Task.
		(OS_PRIO     ) 5,
		(CPU_STK    *)&Explosion_TaskStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);

	num_bombs--;
}

// Inicializa a matrix de bombas com 0 (n�o h� nenhuma no in�cio do jogo).
static void Initiate_Bombs_Matrix(void)
{
	int i;
	int j;

	for (i = 0 ; i < 1000; i++) 
	{
		for (j = 0; j < 3; j++) 
		{
			BOMBS[i][j] = 0; // Todas as bombas poss�veis s�o inicializadas na posi��o (0,0) pois est� (fora do mapa jog�vel) .
		}
	}
}

// Finaliza o jogo
static void Finish_Game(void) 
{
	OS_ERR  err_os;

	if (GOD_MODE == 1) 
	{
		printf("\nDeuses nunca perdem o jogo.");
		return;
	}

	// Caso o usu�rio perca o jogo, todas as tasks s�o mortas.
	printf("PERDEU");
	OSTaskDel(&Player_TaskTCB, &err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
	OSTaskDel(&Bombs_TaskTCB, &err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
	OSTaskDel(&AppStartTaskTCB, &err_os);
	APP_TEST_FAULT(err_os, OS_ERR_NONE);
}

// Step 4: the Window Procedure
LRESULT CALLBACK HandleGUIEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

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
			Make_Move(1);
			break;

		case VK_RIGHT:
			// Insert code here to process the RIGHT ARROW key
			Make_Move(3);
			break;

		case VK_UP:
			// Insert code here to process the UP ARROW key
			Make_Move(2);
			break;

		case VK_DOWN:
			// Insert code here to process the DOWN ARROW key
			Make_Move(4);
			break;

		case VK_SPACE:
			Make_Move(5);
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