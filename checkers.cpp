#include <pspkernel.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "checkers.h"
#include "menu.h"
#include "graphics.h"
#include "gstate.h"

Checkers::Checkers() {
	gametype = GAMETYPE_UNSET;
	difficulty = DIFFICULTY_UNSET;
	startScreen = 0;
	NB_PIECE_PLAYER_TWO=12;
	NB_PIECE_PLAYER_ONE=12;
	LOCK_JMP=false;
	startX =  (SCREEN_WIDTH / 2) - ((BOARD_SQUARE_WIDTH *8) / 2);
	curretPlayer = rand() % 2; //random value 0 or 1 for player to begun 
	wine=false; 
	
	sprintf(playerOneName, "Player ILYES");
	sprintf(playerTwoName, "Player ZAKARIA");
	if (curretPlayer ==0){
		sprintf(debugMessage, "Debug: %s","The player ILYES will begun");
	}else{
		sprintf(debugMessage, "Debug: %s","The player ZAKARIA will begun");
	}
	
	
	playersTurn = true;
	
	selectedRow = 0;
	selectedCol = 0;
	
	tickResolution = sceRtcGetTickResolution();
	sceRtcGetCurrentTick( &lastMove );
	sceRtcGetCurrentTick( &lastSelection );
	
	//setup gametype menu
	menu = new Menu(GAMETYPE_UNSET);
	menu->AddItem("Player VS Computer", GAMETYPE_PVC);
	menu->AddItem("Player VS Player", GAMETYPE_PVP);
	menu->AddItem("Internet Play", GAMETYPE_PVI);
}

Checkers::~Checkers() {
	delete menu;
}

bool Checkers::Run(SceCtrlData &pad) {
	if (startScreen < 60) {
		fillScreenColor(BLACK);
		printTextCenter("Starting PSPCheckers", WHITE);
		startScreen++;
		return true;
	}
	
	if (gametype == GAMETYPE_UNSET) {
		fillScreenColor(BLACK);
		printTextCenter("Select a game type:", WHITE, 40);
		gametype = menu->Run(pad);
		if(gametype != GAMETYPE_UNSET) {
			delete menu;
			
			if (gametype == GAMETYPE_PVC) {
				//setup difficulty menu
				menu = new Menu(DIFFICULTY_UNSET);
				menu->AddItem("Easy", DIFFICULTY_EASY);
				menu->AddItem("Medium", DIFFICULTY_MEDIUM);
				menu->AddItem("Hard", DIFFICULTY_HARD);
				//clean board
				StartFreshGame();
				
				return true;
			}
			
			if (gametype == GAMETYPE_PVP) {
				//clean board
				StartFreshGame();	
				return true;
			}
		}
		return true;
	}
	
	if (gametype == GAMETYPE_PVC) {
		if (difficulty == DIFFICULTY_UNSET) {
			fillScreenColor(BLACK);	
			printTextCenter("Select a game difficulty:", WHITE, 40);
			difficulty = menu->Run(pad);
			return true;
		}
		
		fillScreenColor(BLACK);	
		DrawBoard();
		return true;
	}
	
	if (gametype == GAMETYPE_PVP) {
		fillScreenColor(BLACK);
		HandleInput(pad);
		DrawBoard();
		DrawPlayerOneHud();
		DrawPlayerTwoHud();
		DrawDebugHud();
		
		return true;
	}
	
	if (gametype == GAMETYPE_PVI) {
		fillScreenColor(BLACK);	
		printTextCenter("This game type is not supported yet", RED);
		
		startScreen++;
		
		if (startScreen >= 180) {
			return false;	
		}
		else {
			return true;
		}
	}
	
	fillScreenColor(BLACK);
	return false;
}

void Checkers::DrawBoard() {
	
	for (int i=0; i<8; i++) { // row ligne
		for (int j=0; j<8; j++) { // col
			
			if (boardData[j][i].color == true) { // dark
				fillScreenRect(BOARD_DARK_COLOR, (startX + (j * BOARD_SQUARE_WIDTH))+1, (i * BOARD_SQUARE_HEIGHT)+1, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT);	
			}
			else { // light
				fillScreenRect(BOARD_LIGHT_COLOR, (startX + (j * BOARD_SQUARE_WIDTH))+1, (i * BOARD_SQUARE_HEIGHT)+1, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT);	
			}  
			
			//checker piece drawing
			if (boardData[j][i].piece != PIECE_NONE) {
				int offsetX = startX + (BOARD_SQUARE_WIDTH / 2) + (BOARD_SQUARE_WIDTH * j)+1;
				int offsetY = (BOARD_SQUARE_WIDTH / 2) + (BOARD_SQUARE_HEIGHT * i)+1;
				
				if (boardData[j][i].piece == PIECE_PLAYER_ONE) {
					drawFilledCircle(offsetX, offsetY, (PIECE_WIDTH / 2), PLAYER_ONE_COLOR);
					//fillImageRect(PLAYER_ONE_COLOR,(startX + (j * BOARD_SQUARE_WIDTH))+1, (i * BOARD_SQUARE_HEIGHT)+1, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT,PLAYER_ONE_IMAGE);
				}
				else if (boardData[j][i].piece == PIECE_PLAYER_TWO) {
					drawFilledCircle(offsetX, offsetY, (PIECE_WIDTH / 2), PLAYER_TWO_COLOR);
				}

				/*
				else if (boardData[j][i].piece == PIECE_PLAYER_ONE_KING) {
					drawFilledCircle(offsetX, offsetY, (PIECE_WIDTH / 2), PLAYER_TWO_COLOR);
				}
				else if (boardData[j][i].piece == PIECE_PLAYER_TWO) {
					drawFilledCircle(offsetX, offsetY, (PIECE_WIDTH / 2), PLAYER_TWO_COLOR);
				}*/
				else {
					printTextCenter("Unkown Piece", RED);	
				}
			}
			
			//selection drawing
			if (boardData[j][i].selected == true) { // blue
				drawRectangleScreen((startX + (j * BOARD_SQUARE_WIDTH))+1, (i * BOARD_SQUARE_HEIGHT)+1, BOARD_SQUARE_WIDTH-1, BOARD_SQUARE_HEIGHT-1, SELECTION_COLOR);
				drawRectangleScreen((startX + (j * BOARD_SQUARE_WIDTH))+2, (i * BOARD_SQUARE_HEIGHT)+2, BOARD_SQUARE_WIDTH-3, BOARD_SQUARE_HEIGHT-3, SELECTION_COLOR);
			}
			
			//move selection drawing
			if (boardData[j][i].moveSelection >= 0) { // green
				drawRectangleScreen((startX + (j * BOARD_SQUARE_WIDTH))+1, (i * BOARD_SQUARE_HEIGHT)+1, BOARD_SQUARE_WIDTH-1, BOARD_SQUARE_HEIGHT-1, SELECTED_COLOR);
				drawRectangleScreen((startX + (j * BOARD_SQUARE_WIDTH))+2, (i * BOARD_SQUARE_HEIGHT)+2, BOARD_SQUARE_WIDTH-3, BOARD_SQUARE_HEIGHT-3, SELECTED_COLOR);
			}
		}
	}
}

void Checkers::SelectSquare(signed int col, signed int row) {
	if(row > 7) {
		row = 0;	
	}
	if (row < 0) {
		row = 7;	
	}
	
	if (col > 7) {
		col = 0;	
	}
	if (col < 0) {
		col = 7;	
	}
	
	boardData[selectedCol][selectedRow].selected = false;
	boardData[col][row].selected = true;
	selectedRow = row;
	selectedCol = col;
}

void Checkers::StartFreshGame() { //initialise the board with collors and where we put a piece or not 
	bool squareColor;
	
	//setup board
	for (int i=0; i<8; i++) {
		if ((i % 2) == 0) {
			squareColor = true;
		}
		else {
			squareColor = false;
		}
		for (int j=0; j<8; j++) {
			boardData[j][i].col = j;
			boardData[j][i].row = i;
			boardData[j][i].color = squareColor; // if true vert else beige color 
			boardData[j][i].selected = false;
			boardData[j][i].moveSelection = -1;
			boardData[j][i].coords.col = j;
			boardData[j][i].coords.row = i;
			if (squareColor) {
				squareColor = false;
				if (j < 3) {
					boardData[j][i].piece = PIECE_PLAYER_ONE;
				}
				else if (j > 4) {
					boardData[j][i].piece = PIECE_PLAYER_TWO;
				}
				else {
					boardData[j][i].piece = PIECE_NONE;
				}
			}
			else {
				squareColor = true;
				boardData[i][j].piece = PIECE_NONE;
			}
		}	
	}
	
	boardData[selectedCol][selectedRow].selected = true;
	selectionTotal = 0;
}

void Checkers::DrawPlayerOneHud() {
	//print name
	printTextScreenVertLeft((startX - 10), 0, playerOneName, WHITE);	
	//printTextScreenVertRight(, (SCREEN_HEIGHT - (strlen(playerTwoName) * 8) -2), playerOneName, WHITE);	
}

void Checkers::DrawPlayerTwoHud() {
	//print name
	printTextScreenVertRight(((SCREEN_WIDTH - startX) +4), (SCREEN_HEIGHT - (strlen(playerTwoName) * 8) -2), playerTwoName, WHITE);	
}

void Checkers::DrawDebugHud(){
	////////////// print the debug
	printTextScreenVertLeft((startX - (BOARD_SQUARE_WIDTH *3)), BOARD_SQUARE_WIDTH *3, debugMessage, BLUE);
}

checkerPiece Checkers::GetSquare(signed int col, signed int row) {
	if (col < 0 || col > 7 || row < 0 || row > 7) { //out of bounds
		return PIECE_INVALID;	
	}
	else {
		return	boardData[col][row].piece;
	}
}

bool Checkers::HasMove() {
	int player_road;
	checkerPiece ennemi;
	if (curretPlayer == 0) { // PlayerOne set
        player_road=1;
        ennemi= PIECE_PLAYER_TWO;
	}else{
		player_road=-1;
        ennemi= PIECE_PLAYER_ONE;
	} 
    //basic moves
	if (GetSquare(selectedCol+(1*player_road), selectedRow-1) == PIECE_NONE) { // up and left
		return true;	
	}
	if (GetSquare(selectedCol+(1*player_road), selectedRow+1) == PIECE_NONE) { // up and right
		return true;
	}
	
	//skip moves
	if (GetSquare(selectedCol+(1*player_road), selectedRow-1) == ennemi) { // possible up and left skip
		if (GetSquare(selectedCol+(2*player_road), selectedRow-2) == PIECE_NONE) { // empty place after skip
			return true;
		}
	}
	if (GetSquare(selectedCol+(1*player_road), selectedRow+1) == ennemi) { // possible up and right skip
		if (GetSquare(selectedCol+(2*player_road), selectedRow+2) == PIECE_NONE) { // empty place after skip
			return true;
		}
	}
	return false;
}


////////////////////////////////// saut =quand on mange; deplacement= q on deplace d'une case ///////////////////////////////////////////////

bool Checkers::AvailableFromPreviousJump() {
	int player_road;
	checkerPiece ennemi;
	int *nbc=NULL;
	if (curretPlayer == 0) { // PlayerOne set
        player_road=1;
        ennemi= PIECE_PLAYER_TWO;
        nbc= &NB_PIECE_PLAYER_TWO;
	}else{
		player_road=-1;
        ennemi= PIECE_PLAYER_ONE;
        nbc= &NB_PIECE_PLAYER_ONE;
	} 
    
	if (selections[selectionTotal-1].col == (selectedCol - (2*player_road))) { // si on essaie un saut d'une case a droite (cas de manger une piece pour player one)
		if (selections[selectionTotal-1].row == (selectedRow - 2)) { // Saut vers le bas
			//check for jumped piece
			if (boardData[selectedCol-(1*player_road)][selectedRow-1].piece == ennemi) { // si trouve une piece deplayer TWo a manger 
				// del the eaten piece
                boardData[selectedCol-(1*player_road)][selectedRow-1].piece= PIECE_NONE;
                --*nbc;
                //sprintf(playerOneName, "A:%p,V:%da:%p,v:%d",&NB_PIECE_PLAYER_ONE,NB_PIECE_PLAYER_ONE,nbc,*nbc);
                if(*nbc <= 0)
                	wine= true;
				return true;
			}
		}
		if (selections[selectionTotal-1].row == (selectedRow + 2)) { // Saut vers le Haut
			if (boardData[selectedCol-(1*player_road)][selectedRow+1].piece == ennemi) {
				// del eaten piece
				boardData[selectedCol-(1*player_road)][selectedRow+1].piece =PIECE_NONE;
				--*nbc;
				sprintf(debugMessage, "nbc: %d",*nbc);
				if(*nbc <= 0)
                	wine= true;
				return true;
			}
		}
	}
    return false;	
}

loc Checkers::GetLoc(signed int c, signed int r) {
	if (c >= 0 && c < 8 && r >= 0 && r <8) {
		return boardData[c][r].coords;
	}
	loc tl;
	tl.row = -1;
	tl.col = -1;
	return tl;
}	

bool Checkers::AvailableFromMove() {
	int player_road;
	checkerPiece current_piece;
	if (curretPlayer == 0) { // PlayerOne set
        player_road=1;
        current_piece =PIECE_PLAYER_ONE;
	}else{
		player_road=-1;
        current_piece =PIECE_PLAYER_TWO;
	}
	if (boardData[selectedCol][selectedRow].piece == PIECE_NONE) {
		//check column
		if ((boardData[selectedCol][selectedRow].col-(1*player_road)) == selections[selectionTotal-1].col) {   // cas de déplacement par 1 player One 
			if (boardData[selectedCol][selectedRow].row == selections[selectionTotal-1].row-1) { // deplacemet de 1 vers le haut   
				if (boardData[selectedCol-(1*player_road)][selectedRow+1].piece == current_piece) { // si la piece a deplacer est de player_one 
					return true;
				}
			}
			if (boardData[selectedCol][selectedRow].row == selections[selectionTotal-1].row+1) { // deplacemet de 1 vers le bas
				if (boardData[selectedCol-(1*player_road)][selectedRow-1].piece == current_piece) { // si la piece a deplacer est de player_one 
					return true;
				}
			}
		}
	}
	return false;
}

bool Checkers::IStillPieceToEatForPOne(checkerPiece piece){
	// if the piece in right and up  of is eatable 
	int player_road,cur_col1,cur_col2,cur_row1,cur_row2;
	if (curretPlayer == 0) { // PlayerOne set
        player_road=1;
	}else{
		player_road=-1;
	}
	cur_col1= selectedCol+(1*player_road);
	cur_row1= selectedRow-1;

	cur_col2= selectedCol+(2*player_road);
	cur_row2= selectedRow-2;
    if(cur_col1 >7 || cur_col1 <0 || cur_col2 >7 || cur_col2 <0 || cur_row1 >7 || cur_row1 <0 || cur_row2 >7 || cur_row2 <0 )
    	return false;
	if (boardData[cur_col1][cur_row1].piece == piece && boardData[cur_col2][cur_row2].piece == PIECE_NONE ){ 
      return true;
    }

    cur_col1= selectedCol+(1*player_road);
	cur_row1= selectedRow+1;

	cur_col2= selectedCol+(2*player_road);
	cur_row2= selectedRow+2;
    if(cur_col1 >7 || cur_col1 <0 || cur_col2 >7 || cur_col2 <0 || cur_row1 >7 || cur_row1 <0 || cur_row2 >7 || cur_row2 <0 )
    	return false;
	if (boardData[cur_col1][cur_row1].piece == piece && boardData[cur_col2][cur_row2].piece == PIECE_NONE){
	 	return true;
	 }
	return false;
}
bool Checkers::IsValidSelection() {
	int player_road;
	checkerPiece ennemi;
	checkerPiece current_piece;
	if (curretPlayer == 0) { // PlayerOne set
        player_road=1;
        ennemi= PIECE_PLAYER_TWO;
        current_piece =PIECE_PLAYER_ONE;
	}else{
		player_road=-1;
        ennemi= PIECE_PLAYER_ONE;
        current_piece =PIECE_PLAYER_TWO;
	} 
	if (selectionTotal == 0) { // first selection
		if (boardData[selectedCol][selectedRow].piece == current_piece) { // current player's piece
			if (HasMove()) { // first selection can be any piece with a move
				return true;
			}
		}
	}
	else {
       // second or more move
		//check for undo
	    //sprintf(debugMessage,"Debug: ST: %d, moveS: %d",selectionTotal,boardData[selectedCol][selectedRow].moveSelection);
		if (boardData[selectedCol][selectedRow].moveSelection == selectionTotal && LOCK_JMP == false) {
			    sprintf(debugMessage,"undo");
				return true;	
		}
		if(boardData[selectedCol][selectedRow].piece == PIECE_NONE) { // available landing
			if(AvailableFromMove()) {
				//sprintf(debugMessage,"simple move square, and pass he hand"); 
				Make_move(selections[selectionTotal-1].col,selections[selectionTotal-1].row,selectedCol,selectedRow,current_piece);
				Remove_trace(selections[selectionTotal-1].col,selections[selectionTotal-1].row,selectedCol,selectedRow);
				Pass_theHand();
				//return true;
			}
			if(AvailableFromPreviousJump() ){
				sprintf(debugMessage, "Ilyes: %d, Zak: %d",NB_PIECE_PLAYER_ONE,NB_PIECE_PLAYER_TWO);
				Make_move(selections[selectionTotal-1].col,selections[selectionTotal-1].row,selectedCol,selectedRow,current_piece);
                //selectionTotal++;
                //selections[selectionTotal-1].col=selectedCol;
                //selections[selectionTotal-1].row=selectedRow;
                if (IStillPieceToEatForPOne(ennemi)) {
                    boardData[selections[selectionTotal-1].col][selections[selectionTotal-1].row].moveSelection = -1;
                    sceRtcGetCurrentTick( &lastSelection );
                    //sprintf(debugMessage,"jmp move, continue & lock");
                    LOCK_JMP=true;
                    return true; // continue to eat until we can't 
                }else{
                 	// delete selection and finish this acction
                  	Remove_trace(selections[selectionTotal-1].col,selections[selectionTotal-1].row,selectedCol,selectedRow);
                   	LOCK_JMP=false;
                   	Pass_theHand();
                   	//sprintf(debugMessage,"jmp move square, and pass he hand");
                }		
			}
		}
	}
	return false;
}

// pass the hand to the other player
void Checkers::Pass_theHand(){
	curretPlayer = 1 ^ curretPlayer;
}

// remove a move trace
void Checkers::Remove_trace(int prev_colm, int prev_row, int selectedCol, int selectedRow){
  selectionTotal=0;
  boardData[selectedCol][selectedRow].moveSelection = -1;
  sceRtcGetCurrentTick( &lastSelection );
  boardData[prev_colm][prev_row].moveSelection = -1;
  sceRtcGetCurrentTick( &lastSelection );
}

//move a piece
void Checkers::Make_move(int prev_colm, int prev_row, int selectedCol, int selectedRow, checkerPiece piece){
  //previous del piece
  boardData[prev_colm][prev_row].piece= PIECE_NONE;
  boardData[selectedCol][selectedRow].piece= piece;

}

void Checkers::HandleInput(SceCtrlData &pad) {
	sceRtcGetCurrentTick( &thisMove );
	sceRtcGetCurrentTick( &thisSelection );
	
	
	if( ((thisSelection - lastSelection)/((float)tickResolution)) >= SELECT_DELAY) {
		if (pad.Buttons & PSP_CTRL_SELECT) { // switch turns
			sprintf(debugMessage, "Debug: PSP_CTRL_SELECT, selecttotal: %d",selectionTotal);
			if (selectionTotal > 0) {
				//SwitchTurns();
			}
		}

		if (pad.Buttons & PSP_CTRL_CROSS) {
			if (!wine) {
				if (IsValidSelection()) {
					//sprintf(debugMessage, "Debug: Valid PSP_CTRL_CROSS");
					//check if selection needs to be removed
					if (boardData[selectedCol][selectedRow].moveSelection != -1) {
						//sprintf(debugMessage, "Debug: del slc: %d,%d", boardData[selectedCol][selectedRow].moveSelection,selectionTotal);
						boardData[selectedCol][selectedRow].moveSelection = -1;
						selectionTotal--;
						sceRtcGetCurrentTick( &lastSelection );
					}
					else { //adding selection
						boardData[selectedCol][selectedRow].moveSelection = selectionTotal + 1;
						//sprintf(debugMessage, "Debug: add slc: %d,%d", boardData[selectedCol][selectedRow].moveSelection,selectionTotal);
						selections[selectionTotal].row = selectedRow;
						selections[selectionTotal].col = selectedCol;
						//sprintf(debugMessage, "Debug: (%d,%d),(%d,%d)",selections[selectionTotal-1].col,selections[selectionTotal-1].row,selections[selectionTotal].col,selections[selectionTotal].row);
						
						selectionTotal++;
						sceRtcGetCurrentTick( &lastSelection );
					}
				}
				else {
					//sprintf(debugMessage, "Ilyes: %d, Zak: %d",NB_PIECE_PLAYER_ONE,NB_PIECE_PLAYER_TWO);
				}
			}
		}
	}
	
	if( ((thisMove - lastMove)/((float)tickResolution)) >= MOVE_DELAY) {
		if (playersTurn) {
			
			if (pad.Buttons & PSP_CTRL_DOWN) {
				SelectSquare(selectedCol, selectedRow+1);
				sceRtcGetCurrentTick( &lastMove );	
			}
			if (pad.Buttons & PSP_CTRL_UP) {
				SelectSquare(selectedCol, selectedRow-1);
				sceRtcGetCurrentTick( &lastMove );
			}
			
			if (pad.Buttons & PSP_CTRL_LEFT) {
				SelectSquare(selectedCol-1, selectedRow);
				sceRtcGetCurrentTick( &lastMove );
			}
			
			if (pad.Buttons & PSP_CTRL_RIGHT) {
				SelectSquare(selectedCol+1, selectedRow);
				sceRtcGetCurrentTick( &lastMove );
			}
		}
		else {
			//player two controls
		}
	}
}

