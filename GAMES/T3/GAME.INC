{$ifndef __GAME_INC__}
{$define __GAME_INC__}

{ // GAME HANDLERS ///////////////////////////////////////////////////////// }

procedure       gameInit(major, minor: longint; build: string);			forward;
procedure	gameShutdown;                                                   forward;
procedure	gameMenu;                                                       forward;
procedure	gamePlay;                                                       forward;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	gameStatus(redraw: boolean);                                    forward;
procedure	gameShowVersion;                                                forward;
procedure	gameBackground;                                                 forward;
procedure	gameReset;                                                      forward;
procedure	gameSwitchPlayer;                                               forward;
function        gameWin(p: COLOR_TYPE): boolean;                           	forward;
function	gameState: GAME_STATE;                                          forward;
function	gameAbort: boolean;                                             forward;
procedure	gameMoveHuman(c, r: integer);                          		forward;
procedure	gameMoveComputer;                                               forward;
procedure	gameAbout;                                                      forward;
procedure	gameHelp;                                                       forward;

{ ////////////////////////////////////////////////////////////////////////// }

var	game	: GAME_TYPE;
	mode,
	notif	: PLABEL;

procedure       gameInit(major, minor: longint; build: string);
begin
        if not file_exist(FILE_HELP) then
		die(-1, 'Missing game file. Please reinstall.');

	ltx_init;

        if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
                GAME_CONFIG := APP_DEFAULT_CFG;

	GAME_VERSION	:= major shl 8 + minor;
	GAME_BUILD	:= build;
	GAME_COPYRIGHTS	:= 'TIC-TAC-TOE (c) 2019 by Infinity Group'#13#10+
			   'Coded by Trinh D.D. Nguyen';

	cursor(CURSOR_OFF);

	game.cWinX  := counterCreate(0, STATUS_X,  STATUS_Y+4,  17, 0, 0, SHADE_BLUE, '~0BX ~07WINS' );
	game.cWinO  := counterCreate(0, STATUS_X,  STATUS_Y+8,  17, 0, 0, SHADE_BLUE, '~0BO ~07WINS' );
	game.cDraw  := counterCreate(0, STATUS_X,  STATUS_Y+12, 17, 0, 0, SHADE_CYAN,  'TIES'  );
	notif	    := labelCreate(-1, 24, 20, '');
	mode	    := labelCreate(INFO_X + 2, INFO_Y + 11, 12, '');

	boardInit(game.board, ltx_centerx(boardWidth)+1, ltx_centery(boardHeight)+2);
end;

procedure	gameShutdown;
begin
        config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));

	labelDestroy(mode);
	labelDestroy(notif);
	counterDestroy(game.cWinX);
	counterDestroy(game.cWinO);
	counterDestroy(game.cDraw);
	cursor(CURSOR_ON);
	ltx_done;

	writeln(' _______ _______ ______ _______ _______ ______ _______ _______ _______ ');
	writeln('|_     _|_     _|      |_     _|   _   |      |_     _|       |    ___|');
	writeln('  |   |  _|   |_|   ---| |   | |       |   ---| |   | |   -   |    ___|');
	writeln('  |___| |_______|______| |___| |___|___|______| |___| |_______|_______|');

	write  ('v');
	write  (vtos(GAME_VERSION, GAME_BUILD));
	writeln(' [', COMPILER, ']');
	writeln('Thanks for playing!');
	writeln;
end;

procedure	gameShowVersion;
begin
	prints(72, 25, $08, 'v' + vtos(GAME_VERSION, GAME_BUILD));
end;

procedure	gameBackground;
begin
	ANSI_draw(BACKGD_IMG);
end;

{$R-}
function        gamePlayerSymbol(player: COLOR_TYPE): STR8;
var	c	: COLOR_TYPE;
	a	: byte;
begin
	c := COLOR_TYPE(player);
        case c of
        CROSS	: a := GAME_CONFIG.clrX;
        NOUGHT	: a := GAME_CONFIG.clrO;
        end;
        gamePlayerSymbol := make_attr(a) + PLAYER_SYMBOL[c];
end;
{$R+}

procedure	gameHelp;
var	hb	: PHELPBOX;
begin
	hb := helpboxCreate(FILE_HELP, 'HELP', -1, -1);
        helpboxExec(hb);
        helpboxDestroy(hb);
end;

procedure	gameAbout;
begin
	aboutBox(-1, 15, '~0CTIC~07-~0DTAC~07-~09TOE ~07FOR DOS'+ENDL+
			 'CODED BY ~05TRINH D.D. NGUYEN');
end;

procedure	gameStatus(redraw: boolean);
var	i	: integer;
begin
	if redraw then
	begin
		gameShowVersion;

		for i := 1 to 4 do center(i+1, GAME_LOGO[i]);

		shaded_box(INFO_X, INFO_Y, 16, 9, SHADE_WHITE, AF_LEFT, 'HELP');
		shaded_box(INFO_X, INFO_Y+10, 16, 3, SHADE_WHITE, AF_LEFT, 'MODE');
		shaded_box(STATUS_X, STATUS_Y, 17, 3, SHADE_WHITE, AF_LEFT, 'TURN');

		print(INFO_X+2, INFO_Y+1, '~0BSPACE: ~03PLAY');
		print(INFO_X+2, INFO_Y+2, '~0A    W: ~02UP');
		print(INFO_X+2, INFO_Y+3, '~0A    A: ~02DOWN');
		print(INFO_X+2, INFO_Y+4, '~0A    S: ~02LEFT');
		print(INFO_X+2, INFO_Y+5, '~0A    D: ~02RIGHT');
		print(INFO_X+2, INFO_Y+6, '~0E    H: ~06HELP');
		print(INFO_X+2, INFO_Y+7, '~0C  ESC: ~04QUIT');
	end;

	print(STATUS_X+7, STATUS_Y+1, '~07['+gamePlayerSymbol(game.player)+'~07]');

	labelShow(mode);
	case GAME_CONFIG.mode of
	PVP: labelSetText(mode, '  ~01VS ~09HUMAN  ');
	PVA: labelSetText(mode, TEXT_LEVEL[GAME_CONFIG.level]);
	end;

	counterShow(game.cWinX);
	counterShow(game.cWinO);
	counterShow(game.cDraw);
end;

procedure	gameReset;
begin
	clear($07);

	boardClear(game.board);
	boardDraw (game.board);

	counterShow(game.cWinX);
	counterShow(game.cWinO);
	counterShow(game.cDraw);

	game.player := HUMAN;

	gameStatus(TRUE);
end;

procedure	gameSwitchPlayer;
begin
	game.player := boardOpponent(game.player);
end;

function        gameWin(p: COLOR_TYPE): boolean;
var	i	: integer;
begin
        {$R-}
	with game.board do
	begin
		for i := 1 to 3 do
			if (equals(board[i,1], board[i,2], board[i,3]) and
			   (board[i,1] = p)) then
			begin
				gameWin := TRUE;
				exit;
			end;
		for i := 1 to 3 do
			if (equals(board[1,i], board[2,i], board[3,i]) and
			   (board[1,i] = p)) then
			begin
				gameWin := TRUE;
				exit;
			end;
		if (equals(board[1,1], board[2,2], board[3,3]) and
		   (board[1,1] = p)) then
			begin
				gameWin := TRUE;
				exit;
			end;
		if (equals(board[1,3], board[2,2], board[3,1]) and
		   (board[1,3] = p)) then
			begin
				gameWin := TRUE;
				exit;
			end;
	end;
        {$R+}
	gameWin := FALSE;
end;

function	gameState: GAME_STATE;
begin
	if gameWin(COMPUTER) then	gameState := GAME_CROSS else
	if gameWin(HUMAN) then		gameState := GAME_NOUGHT else
	if boardFull(game.board) then
		gameState := GAME_DRAW
	else	gameState := GAME_PLAYING;
end;

function	gameAbort: boolean;
begin
	labelSetText(notif, 'WAIT');
	gameAbort := query(SHADE_RED, '~0CABORT CURRENT GAME (~0FY~0C/~0FN~0C)?');
end;

procedure	gameMoveHuman(c, r: integer);
begin
	if GAME_CONFIG.mode = PVA then
	begin
		boardPut(game.board, c, r, HUMAN);
		gameSwitchPlayer;
	end
	else
	begin
		boardPut(game.board, c, r, game.player);
		gameSwitchPlayer;
	end;
end;

procedure	gameMoveComputer;
var	c, r	: INDEX_TYPE;
begin
	aiBestMove(game.board, c, r);
	boardPut(game.board, c, r, COMPUTER);
	gameSwitchPlayer;
end;

procedure	gamePlay;
var	quit	: boolean;
	key	: word;
	c, r	: INDEX_TYPE;
	state	: GAME_STATE;
	cell	: TILE_TYPE;
begin
	gameReset;

        BLOCK_CROSS := @ICON_CROSS;
        if GAME_CONFIG.isHeart then
	        BLOCK_NOUGHT := @ICON_HEART
        else	BLOCK_NOUGHT := @ICON_NOUGHT;

	labelShow(notif);
	quit	:= FALSE;
	c	:= 2;
	r	:= 2;
	state	:= GAME_PLAYING;

	while not quit do
	begin
                vsync;

		gameStatus(FALSE);

		if state = GAME_PLAYING then
		begin
                	cell := boardGet(game.board, c, r);
                        if cell = NULL then
                        	labelSetText(notif, 'READY')
                        else	labelSetText(notif, 'CELL ['+gamePlayerSymbol(COLOR_TYPE(cell))+'~07]');

			boardJump(game.board, c, r, TRUE);
			key := getkey;
			boardJump(game.board, c, r, FALSE);

			case key of
                        KEY_QUESTION:	gameAbout;
                        KEY_H:		gameHelp;
			KEY_LEFT,
			KEY_A:	if c > 1 then dec(c) else c := 3;
			KEY_RIGHT,
			KEY_D:	if c < 3 then inc(c) else c := 1;
			KEY_UP,
			KEY_W:	if r > 1 then dec(r) else r := 3;
			KEY_DOWN,
			KEY_S:	if r < 3 then inc(r) else r := 1;
                        KEY_ENTER,
			KEY_SPACE:
				if cell = NULL then	{ dont overwrite previous moves }
				begin
                                	gameMoveHuman(c, r);
                                        gameStatus(FALSE);
					boardDraw(game.board);
					state := gameState;
					if (state = GAME_PLAYING) then
					begin
						if (GAME_CONFIG.mode = PVA) then
						begin
							labelSetText(notif, '~03THINKING');
							gameMoveComputer;
							vsync_sleep(20);
							boardDraw(game.board);
							state := gameState;
						end
						else
					end;
				end
				else
				begin
					labelSetText(notif, '~04INVALID MOVE');
					vsync_sleep(20);
				end;
			KEY_ESC:
				quit := gameAbort;
			end;
		end
		else
		begin
			case state of
			GAME_CROSS:
				begin
					labelSetText(notif, '  PLAYER '+gamePlayerSymbol(COMPUTER)+' ~07WINS!  ');
					counterIncrease(game.cWinX);
					wait([#13,#27,#32]);
				end;
			GAME_NOUGHT:
				begin
					labelSetText(notif, '  PLAYER '+gamePlayerSymbol(HUMAN)+' ~07WINS!  ');
					counterIncrease(game.cWinO);
					wait([#13,#27,#32]);
				end;
			GAME_DRAW:	begin
					labelSetText(notif, '  IT''S A ~0ETIE~07!  ');
					counterIncrease(game.cDraw);
					wait([#13,#27,#32]);
				end;
			end;

			if not quit then
			begin
				gameReset;
				r := 2;
				c := 2;
				state := GAME_PLAYING;
			end;
		end;
	end;
	labelHide(notif);
	clear($0F);
end;

procedure	gameMenu;
const   ITEM_MODE	: array[GAME_MODE] of string[14] =
				('VS HUMAN      ',
				 'VS AI');
	ITEM_LEVEL	: array[AI_LEVEL] of string[8] =
				('(EASY)  ',
				 '(MEDIUM)',
				 '(HARD)  ');
	ITEM_PLAY	= '   PLAY ';

var	gm, op,
	m	: PMENU;
        newc	: integer;
	choice	: word;
	changed	: boolean;
begin
	op := menuCreate;
	menuAppend(op, ' ['+IS_HEART[GAME_CONFIG.isHeart]+'] USE HEART ICON ', CMD_OPT_ICON, FALSE, nil);
	menuAppend(op, '     CROSS : ['+COLOR_NAMES[GAME_CONFIG.clrX]+'] ', CMD_OPT_COLOR_X,  FALSE, nil);
	menuAppend(op, '     NOUGHT: ['+COLOR_NAMES[GAME_CONFIG.clrO]+'] ', CMD_OPT_COLOR_O,  FALSE, nil);

	gm := menuCreate;
	menuAppend(gm, '   VS HUMAN       ', CMD_MODE_HUMAN,  FALSE, nil);
	menuSeparator(gm);
	menuAppend(gm, '   VS AI (EASY)   ', CMD_MODE_EASY,   FALSE, nil);
	menuAppend(gm, '   VS AI (MEDIUM) ', CMD_MODE_MEDIUM, FALSE, nil);
	menuAppend(gm, '   VS AI (HARD)   ', CMD_MODE_HARD,   FALSE, nil);

	m := menuCreate;
        if GAME_CONFIG.mode = PVP then
		menuAppend(m, ITEM_PLAY+ITEM_MODE[GAME_CONFIG.mode], CMD_GAME_START, FALSE, nil)
        else	menuAppend(m, ITEM_PLAY+ITEM_MODE[GAME_CONFIG.mode]+' '+
			      ITEM_LEVEL[GAME_CONFIG.level], CMD_GAME_START, FALSE, nil);
	menuAppend(m, '   GAME MODE   ', CMD_GAME_MODE, TRUE, gm);
        menuAppend(m, '   OPTIONS     ', CMD_GAME_OPT , TRUE, op);
	menuSeparator(m);
	menuAppend(m, '   ABOUT       ', CMD_GAME_INFO, FALSE, nil);
	menuAppend(m, '   HELP        ', CMD_GAME_HELP, FALSE, nil);
	menuSeparator(m);
	menuAppend(m, '   QUIT GAME   ', CMD_GAME_EXIT, FALSE, nil);

	changed := FALSE;

	repeat
		gameBackground;
		gameShowVersion;

		if changed then
		begin
			if GAME_CONFIG.mode = PVP then
				menuModify(m, CMD_GAME_START, ITEM_PLAY + ITEM_MODE[GAME_CONFIG.mode], FALSE, nil)
			else	menuModify(m, CMD_GAME_START, ITEM_PLAY+ITEM_MODE[GAME_CONFIG.mode]+' '+
					      ITEM_LEVEL[GAME_CONFIG.level], FALSE, nil);
			changed := FALSE;
		end;

		choice := menuPopup(m, SHADE_WHITE, -1, 15,
				    MENU_TEXT_CLR, MENU_ACCENT_CLR, '');
		case choice of
		CMD_GAME_START:	gamePlay;
		CMD_MODE_HUMAN:
			begin
				GAME_CONFIG.mode := PVP;
				changed := TRUE;
			end;
		CMD_MODE_EASY:
			begin
				GAME_CONFIG.mode := PVA;
				GAME_CONFIG.level := EASY;
                                aiSetLevel(GAME_CONFIG.level);
				changed := TRUE;
			end;
		CMD_MODE_MEDIUM:
			begin
				GAME_CONFIG.mode := PVA;
				GAME_CONFIG.level := MEDIUM;
                                aiSetLevel(GAME_CONFIG.level);
                                changed := TRUE;
			end;
		CMD_MODE_HARD:
			begin
				GAME_CONFIG.mode := PVA;
				GAME_CONFIG.level := HARD;
                                aiSetLevel(GAME_CONFIG.level);
                                changed := TRUE;
			end;
                CMD_OPT_ICON:
			begin
                        	GAME_CONFIG.isHeart := not GAME_CONFIG.isHeart;
				menuModify(op, CMD_OPT_ICON, ' ['+IS_HEART[GAME_CONFIG.isHeart]+'] USE HEART ICON ', FALSE, nil);
			end;
                CMD_OPT_COLOR_X:
                        repeat
                                newc := colorPicker(CPF_ALL, -1, 17, GAME_CONFIG.clrX,
						    '['+gamePlayerSymbol(CROSS)+'~07] COLOR');
                                if newc = GAME_CONFIG.clrO then
                                	info(SHADE_RED, 'CROSS COLOR MUST DIFFERENT TO NOUGHT COLOR')
                                else
				begin
                                	GAME_CONFIG.clrX := newc;
					menuModify(op, CMD_OPT_COLOR_X, '     CROSS : ['+COLOR_NAMES[newc]+'] ',  FALSE, nil);
                                end;
                        until newc <> GAME_CONFIG.clrO;
                CMD_OPT_COLOR_O:
                	repeat
                                newc := colorPicker(CPF_ALL, -1, 17, GAME_CONFIG.clrO,
						    '['+gamePlayerSymbol(NOUGHT)+'~07] COLOR');
                                if newc = GAME_CONFIG.clrX then
                                	info(SHADE_RED, 'NOUGHT COLOR MUST DIFFERENT TO CROSS COLOR')
                                else
                                begin
                                	GAME_CONFIG.clrO := newc;
					menuModify(op, CMD_OPT_COLOR_O, '     NOUGHT: ['+COLOR_NAMES[newc]+'] ',  FALSE, nil);
                                end;
                        until newc <> GAME_CONFIG.clrX;
		CMD_GAME_INFO:	gameAbout;
		CMD_GAME_HELP:	gameHelp;
		end;
	until choice = CMD_GAME_EXIT;
        menuDestroy(op);
	menuDestroy(gm);
	menuDestroy(m);
end;

{$endif}