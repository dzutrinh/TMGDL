{$ifndef	__GLOBALS_TETRIS__}
{$define	__GLOBALS_TETRIS__}

{////////////////////////////////////////////////////////////////////////////}

type	PIECE_SHAPE	= array[1..2] of char;

const	BOARD_WIDTH	= 12;
	BOARD_HEIGHT	= 25;
	CELL		= 4;	{ a block: 4 x 4 pixels of the 160 x 100 screen }
	BOARD_X		= (LORES_W - BOARD_WIDTH * CELL) div 2;
	BOARD_Y		= 0;

	PIECE_SIZE	= 4;	{ size of each piece }
	PIECE_MAX	= 7;	{ maximum 7 piece types }
	MAX_LEVELS	= 10;

        BOARD_BLANK	= 0;
	{
	BOARD_PIECE_I	= 1;
	BOARD_PIECE_Z	= 2;
	BOARD_PIECE_O	= 3;
	BOARD_PIECE_T	= 4;
	BOARD_PIECE_S	= 5;
	BOARD_PIECE_L	= 6;
	BOARD_PIECE_J	= 7;
	}
	BOARD_WIPE	= 8;
	BOARD_SHADOW	= 9;
	BOARD_BORDER	= 10;

{////////////////////////////////////////////////////////////////////////////}

type	TETROMINO	= array[0..PIECE_SIZE*PIECE_SIZE-1] of byte;

	MOVE_TYPE	= record
		piece	: longint;
		rotate	: longint;
	end;

	BOARD_TYPE	= array[0..BOARD_WIDTH*BOARD_HEIGHT-1] of byte;

	PIECE_TYPE	= array[1..PIECE_MAX] of TETROMINO;

	MARKER_TYPE	= record
		list	: array[1..BOARD_HEIGHT] of byte;
		size	: longint;
	end;

	KEY_MAPPER	= (MAPPER_DOWN,		{ default S }
			   MAPPER_LEFT,		{ default A }
			   MAPPER_RIGHT,	{ default D }
			   MAPPER_SPIN_CW,	{ default W }
			   MAPPER_SPIN_CCW,	{ default Q }
			   MAPPER_PAUSE,	{ default P }
			   MAPPER_HINT,       	{ default H }
			   MAPPER_DROP,		{ default SPACE }
			   MAPPER_QUIT);	{ default ESC }
	CONTROLS	= array[KEY_MAPPER] of word;
	KEY_SELECTOR	= (KS_WSAD, KS_ARROWS);

	CPAL_TYPE	= array[BOARD_BLANK..BOARD_BORDER] of byte;

        { for storing configuration }
        APP_CONFIG	= record
        	control	: KEY_SELECTOR;		{ keyboard mapper  }
                hint	: boolean;		{ show hint        }
                next	: boolean;		{ show next piece  }
                speed	: word;			{ starting speed   }
                height	: word;			{ starting height  }
                cblind	: boolean;		{ color scheme     }
                scheme	: CPAL_TYPE;
                sound	: boolean;		{ sound effects    }
        end;

{////////////////////////////////////////////////////////////////////////////}

const	TETRIS_PIECES	: array[1..PIECE_MAX] of TETROMINO =
		((0,0,0,0,
		  1,1,1,1,		{ I }
		  0,0,0,0,
		  0,0,0,0),

		 (0,0,0,0,
		  0,1,1,0,		{ Z }
		  0,0,1,1,
		  0,0,0,0),

		 (0,0,0,0,
		  0,1,1,0,		{ O }
		  0,1,1,0,
		  0,0,0,0),

		 (0,0,0,0,
		  0,1,0,0,		{ T }
		  1,1,1,0,
		  0,0,0,0),

		 (0,0,0,0,
		  0,1,1,0,		{ S }
		  1,1,0,0,
		  0,0,0,0),

		 (0,0,0,0,
		  0,0,1,0,		{ L }
		  1,1,1,0,
		  0,0,0,0),

		 (0,0,0,0,
		  0,1,0,0,		{ J }
		  0,1,1,1,
		  0,0,0,0));

	LEVEL_LINES	: array[1..MAX_LEVELS] of longint =
			(10, 15, 20, 25, 30,
			 35, 40, 45, 50, 55);

	LEVEL_SPEED	: array[1..MAX_LEVELS] of longint =
			(30, 28, 26, 24, 22,
			 20, 19, 18, 17, 16);


	CONTROLS_WSAD	: CONTROLS =
			  (KEY_S,
			   KEY_A,
			   KEY_D,
			   KEY_W,
			   KEY_Q,
			   KEY_P,
			   KEY_H,
			   KEY_SPACE,
			   KEY_ESC);

	CONTROLS_ARROWS	: CONTROLS =
			  (KEY_DOWN,
			   KEY_LEFT,
			   KEY_RIGHT,
			   KEY_UP,
			   KEY_SLASH,
			   KEY_P,
			   KEY_H,
			   KEY_ENTER,
			   KEY_ESC);

{////////////////////////////////////////////////////////////////////////////}

const	PNL_NEXT_X	= 55;
	PNL_NEXT_Y	= 4;

	PNL_SCORE_X	= 55;
	PNL_SCORE_Y	= PNL_NEXT_Y + 7;

	PNL_PIECES_X	= 55;
	PNL_PIECES_Y	= PNL_SCORE_Y + 4;

	PNL_LEVEL_X	= 55;
	PNL_LEVEL_Y	= PNL_PIECES_Y + 4;

	PNL_LINES_X	= 55;
	PNL_LINES_Y	= PNL_LEVEL_Y + 4;

	PNL_HOF_X	= 3;
	PNL_HOF_Y	= 19;

	PNL_CONTROL_X	= 3;
	PNL_CONTROL_Y	= 6;

	CMD_GAME_NEW	= 1000;
	CMD_GAME_HOF	= 1001;
	CMD_GAME_HELP   = 1002;
	CMD_GAME_INFO	= 1003;
	CMD_GAME_QUIT	= 1004;

        CMD_OPT_CBLIND  = 1100;
	CMD_OPT_SPEED	= 1101;
	CMD_OPT_HEIGHT	= 1102;
	CMD_OPT_HINT	= 1103;
	CMD_OPT_NEXT	= 1104;
	CMD_OPT_CONTROL	= 1105;
	CMD_OPT_SOUND	= 1106;

	CMD_CTRLS_ARROWS= 2001;
	CMD_CTRLS_WASD	= 2002;

{////////////////////////////////////////////////////////////////////////////}

	FILE_CONFIG	: string[12] = 'TETRIS.CNF';
	FILE_HELP	: string[12] = 'TETRIS.HLP';
	FILE_SCORES	: string[12] = 'TETRIS.HI';
	SCORE_NAMES	= 'STEVE|KACEY|JOSH|CHRIS|MIRANDA';

	MENU_TEXT_CLR	: byte = $07;
	MENU_ACCENT_CLR	: byte = $1F;

	{ color schemes }
	COLOR_SCHEME	: CPAL_TYPE = ( $08,	{ BOARD_BLANK }
					$0B,	{ I }
					$0C,	{ Z }
					$0E, 	{ O }
					$0D,	{ T }
					$0A,	{ S }
					$06,	{ L }
					$09,	{ J }
					$0F,	{ BOARD WIPE }
					$08,	{ BOARD SHADOW }
					$07);	{ BOARD BORDER }

	{ color-blind scheme }
	CBLIND_SCHEME	: CPAL_TYPE = ( $08,	{ BOARD_BLANK }
					$0F,	{ I }
					$0F,	{ Z }
					$0F, 	{ O }
					$0F,	{ T }
					$0F,	{ S }
					$0F,	{ L }
					$0F,	{ J }
					$0F,	{ BOARD WIPE }
					$08,	{ BOARD SHADOW }
					$07);	{ BOARD BORDER }

	GAME_LINES	: PCOUNTER  = nil; { number of lines collapsed    }
	GAME_PIECES	: PCOUNTER  = nil; { number of pieces generated   }
	GAME_LEVEL	: PCOUNTER  = nil; { current play level           }
	GAME_SCORE	: PCOUNTER  = nil; { current play score           }
	GAME_KEYS	: CONTROLS  = (0, 0, 0, 0, 0, 0, 0, 0, 0);

        { default app configuration }
        APP_DEFAULT_CFG	: APP_CONFIG =
        	(control: KS_WSAD;	{ game controlling schemes     }
                 hint	: TRUE;		{ toggle piece shadow hint     }
                 next	: TRUE;		{ toggle next piece flag       }
                 speed	: 1;		{ play speed, level dependant  }
                 height	: 0;		{ number of pre-generated rows }
		 cblind : FALSE;	{ color blind mode             }
                 { default color scheme }
                 scheme	: ($08, $0B, $0C, $0E, $0D, $0A, $06, $09, $0F, $08, $07);
                 sound	: TRUE);

var	GAME_CONFIG	: APP_CONFIG;

var	GAME_SCORES	: HISCORE_TABLE;		{ the best scores, in TETRIS.HI }

{$endif}