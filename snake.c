#include "terminal_cntl.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

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

void clearSnakeOnGrid(enum cellType grid[GRID_WIDTH][GRID_HEIGHT],
                      struct snakeCircularBuffer *snakeRef) {
  for (int i = 0; i < snakeRef->_length; i++) {
    // draw cell
    int *coords = snakeGetCoordinates(snakeRef, i);
    grid[coords[0]][coords[1]] = EMPTY;
  }
}

void renderGameGrid(enum cellType grid[GRID_WIDTH][GRID_HEIGHT], int centerX,
                    int centerY) {
  tc_clear_screen();
  int x0 = centerX - (GRID_WIDTH);
  int y0 = centerY - (GRID_HEIGHT) / 2;
  tc_move_cursor(x0, y0 - 1);
  printf("Welcome to CNAKE - Ben");

  for (int i = GRID_HEIGHT - 1; i >= 0; i--) {
    tc_move_cursor(x0, y0 + i);
    for (int j = 0; j < GRID_WIDTH; j++) {
      if (grid[j][i] == SNAKE) {
        printf("%s  %s", TC_BG_RED, TC_BG_NRM);
      } else if (grid[j][i] == FRUIT) {
        printf("%s  %s", TC_BG_GRN, TC_BG_NRM);
      } else if (grid[j][i] == EMPTY) {
        printf("%s  %s", TC_BG_WHT, TC_BG_NRM);
      } else {
        printf("%s%c %s", TC_BG_RED, (char)grid[i][j], TC_BG_NRM);
      }
      // usleep(1000);
    }
  }
  fflush(stdout);
}

void gameOver(int error, struct snakeCircularBuffer *snakeRef) {
  tc_echo_on();
  tc_exit_alt_screen();
  tc_show_cursor();
  tc_canonical_on(); // Don't wait for new line.
  tc_getc_block_on();

  free((*snakeRef)._buffer);

  printf("GAME OVER LOL BOZO %d\n", error);

  exit(error);
}

enum Direction { UP, DOWN, LEFT, RIGHT, NONE };
void snakeUpdate(enum cellType grid[GRID_WIDTH][GRID_HEIGHT],
                 struct snakeCircularBuffer *snakeRef, enum Direction dir) {
  int *headCoords = snakeGetCoordinates(snakeRef, snakeRef->_length - 1);
  int currHeadX = headCoords[0];
  int currHeadY = headCoords[1];

  if (dir == UP) {
    currHeadY--;
  } else if (dir == DOWN) {
    currHeadY++;
  } else if (dir == LEFT) {
    currHeadX--;
  } else if (dir == RIGHT) {
    currHeadX++;
  }

  // collisions
  if (currHeadX >= GRID_WIDTH || currHeadY >= GRID_HEIGHT || currHeadX < 0 ||
      currHeadY < 0) {
    gameOver(1, snakeRef);
  }

  for (int i = 1; i < snakeRef->_length - 2; i++) {
    // skip head and tail cuz they can be the same;
    int *coords = snakeGetCoordinates(snakeRef, i);
    if (currHeadX == coords[0] && currHeadY == coords[1]) {
      gameOver(2, snakeRef); // self collision;
    }
  }

  // check if we got a fruit
  if (grid[currHeadX][currHeadY] == FRUIT) {
    // we landed on fruit;
    snakeGrow(snakeRef, currHeadX, currHeadY);
  } else {
    snakeAdvance(snakeRef, currHeadX, currHeadY);
  }
}
// GAME GRID END

void cleanup_buf(void) { gameOver(4, NULL); }

int main(int argc, char *argv[]) {
  fprintf(stdout, "Hello Woorld\n");
  int screenWidth, screenHeight;
  tc_get_cols_rows(&screenWidth, &screenHeight);
  enum cellType gameGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
  gameGrid[14][6] = FRUIT;
  gameGrid[16][12] = FRUIT;
  gameGrid[14][19] = FRUIT;
  gameGrid[19][18] = FRUIT;
  gameGrid[14][16] = FRUIT;
  gameGrid[14][6] = FRUIT;
  gameGrid[1][6] = FRUIT;
  gameGrid[4][5] = FRUIT;
  gameGrid[3][8] = FRUIT;
  gameGrid[8][18] = FRUIT;
  gameGrid[9][2] = FRUIT;
  gameGrid[10][3] = FRUIT;
  gameGrid[1][12] = FRUIT;
  gameGrid[14][18] = FRUIT;
  gameGrid[7][4] = FRUIT;
  gameGrid[4][9] = FRUIT;

  printf("step 2\n");

  struct snakeCircularBuffer gameSnake;
  initSnake(&gameSnake, 5, 5);
  printf("snake length: %d\n", gameSnake._length);
  fprintf(stdout, "Head: %d\nTail: %d\n", gameSnake._tail, gameSnake._head);

  snakePrintDebug(&gameSnake);
  snakePrintDebug(&gameSnake);

  enum Direction d = DOWN;

  tc_echo_off();         // Don't echo sdin -> stdout
  tc_enter_alt_screen(); // Change to another buffer
  tc_hide_cursor();      // Don't blink cursor
  tc_canonical_off();    // Don't wait for new line.
  tc_getc_block_off();

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
    clearerr(stdin); // clears the EOF flag so that getc doesnt give up
    int result = EOF;
    int temp;

    // Keep reading until the buffer is empty
    // This "fast-forwards" to the most recent keypress
    while ((temp = getc(stdin)) != EOF) {
      result = temp;
      gameGrid[0][0] = (char)result;
    }

    enum Direction d;
    switch (result) {
    case 'j':
      d = DOWN;
      break;
    case 'k':
      d = UP;
      break;
    case 'h':
      d = LEFT;
      break;
    case 'l':
      d = RIGHT;
      break;
    }

    clearSnakeOnGrid(gameGrid, &gameSnake);
    snakeUpdate(gameGrid, &gameSnake, d);
    drawSnakeOnGrid(gameGrid, &gameSnake);
    renderGameGrid(gameGrid, screenWidth / 2, screenHeight / 2);
    fflush(stdout);
    usleep(200000);
  }
  gameOver(0, &gameSnake);

  atexit(cleanup_buf);
  return 0;
}

// TODO: Set an atexit()

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
