
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncursesw/ncurses.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

int gameBoard[BOARD_WIDTH][BOARD_HEIGHT] = {0};
bool should_create_tetro = false;


void clearTerminal() {
    refresh();
    //system("clear");
    //printw("\e[1;1H\e[2J"); // ANSI escape code to clear the terminal
}

void create_tetro(){
    gameBoard[2][BOARD_HEIGHT -1] = 3;
    gameBoard[4][BOARD_HEIGHT -1] = 3;
    gameBoard[3][BOARD_HEIGHT -1] = 3;
    should_create_tetro = false;
}
void drawGameBoard() {
    clearTerminal();

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
               mvprintw(BOARD_HEIGHT-y-1,x,"%d",gameBoard[x][y]); // Occupied cell
        }
        printw("\n"); // Newline at the end of each row
    }
}

void clean_board(){
   for (int y = 0; y < BOARD_HEIGHT ; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (gameBoard[x][y] == 3){
                gameBoard[x][y] = 1;
            }
        }
    }
}

void gravity(int mov){
   for (int y = 0; y < BOARD_HEIGHT ; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (gameBoard[x][y] == 0){
                if(y+1 < BOARD_HEIGHT && gameBoard[x][y+1] == 3){
                    if(x + mov >= 0 && x + mov < BOARD_WIDTH){
                         gameBoard[x+mov][y] = 3;
                    }else{
                        gameBoard[x][y] = 3;
                    }
                }
            }else{
                if(gameBoard[x][y] == 3){

                if(y-1 >= 0){
                    if(gameBoard[x][y-1] == 3 ){
                        gameBoard[x][y] = 0;
                    }
                        if(gameBoard[x][y-1] == 1){
                            should_create_tetro = true;
                        }
                }
                if(y == 0) should_create_tetro = true;
                }
            }
        }
    }
}

int main() {
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, TRUE);
   nodelay(stdscr, TRUE);

    int ch ;
    int movement = 0;
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            gameBoard[x][y] = 0;
        }
    }
    create_tetro();
    while (1) {
       drawGameBoard();
       ch = getch();
        if(ch == KEY_LEFT) movement =-1 ;
        if(ch == KEY_RIGHT) movement =1 ;
       gravity(movement);
        movement = 0;
       if(should_create_tetro){
            clean_board();
            create_tetro();
        }
       usleep(900000);
    }

    endwin();
    return 0;
}
