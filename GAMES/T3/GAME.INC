{$ifndef __GAME_INC__}
{$define __GAME_INC__}

{ // GAME HANDLERS ///////////////////////////////////////////////////////// }

procedure   gameInit(major, minor: longint; build: string);			forward;
procedure	gameShutdown;                                                   forward;
procedure	gameMenu;                                                       forward;
procedure	gamePlay;                                                       forward;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	gameStatus(text: string; color: byte);                          forward;
procedure	gameShowVersion;                                                forward;
procedure	gameBackground;                                                 forward;
procedure	gameSwitchPlayer;                                               forward;
function    gameWin(p: COLOR_TYPE): boolean;                           	forward;
function	gameState: GAME_STATE;                                          forward;
procedure	gameMoveHuman(c, r: integer);                          		forward;
procedure	gameMoveComputer;                                               forward;
procedure	gameAbout;                                                      forward;
procedure	gameHelp;                                                       forward;

{ ////////////////////////////////////////////////////////////////////////// }

var	game		: GAME_TYPE;
	curC, curR	: longint;	{ the cursor, curC 0 when hidden   }
	status		: string[40];	{ the line under the board          }
	statusColor	: byte;

procedure       gameInit(major, minor: longint; build: string);
begin
        if not file_exist(FILE_HELP) then
		die(-1, 'Missing game file. Please reinstall.');

	ltx_init;

        if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
                GAME_CONFIG := APP_DEFAULT_CFG;
	SND_ENABLED := GAME_CONFIG.sound;
	if GAME_CONFIG.mode > PVA then GAME_CONFIG.mode := APP_DEFAULT_CFG.mode;
	if GAME_CONFIG.level > HARD then GAME_CONFIG.level := APP_DEFAULT_CFG.level;
	if GAME_CONFIG.clrX > 15 then GAME_CONFIG.clrX := APP_DEFAULT_CFG.clrX;
	if GAME_CONFIG.clrO > 15 then GAME_CONFIG.clrO := APP_DEFAULT_CFG.clrO;
	if GAME_CONFIG.clrX = GAME_CONFIG.clrO then
		GAME_CONFIG.clrO := APP_DEFAULT_CFG.clrO;
	aiSetLevel(GAME_CONFIG.level);

	GAME_VERSION	:= major shl 8 + minor;
	GAME_BUILD	:= build;
	GAME_COPYRIGHTS	:= 'TIC-TAC-TOE (c) 2019 by Infinity Group'#13#10+
			   'Coded by Trinh D.D. Nguyen';

	cursor(CURSOR_OFF);

	game.winX := 0;
	game.winO := 0;
	game.ties := 0;
	boardInit(game.board, ltx_centerx(boardWidth)+1, ltx_centery(boardHeight)+2);
end;

procedure	gameShutdown;
begin
        config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));

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
	a := $07;
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
	if hb <> nil then
	begin
		helpboxExec(hb);
		helpboxDestroy(hb);
	end
	else
		info(SHADE_RED, 'CANNOT OPEN HELP FILE.');
end;

procedure	gameAbout;
begin
	aboutBox(-1, 15, '~0CTIC~07-~0DTAC~07-~09TOE ~07FOR DOS'+ENDL+
			 'CODED BY ~05TRINH D.D. NGUYEN');
end;

{ a key and what it does, in the left panel }
procedure	gameKey(y: longint; key, text: string);
begin
	lores_text(1, y, key, 15);
	lores_text(1 + length(key) * 4, y, ' ' + text, 7);
end;

{ the whole screen: panels, board, cursor and the line of text }
procedure	gameScreen;
begin
	lores_clear(0);
	lores_text_big(1, 2,  'TIC', 12, 2, 2);
	lores_text_big(1, 14, 'TAC', 13, 2, 2);
	lores_text_big(1, 26, 'TOE', 9,  2, 2);
	gameKey(44, 'SPC', 'PLAY');
	gameKey(52, 'H', 'HELP');
	gameKey(60, '?', 'INFO');
	gameKey(68, 'ESC', 'QUIT');
	if GAME_CONFIG.mode = PVP then
		lores_text(1, 82, 'VS HUMAN', 15)
	else
	begin
		lores_text(1, 82, 'VS CPU', 15);
		lores_text(1, 90, LEVEL_NAMES[GAME_CONFIG.level], 11);
	end;

	lores_text(126, 2, 'TURN', 14);
	markMini(126, 10, game.player);
	markMini(126, 26, CROSS);
	lores_text(135, 26, 'WINS', 7);
	lores_text(126, 34, ltoa(game.winX), 15);
	markMini(126, 46, NOUGHT);
	lores_text(135, 46, 'WINS', 7);
	lores_text(126, 54, ltoa(game.winO), 15);
	lores_text(126, 66, 'TIES', 7);
	lores_text(126, 74, ltoa(game.ties), 15);

	boardDraw(game.board, curC, curR);
	lores_text((LORES_W - lores_text_width(status)) div 2, STATUS_Y, status, statusColor);
	lores_flip;
end;

procedure	gameStatus(text: string; color: byte);
begin
	status      := text;
	statusColor := color;
	gameScreen;
end;

{ the mark just put in square (c, r) is drawn stroke by stroke }
procedure	gameAnimate(c, r: longint);
var	i	: longint;
begin
	gameScreen;
	lores_bar(cellX(c), cellY(r), CELL, CELL, 0);
	if (c = curC) and (r = curR) then lores_bar(cellX(c), cellY(r), CELL, CELL, COLOR_HILITE);
	for i := 1 to PIECE_STEPS do
	begin
		pieceDraw(c, r, boardGet(game.board, c, r), i);
		lores_flip;
		sleep(PIECE_TIME);
	end;
end;

procedure	gameSwitchPlayer;
begin
	game.player := boardOpponent(game.player);
end;

{ three of p in a row? the row goes from (c1, r1) to (c2, r2) }
function	gameRow(p: COLOR_TYPE; var c1, r1, c2, r2: longint): boolean;
var	i	: integer;

	function	same(ca, ra, cb, rb, cc, rc: longint): boolean;
	begin
		same := (boardGet(game.board, ca, ra) = p) and (boardGet(game.board, cb, rb) = p) and
			(boardGet(game.board, cc, rc) = p);
		c1 := ca; r1 := ra;
		c2 := cc; r2 := rc;
	end;

begin
	gameRow := TRUE;
	for i := 1 to 3 do
	begin
		if same(1, i, 2, i, 3, i) then exit;
		if same(i, 1, i, 2, i, 3) then exit;
	end;
	if same(1, 1, 2, 2, 3, 3) then exit;
	if same(3, 1, 2, 2, 1, 3) then exit;
	gameRow := FALSE;
end;

function        gameWin(p: COLOR_TYPE): boolean;
var	c1, r1,
	c2, r2	: longint;
begin
	gameWin := gameRow(p, c1, r1, c2, r2);
end;

function	gameState: GAME_STATE;
begin
	if gameWin(COMPUTER) then	gameState := GAME_CROSS else
	if gameWin(HUMAN) then		gameState := GAME_NOUGHT else
	if boardFull(game.board) then
		gameState := GAME_DRAW
	else	gameState := GAME_PLAYING;
end;

procedure	gameMoveHuman(c, r: integer);
begin
	if GAME_CONFIG.mode = PVA then
		boardPut(game.board, c, r, HUMAN)
	else	boardPut(game.board, c, r, game.player);
	gameAnimate(c, r);
	gameSwitchPlayer;
end;

procedure	gameMoveComputer;
var	c, r	: INDEX_TYPE;
begin
	if boardAvail(game.board) then
	begin
		aiBestMove(game.board, c, r);
		boardPut(game.board, c, r, COMPUTER);
		gameAnimate(c, r);
		gameSwitchPlayer;
	end;
end;

{ the end of a game: a line through the row, and a message }
function	gameOver(state: GAME_STATE): boolean;
var	c1, r1,
	c2, r2,
	i	: longint;
	p	: COLOR_TYPE;
	who	: string[20];
begin
	curC := 0;
	if state = GAME_DRAW then
	begin
		inc(game.ties);
		snd_play(SFX_FALL);
		gameStatus('A TIE! ENTER: NEXT GAME', 14);
	end
	else
	begin
		if state = GAME_CROSS then
		begin
			p := CROSS;
			inc(game.winX);
		end
		else
		begin
			p := NOUGHT;
			inc(game.winO);
		end;
		if (GAME_CONFIG.mode = PVA) and (p = COMPUTER) then
			snd_play(SFX_DIE)
		else	snd_play(SFX_WIN);
		if GAME_CONFIG.mode = PVP then
			who := PLAYER_SYMBOL[p] + ' WINS!'
		else if p = HUMAN then
			who := 'YOU WIN!'
		else	who := 'THE CPU WINS!';
		gameStatus(who + ' ENTER: NEXT GAME', 14);
		gameRow(p, c1, r1, c2, r2);
		for i := 1 to PIECE_STEPS do
		begin
			winLine(c1, r1, c2, r2, i);
			lores_flip;
			sleep(PIECE_TIME);
		end;
	end;
	gameOver := wait([#13, #27, #32]) = #27;
end;

procedure	gamePlay;
var	quit	: boolean;
	key	: word;
	c, r	: INDEX_TYPE;
	state	: GAME_STATE;
	cell	: TILE_TYPE;
begin
	if not lores_init then
	begin
		info(SHADE_RED, 'THE GAME NEEDS AN EGA OR VGA CARD.');
		exit;
	end;
	boardClear(game.board);
	game.player := HUMAN;
	quit	:= FALSE;
	c	:= 2;
	r	:= 2;
	status	:= '';

	while not quit do
	begin
		state := gameState;
		if state <> GAME_PLAYING then
		begin
			quit := gameOver(state);
			boardClear(game.board);
			game.player := HUMAN;
			c := 2;
			r := 2;
			continue;
		end;

		curC := c;
		curR := r;
		cell := boardGet(game.board, c, r);
		if GAME_CONFIG.mode = PVA then
			gameStatus('YOUR TURN', 7)
		else	gameStatus(PLAYER_SYMBOL[game.player] + ' TO PLAY', 7);

		key := getkey_single;
		case key of
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
				snd_play(SFX_CLICK);
				gameMoveHuman(c, r);
				if (gameState = GAME_PLAYING) and (GAME_CONFIG.mode = PVA) then
				begin
					gameStatus('THINKING...', 13);
					sleep(320);
					gameMoveComputer;
					snd_play(SFX_BLIP);
				end;
			end
			else
			begin
				gameStatus('THAT SQUARE IS TAKEN', 12);
				snd_play(SFX_ERROR);
				sleep(400);
			end;
		KEY_H,
		KEY_QUESTION:
			begin
				lores_suspend;
				gameBackground;
				if key = KEY_H then gameHelp else gameAbout;
				lores_resume;
			end;
		KEY_ESC:
			quit := lores_ask('QUIT THIS GAME? Y/N', 'YN'#27) = 'Y';
		end;
	end;
	lores_done;
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
	menuSeparator(op);
	menuAppend(op, ' '+C_ONOFF[GAME_CONFIG.sound]+' SOUND EFFECTS  ', CMD_OPT_SOUND, FALSE, nil);

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

		choice := menuPopup(m, SHADE_WHITE, -1, 14,
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
                CMD_OPT_SOUND:
			begin
				GAME_CONFIG.sound := not GAME_CONFIG.sound;
				SND_ENABLED := GAME_CONFIG.sound;
				menuModify(op, CMD_OPT_SOUND, ' '+C_ONOFF[GAME_CONFIG.sound]+' SOUND EFFECTS  ', FALSE, nil);
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