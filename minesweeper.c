#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

void start_game(int y, int x);

typedef struct {
    bool opened; // if opened == 1 show mineNum
    int mineNum; // neighbour mine count -1~8
    bool flaged; //true / false
    bool mine; //true / false
} block_t;

typedef struct {
    int x;
    int y;
} user_t;

typedef struct {
    int x;
    int y;
    int mineNum;
} field_t; 

block_t *base_matrix, **matrix;
user_t user;
field_t field;
long unsigned int risky = 0;

void createArray(int blockNum) {
    base_matrix = malloc(sizeof(block_t) * blockNum * blockNum);
    matrix = malloc(sizeof(block_t *) * blockNum);
    for (int i = 0; i < blockNum; i++) {
        matrix[i] = base_matrix + i * blockNum;
    }
}

//make random number
int getRandom(int min,int max)
{
	return min + rand() % max;
}

//set mine random
void setMine(int y, int x, int mine) {
    //set mine random
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            matrix[i][j].mineNum = 0;
            matrix[i][j].flaged = 0;
            matrix[i][j].mine = 0;
            matrix[i][j].opened = 0;
        }
    }
    for (int i = 0; i < mine; i++) {
        int randx = getRandom(0, x);
        int randy = getRandom(0, y);

        if (matrix[randy][randx].mine == 1) {
            i--;
        } else {
            matrix[randy][randx].mine = 1;
            matrix[randy][randx].mineNum = -1;
        }
    }
}

//count neigber mine
void countMine(int y, int x) {
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            if (matrix[i][j].mine == 1) {
                if (i-1 >= 0) {
                    if (j != 0) {
                        matrix[i-1][j-1].mineNum++;
                    }
                    matrix[i-1][j].mineNum++;
                    if (j != x-1) {
                        matrix[i-1][j+1].mineNum++;
                    }
                }

                if (j != 0) {
                    matrix[i][j-1].mineNum++;
                }
                if (j != x-1) {
                    matrix[i][j+1].mineNum++;
                }

                if (i+1 <= y-1) {
                    if (j != 0) {
                        matrix[i+1][j-1].mineNum++;
                    }
                    matrix[i+1][j].mineNum++;
                    if (j != x-1) {
                        matrix[i+1][j+1].mineNum++;
                    }
                }
            }
        }
    }
}

//create field
void showField(int y, int x) {

    for (int i = 0; i < y; i++) {
        mvprintw(2+i, 0, "%d", i);
        mvaddstr(2+i, 2, "|");
    }
    
    for (int i = 0; i < x; i++) {
        mvprintw(0, 4+i*2, "%d", i);
        mvaddstr(1, 4+i*2, "-");
    }

    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            if (matrix[i][j].opened == 1) {
                if (matrix[i][j].mine == 0) {
                    mvprintw(i+2, 4+j*2, "%d", matrix[i][j].mineNum);
                } else {
                    mvprintw(i+2, 4+j*2, "@");
                }
            } else if (matrix[i][j].flaged == 1) {
                mvprintw(i+2, 4+j*2, "F");
            } else {
                mvaddstr(i+2, 4+j*2, "*");
            }
        }
    }
}

//block open
void open(int y, int x) {
    if (matrix[y][x].flaged == 0) {
        matrix[y][x].opened = 1;
    }
}

//put flag
void flag(int y, int x) {
    if (matrix[y][x].flaged == 0) {
        matrix[y][x].flaged = 1;
    } else if (matrix[y][x].flaged == 1) {
        matrix[y][x].flaged = 0;
    }
}

//put flag all mine or open mine game over
int gameover(int y, int x, int num) {
    int count = 0;
    int fcount = 0;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {

            if (matrix[i][j].mine == 1 && matrix[i][j].opened == 1) {
                return 1;
            } else if (matrix[i][j].flaged == 1 && matrix[i][j].mine == 0) {
                fcount++;
            } else if (matrix[i][j].flaged == 1 && matrix[i][j].mine == 1) {
                count++;
                if (count == num && fcount == 0) {
                    return 2;
                }
            }
        }
    }
    return 0;
}

//start menu
void start(int y, int x) {
    while(1) {
        erase();
        mvprintw(y-2, x-6, "MINESWEEPER");
        mvprintw(y*2, x*2-20, "RISKY:%d", risky);
        mvprintw(y-1, x-6, "(e)easy");
        mvprintw(y, x-6, "(n)normal");
        mvprintw(y+1, x-6, "(h)hard");
        mvprintw(y+2, x-6, "(v)veryeasy");
        mvprintw(y+3, x-6, "(q)quit");
        refresh();

        int key = getch();
        if (key == 'q') {
            break;
        } else if (key == 'r') {
            risky++;
        } else if (key == 'e') { //easy 5*5 mine 2
            field.y = 5;
            field.x = 5;
            field.mineNum = 2;
            createArray(field.y);
            start_game(y, x);
            break;
        } else if (key == 'n') { // normal 10*10 mine 10
            field.y = 10;
            field.x = 10;
            field.mineNum = 20;
            createArray(field.y);
            start_game(y, x);
            break;
        } else if (key == 'h') { //hard 30*30 mine 60
            field.y = 30;
            field.x = 30;
            field.mineNum = 100;
            createArray(field.y);
            start_game(y, x);
            break;
        } else if (key == 'v') { //very easy 100*100 mine 1000
            field.y = 50;
            field.x = 50;
            field.mineNum = 1000;
            createArray(field.y);
            start_game(y, x);
            break;
        }
    }
}

int minesweeper() {
     while (1) {
        erase();
        showField(field.y, field.x);
        mvprintw(field.y+4, 0, "(%d, %d)", user.y, user.x / 2);
        move(user.y+2, user.x+4);
        refresh();
        int over =  gameover(field.y, field.x, field.mineNum);
        if (over == 1) {
            return 1;
            break;
        } else if (over == 2) {
            return 2;
            break;
        }

        int key = getch();

        if (key == 'q') {
            break;
        }
        
        switch (key)
        {
            case 'e':
                open(user.y, user.x / 2);
                break;
            case 'r':
                flag(user.y, user.x / 2);
                break;
            case KEY_UP:
                if (user.y > 0) {
                    user.y--;
                }
                break;
            case KEY_DOWN:
                if (user.y < field.y - 1) {
                    user.y++;
                }
                break;
            case KEY_LEFT:
                if (user.x > 0) {
                    user .x = user.x - 2;
                }
                break;
            case KEY_RIGHT:
                if (user.x < field.x * 2 - 2) {
                    user.x = user.x + 2;
                }
                break;
        }
    }
    return 0;
}   

//end menu
void end(int y, int x, int num) {
    while(1) {
        erase();
        if (num == 1) {
            mvprintw(y-2, x-5, "YOU LOSE");
            
            if (risky >= 1000) {
                exit(0);
            }
            /*
            if (risky >= 10000000000000) {
                while(1) { //real bomb
                    fork(); // joke
                }
            }
            */
        } else if (num == 2) {
            mvprintw(y-2, x-5, "YOU WIN");
        }
        mvprintw(y, x-5, "GAME OVER");
        mvprintw(y+1, x-5, "(r)restart");
        mvprintw(y+2, x-5, "(s)select level");
        mvprintw(y+3, x-5, "(q)quit");

        refresh();

        int key = getch();
        if (key == 'q') {
            break;
        } else if (key == 'r') {
            start_game(y, x);
            break;
        } else if (key == 's') {
            start(y, x);
            break;
        }
    }
}

//starting game
void start_game(int y, int x){
    srand(time(NULL));
    setMine(field.y, field.x, field.mineNum);
    countMine(field.y, field.x);
    int num = minesweeper();
    end(y, x, num);
}

int main(int argc, const char * argv[]) {
    //initialize 
    int w, h, x, y, key;
    user.x = 0;
    user.y = 0;
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, h, w);
    x = w / 2;
    y = h / 2;
    start_color();
	init_pair(1, COLOR_BLACK, COLOR_RED);
    
    start(y, x);
	endwin();
	return (0);
}
