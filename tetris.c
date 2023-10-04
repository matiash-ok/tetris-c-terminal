
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

typedef struct Tetroid {
    int x ;
    int y ;
    bool stopped;
}tetro;

tetro tetroid;
void clearTerminal() {
    refresh();
    //system("clear");
    //printw("\e[1;1H\e[2J"); // ANSI escape code to clear the terminal
}

void monitor_screen(char * message){
mvprintw(7,12,"%s",message);
}

void create_tetro(){
    gameBoard[2][BOARD_HEIGHT -1] = 3;
    should_create_tetro = false;
    tetroid.x = 2 ;
    tetroid.y = BOARD_HEIGHT - 1 ;
    tetroid.stopped = false;
}
void drawGameBoard() {
    clearTerminal();

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
               mvprintw(BOARD_HEIGHT-y-1,x,"%d",gameBoard[x][y]); // Occupied cell
        }

        if(tetroid.y == y){
            init_pair(1, COLOR_RED, COLOR_BLACK); // Define a color pair (1) with red text on a black background
            attron(COLOR_PAIR(1)); // Enable color pair 1 (red text)
            printw(" %d \n",y); // Newline at the end of each row
            attroff(COLOR_PAIR(1));
        }else{
            printw(" %d \n",y); // Newline at the end of each row
        }
    }
    mvprintw(5,14,"tetro X : %d tetro Y : %d",tetroid.x ,tetroid.y);
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
void move_tetro(int mov){
   int tetroY = tetroid.y ;
   int tetroX = tetroid.x ;

    if(tetroX + mov < BOARD_WIDTH && tetroX + mov >= 0 && gameBoard[tetroX+mov][tetroY]== 0 ){
        gameBoard[tetroX][tetroY] = 0;
        gameBoard[tetroX + mov][tetroY] = 3;
        tetroid.x = tetroX + mov;
        drawGameBoard();
    }

}
void gravity(){
   for (int y = 0; y < BOARD_HEIGHT ; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
           if(gameBoard[x][y] == 0){

                if(y+1< BOARD_HEIGHT && gameBoard[x][y+1] == 3){
                    gameBoard[x][y] = 3;
                    monitor_screen("move down");
                }
           }

            if(gameBoard[x][y] == 3){
                if(gameBoard[x][y-1] == 3){
                    gameBoard[x][y] = 0;
                    tetroid.y = y-1;
                }
              //  if(gameBoard[next_mov][y+1] == 1) gameBoard[x][y] = 1;
            }
        }
    }
    drawGameBoard();
}

int main() {
    initscr();
    cbreak();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

   time_t startTime, currentTime;
   int elapsedSeconds = 3;
   int ch ;
   int add_gravity = 0 ;
   const int GRAVITY_FLAG = 10;
   int movement = 0;
   for (int x = 0; x < BOARD_WIDTH; x++) {
       for (int y = 0; y < BOARD_HEIGHT; y++) {
           gameBoard[x][y] = 0;
       }
   }
   create_tetro();

   time(&startTime);
   while (1) {
      time(&currentTime);
      int secondsElapsed = difftime(currentTime, startTime);
      if(secondsElapsed >= elapsedSeconds){
          gravity();
          time(&startTime);
      }
      drawGameBoard();
      ch = getch();
       if(ch == KEY_LEFT)move_tetro(-1) ;
       if(ch == KEY_RIGHT)move_tetro(+1) ;
      if(should_create_tetro){
           clean_board();
           create_tetro();
       }
      usleep(200000);
    }

    endwin();
    return 0;
}
