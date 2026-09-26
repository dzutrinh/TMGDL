{$ifndef	__GLOBALS_2048__}
{$define	__GLOBALS_2048__}

const
	COPYRIGHTS	: string = #13#10+
				   '2048 (c) 2019 by Infinity Group'#13#10+
				   'Coded by Trinh D.D. Nguyen';

{ ////////////////////////////////////////////////////////////////////////// }

{ Board constants }
const   BOARD_DIM	= 4;
	BOARD_SIZE	= BOARD_DIM*BOARD_DIM;
	BOARD_W		= 6;
	BOARD_H		= BOARD_W shr 1;
	BOARD_X		= (SCR_WIDTH-BOARD_DIM * BOARD_W) shr 1 + 1;
	BOARD_Y 	= 11;
	BOARD_FILLER	= #32#32#32#32#32#32;
	ANIM_FRAMES	= 6;		{ frames per slide                  }
	FRAME_TIME	= 14;		{ ms a frame of the slide           }

	{ the board in the 160 x 100 mode }
	LX		= 35;		{ top left pixel of its frame       }
	LY		= 5;
	TILE		= 20;		{ a tile, in pixels                 }
	GAP		= 2;		{ between two tiles                 }
	PITCH		= TILE + GAP;
	FRAME_C		= 8;
	EMPTY_C		= 0;

type    TILE_TYPE	= longint;
	VEC4_TYPE    	= array[0..BOARD_DIM-1] of TILE_TYPE;
	BOARD_TYPE	= array[0..BOARD_DIM-1] of VEC4_TYPE;

	DIR_TYPE	= (DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT);
	BIG_TYPE	= array[0..BOARD_DIM-1, 0..BOARD_DIM-1] of boolean;

	{ a tile's path during one slide, in board cells }
	ANIM_TILE	= record
		value,			{ log2 of the tile value before the slide }
		fx, fy,
		tx, ty	: longint;
	end;
	ANIM_TYPE	= record
		count	: longint;
		tiles	: array[0..BOARD_SIZE-1] of ANIM_TILE;
	end;

const   GAME_WINCOND	: longint = 11;
	GAME_SCORE   	: longint = 0;
	GAME_HISCORE	: longint = 500;
	GAME_MOVES	: longint = 0;
        FILE_HELP	: string[13] = '2048.HLP';
	FILE_SCORE	: string[13] = '2048.HI';
	SCORE_NAMES	= 'GABRIELE|TWO|FOUR|EIGHT|SIXTEEN';
	FILE_SAVE	: string[13] = '2048.SAV';
	FILE_CONFIG	: string[13] = '2048.CNF';
	CELL_COLORS  	: array[0..16] of byte =
			($80, $70,
			 $10, $20, $30, $40, $50, $90, $A0,
			 $B0, $C0, $D0, $E0, $F0, $F0, $F0, $F0);
	{ the tiles of 2, 4, 8 ... in the 160 x 100 mode }
	TILE_BG		: array[1..17] of byte =
			(7, 15, 6, 12, 4, 5, 14, 10, 2, 11, 9, 1, 3, 13, 8, 8, 8);
	TILE_FG		: array[1..17] of byte =
			(0, 0, 15, 15, 15, 15, 0, 0, 15, 0, 15, 14, 15, 15, 15, 15, 15);

{ ////////////////////////////////////////////////////////////////////////// }

const
	HELP_X		= 5;
	HELP_Y		= 12;
	INFO_X  	= 62;
	INFO_SCORE	= 13;
	INFO_HISCORE	= 16;
	INFO_MOVES	= 19;

type	APP_CONFIG	= record
		sound	: boolean;		{ sound effects }
	end;

const	APP_DEFAULT_CFG	: APP_CONFIG = (sound: TRUE);

var	GAME_SCORES	: HISCORE_TABLE;		{ the best scores, in 2048.HI }
	GAME_CONFIG	: APP_CONFIG;

{$endif}