#ifndef BLOCK_H
#define BLOCK_H
#include <stdint.h>
#include "ST7735.h"
//#include "Images.h"


struct block{
	uint32_t x; //x coordinate of bottom left corner
	uint32_t y; //y coordinate of bottom right corner
	uint32_t greatestY; // the y coordinate of the block that is the lowest on the screen
	uint32_t greatestX; // the x coordinate of the block that the furthest to the right on the screen
	uint32_t lowestX; // for collision detection
	uint32_t pieceID; //numbers 0-7 correspond to a specific piece 
	uint32_t spawnFlag; //1 = a new piece should be spawned
	uint32_t rotationCount; // the number of times the current piece has been rotated from its original position
	uint32_t disthigh;
	uint32_t distlow;
};
typedef struct block block_t;

struct gridBlock{
	uint32_t life;		//this block will be drawn
	uint32_t color; //this number corresponds to a pieceID
};
typedef struct gridBlock gridBlock_t;


uint32_t CheckGrid_X(void);
uint32_t checkY(void);

//*******delete_Rows********
//constantly runs at the end of the main loop
//if any rows are completely filled, delete them and reposition appropriate blocks
void delete_Rows(void);
void draw_Rows(void);

void FindEmptyRow(uint32_t row);

void SpawnLPiece(void); // This piece is teal
void SpawnZPiece(void); //This piece is red
void SpawnLinePiece(void); //This piece is orange
void SpawnSquarePiece(void); //This piece is yellow
void SpawnTriPiece(void); //This piece is purple
void SpawnJPiece(void);
void SpawnSPiece(void);

void updateLPiece(block_t currentPieceArr[]); // This piece is teal
void updateZPiece(block_t currentPieceArr[]); //This piece is red
void updateLinePiece(block_t currentPieceArr[]); //This piece is orange
void updateSquarePiece(block_t currentPieceArr[]); //This piece is yellow
void updateTriPiece(block_t currentPieceArr[]); //This piece is purple
void updateJPiece(block_t currentPieceArr[]); //This piece pink
void updateSPiece(block_t currentPieceArr[]); //This piece brown

void rotateLPiece(void); // This piece is teal
void rotateZPiece(void); //This piece is red
void rotateLinePiece(void); //This piece is orange
void rotateTriPiece(void); //This piece is purple
void rotateJPiece(void); //This piece pink
void rotateSPiece(void); //This piece brown

void clear(void);


void DrawCurrentPiece(uint32_t ID); 							//only draws the currentPiece with its updated coordinates


extern uint32_t score;

extern block_t currentPiece[4];
extern block_t futurePiece[4]; 	//for seeing if the current piece is allowed to move
extern gridBlock_t grid[16][31]; // each item in the array represents a 5x5 block of pixels. If grid[i][j] = 1 -> there is a block there.

extern uint32_t YchangedFlag; // 1 = the Y coordinates of the current piece were incremented by Systick Handler

extern uint32_t previousCurrentPieceX;








#endif

