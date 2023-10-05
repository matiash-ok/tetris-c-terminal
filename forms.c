#include <stdbool.h>
#include <stdio.h>
#include <string.h>
typedef struct Tetroid {
    int x ;
    int y ;
    bool stopped;
    int mode;
}tetro;

int create_tetro_form(int mode,int widthX,int heightY, int board[widthX][heightY] ){
    switch (mode) {
        case 1:
            board[widthX/2][heightY] = 3;
            board[widthX/2][heightY-1] = 2;
            board[widthX/2][heightY-2] = 2;
            board[widthX/2][heightY+1] = 2;
            return widthX / 2;

  //      case 2: create_tetro_form(2);;
  //      case 3: create_tetro_form(3);
  //      case 4: create_tetro_form(4);
  //      case 5: create_tetro_form(5);
  //      case 6: create_tetro_form(6);
  //      case 7: create_tetro_form(7);
    }
    return 2;
}
