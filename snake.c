#include "terminal_cntl.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// SNAKE BUFFER
#define MAX_SNAKE_LENGTH 16
const int COLS = 2;

struct snakeCircularBuffer {
  int *_buffer;
  int _tail;
  int _head;
  int _length;
};

void initSnake(struct snakeCircularBuffer *snakeRef, int headX, int headY) {
  // dereferencing and then storing into a variable to use "." creates a copy :(
  snakeRef->_buffer = malloc(sizeof(int) * MAX_SNAKE_LENGTH * COLS);
  snakeRef->_buffer[0 * COLS + 0] = headX;
  snakeRef->_buffer[0 * COLS + 1] = headY;
  snakeRef->_tail = 0;
  snakeRef->_head = 0;
  snakeRef->_length = 1;
}

int *snakeGetCoordinates(struct snakeCircularBuffer *snakeRef, int index) {
  if (index >= snakeRef->_length)
    return NULL; // snake not long enough;

  int *coordinates =
      &snakeRef->_buffer[((snakeRef->_tail + index) % MAX_SNAKE_LENGTH) * COLS];
  return coordinates;
};

void snakeGrow(struct snakeCircularBuffer *snakeRef, int headX, int headY) {
  // move only head pointer up by 1
  snakeRef->_head = (snakeRef->_head + 1) % MAX_SNAKE_LENGTH;
  snakeRef->_length++;

  // overwrite coordinates of new head
  int *headCoords = snakeGetCoordinates(snakeRef, snakeRef->_length - 1);
  headCoords[0] = headX;
  headCoords[1] = headY;
};

void snakeAdvance(struct snakeCircularBuffer *snakeRef, int headX, int headY) {
  // move head and tail pointers up by 1
  snakeRef->_head = (snakeRef->_head + 1) % MAX_SNAKE_LENGTH;
  snakeRef->_tail = (snakeRef->_tail + 1) % MAX_SNAKE_LENGTH;
  // overwrite coordinates of new head
  snakeGetCoordinates(snakeRef, (snakeRef->_length) - 1)[0] = headX;
  snakeGetCoordinates(snakeRef, (snakeRef->_length) - 1)[1] = headY;
}

void removeSnake(struct snakeCircularBuffer *snakeRef) {
  free(snakeRef->_buffer);
};

void snakePrintDebug(struct snakeCircularBuffer *snakeRef) {
  int *tailCoords = snakeGetCoordinates(snakeRef, 0);
  int *headCoords = snakeGetCoordinates(snakeRef, snakeRef->_length - 1);

  printf("Your snake is %d cells long.\nIts head is @ (%d, %d) and its tail is "
         "@ (%d, %d)\n",
         snakeRef->_length, headCoords[0], headCoords[1], tailCoords[0],
         tailCoords[1]);
}

// SNAKE BUFFER END

// GAME GRID
#define GRID_WIDTH 20
#define GRID_HEIGHT 20

enum cellType { EMPTY, SNAKE, FRUIT };

void drawSnakeOnGrid(enum cellType grid[GRID_WIDTH][GRID_HEIGHT],
                     struct snakeCircularBuffer *snakeRef) {
  for (int i = 0; i < snakeRef->_length; i++) {
    // draw cell
    int *coords = snakeGetCoordinates(snakeRef, i);
    grid[coords[0]][coords[1]] = SNAKE;
  }
}

void renderGameGrid(enum cellType grid[GRID_WIDTH][GRID_HEIGHT], int centerX,
                    int centerY) {
  // TODO: move to (0,0)
  int x0 = centerX - (GRID_WIDTH) / 2;
  int x1 = centerY - (GRID_HEIGHT) / 2;

  for (int i = GRID_HEIGHT - 1; i >= 0; i--) {
    for (int j = 0; j < GRID_WIDTH; j++) {
      if (grid[j][i] == SNAKE) {
        printf("%s %s", TC_BG_RED, TC_BG_NRM);
      } else if (grid[j][i] == FRUIT) {
        printf("%s %s", TC_BG_WHT, TC_BG_NRM);
      } else if (grid[j][i] == EMPTY) {
        printf("%s %s", TC_BG_NRM, TC_BG_NRM);
      } else {
        printf("%s %s", TC_BG_RED, TC_BG_NRM);
      }
    }
    printf("\n");
  }
}
// GAME GRID END

int main(int argc, char *argv[]) {
  fprintf(stdout, "Hello Woorld\n");

  enum cellType gameGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
  gameGrid[14][6] = FRUIT;

  printf("step 2\n");

  struct snakeCircularBuffer gameSnake;
  initSnake(&gameSnake, 5, 5);
  printf("snake length: %d\n", gameSnake._length);
  fprintf(stdout, "Head: %d\nTail: %d\n", gameSnake._tail, gameSnake._head);

  snakePrintDebug(&gameSnake);
  snakePrintDebug(&gameSnake);

  snakeGrow(&gameSnake, 5, 6);
  snakePrintDebug(&gameSnake);
  snakeGrow(&gameSnake, 5, 7);
  snakeGrow(&gameSnake, 6, 7);
  snakePrintDebug(&gameSnake);
  snakeAdvance(&gameSnake, 6, 8);
  snakePrintDebug(&gameSnake);

  tc_enter_alt_screen();

  for (int i = 0; i < 1000; i++) {
    // TODO:
    // check for last char input
    // char letta = getc(stdin);
    // gameGrid[0][0] = letta;
    // get new head coord
    // grow / advance snake in the right direction check collisions
    //	-> with itself (check circular array coords)
    //  -> OR with the sides of grid (check coords with if)
    // check for fruit in new head
    // draw snake on grid + generate fruit if needed
    // get window dimensions {windowWidth}, {windowHeight}
    // renderGameGrid(enum cellType grid[][], centerX, centerY);
    // (find a way to hide cursor)

    drawSnakeOnGrid(gameGrid, &gameSnake);
    renderGameGrid(gameGrid);
    usleep(1000);
  }
  tc_exit_alt_screen();
  return 0;
}

// REQUIREMENTS
// snake game
// Game grid: 16*16 grid
// each cell can have a fruit, or no fruit (boolean)
//
//

// ARCHITECTURE
// 2d uint array [16][16]
// 	0 = Empty cell
// 	1 = Snake
// 	2 = Fruit
//
//
// [ [-1, -1], [2, 1], [2, 2], [2, 3], ... [-1, -1], [-1, -1] ]
// 	         ^		 ^
//		tail	 	head
// 1by2 uint circular array for snake coordinates [16*16][3]
// 	if length == 100 => game won
//
//
// Game loop (sleep 500ms)
// 	Check input
//	goStraight(), goUp(), goLeft(), goRight(), goDown();
//
// go[Up|Left|right|down|straight]()
// 	check cell
//		if snake => game over return;
//
// 		if empty => move head and tail
// 		if fruit => move only head & update map & add new fruit
//		renderGameGrid()
//
// renderGameGrid()
// 	find left and right
//
// 	draw border with | and _
//
//	iterate through grid and paint the cells
