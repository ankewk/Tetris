#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "DrawWindow.h"
#include "Game.h"

#define   ROW    42     
#define   COL    36     

enum key {
   DOWN,   
   LEFT,    
   RIGHT,   
   CHANGE,  
   STOP,    
   EXIT,    
   UNKNOW,  
};

static char gameArea[ROW][COL] = {0};   // 游戏区域数据
static int startX = 7, startY = 6;      // 方块出现的起始位置
static int type = 0;                    // 方块当前类型
static int nextType = 0;                // 方块的下一种类型
static int diret = 0;                   // 方块的方向
char *state = "\033[32m游戏中...\033[0m";// 游戏运行状态
static unsigned int level = 0;          // 游戏等级
static unsigned int score = 0;          // 游戏分数
static unsigned int maxScore = 0;       // 游戏最高记录
static FILE *fp = NULL;                 // 用于把记录保存到文件

int main(void)
{
    struct itimerval  timer = {{0,700000},{0,700000}};
    setitimer(ITIMER_REAL, &timer,NULL);
    signal(SIGALRM, gameSelf);

    srand(time(NULL));
    type     = rand()%7;
    nextType = rand()%7;

    fp = fopen("./record","r+");
    if (NULL == fp)
    {
        fp = fopen("./record","w");
    }
    fscanf(fp,"%u",&maxScore);

    int key;
    while (1)
    {
        key = getInput();
        switch (key)
        {
            case RIGHT : checkMove(bl[type][diret],RIGHT);
                         break;
            case LEFT  : checkMove(bl[type][diret],LEFT);
                         break;
            case DOWN  : checkMove(bl[type][diret],DOWN);
                         break;
            case CHANGE: turnBlock(bl[type][(diret+1)%4]);
                         break;
            case STOP  : gameStop();
                         break;
            case EXIT  : gameEnd();
                         break;
            case UNKNOW: continue;
        }
        initalGameArea();
        showGame();
        drawBlock(bl[type][diret]);
    }
    return 0;
}