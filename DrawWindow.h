#include "Conf.h"
#include "Game.h"

void initalGameArea(void);                  // 初始化游戏区域
void drawBlock(char bl[NR][NR]);            // 画方块
void cleanBlock(char bl[NR][NR]);           // 清除方块
void turnBlock(char bl[NR][NR]);            // 旋转方块

void initalGameArea(void)
{
    int i;
    printf("\033[2J");            
    system("stty -icanon");     
    system("stty -echo");        
    fprintf(stdout,"\033[?25l"); 

    for (i = 0; i < COL; i++)
    {
        gameArea[0][i]     = 8;   
        gameArea[5][i]     = 8;   
        gameArea[ROW-1][i] = 8;  
    }

    for (i = 0; i < ROW; i++)
    {
        gameArea[i][0]     = 8;  
        gameArea[i][COL-1] = 8;  
    }

    for (i = 1; i < 5; i++)
    {
        gameArea[i][6] = 8;
    }
}

//画方块
void drawBlock(char block[NR][NR])
{
    int x, y;

    for (y = 0; y < NR; y++)
    {
        for (x = 0; x < NR; x++)
        {
            if (block[y][x] != 0)
            {
                gameArea[startY+y][startX+x] = block[y][x];
            }
        }
    }

    for (x = 0; x < 2; x++)
    {
        for (y = 0; y < NR; y++)
        {
            if (bl[nextType][0][x][y] != 0)
                gameArea[3+x][2+y] = bl[nextType][0][x][y];
            else
                gameArea[3+x][2+y] = 0;
        }
    }
}

//清除方块
void cleanBlock(char bl[NR][NR])
{
    int x, y;

    for (y = 0; y < NR; y++)
    {
        for (x = 0; x < NR; x++)
        {
            if (bl[y][x] != 0)
            {
                gameArea[startY+y][startX+x] = 0;
            }
        }
    }
}

void turnBlock(char bl[NR][NR])
{
    int x, y;

    /* 检查是否越界 */
    for (y = 0; y < NR; y++)
    {
        for (x = 0; x < NR; x++)
        {
            /* 只能判断到达了边界 */
            if (bl[y][x] != 0 && gameArea[startY+y][startX+x] != 0)
            {
                return;
            }
        }
    }

    /* 两边都没有越界则旋转方块方向 */
    diret = (diret+1)%4;
}