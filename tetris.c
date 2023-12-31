
#include <curses.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
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
bool crashed = false;
int tetrocells_count = 0 ;
int spin = 0;
char tetro_type;
tetro tetroid;
tetro tetroid1;
tetro tetroid2;
tetro tetroid3;
int lines_complete[BOARD_HEIGHT] = {-1};
int lines_complete_count = 0 ;

void clearTerminal() {
    refresh();
}

int comparefunction(const void* a ,const void * b){
    if(*(int*)a == 0 ) return 1;
    return *(int*)a -*(int*) b;
}

void reset_lines_complete(){
    for(int i = 0; i < BOARD_HEIGHT;i++){
        lines_complete[i] = -1 ;
    }
    lines_complete_count = 0;
}
void clear_tetro(){

    gameBoard[tetroid .x][tetroid .y] = 1;
    gameBoard[tetroid1.x][tetroid1.y] = 1;
    gameBoard[tetroid2.x][tetroid2.y] = 1;
    gameBoard[tetroid3.x][tetroid3.y] = 1;

}

void create_tetro(){
   // int rand_num = (rand() % 7) + 1;
    int rand_num = 0 ;
    int starting_x = BOARD_WIDTH/2;
    spin = 0 ;
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

int tetroblock_can_spin(int*xs , int*ys){
   for (int i = 0; i < 3; i++) {
        if (xs[i] < 0 || xs[i] >= BOARD_WIDTH || ys[i] < 0 || ys[i] >= BOARD_HEIGHT ||
            gameBoard[xs[i]][ys[i]] == 1) {
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

        (gameBoard[tetroid.x+mov][tetroid.y] != 1 )
    &&

        (gameBoard[tetroid1.x+mov][tetroid1.y] != 1)
    &&

        (gameBoard[tetroid2.x+mov][tetroid2.y] != 1 )
    &&

        (gameBoard[tetroid3.x+mov][tetroid3.y] != 1)
    );
};

void drawGameBoard() {

    for (int y = BOARD_HEIGHT-1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            mvprintw(BOARD_HEIGHT,x,"%d",x);
        }
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if(gameBoard[x][y] == 0){
                mvprintw(BOARD_HEIGHT-y-1,x,"."); // Occupied cell
            }else{
                mvprintw(BOARD_HEIGHT-y-1,x,"%d",gameBoard[x][y]); // Occupied cell
            }
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
    mvprintw(10,14,"stopped : %s",stopped?"yes":"no");
    int rotation = pow(-1,spin/2);
    mvprintw(11,14,"spin : %d rotation:%d",spin,rotation);
    mvprintw(13,14,"lines complete:%d",lines_complete_count);
    mvprintw(14,14,"crashed %s", crashed?"true":"false");
}

int get_rotation(int spin ){
    return pow(-1,spin/2);
}

void go_down(){

    int xs[] = {
         tetroid.x,
         tetroid1.x,
         tetroid2.x,
         tetroid3.x,
    };

   int ys[] = {
        tetroid.y,
        tetroid1.y,
        tetroid2.y,
        tetroid3.y
    };

    int lowers_xs[4] = {-1};
    int lowers_ys[4] = {BOARD_HEIGHT};

    lowers_ys[0] = BOARD_HEIGHT;
    lowers_ys[1] = BOARD_HEIGHT;
    lowers_ys[2] = BOARD_HEIGHT;
    lowers_ys[3] = BOARD_HEIGHT;

    lowers_xs[0] = -1;
    lowers_xs[1] = -1;
    lowers_xs[2] = -1;
    lowers_xs[3] = -1;
    // check the widht of the tetroid and if every x can land on the diff y

    for(int i = 0 ; i < 4 ; i++){
        bool already_included = false;
        int _saved_place = -1 ;
        int _saved_x = -1 ;

        for(int j = 0  ; j < 4 ; j++){

            if(already_included) break;

            if(xs[i] != lowers_xs[j] && lowers_xs[j] == -1){
                    lowers_xs[i] = xs[j];
                    lowers_ys[i] = ys[j];
                    already_included = true;
            }else if(lowers_ys[j] == xs[i]){
                if(lowers_ys[j] >= ys[i]) lowers_ys[j] = ys[i];
            }
        }
    }

    // check where is the upper point where the tetro can land


    bool can_land = true;

    for(int i = 0 ; i < 4 ; i++){

        int a = lowers_ys[i] -1 ;

        if( lowers_xs[i] != -1
            && gameBoard[lowers_xs[i]][a] == 1){
            can_land = false;
            crashed = true;
        }
    }

    if(!can_land) return;

    unsigned int height_diff = 1 ;

    gameBoard[tetroid.x][tetroid.y] = 0;
    gameBoard[tetroid1.x][tetroid1.y] = 0;
    gameBoard[tetroid2.x][tetroid2.y] = 0;
    gameBoard[tetroid3.x][tetroid3.y] = 0;

    tetroid.y = tetroid.y - height_diff;
    tetroid1.y = tetroid1.y - height_diff;
    tetroid2.y = tetroid2.y - height_diff;
    tetroid3.y = tetroid3.y - height_diff;

    gameBoard[tetroid.x][tetroid.y] = 3;
    gameBoard[tetroid1.x][tetroid1.y] = 2;
    gameBoard[tetroid2.x][tetroid2.y] = 4;
    gameBoard[tetroid3.x][tetroid3.y] = 5;

}

void go_bottom(){
    int xs[] = {
         tetroid.x,
         tetroid1.x,
         tetroid2.x,
         tetroid3.x,
    };

   int ys[] = {
        tetroid.y,
        tetroid1.y,
        tetroid2.y,
        tetroid3.y
    };

    // check whose the lower available & the go everything down to that level
    int lower_y = BOARD_HEIGHT;
    int lower_x = -1;
    int lowers_xs[4] = {0};

    for(int i = 0 ; i < 4 ; i++){
        if(ys[i] < lower_y ){

            lower_y = ys[i];

            lowers_xs[0] = xs[i];
            lowers_xs[1] = 0;
            lowers_xs[2] = 0;
            lowers_xs[3] = 0;
        }else if(ys[i] == lower_y ){
            int j = 0 ;
            do{
                if(lowers_xs[j] == 0 ) {
                    lowers_xs[j] = xs[i];
                };

                j++;
            }

            while(j < 4 && lowers_xs[j] == 0 );
        }
    }
    // check where is the upper point where the tetro can land

    int rlt_upper_point = 0 ;
    for(int i = 0 ; i < lower_y; i++){
        for (int j = 0; j < 4; j++) {
            if(lowers_xs[j] != 0){

                int new_possible_upper_point = gameBoard[lowers_xs[j]][i];
                mvprintw(14,14,"x %d",lowers_xs[j]);
                if(new_possible_upper_point &&  new_possible_upper_point == 1 ){
                    rlt_upper_point = i;
                }
            }
        }
    }

    unsigned int height_diff = lower_y - rlt_upper_point -1 ;

    gameBoard[tetroid.x][tetroid.y] = 0;
    gameBoard[tetroid1.x][tetroid1.y] = 0;
    gameBoard[tetroid2.x][tetroid2.y] = 0;
    gameBoard[tetroid3.x][tetroid3.y] = 0;

    tetroid.y = tetroid.y - height_diff;
    tetroid1.y = tetroid1.y - height_diff;
    tetroid2.y = tetroid2.y - height_diff;
    tetroid3.y = tetroid3.y - height_diff;

    gameBoard[tetroid.x][tetroid.y] = 3;
    gameBoard[tetroid1.x][tetroid1.y] = 2;
    gameBoard[tetroid2.x][tetroid2.y] = 4;
    gameBoard[tetroid3.x][tetroid3.y] = 5;

    stopped = true;

}

void spin_tetro(){
    if(stopped)return;
    int xs[3];
    int ys[3];
    bool flag_spin;
    switch(tetro_type){
        case L:
            xs[0] = tetroid1.x + 1 * get_rotation(spin +1);
            ys[0] = tetroid1.y - 1 * get_rotation(spin);

            xs[1] = tetroid2.x - 1 * get_rotation(spin+1);
            ys[1] = tetroid2.y + 1 * get_rotation(spin);

            xs[2] = tetroid3.x + 1 * (get_rotation(spin+2)+get_rotation(spin+3));
            ys[2] = tetroid3.y + 1 * (get_rotation(spin+3)+get_rotation(spin));

            flag_spin = tetroblock_can_spin(xs,ys);
            if(!flag_spin) break;
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

    }

}

void check_complete_line(){

    for(int i = 1 ; i < BOARD_HEIGHT ; i++){

        bool line_complete = true;

        for(int j = 0 ; j < BOARD_WIDTH;j++){
           if(gameBoard[j][i] != 1) line_complete = false;
        }

        if(line_complete){
            lines_complete[lines_complete_count] = i;
            lines_complete_count++;
            for(int j = 0 ; j < BOARD_WIDTH;j++){
                gameBoard[j][i] = 0;
            }
        }
    }
}

void gravity(){
   if(stopped && tetrocells_count == 4) return ;
    if(crashed){
        stopped = true;
        return;
    }
   int gravity_count = 0 ;
   for (int y = 1; y < BOARD_HEIGHT+4 ; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if(gameBoard[x][y] == 1){
                int previous_y_thats_not_complete_line = y+1;
                for (int i = 0; i < sizeof(lines_complete) / sizeof(int); i++) {
                    if (lines_complete[i] == previous_y_thats_not_complete_line) {
                        previous_y_thats_not_complete_line++;
                    }
                }

                if(y+1<= BOARD_HEIGHT){
                    if(gravity_count > 0){
                        gameBoard[x][y] = gameBoard[x][previous_y_thats_not_complete_line];
                    }
                }
            }

           if(gameBoard[x][y] == 0){
                for (int i = 0; i < sizeof(lines_complete) / sizeof(int); i++) {
                    if (lines_complete[i] == y) {
                        gravity_count++;
                        break; // If found, exit the loop
                    }
                }

                if(y+1<= BOARD_HEIGHT){
                    if(gravity_count > 0 || gameBoard[x][y+1] != 1){
                        gameBoard[x][y] = gameBoard[x][y+1];
                    }
                }
           }

// lo que pasa es k tenes que dejar que todo el tetroid de asiente y despues pararlo
            //  lo cual es complicado , porque no sabe cuando esta parado entonces
            //  puede seguir bajando independiemente
            //  de lo que tenga abajo eso no tiene sentido
            //
            if(
                gameBoard[x][y] == 3 ||
                gameBoard[x][y] == 2 ||
                gameBoard[x][y] == 4 ||
                gameBoard[x][y] == 5
            ){
                tetrocells_count++;
                if(y-1 >= 1 && y+1 < BOARD_HEIGHT + 4 && gameBoard[x][y-1] != 1){
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

    for (int i = 0; i < sizeof(lines_complete) / sizeof(int); i++) {
        lines_complete[i] = -1 ;
     }

    lines_complete_count = 0;

}

int main() {
    srand(time(NULL));
    initscr();
    cbreak();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);


    for(int i = 0 ; i < BOARD_WIDTH ; i++ ){
        gameBoard[i][0] = 1 ;
    };

   time_t startTime, currentTime;
   int elapsedSeconds = 3;
   int ch ;
   int add_gravity = 0 ;
   const int GRAVITY_FLAG = 10;
   int movement = 0;

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
       if(ch == KEY_DOWN)go_down();
       if(ch == KEY_LEFT)move_tetro(-1) ;
       if(ch == KEY_RIGHT)move_tetro(+1) ;
       if(ch == 32) go_bottom();

      if(stopped){
           clear_tetro();
           check_complete_line();
           create_tetro();
           stopped = false;
           crashed = false;
           tetrocells_count = 0 ;
       }
      usleep(10000);
    }

    endwin();
    return 0;
}
