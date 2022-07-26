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
	BOARD_FILLER	= #177#177#177#177#177#177;

type    TILE_TYPE	= longint;
	VEC4_TYPE    	= array[0..BOARD_DIM-1] of TILE_TYPE;
	BOARD_TYPE	= array[0..BOARD_DIM-1] of VEC4_TYPE;

const   GAME_WINCOND	: longint = 11;
	GAME_SCORE   	: longint = 0;
	GAME_HISCORE	: longint = 500;
	GAME_MOVES	: longint = 0;
        FILE_HELP	: string[13] = '2048.HLP';
	FILE_SCORE	: string[13] = '2048.SCR';
	FILE_SAVE	: string[13] = '2048.SAV';
	CELL_COLORS  	: array[0..16] of byte =
			($80, $70,
			 $10, $20, $30, $40, $50, $90, $A0,
			 $B0, $C0, $D0, $E0, $F0, $F0, $F0, $F0);

{ ////////////////////////////////////////////////////////////////////////// }

const
	HELP_X		= 5;
	HELP_Y		= 12;
	INFO_X  	= 62;
	INFO_SCORE	= 13;
	INFO_HISCORE	= 16;
	INFO_MOVES	= 19;

{$endif}