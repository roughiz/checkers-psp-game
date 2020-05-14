#ifndef __CHECKERS_H_
#define __CHECKERS_H_

#include "menu.h"
#include "gstate.h"
#include "graphics.h"

const int BOARD_SQUARE_WIDTH = 34;
const int BOARD_SQUARE_HEIGHT = 34;

const int PIECE_WIDTH = 25;
const int PIECE_HEIGHT = 25;

const Color BOARD_DARK_COLOR = RGB(0, 75, 0);
const Color BOARD_LIGHT_COLOR = RGB(200, 190, 160);

const Color PLAYER_ONE_COLOR = RED;
const Color PLAYER_TWO_COLOR = BLACK;
const Color CROWN_COLOR = WHITE;

const Color SELECTION_COLOR = BLUE;
const Color SELECTED_COLOR = GREEN;


const float MOVE_DELAY = 0.20f;
const float SELECT_DELAY = 0.20f;

enum checkerPiece {
	PIECE_PLAYER_ONE,
	PIECE_PLAYER_TWO,
	PIECE_PLAYER_ONE_KING,
	PIECE_PLAYER_TWO_KING,
	PIECE_NONE,
	PIECE_INVALID
};

struct loc {
	signed int row;
	signed int col;
};

struct loc2 {
	signed int row;
	signed int col;
	bool behindObs=false;
};

struct boardSquare {
	int row; // row 0 is player one's first row
	int col; // col 0 is the left side of a northern facing board
	checkerPiece piece; // piece status
	bool color; // square color, true = dark, false=light
	bool selected; // selected or not
	signed int moveSelection; // selection number, defaults to -1
	loc coords;
	bool isCrowd=false;
};

class Checkers {
private:
	gstate gametype;
	gstate difficulty;
	Menu* menu;
	int startScreen;
	int startX;
	int NB_PIECE_PLAYER_TWO;
	int NB_PIECE_PLAYER_ONE;
	bool LOCK_JMP;
	int curretPlayer;
	bool wine;
	boardSquare boardData[8][8];
	
	u64 thisMove;
	u64 lastMove;
	u64 thisSelection;
	u64 lastSelection;
	u32 tickResolution;
	
	char playerOneName[50];
	char playerTwoName[50];
	char debugMessage[50];
	
	bool inAnimation;
	
	int selectedRow;
	int selectedCol;
	
	int selectionTotal;
	loc selections[32];

	int possibleSelectTotal;
    loc2 possibleSelections[32];
	
	bool playersTurn;
	
	void DrawBoard(void);
	void StartFreshGame(void);
	void DrawPlayerOneHud(void);
	void DrawPlayerTwoHud(void);
	void DrawDebugHud(void); ////////////////////////////
	void HandleInput(SceCtrlData &pad);
	void SelectSquare(signed int row, signed int col);
	bool IsValidSelection(void);
	bool HasMove(void);
	bool AvailableFromPreviousJump(void);
	bool AvailableFromMove(int pcol, int prow, checkerPiece current_piece);
	void Make_move(int prev_colm, int prev_row, int selectedCol, int selectedRow, checkerPiece piece);
	void Remove_trace(int prev_colm, int prev_row, int selectedCol, int selectedRow);
	void Pass_theHand();
	bool CanSimplyMove();
	void CanBeMovedToSelectedSquare(int col, int row);
	bool IStillPieceToEatForPOne(checkerPiece piece);
	void DelPreviousSelectedSquare();
	void UpdateNewSelection();
	checkerPiece GetSquare(signed int col, signed int row);
	loc GetLoc(signed int c, signed int r);
	
public:
	Checkers(void);
	virtual ~Checkers(void);
	
	bool Run(SceCtrlData &pad);
};

#endif /*__CHECKERS_H_*/
