#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "Block.h"
#include "ST7735.h"
#include "Images.h"
#include "Sound.h"



block_t currentPiece[4] = {	{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
													};


block_t futurePiece[4] = {	{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
														{0,0,0,0,0,0,0,0,0,0},
																};
uint32_t score = 0;
													
gridBlock_t grid[16][31]; 
																

uint32_t YchangedFlag = 0;

uint32_t previousCurrentPieceX; // has original x coordinate (the X before it was changed according to the slide pot position). This is in units of the grid, NOT THE ACTUAL DISPLAY


uint32_t checkY(void){
	uint32_t x;
	uint32_t newY;
	for(int i = 0; i < 4; i++){
		x = (currentPiece[i].x - 48)/5;
		newY = (currentPiece[i].y + 5)/ 5;
		if(grid[x][newY].life != 0){
			currentPiece[0].spawnFlag = 1; // the piece is done moving
			return 1;        //invalid
		}
		
	}
	//valid
	return 0;
}


																
uint32_t CheckGrid_X(void){ 			//this function is called AFTER x and y coords are changed
	uint32_t futureX;
	uint32_t currentY;
		for(int i = 0; i < 4; i++){ 
			futureX = (futurePiece[i].x - 48)/5;
			currentY = (currentPiece[i].y)/ 5;
			if(grid[futureX][currentY].life == 1){
				return 1;  //invalid
			}
		}
		
	return 0; // valid
}	



uint32_t RowStop;
void delete_Rows(void){
	uint32_t x;
	uint32_t y;
	uint32_t breakFlag = 0;
	uint32_t rowNumber; //the row number of the row that should be deleted
	
	for(int row = 31; row >= 1; row--){
		for(int j = 0; j <  16; j++){
			if(grid[j][row].life == 0){						//if there is an empty space in a row, it DOESN'T need to be deleted
				breakFlag = 0;							//empty space, break
				break;
			}
			else{
				breakFlag = 1; // current space is full
			}
		}
		if(breakFlag){
			rowNumber = row;			//the current row was completely full, it needs to be deleted
			break;
		}	
	}

	uint32_t scoreFlag = 0;
	if(breakFlag){
		FindEmptyRow(rowNumber);
		while(rowNumber >= RowStop){
			for(int k = 0; k < 16; k++){
				grid[k][rowNumber].life = 0;		//these blocks should not be printed
				uint32_t a = ((k)*5) + 48;
				uint32_t b = ((rowNumber) *5);
				ST7735_DrawBitmap(a, b, blackSquareBmp, 5, 5);
				ST7735_DrawBitmap(a, b, gridlineborderbmp, 5, 5);
				if(grid[k][rowNumber-1].life){
					grid[k][rowNumber-1].life = 0;				//there is a block above the one that is being deleted, move it down one block
					x = ((k)*5) + 48;
					y = ((rowNumber-1) *5);
					ST7735_DrawBitmap(x, y, blackSquareBmp, 5, 5);
					ST7735_DrawBitmap(x, y, gridlineborderbmp, 5, 5);						//erase
					grid[k][rowNumber].life = 1;
					grid[k][rowNumber].color = grid[k][rowNumber-1].color;			//transfer colors
				}
			}
			rowNumber--;
			
		}
		scoreFlag = 1;
	}
	if(scoreFlag){
		score += 16;
		Sound_Shoot();
	}
	
}


void FindEmptyRow(uint32_t row){
	uint32_t returnFlag = 0;
		
	for(int i = row; i >= 1; i--){
		for(int k = 0; k < 16; k++){
			if(grid[k][i].life == 1){
				returnFlag = 0;
				break;									//this row isnt empty
			}
			else{
				returnFlag = 1;
			}
		}
		if(returnFlag){
			RowStop = i;       //This row is completely empty. Stop drawing when you get to this row
			break;
		}
	}

}



//draws the blocks that are alive
void draw_Rows(void){ 								
	uint32_t Xcoord;
	uint32_t Ycoord;
	for(int row = 31; row >= 1; row--){
		for(int j = 0; j < 16; j++){
			Xcoord = (j*5) + 48;
			Ycoord = row*5;							//convert grid_t coordinates to display coordinates
			if(grid[j][row].life){
				switch(grid[j][row].color){
					case 0:
						ST7735_DrawBitmap(Xcoord, Ycoord, tealblockbmp, 5, 5);
						break;
					case 1:
						ST7735_DrawBitmap(Xcoord, Ycoord, redblockbmp, 5, 5);
						break;
					case 2:
						ST7735_DrawBitmap(Xcoord, Ycoord, orangeblockbmp, 5, 5);
						break;
					case 3:
						ST7735_DrawBitmap(Xcoord, Ycoord, yellowblockbmp, 5, 5);
						break;
					case 4:
						ST7735_DrawBitmap(Xcoord, Ycoord, purpleblockbmp, 5, 5);
						break;
					case 5: 
						ST7735_DrawBitmap(Xcoord, Ycoord, pinkblockbmp, 5, 5);
						break;
					case 6:
						ST7735_DrawBitmap(Xcoord, Ycoord, brownblockbmp, 5, 5);
						break;
				}															
			}
		}
	}
	
}
													

													
void SpawnLPiece(void){
	ST7735_DrawBitmap(83, 5, tealblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 10, tealblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 15, tealblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 15, tealblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 5;
	
	currentPiece[1].x = 83;
	currentPiece[1].y = 10;
	
	currentPiece[2].x = 83;
	currentPiece[2].y = 15;
	
	currentPiece[3].x = 88;
	currentPiece[3].y = 15;
	
	currentPiece[0].greatestY = 15;
	currentPiece[0].greatestX = 88;
	
	currentPiece[0].pieceID = 0;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 5;
	currentPiece[0].distlow = 0;
}

void SpawnZPiece(void){
	ST7735_DrawBitmap(83, 5, redblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 5, redblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 10, redblockbmp, 5, 5);
	ST7735_DrawBitmap(93, 10, redblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 5;
	
	currentPiece[1].x = 88;
	currentPiece[1].y = 5;
	
	currentPiece[2].x = 88;
	currentPiece[2].y = 10;
	
	currentPiece[3].x = 93;
	currentPiece[3].y = 10;
	
	currentPiece[0].greatestY = 10;
	currentPiece[0].greatestX = 93;
	
	currentPiece[0].pieceID = 1;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 10;
	currentPiece[0].distlow = 0;

}

void SpawnLinePiece(void){
	ST7735_DrawBitmap(83, 5, orangeblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 10, orangeblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 15, orangeblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 20, orangeblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 5;
	
	currentPiece[1].x = 83;
	currentPiece[1].y = 10;
	
	currentPiece[2].x = 83;
	currentPiece[2].y = 15;
	
	currentPiece[3].x = 83;
	currentPiece[3].y = 20;
	
	currentPiece[0].greatestY = 20;
	currentPiece[0].greatestX = 83;
	
	currentPiece[0].pieceID = 2;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 0;
	currentPiece[0].distlow = 0;

}

void SpawnSquarePiece(void){
	ST7735_DrawBitmap(83, 5, yellowblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 5, yellowblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 10, yellowblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 10, yellowblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 5;
	
	currentPiece[1].x = 88;
	currentPiece[1].y = 5;
	
	currentPiece[2].x = 83;
	currentPiece[2].y = 10;
	
	currentPiece[3].x = 88;
	currentPiece[3].y = 10;
	
	currentPiece[0].greatestY = 10;
	currentPiece[0].greatestX = 88;
	
	currentPiece[0].pieceID = 3;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	


}

void SpawnTriPiece(void){
	ST7735_DrawBitmap(83, 5, purpleblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 5, purpleblockbmp, 5, 5);
	ST7735_DrawBitmap(93, 5, purpleblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 10, purpleblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 5;
	
	currentPiece[1].x = 88;
	currentPiece[1].y = 5;
	
	currentPiece[2].x = 93;
	currentPiece[2].y = 5;
	
	currentPiece[3].x = 88;
	currentPiece[3].y = 10;
	
	currentPiece[0].greatestY = 10;
	currentPiece[0].greatestX = 93;
	
	currentPiece[0].pieceID = 4;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 10;
	currentPiece[0].distlow = 0;

}

void SpawnJPiece(void){
	ST7735_DrawBitmap(83, 5, pinkblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 10, pinkblockbmp, 5, 5);
	ST7735_DrawBitmap(83, 15, pinkblockbmp, 5, 5);
	ST7735_DrawBitmap(78, 15, pinkblockbmp, 5, 5);
	
	currentPiece[0].x = 78;
	currentPiece[0].y = 15;
	
	currentPiece[1].x = 83;
	currentPiece[1].y = 15;
	
	currentPiece[2].x = 83;
	currentPiece[2].y = 10;
	
	currentPiece[3].x = 83;
	currentPiece[3].y = 5;
	
	currentPiece[0].greatestY = 15;
	currentPiece[0].greatestX = 83;
	
	currentPiece[0].pieceID = 5;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 5;
	currentPiece[0].distlow = 0;

}

void SpawnSPiece(void){
	ST7735_DrawBitmap(83, 10, redblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 10, redblockbmp, 5, 5);
	ST7735_DrawBitmap(88, 5, redblockbmp, 5, 5);
	ST7735_DrawBitmap(93, 5, redblockbmp, 5, 5);
	
	currentPiece[0].x = 83;
	currentPiece[0].y = 10;
	
	currentPiece[1].x = 88;
	currentPiece[1].y = 10;
	
	currentPiece[2].x = 88;
	currentPiece[2].y = 5;
	
	currentPiece[3].x = 93;
	currentPiece[3].y = 5;
	
	currentPiece[0].greatestY = 10;
	currentPiece[0].greatestX = 93;
	
	currentPiece[0].pieceID = 6;
	currentPiece[0].spawnFlag = 0;
	currentPiece[0].rotationCount = 0;

	currentPiece[0].disthigh = 10;
	currentPiece[0].distlow = 0;
}


// update*piece*Piece functions - meant to make updating x-coordinates according to slide pot position much simpler
// These functions will not update positions if there is a block in the way
void updateLPiece(block_t currentPieceArr[]){
	
		switch(currentPieceArr[0].rotationCount){
		case 0 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x + 5;

			currentPieceArr[0].greatestX = currentPiece[3].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;

		case 1 :
			currentPieceArr[1].x = currentPiece[0].x-5;
			currentPieceArr[2].x = currentPiece[1].x-5;
			currentPieceArr[3].x = currentPiece[2].x;

			currentPieceArr[0].greatestX = currentPiece[0].x;
			currentPieceArr[0].lowestX = currentPiece[2].x;
			break;

		case 2 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x-5;

			currentPieceArr[0].greatestX = currentPiece[0].x;
			currentPieceArr[0].lowestX = currentPiece[3].x;
			break;

		case 3 :
			currentPieceArr[1].x = currentPiece[0].x+5;
			currentPieceArr[2].x = currentPiece[1].x+5;
			currentPieceArr[3].x = currentPiece[2].x;

			currentPieceArr[0].greatestX = currentPiece[2].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;
	}
}

void updateZPiece(block_t currentPieceArr[]){

		switch(currentPieceArr[0].rotationCount){
		case 0 :
			currentPieceArr[1].x = currentPiece[0].x + 5;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x + 5;

			currentPieceArr[0].greatestX = currentPiece[3].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;

		case 1 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x+5;
			currentPieceArr[3].x = currentPiece[2].x;

			currentPieceArr[0].greatestX = currentPiece[2].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;
	}
}

void updateLinePiece(block_t currentPieceArr[]){
	
		switch(currentPieceArr[0].rotationCount){
		case 0 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x;

			currentPieceArr[0].greatestX = currentPiece[3].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;

		case 1 :
			currentPieceArr[1].x = currentPiece[0].x+5;
			currentPieceArr[2].x = currentPiece[1].x+5;
			currentPieceArr[3].x = currentPiece[2].x+5;

			currentPieceArr[0].greatestX = currentPiece[3].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;
	}
	

	
}

void updateSquarePiece(block_t currentPieceArr[]){
		currentPieceArr[1].x = currentPiece[0].x + 5;
	
		currentPieceArr[2].x = currentPiece[0].x;
	
		currentPieceArr[3].x = currentPiece[1].x;
	
		currentPieceArr[0].greatestX = currentPiece[3].x;
		currentPieceArr[0].lowestX = currentPiece[0].x;
	
	
	
}


void updateTriPiece(block_t currentPieceArr[]){
		
	switch(currentPieceArr[0].rotationCount){
		case 0 :
			currentPieceArr[1].x = currentPiece[0].x + 5;
			currentPieceArr[2].x = currentPiece[1].x + 5;
			currentPieceArr[3].x = currentPiece[1].x;

			currentPieceArr[0].greatestX = currentPiece[2].x;
			currentPieceArr[0].lowestX = currentPiece[0].x;
			break;

		case 1 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x-5;

			currentPieceArr[0].greatestX = currentPiece[0].x;
			currentPieceArr[0].lowestX = currentPiece[3].x;
			break;

		case 2 :
			currentPieceArr[1].x = currentPiece[0].x-5;
			currentPieceArr[2].x = currentPiece[1].x-5;
			currentPieceArr[3].x = currentPiece[1].x;

			currentPieceArr[0].greatestX = currentPiece[0].x;
			currentPieceArr[0].lowestX = currentPiece[2].x;
			break;

		case 3 :
			currentPieceArr[1].x = currentPiece[0].x;
			currentPieceArr[2].x = currentPiece[1].x;
			currentPieceArr[3].x = currentPiece[2].x+5;

			currentPieceArr[0].greatestX = currentPiece[3].x;
			currentPieceArr[0].lowestX = currentPiece[2].x;
			break;
	}
}


void updateJPiece(block_t currentPieceArr[]){
		
	switch(currentPieceArr[0].rotationCount){
		case 0 :
		currentPieceArr[1].x = currentPiece[0].x + 5;
		currentPieceArr[2].x = currentPiece[1].x;
		currentPieceArr[3].x = currentPiece[2].x;
	
		currentPieceArr[0].greatestX = currentPiece[3].x;
		currentPieceArr[0].lowestX = currentPiece[0].x;
		break;

		case 1 :
		currentPieceArr[1].x = currentPiece[0].x;
		currentPieceArr[2].x = currentPiece[1].x+5;
		currentPieceArr[3].x = currentPiece[2].x+5;
	
		currentPieceArr[0].greatestX = currentPiece[3].x;
		currentPieceArr[0].lowestX = currentPiece[0].x;
		break;

		case 2 :
		currentPieceArr[1].x = currentPiece[0].x - 5;
		currentPieceArr[2].x = currentPiece[1].x;
		currentPieceArr[3].x = currentPiece[2].x;
	
		currentPieceArr[0].greatestX = currentPiece[0].x;
		currentPieceArr[0].lowestX = currentPiece[1].x;
		break;

		case 3 :
		currentPieceArr[1].x = currentPiece[0].x;
		currentPieceArr[2].x = currentPiece[1].x-5;
		currentPieceArr[3].x = currentPiece[2].x-5;
	
		currentPieceArr[0].greatestX = currentPiece[0].x;
		currentPieceArr[0].lowestX = currentPiece[3].x;
		break;
		}
}


void updateSPiece(block_t currentPieceArr[]){
		
	switch(currentPieceArr[0].rotationCount){
		case 0:
		currentPieceArr[1].x = currentPiece[0].x + 5;
	
		currentPieceArr[2].x = currentPiece[1].x;
	
		currentPieceArr[3].x = currentPiece[2].x + 5;
	
		currentPieceArr[0].greatestX = currentPiece[3].x;
		currentPieceArr[0].lowestX = currentPiece[0].x;
		break;

		case 1 :
		currentPieceArr[1].x = currentPiece[0].x;
	
		currentPieceArr[2].x = currentPiece[1].x+5;
	
		currentPieceArr[3].x = currentPiece[2].x;
	
		currentPieceArr[0].greatestX = currentPiece[3].x;
		currentPieceArr[0].lowestX = currentPiece[0].x;

		}
}




// rotations should be clockwise
void rotateLPiece(void){
	clear();
	switch(currentPiece[0].rotationCount){
		case 0 :
			
			currentPiece[0].x += 10;
			currentPiece[0].y += 5;
	
			currentPiece[1].x += 5;
			//currentPiece[1].y = same
	
			//currentPiece[2].x = same
			currentPiece[2].y -= 5;
	
			currentPiece[3].x -= 5;
			//currentPiece[3].y = same;


			currentPiece[0].greatestX = 93;
			
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 10;
			break;
		
		
		case 1 :
			currentPiece[0].x -= 5;
			currentPiece[0].y += 5;
	
			//currentPiece[1].x = same;
			//currentPiece[1].y = same;
	
			currentPiece[2].x += 5;
			currentPiece[2].y -= 5;
	
			//currentPiece[3].x = same;
			currentPiece[3].y -= 10;
	
			currentPiece[0].greatestX = 88;
		
		  	currentPiece[0].rotationCount++;

		  	currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 5;
		
			break;
		case 2 :
			currentPiece[0].x -= 5;
			//currentPiece[0].y = same;
	
			//currentPiece[1].x = same;
			currentPiece[1].y += 5;
	
			currentPiece[2].x += 5;
			currentPiece[2].y += 10;
	
			currentPiece[3].x += 10;
			currentPiece[3].y += 5;
	
			currentPiece[0].greatestX = 93;
		
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;
		
			break;
		case 3 :
			//currentPiece[0].x = same;
			currentPiece[0].y -= 10;
	
			currentPiece[1].x -= 5;
			currentPiece[1].y -= 5;
	
			currentPiece[2].x -= 10;
			//currentPiece[2].y = same;
	
			currentPiece[3].x -= 5;
			currentPiece[3].y += 5;
	
			currentPiece[0].greatestX = 88;
		
			currentPiece[0].rotationCount = 0;

			currentPiece[0].disthigh = 5;
			currentPiece[0].distlow = 0;
		
			break;
	}
	
}

void rotateZPiece(void){
	clear();	
	switch(currentPiece[0].rotationCount){
		case 0 :
			
			currentPiece[0].x += 5;
			currentPiece[0].y -= 5;
	
			//currentPiece[1].x = same;
			//currentPiece[1].y = same;
	
			currentPiece[2].x -= 5;
			currentPiece[2].y -= 5;
	
			currentPiece[3].x -= 10;
			//currentPiece[3].y = same;
		
			
			currentPiece[0].greatestX = 88;
		
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 5;

			break;
		
		
		case 1 :
			currentPiece[0].x -= 5;
			currentPiece[0].y += 5;
	
			//currentPiece[1].x = same;
			//currentPiece[1].y = same;
	
			currentPiece[2].x += 5;
			currentPiece[2].y += 5;
	
			currentPiece[3].x += 10;
			//currentPiece[3].y -= same;
	
			
			currentPiece[0].greatestX = 93;
		
		  currentPiece[0].rotationCount = 0;

		  	currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;
		
			break;
		
			
	}
}

void rotateLinePiece(void){
	clear();
	switch(currentPiece[0].rotationCount){
		case 0: 
			currentPiece[0].x -= 5;
			currentPiece[0].y = currentPiece[3].y;
		
			currentPiece[1].y = currentPiece[0].y;
			
			currentPiece[2].x = currentPiece[1].x + 5;
			currentPiece[2].y = currentPiece[0].y;
		
			currentPiece[3].x = currentPiece[2].x + 5;
			
			currentPiece[0].greatestX = currentPiece[3].x;
			currentPiece[0].rotationCount ++;

			currentPiece[0].disthigh = 15;
			currentPiece[0].distlow = 0;

			break;
		case 1:
			currentPiece[0].x = currentPiece[1].x;
			currentPiece[0].y += 15;
		
			currentPiece[1].y += 10;
			
			currentPiece[2].x = currentPiece[0].x;
			currentPiece[2].y += 5;
			
			currentPiece[3].x = currentPiece[0].x;
			
			currentPiece[0].greatestX = currentPiece[0].x;
			currentPiece[0].rotationCount = 0;

			currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 0;
			break;
	}
}



void rotateTriPiece(){
	clear();
	switch(currentPiece[0].rotationCount){
		case 0:
			currentPiece[0].x += 5;
			currentPiece[0].y -= 5;


	
			currentPiece[2].x -= 5;
			currentPiece[2].y += 5;
	
			currentPiece[3].x -= 5;
			currentPiece[3].y -= 5;
	
			currentPiece[0].greatestY = currentPiece[2].y;
			currentPiece[0].greatestX = currentPiece[0].x;
			
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 5;

			break;
		
		case 1:
			currentPiece[0].x += 5;
			currentPiece[0].y += 10;

			currentPiece[1].y += 5;

			currentPiece[2].x -= 5;

		
			currentPiece[3].x += 5;

	
			currentPiece[0].greatestY = currentPiece[2].y;
			currentPiece[0].greatestX = currentPiece[2].x;
		
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 10;

			break;
		
		case 2:
			currentPiece[0].x -= 10;

	
			currentPiece[1].x -= 5;
			currentPiece[1].y -= 5;
	

			currentPiece[2].y -= 10;
	


	
			currentPiece[0].greatestY = currentPiece[0].y;
			currentPiece[0].greatestX = currentPiece[3].x;
			
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 5;
			currentPiece[0].distlow = 0;

			break;
		
		case 3:
			currentPiece[0].y -= 5;
			
			currentPiece[1].x += 5;
	
			currentPiece[2].x += 10;
			currentPiece[2].y += 5;
	
			currentPiece[3].y += 5;
	
			currentPiece[0].greatestY = currentPiece[3].y;
			currentPiece[0].greatestX = currentPiece[2].x;
			
			currentPiece[0].rotationCount = 0;

			currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;

			break;
		
		
	}
}

void rotateJPiece(void){
clear();	
	switch(currentPiece[0].rotationCount){
		case 0 :
			
			//currentPiece[0].x = same;
			currentPiece[0].y -= 5;
	
			currentPiece[1].x -= 5;
			//currentPiece[1].y = same;
	
			//currentPiece[2].x = same;
			currentPiece[2].y += 5;
	
			currentPiece[3].x += 5;
			currentPiece[3].y += 10;
	
			
			currentPiece[0].greatestX = currentPiece[3].x;
			
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;

			break;
		
		
		case 1 :
			currentPiece[0].x += 5;
			currentPiece[0].y -= 5;
	
			//currentPiece[1].x = same;
			currentPiece[1].y -= 10;
	
			currentPiece[2].x -= 5;
			currentPiece[2].y -= 5;
	
			currentPiece[3].x -= 10;
			//currentPiece[3].y = same;
	
			
			currentPiece[0].greatestX = currentPiece[0].x;

		
		  currentPiece[0].rotationCount++;

		  	currentPiece[0].disthigh = 0;
			currentPiece[0].distlow = 5;
		
			break;
		case 2 :
            currentPiece[0].x += 5;
            currentPiece[0].y += 10;
            currentPiece[1].x += 10;
            currentPiece[1].y += 5;
            currentPiece[2].x += 5;
            currentPiece[3].y -= 5;
            currentPiece[0].greatestX = currentPiece[0].x;
            currentPiece[0].rotationCount++;
            currentPiece[0].disthigh = 0;
            currentPiece[0].distlow = 10;
            break;
		case 3 :
            currentPiece[0].x -= 10;
            currentPiece[1].x -= 5;
						currentPiece[1].y += 5;
            currentPiece[3].x += 5;
            currentPiece[3].y -= 5;
            currentPiece[0].greatestX = currentPiece[3].x;
            currentPiece[0].rotationCount = 0;
            currentPiece[0].disthigh = 5;
            currentPiece[0].distlow = 0;
            break;
	}
}

void rotateSPiece(void){
	clear();	
	switch(currentPiece[0].rotationCount){
		case 0 :
			
			currentPiece[0].x += 5;
			currentPiece[0].y -= 10;
	
			currentPiece[1].y -= 5;
	
			currentPiece[2].x += 5;
			
	
			currentPiece[3].x -= 5;
			currentPiece[3].y += 5;
	
			
			currentPiece[0].greatestX = 93;
		
			currentPiece[0].rotationCount++;

			currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;

			break;
		
		
		case 1 :
			currentPiece[0].x -= 5;
			currentPiece[0].y += 10;
	
			
			currentPiece[1].y += 5;
	
			currentPiece[2].x -= 5;
			
	
			currentPiece[3].x += 5;
			currentPiece[3].y -= 5;
	
			
			currentPiece[0].greatestX = 88;

		  currentPiece[0].rotationCount = 0;

			currentPiece[0].disthigh = 10;
			currentPiece[0].distlow = 0;
		
			break;
		
			
	}
}




void clear(void){
		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, blackSquareBmp, 5, 5); // erase previous position
		
		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, gridlineborderbmp, 5, 5); //re-draw the gridline
}

void DrawCurrentPiece(uint32_t ID){
	switch(ID){
		case 0:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, tealblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, tealblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, tealblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, tealblockbmp, 5, 5); 
			break;
		case 1:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, redblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, redblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, redblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, redblockbmp, 5, 5); 
			break;
		case 2:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, orangeblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, orangeblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, orangeblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, orangeblockbmp, 5, 5); 
			break;
		case 3:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, yellowblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, yellowblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, yellowblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, yellowblockbmp, 5, 5); 
			break;
		case 4:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, purpleblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, purpleblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, purpleblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, purpleblockbmp, 5, 5); 
			break;
		case 5: 
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, pinkblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, pinkblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, pinkblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, pinkblockbmp, 5, 5); 
			break;
		case 6:
			ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, brownblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, brownblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, brownblockbmp, 5, 5);
			ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, brownblockbmp, 5, 5);
			break;
	}
}

