{$ifndef __GLOBALS_INC__}
{$define __GLOBALS_INC__}

const	C_ALIVE	= 1;				{ denote an alive cell }
	C_DEAD	= 0;				{ denote a dead cell   }

        { field parameters }
	FIELD_W	= SCR_HEIGHT;			{ field width          }
	FIELD_H	= SCR_HEIGHT;			{ field height         }
	FIELD_C	= #177#177;			{ field pixel          }

	BOARD_W	= FIELD_W shl 1;		{ board dimensions     }
	BOARD_H	= FIELD_H;
	BOARD_X	= 1;				{ board coordinates    }
	BOARD_Y	= (SCR_HEIGHT - BOARD_H) shr 1 + 1;

        PANEL_X	= 53;
        PANEL_Y = 1;
        PANEL_W = SCR_WIDTH - BOARD_W - 4;
	PANEL_W2= PANEL_W div 2;

type    STATE		= byte;
	{ represents a 2D grid, used as a colony }
	FIELD_TYPE	= array[0..FIELD_H*FIELD_W-1] of STATE;

	COLONY		= ^COLONY_TYPE;		{ represent a colony  }
	COLONY_TYPE	= record
		field	: FIELD_TYPE;		{ current generation  }
		next	: FIELD_TYPE;		{ next generation     }
	end;

        APP_CONFIG	= record
                speed	: word;			{ game speed          }
                step	: boolean;		{ stepping mode flag  }
                invert	: boolean;		{ invert color        }
                c_alive	: byte;			{ alive cell color    }
                c_dead	: byte;			{ dead cell color     }
        end;

        MAP_MAGIC	= array[1..3] of char;	{ map file signature  }
        STATUS_MSG	= string[FIELD_W];	{ store status msg    }

        { user interface mode }
        UI_MODE	= (UI_MENU, UI_RUN, UI_EDIT, UI_PLACE);

const   FILE_CONFIG	: string[12] = 'GOL.CNF';
	FILE_HELP	: string[12] = 'GOL.HLP';
	FILE_COLONY	: string[12] = 'GOL.MAP';

        { number of predefined patterns }
        PATTERNS_MAX	= 12;
	
        MENU_TEXT_CLR	: byte = $07;
        MENU_ACCENT_CLR	: byte = $2F;

        MSG_RUN		: STATUS_MSG = 'SIMULATION     ';
        MSG_EDIT	: STATUS_MSG = 'EDITING COLONY ';
        MSG_READY	: STATUS_MSG = 'READY          ';
        MSG_PLACE	: STATUS_MSG = 'PLACING PATTERN';
        MSG_RUN_AUTO	: STATUS_MSG = '~03AUTO     ';
        MSG_RUN_PAUSED	: STATUS_MSG = '~0BPAUSED   ';
        MSG_RUN_STEPPING: STATUS_MSG = '~0DSTEPPING ';
        MSG_RUN_RANDOM	: STATUS_MSG = '~05RANDOMIZE';

        MAP_SIGNATURE	: MAP_MAGIC = 'GMF';

        SIMUL_SPEED_MAX	= 10;
        SIMUL_SPEED_MIN = 1;

        CMD_EDIT	= 1001;
	CMD_START	= 1002;
        CMD_OPTION	= 1003;
        CMD_OPT_INVERT	= 1004;
        CMD_OPT_ALIVE	= 1005;
        CMD_OPT_DEAD	= 1006;
        CMD_ABOUT	= 1007;
        CMD_HELP	= 1008;
        CMD_EXIT	= 1009;

        GAME_LOGO	: array[1..4] of string[PANEL_W+4] =
 			  ('~0F Ú¿Ú¿Ú¿¿Ú¿  Ú¿Ú¿  ¿ ùÚ¿Ú¿ ',
			   '~0E ³¿Ã³³ ³Ã   ³³Ã   ³ ³Ã Ã  ',
			   '~0A ³³³³³ ³³³  ³³³   ³³³³ ³³ ',
			   '~02 ÀÙÙÙÙ ÙÀÙ  ÀÙÙ   ÀÙÙÙ ÀÙ ');

	APP_DEFAULT_CFG	: APP_CONFIG =
		(speed	: 2;
                 step   : FALSE;
		 invert	: FALSE;
		 c_alive: 15;
		 c_dead	: 3);

var 	vscreen		: SCREEN;		{ double-buffering   }
	GAME_COLONY	: COLONY;		{ our game colony    }
	GAME_CONFIG	: APP_CONFIG;

{$endif}

