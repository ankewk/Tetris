#include "Conf.h"
#include "DrawWindow.h"

void gameEnd(void);                         // 结束游戏
void gameStop(void);                        // 暂停游戏
void showGame(void);                        // 显示游戏
void gameSelf(int signo);                   // 游戏自动运行
void checkDeleteLine(void);                 // 检查是否满一行
void checkGameOver(char bl[NR][NR]);        // 检查是否游戏结束
int  checkMove(char bl[NR][NR], int flag);  // 检查方块是否可移动
int  getInput(void);                        // 获取输入

void gameEnd(void)
{
    /* 配置游戏结束后的界面 */
    state = "\033[31m游戏结束!!\033[0m";
    drawBlock(bl[type][diret]);
    showGame();

    /* 恢复终端设置 */
    system("stty icanon");          // 恢复缓冲
    system("stty echo");            // 恢复回显
    fprintf(stdout,"\033[?25h");    // 恢复鼠标显示

    /* 尾部处理 */
    fprintf(stdout,"\033[200;1H");  // 定位光标到最后一行
    fclose(fp);                     // 关闭文件
    exit(0);                        // 退出程序
}

void gameStop(void)
{
    /* 创建一个暂停的是时钟 */
    struct itimerval stop = {0}, older;

    /* 设置新闹钟并存储旧闹钟 */
    setitimer(ITIMER_REAL,&stop,&older);

    /* 配置暂停后的界面 */
    state = "\033[31m暂停中...\033[0m";
    startY--;             // 为了防止按下暂停键后方块下滑一格
    drawBlock(bl[type][diret]);
    showGame();
    cleanBlock(bl[type][diret]);

    /* 等待用户按开始键或退出键 */
    int key;
    while (1)
    {
        key = fgetc(stdin);

        /* 空格开始 */
        if (key == ' ')
            break;

        /* q 退出 */
        else if (key == 'q')
            gameEnd();
    }

    /* 恢复闹钟和游戏 */
    setitimer(ITIMER_REAL,&older,NULL);
    state = "\033[32m游戏中...\033[0m";
}

void showGame(void)
{
    int i, j;

    /* 定位到第一行第一列 */
    fprintf(stdout,"\033[1;1H");
    fflush(stdout);

    /* 打印所有数据 */
    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if (gameArea[i][j] == 0)       // 空白区域
            {
                fprintf(stdout,"  ");
            }
            else if (gameArea[i][j] == 8)  // 边界区域
            {
                fprintf(stdout,"\033[40m  \033[0m");
            }
            else                           // 方块区域
            {
                fprintf(stdout,"\033[%dm[]\033[0m",gameArea[i][j]+40);
            }
        }
        fputc('\n',stdout);
    }

    /* 打印提示信息 */
    fprintf(stdout,"\033[2;3H\033[33m【下一个】\033[0m\n");
    fprintf(stdout,"\033[2;15H当前级别:\033[36m%u\033[0m\n",level);
    fprintf(stdout,"\033[3;15H当前分数:\033[32m%u\033[0m\n",score);
    fprintf(stdout,"\033[4;15H最高记录:\033[35m%u\033[0m\n",maxScore);
    fprintf(stdout,"\033[5;15H当前状态:%s\n",state);
}

void gameSelf(int signo)
{
    /* 画方块 */
    drawBlock(bl[type][diret]);

    /* 显示游戏 */
    showGame();

    /* 清除方块 */
    cleanBlock(bl[type][diret]);

    /* 如果方块已经到底 */
    if (!checkMove(bl[type][diret],DOWN))
    {
        /* 检查是否游戏结束 */
        checkGameOver(bl[type][diret]);

        /* 保留已经到底的方块 */
        drawBlock(bl[type][diret]);

        /* 显示游戏结果 */
        showGame();

        /* 到达边界后检查是否可消行 */
        checkDeleteLine();

        /* 重新开始下一个方块 */
        startY = 6;
        startX = 7;
        type = nextType;
        nextType = rand()%7;
        diret = 0;
    }
}

void checkDeleteLine(void)
{
    int i, j;
    int x, y;

    /* 检查当前方块的四行区域内 */
    for (i = 3; i >= 0; i--)
    {
        for (j = 1; j < COL-1; j++)
        {
            /* 检测方块是否满一行 */
            if (gameArea[startY+i][j] == 0)
                break;
            /* 跳过边框区域 */
            else if (gameArea[startY+i][j] == 8)
                break;
        }
        /* 如果满了一行则删除一行 */
        if (j == COL-1)
        {
            /* 删除满了的一行 */
            for (j = 1; j < COL-1; j++)
            {
                gameArea[startY+i][j] = 0;
            }

            /* 分数累加 */
            score += 100;

            /* 记录最高分 */
            if (score > maxScore)
            {
                maxScore = score;

                /* 保存最高分 */
                rewind(fp);
                fprintf(fp,"%u\n",maxScore);
            }

            /* 记录级别 */
            if (score%200 == 0)
            {
                /* 每200分加一级 */
                level++;
            }

            /* 删除后往下移动一行 */
            for (x = 1; x < COL-1; x++)
            {
                for (y = startY+i; y >= 7; y--)
                {
                    gameArea[y][x] = gameArea[y-1][x];
                }
            }

            /* 移动的一行需要检测范围加一行 */
            i++;
        }
    }
}

void checkGameOver(char block[NR][NR])
{
    int i;

    for (i = 0; i < NR; i++)
    {
        /* 方块碰到上方边界则游戏结束 */
        if (block[0][i] != 0 && gameArea[startY-1][startX+i] == 8)
        {
            gameEnd();
        }
    }
}

int checkMove(char bl[NR][NR], int flag)
{
    int m, n;   // 用于标明可移动方向
    int x, y;   // 用于循环

    switch (flag)
    {
        case RIGHT : n = 0; m = 1;  break;
        case LEFT  : n = 0; m = -1; break;
        case DOWN  : n = 1; m = 0;  break;
    }

    /* 全局检查 */
    for (y = 0; y < NR; y++)
    {
        for (x = 0; x < NR; x++)
        {
            /* 只能判断到达了边界 */
            if (bl[y][x] != 0 && gameArea[startY+y+n][startX+x+m] != 0)
            {
                return 0;
            }
        }
    }

    /* 出来说明没有到达边界 */
    startY += n;
    startX += m;

    return 1;
}

int getInput(void)
{
    char key;

    key = fgetc(stdin);

    if (key == '\033' && fgetc(stdin) == '[') // 方向键
    {
        switch (fgetc(stdin))
        {
            case 'A': return CHANGE;
            case 'B': return DOWN;
            case 'C': return RIGHT;
            case 'D': return LEFT;
        }
    }
    else if (key == 'q')    // 退出键
    {
        return EXIT;
    }
    else if (key == ' ')    // 空格键-暂停游戏
    {
        return STOP;
    }
    else                    // 其它不相关的键
        return UNKNOW;
}