#include <stdio.h>
#include <windows.h> // Sleep
#include <conio.h>  // _kbhit, _getch
#include <ctype.h>  // tolower
#include <time.h>   // time, rand

#define ROW 30
#define COL 30
#define TICK 100
char map[ROW][COL];  // 地图
typedef struct {
    char type;
    int x; // 列
    int y; // 行
} snake;
int snake_length = 4;
snake snake_arr[20];
typedef struct {
    int state;
    int x;
    int y;
} fruit;
fruit fruit_arr[1];
int fruit_num = 1;
typedef struct {
    int x;
    int y;
} Position;
Position old_tail;

char direction = 'd';   // w: 0,-1 \ a: -1,0 \ s: 0,1 \ d: 1,0
char next_dir = 'd';

int game_over = 0;

void gotoxy(int x, int y) {
/**
 * @brief 将控制台光标移动到指定坐标位置
 * 
 * @param x 目标位置的列坐标（控制台横向位置，从0开始）
 * @param y 目标位置的行坐标（控制台纵向位置，从0开始）
 * 
 * @details 函数实现细节：
 * 1. COORD 是Windows API中定义的结构体，包含X和Y两个SHORT类型成员
 *    - SHORT 是Windows定义的16位有符号整数类型
 * 2. GetStdHandle(STD_OUTPUT_HANDLE) 获取标准输出设备的句柄
 *    - STD_OUTPUT_HANDLE 是标准输出设备的标识符
 * 3. SetConsoleCursorPosition 是Windows API函数，用于设置控制台光标位置
 *    - 第一个参数是控制台句柄
 *    - 第二个参数是COORD类型的坐标结构
 * 
 * @note 注意事项：
 * - 坐标原点(0,0)在控制台窗口的左上角
 * - X代表列号，向右递增；Y代表行号，向下递增
 * - 该函数仅适用于Windows平台
 * - 如果坐标超出控制台缓冲区大小，实际位置会被自动限制在有效范围内
 * 
 * @example 使用示例：
 * gotoxy(10, 5);  // 将光标移动到第6行第11列（从0开始计数）
 */
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 新版printmap：使用缓冲区(buffer)优化
void printmap() {
    char buffer[COL*ROW*2 + ROW + 1]; // 分配足够大的缓冲区（考虑空格+换行符+结尾\0）
    int pos = 0; // 缓冲区当前位置指针

    // 构建缓冲区内容（内存中组装完整字符串）
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            buffer[pos++] = ' ';  // 每个格子前加空格保持对齐
            buffer[pos++] = map[i][j]; // 添加地图内容
        }
        buffer[pos++] = '\n'; // 每行末尾加换行符
    }
    buffer[pos++] = '\0'; // 字符串结束符（必须要有！）

    system("cls"); 
    // gotoxy(0, 0); // 可以省略，因为cls后光标自动在左上角
    printf("%s", buffer); // 关键区别！用%s一次性输出整个缓冲区
}

// -------------------------------------------------------------------
// 旧版printmap：直接逐个字符输出
// void printmap() {
//     system("cls"); // 📌 每次清屏会导致明显闪烁
//     gotoxy(0, 0);
    
//     // 📌 双重循环逐个输出（20x20地图需要400次printf调用）
//     for (int i = 0; i < ROW; i++) {
//         for (int j = 0; j < COL; j++) {
//             printf(" %c", map[i][j]); // 📌 每个格子单独输出（低效）
//         }
//         printf("\n"); // 📌 每行结束单独换行
//     }
//     // 📌 总输出次数：ROW*(COL + 1) = 420次函数调用（20x21）
// }


void update_fruit() {
    // 检测水果是否存在
    for (int i = 0; i < fruit_num; i++)
    {
        if (!fruit_arr[i].state)
        {
            int conflict;
            // 不存在就随机加入到地图
            // 不断生成坐标直到确保不会和蛇身重叠
            do {
                conflict = 0;
                fruit_arr[i].x = rand() % COL;
                fruit_arr[i].y = rand() % ROW;
                for (int j = 0; j < snake_length; j++)
                {
                    if (fruit_arr[i].x == snake_arr[j].x && fruit_arr[i].y == snake_arr[j].y) {
                        conflict = 1;
                        break;
                    }
                }
            } while (conflict);

            map[fruit_arr[i].y][fruit_arr[i].x] = '@';
            fruit_arr[i].state = 1;
        }else {
            // 如果水果已存在，确保它在地图上显示
            map[fruit_arr[i].y][fruit_arr[i].x] = '@';
        }
    }
}

void update_map() {
    // 初始化地图
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            map[i][j] = '.';
        }
    }

    // 画蛇
    for (int i = 0; i < snake_length; i++) {
        map[snake_arr[i].y][snake_arr[i].x] = snake_arr[i].type;
    }

    // 最后绘制蛇头（确保在最上层）
    map[snake_arr[0].y][snake_arr[0].x] = snake_arr[0].type;

    update_fruit();
}

void movesnake() {
    direction = next_dir;

    // 保存最后一条尾巴的位置
    old_tail.x = snake_arr[snake_length - 1].x;
    old_tail.y = snake_arr[snake_length - 1].y;

    // 移动蛇身
    for (int i = snake_length - 1; i > 0; i--)
    {
        snake_arr[i].x = snake_arr[i - 1].x;
        snake_arr[i].y = snake_arr[i - 1].y;
    }
    
    // 移动蛇头
    switch (direction) {
        case 'w': snake_arr[0].y--; break;
        case 's': snake_arr[0].y++; break;
        case 'a': snake_arr[0].x--; break;
        case 'd': snake_arr[0].x++; break;
    }

    // 判断边界从另一边出来
    if (snake_arr[0].x < 0) snake_arr[0].x = COL - 1;
    if (snake_arr[0].x >= COL) snake_arr[0].x = 0;
    if (snake_arr[0].y < 0) snake_arr[0].y = ROW -1;
    if (snake_arr[0].y >= ROW) snake_arr[0].y = 0;
}

void add_snake(int num) {
    snake_length++;
    snake_arr[snake_length - 1] = (snake){'+', old_tail.x, old_tail.y};

    // 重置水果状态
    for (int i = 0; i < fruit_num; i++)
    {
        if (fruit_arr[i].x == snake_arr[0].x && fruit_arr[i].y == snake_arr[0].y)
        {
            fruit_arr[i].state = 0;
        }
    }
    
}

void hit_check() {
    if (map[snake_arr[0].y][snake_arr[0].x] == '+')
    {
        game_over = 1;
    }else if (map[snake_arr[0].y][snake_arr[0].x] == '@')
    {
        add_snake(1);
    }
    
    
}

int main() {
    // 初始化随机数种子，保证每次运行生成不同的随机数序列
    // srand() 用来设置随机数生成器的起始点（种子）
    // time(NULL) 返回当前时间（单位：秒），因为时间不断变化，所以种子每次都不同
    // 强制转换成 unsigned int 是为了和 srand() 的参数类型匹配
    // 这样调用后，后续使用 rand() 生成的随机数才不会每次都一样，保证随机性
    // rand() % 20就是在0-19之间
    srand((unsigned int)time(NULL));

    // 初始化蛇
    snake_arr[0] = (snake){'#', 10, 10};
    snake_arr[1] = (snake){'+', 9, 10};
    snake_arr[2] = (snake){'+', 8, 10};
    snake_arr[3] = (snake){'+', 7, 10};

    // 初始化果实
    fruit_arr[0] = (fruit){0, -1, -1};

    while (1)
    {
        // // 独立检测ESC键（不会被_getch消耗）
        // // 检测ESC键是否被按下：
        // // 1. GetAsyncKeyState(VK_ESCAPE) 调用Windows API检查ESC键状态
        // //    - VK_ESCAPE 是ESC键的虚拟键码(0x1B)
        // // 2. & 0x8000 是位掩码操作，用于提取返回值的最高位(第15位)
        // //    - 0x8000 的二进制: 10000000 00000000
        // //    - 若最高位为1，表示按键当前正被按下
        // // 3. 整个表达式结果为非零值时，表示ESC键正处于按下状态
        // // GetAsyncKeyState检测的是瞬时状态，游戏循环有500ms延迟，按键可能被错过
        // if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        // printf("退出程序。\n");
        // break;
        // }

        if (game_over)
        {
            printf("you stupid ass bitch..\nhow the fuck did you die?.\nget better next time u retard\nwaste of code.");
            break;
        }
        
        
        // 处理输入
        while (_kbhit())    // 处理所有缓冲的按键
        {
            char key = tolower(_getch());   // 读取这个键
            // ESC键检测
            if (key == 27) {    // 27 是 ESC 键的 ASCII 值
                printf("退出程序。\n");
                return 0;
            }

            // 只记录有效的方向键，并防止180度转弯
            switch (key) {
                case 'w': if (direction != 's') next_dir = 'w'; break;
                case 's': if (direction != 'w') next_dir = 's'; break;
                case 'a': if (direction != 'd') next_dir = 'a'; break;
                case 'd': if (direction != 'a') next_dir = 'd'; break;
            }
        }

        movesnake();
        hit_check();
        update_map();
        printmap();
        printf("X: %d,Y: %d\n", snake_arr[0].x, snake_arr[0].y);
        printf("Snake Head: (%d, %d), Fruit: (%d, %d)\n", snake_arr[0].x, snake_arr[0].y, fruit_arr[0].x, fruit_arr[0].y);
        Sleep(TICK);
    }

    return 0;
}

