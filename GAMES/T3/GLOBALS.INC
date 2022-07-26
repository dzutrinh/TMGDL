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
        end;

	GAME_TYPE = record
		board	: GAME_BOARD;	{ our game board          }
		player	: COLOR_TYPE;	{ current player          }

		{ just counters }
		cWinX,
		cWinO,
		cDraw	: PCOUNTER;
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

	CELL_WIDTH	= 8;
	CELL_HEIGHT	= 4;

        FILE_CONFIG	: string[12] = 'T3.CNF';
        FILE_HELP	: string[12] = 'T3.HLP';

	MENU_TEXT_CLR	: byte = $07;
	MENU_ACCENT_CLR	: byte = $5F;

	STATUS_X	= 60;
	STATUS_Y	= 8;

	INFO_X		= 5;
	INFO_Y		= 9;

	HUMAN		= NOUGHT;	{ minimizer's side      }
	COMPUTER	= CROSS;	{ maximizer's side      }
        COLOR_HILITE	= 1;		{ cell hilighting color }

	BOARD_FRAME	: array[1..15] of string[60] =
			('~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
                         '~08‹‹‹‹‹‹‹‹~78ﬂ~08‹‹‹‹‹‹‹~78ﬂ~08‹‹‹‹‹‹‹‹',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
                         '~08‹‹‹‹‹‹‹‹~78ﬂ~08‹‹‹‹‹‹‹~78ﬂ~08‹‹‹‹‹‹‹‹',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        €       €        ',
			 '~08        ﬂ       ﬂ        ');

	GAME_LOGO	: array[1..4] of string[50] =
		('~08⁄¬ø ˘ ⁄ø    ⁄¬ø ⁄ø ⁄ø    ⁄¬ø ⁄ø ⁄ø',
		 '~04 ≥  ≥ ≥≥  ~05   ≥  √≥ ≥≥  ~09   ≥  ≥≥ √ ',
		 '~0C ≥  ≥ ≥   ~0D   ≥  ≥≥ ≥   ~0B   ≥  ≥≥ ≥≥',
		 '~0F Ÿ  Ÿ ¿Ÿ     Ÿ  ŸŸ ¿Ÿ     Ÿ  ¿Ÿ ¿Ÿ');

	{ built-in player icons }
	ICON_CROSS	: BLOCK_TYPE =
        	(x: 0; y: 0; w: 5; h: 3; size: 15;
		 data:'ﬂ‹ ‹ﬂ'+
                      ' ‹ﬂ‹ '+
                      'ﬂ   ﬂ'; attach: FALSE);
	ICON_NOUGHT	: BLOCK_TYPE =
        	(x: 0; y: 0; w: 5; h: 3; size: 15;
		 data:'‹ﬂﬂﬂ‹'+
                      '€   €'+
                      ' ﬂﬂﬂ '; attach: FALSE);
	ICON_HEART	: BLOCK_TYPE =
        	(x: 0; y: 0; w: 5; h: 3; size: 15;
		 data:'‹€‹€‹'+
                      'ﬂ€€€ﬂ'+
                      '  ﬂ  '; attach: FALSE);

        BLOCK_CROSS	: PBLOCK = nil;	{ player icons          }
        BLOCK_NOUGHT	: PBLOCK = nil;
        IS_HEART	: array[boolean] of char        = (' ', #3);
        PLAYER_SYMBOL	: array[COLOR_TYPE] of char     = ('X','O');
	TEXT_LEVEL	: array[AI_LEVEL] of string[24] =
			('   ~05AI~07=~03EASY    ',
			 '  ~05AI~07=~06MEDIUM   ',
			 '   ~05AI~07=~04HARD    ');

	APP_DEFAULT_CFG	: APP_CONFIG =	{ default configuration }
        	(mode	: PVA;
                 level	: MEDIUM;
                 clrX	: $09;
                 clrO	: $0D;
                 isHeart: TRUE;
		 reserve: 0);

var	GAME_CONFIG	: APP_CONFIG;	{ active configuration }

{$endif}
