{$ifndef __GLOBAL_INC__}
{$define __GLOBAL_INC__}

{ // GAME DATA ///////////////////////////////////////////////////////////// }

const	BOARD_SIZE 	= 3;		{ size of the board }

type    INDEX_TYPE = 1..BOARD_SIZE;	{ indexing to the board }

	TILE_TYPE = (NULL,         	{ empty cell }
		     CROSS,		{ an X }
		     NOUGHT);		{ an O }

	COLOR_TYPE = CROSS..NOUGHT;	{ two players }

	GAME_STATE = (GAME_CROSS,	{ X wins }
		      GAME_NOUGHT,	{ O wins }
		      GAME_DRAW,	{ draw }
		      GAME_PLAYING);	{ still playing }

	GAME_MODE = (PVP,		{ player vs player or }
		     PVA);              { player vs AI        }

	{ a game board structure, ordered as (R,C) }
	BOARD_TYPE = array[INDEX_TYPE, INDEX_TYPE] of TILE_TYPE;

	GAME_BOARD = record
		x, y	: longint;	{ coordinate of the board }
		cc, cr	: longint;	{ current column and row  }
		board	: BOARD_TYPE;	{ board data              }
	end;

        { AI difficulty levels }
	AI_LEVEL	= (EASY, MEDIUM, HARD);

        APP_CONFIG	= record
        	mode	: GAME_MODE;	{ playing mode            }
                level	: AI_LEVEL;	{ AI difficulty           }
		clrX	: byte;		{ color for X             }
		clrO	: byte;		{ color for O             }
                isHeart	: boolean;	{ use heart icon flag     }
		reserve	: byte;		{ padding                 }
		sound	: boolean;	{ sound effects           }
        end;

	GAME_TYPE = record
		board	: GAME_BOARD;	{ our game board          }
		player	: COLOR_TYPE;	{ current player          }

		{ games won and tied }
		winX,
		winO,
		ties	: longint;
	end;

const	CMD_GAME_START	= 1000;
        CMD_GAME_MODE	= 1001;
        CMD_GAME_OPT	= 1002;
	CMD_GAME_INFO	= 1003;
	CMD_GAME_HELP	= 1004;
	CMD_GAME_EXIT	= 1005;
	CMD_MODE_HUMAN	= 1010;
	CMD_MODE_EASY	= 1011;
	CMD_MODE_MEDIUM	= 1012;
	CMD_MODE_HARD	= 1013;
        CMD_OPT_ICON	= 1020;
        CMD_OPT_COLOR_X	= 1021;
        CMD_OPT_COLOR_O	= 1022;
        CMD_OPT_SOUND	= 1023;

	BOARD_FRAME_WIDTH	= 82;
	BOARD_FRAME_HEIGHT	= 82;

	{ the board in the 160 x 100 mode }
	BX		= 39;		{ top left pixel of the board      }
	BY		= 6;
	CELL		= 26;		{ a square                         }
	LINE		= 2;		{ the lines between them           }
	PITCH		= CELL + LINE;
	STATUS_Y	= 92;
	GRID_C		= 7;
	WIN_C		= 14;
	PIECE_STEPS	= 12;		{ frames to draw a mark            }
	PIECE_TIME	= 15;		{ ms a frame                       }

        FILE_CONFIG	: string[12] = 'T3.CNF';
        FILE_HELP	: string[12] = 'T3.HLP';

	MENU_TEXT_CLR	: byte = $07;
	MENU_ACCENT_CLR	: byte = $5F;

	HUMAN		= NOUGHT;	{ minimizer's side      }
	COMPUTER	= CROSS;	{ maximizer's side      }
        COLOR_HILITE	= 1;		{ cell hilighting color }

	HEART_W		= 17;
	HEART_H		= 14;
	HEART_IMG	: array[0..HEART_H-1] of string[HEART_W] =
			('..XXXX.....XXXX..',
			 '.XXXXXX...XXXXXX.',
			 'XXXXXXXX.XXXXXXXX',
			 'XXXXXXXXXXXXXXXXX',
			 'XXXXXXXXXXXXXXXXX',
			 'XXXXXXXXXXXXXXXXX',
			 '.XXXXXXXXXXXXXXX.',
			 '..XXXXXXXXXXXXX..',
			 '...XXXXXXXXXXX...',
			 '....XXXXXXXXX....',
			 '.....XXXXXXX.....',
			 '......XXXXX......',
			 '.......XXX.......',
			 '........X........');
	MINI_HEART	: array[0..4] of string[7] =
			('.XX.XX.', 'XXXXXXX', '.XXXXX.', '..XXX..', '...X...');

        IS_HEART	: array[boolean] of char        = (' ', #3);
        PLAYER_SYMBOL	: array[COLOR_TYPE] of char     = ('X','O');
	LEVEL_NAMES	: array[AI_LEVEL] of string[6] = ('EASY', 'MEDIUM', 'HARD');

	APP_DEFAULT_CFG	: APP_CONFIG =	{ default configuration }
        	(mode	: PVA;
                 level	: MEDIUM;
                 clrX	: $09;
                 clrO	: $0D;
                 isHeart: TRUE;
		 reserve: 0;
		 sound	: TRUE);

var	GAME_CONFIG	: APP_CONFIG;	{ active configuration }

{$endif}
