// SpaceInvaders.c, but this is actually the code for a Tetris clone
// Runs on LM4F120/TM4C123
// Sugam Arora and Joonha Park
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "DAC.h"
#include "Block.h"
#include "Images.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds


typedef enum {English, Spanish} Language_t;
Language_t myLanguage = English;
typedef enum {SCORE, GOODBYE, LANGUAGE} phrase_t;
 char Score_English[] ="Score:";
 char Score_Spanish[] ="Puntuaci\xA2n";
 char Language_English[]="English";
 char Language_Spanish[]="French";
 const char *Phrases[4]={
	Score_English, Score_Spanish, Language_English, Language_Spanish
};

uint32_t Position;    // 32-bit fixed-point 0.001 cm




uint32_t langIndex = 0;
char currentLanguage;

char Words[2] = {'e', 's'};
uint32_t currentScore = 0;

uint32_t gameOverFlag = 0;


//****grid_Init*****
// sets every item in the grid to 0
void grid_Init(void){
	for(int i = 0; i < 16; i++){
		for(int k = 0; k < 31; k++){
			grid[i][k].life = 0;
			grid[i][k].color = 0;
		}
	}
}

// function that gets masked input for PF4 on board button
uint32_t BoardSwitch(void){
	uint32_t ret = GPIO_PORTF_DATA_R & 0x10;
	return ret;
}



// function that gets masked input for PE0-PE2
uint32_t Switches(void){
	uint32_t ret = GPIO_PORTE_DATA_R & 0x07;
	return ret;
}



void switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10;
	volatile int delay;
	delay++;
	
	
	GPIO_PORTE_DIR_R &= ~0x07;
	GPIO_PORTE_DEN_R |= 0x07; // port E pins 0-2 are inputs for the switches. PE0 = select button for language selection, PE1 = rotate button, PE2 - Hold down this button to make the piece go down faster
}


uint32_t Convert(uint32_t input, uint32_t ID){				
  uint32_t lab8Convert = (155*input)/4096+15; // these values range from 15 to 169 
	uint32_t ret = 48;
	
	
	
	int tens = -1;
	while(lab8Convert >= 15){
		lab8Convert -= 10;
		tens ++;
	}
	
	ret += tens*5;

	if((ret + currentPiece[0].disthigh) > 123){
		ret = 123-currentPiece[0].disthigh;
	}

	if((ret - currentPiece[0].distlow) < 48){
		ret = 48+currentPiece[0].distlow;
	}

	// switch(currentPiece[0].pieceID){
	// 	case 0:
	// 		if((ret + 5) > 123){
	// 			ret = 118;
	// 		}
	// 		break;
	// 	case 1: 
	// 		if((ret + 10) > 123){
	// 			ret = 113;
	// 		}
	// 		break;
	// 	case 2:
	// 		if((ret) > 123){
	// 			ret = 123;
	// 		}
	// 		break;
	// 	case 3:
	// 		if((ret + 5) > 123){
	// 			ret = 118;
	// 		}
	// 		break;
	// 	case 4: 
	// 		if((ret + 10) > 123){
	// 			ret = 113;
	// 		}
	// 		break;
	// 	case 5:
	// 		if((ret + 5) > 123){
	// 			ret = 118;
	// 		}
	// 		break;
	// 	case 6:
	// 		if((ret + 10) > 123){
	// 			ret = 113;
	// 		}
	// 		break;
	// }
	
	return ret;
}




uint32_t Random7(void){
  return ((Random32()>>24)%7)+1;  // returns 1, 2, 3, 4, or 5 STILL NEED TO FIX THIS
}





uint32_t moveFlag = 0;
void SysTick_Init(void){ 
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 8000000; // triggers every 100 ms
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0X07;
}

uint32_t previousX; // save the previous ADC coordinate and compare it with the current one
uint32_t ADCdata;
uint32_t semaphore = 0;
uint32_t movingCounter = 0;
uint32_t movingCounterLimit = 10;
void SysTick_Handler(void){				// new blocks need to spawn every 20 seconds, and move downwards once per second. 
	//GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R ^ 0x04;
	ADCdata = ADC_In();
	uint32_t recordFlag = 0;
	semaphore = 1; //new data available to output to LCD
	if((movingCounter == movingCounterLimit) && (currentPiece[0].greatestY < 155)){					//move this to a different function
		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, blackSquareBmp, 5, 5); // erase previous position
		
		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, gridlineborderbmp, 5, 5); //re-draw the gridline
			if(checkY() == 0){  			//if checkY() is valid, then update all the Y values
				for(int y = 0; y < 4; y++){
					currentPiece[y].y += 5; // move the current piece down the screen by one square per every second
					currentPiece[y].greatestY += 5;
					if(currentPiece[0].greatestY == 155){
						currentPiece[0].spawnFlag = 1; 				//this piece is done moving, spawn another piece. ALWAYS CHECK currentPiece[0].spawnFlag for up to date flag value
						recordFlag = 1;
					}
				}
			}
			else{		//if checkY() is invalid, then the piece is done moving. Update the grid
				uint32_t x;
				uint32_t Y;
				for(int j = 0; j < 4; j++){
					x = (currentPiece[j].x - 48)/5;
					Y = (currentPiece[j].y)/ 5;
					grid[x][Y].life = 1;
					grid[x][Y].color = currentPiece[0].pieceID;					
				}
			}
			if(recordFlag){
				uint32_t x;
				uint32_t Y;
				for(int j = 0; j < 4; j++){
					x = (currentPiece[j].x - 48)/5;
					Y = (currentPiece[j].y)/ 5;
					grid[x][Y].life = 1;
					grid[x][Y].color = currentPiece[0].pieceID;
				}
			}
		movingCounter = 0;
	}else{
		movingCounter++;
	}
}	
	




uint32_t redSquareCoords[] = {107, 127};
uint32_t redSquarePos = 0;


int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	switch_Init();
	DAC_Init(); // turns on PORTF clock (for SW1 AND heartbeat)
	currentLanguage = Words[0]; //default language is English
	
	uint32_t BoardSwitchFlag = 0;

  Output_Init();
	Sound_Init();
	uint32_t GridResult;
  
	
	ST7735_DrawBitmap(10, 64, betrisBmp, 92, 64); 
	
	ST7735_DrawBitmap(10, 107, redSquareBmp, 7, 7);

	ST7735_SetCursor(0,8);
	ST7735_OutString("Select your language");
	ST7735_SetCursor(4,10);
  ST7735_OutString("English");
  ST7735_SetCursor(4,12);
	ST7735_OutString((char *)Language_Spanish);
	
	//uint32_t twenty = 20;
	// code for start-up screen
	while(1){
	while(BoardSwitchFlag == 0){
		if(BoardSwitch() == 0){ 				//check if on-board switch SW1 has been hit
			BoardSwitchFlag = 1; 
		}
		
		if( (Switches() & 0x01) != 0){ // if player has hit PE0
			
			
			langIndex = (langIndex + 1) % 2;
			currentLanguage = Words[langIndex];
			if(langIndex == 0){
				ST7735_DrawBitmap(10, redSquareCoords[redSquarePos], blackSquareBmp, 7, 7); //delete the previous red square
				redSquarePos = 0;
			}
			else{
				ST7735_DrawBitmap(10, redSquareCoords[redSquarePos], blackSquareBmp, 7, 7); //delete the previous red square
				redSquarePos = 1;
			}
		}
		ST7735_DrawBitmap(10, redSquareCoords[redSquarePos],redSquareBmp, 7, 7); // print the red square indicator at its new position
		if(BoardSwitch() == 0){ 				//check if on-board switch SW1 has been hit
			BoardSwitchFlag = 1; 
		}
	}
	
  ST7735_FillScreen(0x0000);            // set screen to black
	break;
}
	// the following code is for the actual game
	ADC_Init();
	EnableInterrupts();
	SysTick_Init();
	
	grid_Init();

	ST7735_SetCursor(0,0);
	if(currentLanguage == 'e'){
		ST7735_OutString("Score");
	}
	else{
		ST7735_OutString("Score");
	}
	
	ST7735_SetCursor(0,2);
	LCD_OutDec(currentScore);
	
	for(int k = 5; k < 160; k+=5){
		for(int start = 48; start < 124; start+=5){
			ST7735_DrawBitmap(start, k, gridlineborderbmp, 5, 5); // print gridlines
		}
	}
	
	//Seed(NVIC_ST_CURRENT_R); REMEMBER TO COME BACK TO THIS
	
	SpawnJPiece();
	
	uint32_t PieceDecision;
	
	while(1){
		
		//check if game should end or not
		if((grid[7][2].life == 1) || (grid[8][2].life == 1) || (grid[6][2].life == 1)){
						//gameOverFlag = 1;
			break;
		}
		
		if((GPIO_PORTE_DATA_R & 0x02) != 0){
			while((GPIO_PORTE_DATA_R & 0x02) != 0){}
			switch(currentPiece[0].pieceID){
				case 0 :
					rotateLPiece();
					break;
				case 1 :
					rotateZPiece();
					break;
				case 2:
					rotateLinePiece();
					break;
				case 3:
					break; //squares don't rotate
				case 4:
					rotateTriPiece();
					break;
				case 5:
					rotateJPiece();
					break;
				case 6:
					rotateSPiece();
				break;
			}
			
			uint32_t twenty = 20;
			uint32_t volatile time;
			while(twenty){
				time = 72724*2/91;  // 1msec, tuned at 80 MHz
				while(time){
					time--;
				}
				twenty--;
			}
			
		}
		
		if((Switches() & 0x01) == 1){ 					// if PE0 is pressed, speed up piece movement
			if(movingCounter > 5){
				movingCounter = 0;
			}
			else{
				movingCounterLimit = 5;
			}
		}
		else{
			movingCounterLimit = 10;
		}
		
		if(currentPiece[0].spawnFlag == 1){
			PieceDecision = Random7();
			switch(PieceDecision){
				case 1:
					SpawnLPiece();
					break;
				case 2: 
					SpawnZPiece();
					break;
				case 3:
					SpawnLinePiece();
					break;
				case 4: 
					SpawnSquarePiece();
					break;
				case 5:
					SpawnTriPiece();
					break;
				case 6:
					SpawnJPiece();
					break;
				case 7:
					SpawnSPiece();
					break;
			}
		}
		
		while(!semaphore){}
		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, blackSquareBmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, blackSquareBmp, 5, 5); // erase previous position

		ST7735_DrawBitmap(currentPiece[0].x, currentPiece[0].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[1].x, currentPiece[1].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[2].x, currentPiece[2].y, gridlineborderbmp, 5, 5);
		ST7735_DrawBitmap(currentPiece[3].x, currentPiece[3].y, gridlineborderbmp, 5, 5); //re-draw the gridlines
		
			
		previousCurrentPieceX = currentPiece[0].x;
		currentPiece[0].x = Convert(ADCdata, currentPiece[0].pieceID);				// the block furthest to the left in each piece is currentPiece[0]
		futurePiece[0].x = currentPiece[0].x;
		futurePiece[0].rotationCount = currentPiece[0].rotationCount;
		switch(currentPiece[0].pieceID){																			// the rest of the blocks are updated in accordance to currentPiece[0] coordinates
			case 0:
				updateLPiece(futurePiece); 			//create the new theoretical x-coordinates for each block in the piece according to slide pot position
				GridResult = CheckGrid_X();										//see if these new coordinates are valid
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX; //if they weren't valid, then revert the change in currentPiece[0].x caused by slide pot position
				}
				updateLPiece(currentPiece);			//update the coordinates of the currentPiece accordingly
				break;
			case 1:
				updateZPiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateZPiece(currentPiece);
				break;
			case 2:
				updateLinePiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateLinePiece(currentPiece);
				break;
			case 3:
				updateSquarePiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateSquarePiece(currentPiece);
				break;
			case 4:
				updateTriPiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateTriPiece(currentPiece);
				break;
			case 5:
				updateJPiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateJPiece(currentPiece);
				break;
			case 6:
				updateSPiece(futurePiece);
				GridResult = CheckGrid_X();
				if(GridResult == 1){
					currentPiece[0].x = previousCurrentPieceX;
				}
				updateSPiece(currentPiece);
				break;
				
		}
		DrawCurrentPiece(currentPiece[0].pieceID); // re-position the moving piece according to slide potentiometer position
		
		delete_Rows();
		draw_Rows();
		
		ST7735_SetCursor(0,2);
		LCD_OutDec(score);
		 
		semaphore = 0; //reset sephamore
			
		
  }
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(7, 7);
	if(currentLanguage == 'e'){
		ST7735_OutString("GAME OVER");
	}
	else{
		ST7735_OutString("FIN DU JEU");
	}
}



// you must use interrupts to perform delays

