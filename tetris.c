
#include <curses.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncursesw/ncurses.h>


#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

enum tetris_forms {L , Z , Square , Line , T , rL , rZ };

typedef struct Tetroid {
    int x ;
    int y ;
    int mode;
}tetro;

int gameBoard[BOARD_WIDTH][BOARD_HEIGHT + 4] = {0};
bool should_create_tetro = false;
bool stopped = false;
int spin = 0;
char tetro_type;
tetro tetroid;
tetro tetroid1;
tetro tetroid2;
tetro tetroid3;

void clearTerminal() {
    refresh();
}

void create_tetro(){
   // int rand_num = (rand() % 7) + 1;
    int rand_num = 0 ;
    int starting_x = BOARD_WIDTH/2;
    switch (rand_num) {
        case L:
            // L shape tetro
            // main tetroid
            gameBoard[starting_x][BOARD_HEIGHT+1] = 3;
            tetroid.x = starting_x;
            tetroid.y = BOARD_HEIGHT + 1  ;

            //other tetroid
            gameBoard[starting_x][BOARD_HEIGHT+2] = 2;
            tetroid1.x = starting_x;
            tetroid1.y = BOARD_HEIGHT + 2 ;

            gameBoard[starting_x][BOARD_HEIGHT] = 4;
            tetroid2.x = starting_x;
            tetroid2.y = BOARD_HEIGHT;

            gameBoard[starting_x + 1][BOARD_HEIGHT] = 5;
            tetroid3.x = starting_x + 1;
            tetroid3.y = BOARD_HEIGHT;
            break;
      //  case 2: create_tetro_form(2);
      //  case 3: create_tetro_form(3);
      //  case 4: create_tetro_form(4);
      //  case 5: create_tetro_form(5);
      //  case 6: create_tetro_form(6);
      //  case 7: create_tetro_form(7);
    }
    should_create_tetro = false;
  }

int tetroblock_can_spin(int*xs , int*ys,int spin){
        for(int i = 0 ; i < 3 ; i++){
            if(    xs[i] <  0 && xs[i] >= BOARD_WIDTH
                && ys[i] < 0  && ys[i] >= BOARD_HEIGHT
                && gameBoard[xs[i]][ys[i]] == 1
            ){
                return false;
            }
    }
    return true;
}
bool tetroblock_can_move(int mov){
    return (
        !stopped &&
       (tetroid.x  + mov >= 0 && tetroid.x  + mov < BOARD_WIDTH) &&
       (tetroid1.x + mov >= 0 && tetroid1.x + mov < BOARD_WIDTH) &&
       (tetroid2.x + mov >= 0 && tetroid2.x + mov < BOARD_WIDTH) &&
       (tetroid3.x + mov >= 0 && tetroid3.x + mov < BOARD_WIDTH) &&

        (gameBoard[tetroid.x+mov][tetroid.y] == 0 ||
         gameBoard[tetroid.x+mov][tetroid.y] == 2 ||
         gameBoard[tetroid.x+mov][tetroid.y] == 3 ||
         gameBoard[tetroid.x+mov][tetroid.y] == 4 ||
         gameBoard[tetroid.x+mov][tetroid.y] == 5 )
    &&

        (gameBoard[tetroid1.x+mov][tetroid1.y] == 0 ||
         gameBoard[tetroid1.x+mov][tetroid1.y] == 2 ||
         gameBoard[tetroid1.x+mov][tetroid1.y] == 3 ||
         gameBoard[tetroid1.x+mov][tetroid1.y] == 4 ||
         gameBoard[tetroid1.x+mov][tetroid1.y] == 5)
    &&

        (gameBoard[tetroid2.x+mov][tetroid2.y] == 0 ||
         gameBoard[tetroid2.x+mov][tetroid2.y] == 2 ||
         gameBoard[tetroid2.x+mov][tetroid2.y] == 3 ||
         gameBoard[tetroid2.x+mov][tetroid2.y] == 4 ||
         gameBoard[tetroid2.x+mov][tetroid2.y] == 5)
    &&

        (gameBoard[tetroid3.x+mov][tetroid3.y] == 0 ||
         gameBoard[tetroid3.x+mov][tetroid3.y] == 2 ||
         gameBoard[tetroid3.x+mov][tetroid3.y] == 3 ||
         gameBoard[tetroid3.x+mov][tetroid3.y] == 4 ||
         gameBoard[tetroid3.x+mov][tetroid3.y] == 5)
    );
};

void drawGameBoard() {
    clearTerminal();

    for (int y = BOARD_HEIGHT -1; y >= 0; y--) {
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
    mvprintw(5,14,"3 tetro  X : %d tetro Y : %d",tetroid.x  ,tetroid.y);
    mvprintw(6,14,"2 tetro1 X : %d tetro Y : %d",tetroid1.x ,tetroid1.y);
    mvprintw(7,14,"4 tetro2 X : %d tetro Y : %d",tetroid2.x ,tetroid2.y);
    mvprintw(8,14,"5 tetro3 X : %d tetro Y : %d",tetroid3.x ,tetroid3.y);
    mvprintw(10,14,"stopped : %d",stopped);
    int rotation = pow(-1,spin/2);
    mvprintw(11,14,"spin : %d rotation:%d",spin,rotation);

}

void clean_board(){
}

int get_rotation(int spin ){
    return pow(-1,spin/2);
}
void spin_tetro(){
            int xs[] = { tetroid1.x + 1 * get_rotation(spin +1),
                         tetroid2.x - 1 * get_rotation(spin+1),
                         tetroid3.x + 1 * (get_rotation(spin+2)+get_rotation(spin+3))
                        };
            int ys[] = {tetroid1.y - 1 * get_rotation(spin +1),
                        tetroid2.y + 1 * get_rotation(spin+1),
                        tetroid3.y + 1 * (get_rotation(spin+3)+get_rotation(spin))
                       };
    bool flag_spin ;
    switch(tetro_type){
        case L:
            flag_spin = tetroblock_can_spin(xs,ys,spin);
            if(flag_spin) break;
            gameBoard[tetroid1.x][tetroid1.y] = 0;
            gameBoard[tetroid2.x][tetroid2.y] = 0;
            gameBoard[tetroid3.x][tetroid3.y] = 0;

            tetroid1.x = xs[0];
            tetroid1.y = ys[0];

            gameBoard[tetroid1.x][tetroid1.y] = 2;

            tetroid2.x = xs[1];
            tetroid2.y = ys[1];

            gameBoard[tetroid2.x][tetroid2.y] = 4;

            tetroid3.x = xs[2];
            tetroid3.y = ys[2];

            gameBoard[tetroid3.x][tetroid3.y] = 5;

            if(spin == 4 ){
                spin = 0;
            };
            spin++;
            drawGameBoard();
            break;
    }
};

void move_tetro(int mov){

    if(tetroblock_can_move(mov)){
        gameBoard[tetroid.x][tetroid.y] = 0;
        gameBoard[tetroid1.x][tetroid1.y] = 0;
        gameBoard[tetroid2.x][tetroid2.y] = 0;
        gameBoard[tetroid3.x][tetroid3.y] = 0;

        gameBoard[tetroid.x + mov][tetroid.y] = 3;
        tetroid.x = tetroid.x + mov;

        gameBoard[tetroid1.x + mov][tetroid1.y] = 2;
        tetroid1.x = tetroid1.x + mov;

        gameBoard[tetroid2.x + mov][tetroid2.y] = 4;
        tetroid2.x = tetroid2.x + mov;

        gameBoard[tetroid3.x + mov][tetroid3.y] = 5;
        tetroid3.x = tetroid3.x + mov;

        drawGameBoard();
    }

}
void gravity(){
   for (int y = 0; y < BOARD_HEIGHT+4 ; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
           if(gameBoard[x][y] == 0){

                if(y+1<= BOARD_HEIGHT){
                       gameBoard[x][y] = gameBoard[x][y+1];
                }
           }

            if(!stopped &&
               gameBoard[x][y] == 3 ||
               gameBoard[x][y] == 2 ||
               gameBoard[x][y] == 4 ||
               gameBoard[x][y] == 5
            ){
                if(y-1 >= 0 && y+1 < BOARD_HEIGHT + 4 && gameBoard[x][y-1] != 1){
                    switch(gameBoard[x][y]){
                        case 3: tetroid.y = y-1  ;break;
                        case 2: tetroid1.y = y-1 ;break;
                        case 4: tetroid2.y = y-1 ;break;
                        case 5: tetroid3.y = y-1 ;break;
                    }
                    gameBoard[x][y] = gameBoard[x][y+1];
                }else{
                    stopped = true;
                }
            }
        }
    }
    drawGameBoard();
}

int main() {
    srand(time(NULL));
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
       if(ch == KEY_UP)spin_tetro();
       if(ch == KEY_LEFT)move_tetro(-1) ;
       if(ch == KEY_RIGHT)move_tetro(+1) ;
      if(should_create_tetro){
           clean_board();
           create_tetro();
       }
      usleep(100000);
    }

    endwin();
    return 0;
}
