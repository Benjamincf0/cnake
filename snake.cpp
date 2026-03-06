#include <iostream>

int main(int argc, char* argv[]) {
	std::cout << "Hello WOrld\n";
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
