{$ifndef	__TETRIS_GAME__}
{$define	__TETRIS_GAME__}

{ ////////////////////////////////////////////////////////////////////////// }

{ exposed API }
procedure       gameInit(major, minor: longint; build: string);				forward;
procedure	gameDone;								forward;
procedure	gameMenu;								forward;

{ private API }
procedure	gamePlay;								forward;
procedure	gameReset;								forward;
procedure	gameShowVersion;							forward;
procedure	gamePanels(scr, pcs: longint; mv: MOVE_TYPE; redraw: boolean);   	forward;
procedure	gameBackground;					                        forward;
procedure	gameAbout;                                                              forward;
procedure	gameHelp;                                                               forward;

{ ////////////////////////////////////////////////////////////////////////// }

const   FRAME_TIME	= 16;		{ milliseconds per frame            }
	COLLAPSE_TIME	= 30;		{ milliseconds per collapse frame   }

var	board	: BOARD_TYPE;		{ global board game                 }
	pieces	: PIECE_TYPE;		{ represent current tetromino       }
	marker	: MARKER_TYPE;		{ for counting filled rows          }

procedure	gameSelectColorScheme;
begin
        if GAME_CONFIG.cblind then
		GAME_CONFIG.scheme := CBLIND_SCHEME
        else	GAME_CONFIG.scheme := COLOR_SCHEME;
end;

procedure       gameInit(major, minor: longint; build: string);
begin
        if not file_exist(FILE_HELP) then
        	die(-1, 'Missing game file. Please reinstall.');

	ltx_init;
	if ltx_mono then
		die(-1, 'EGA/VGA adapter is required.');
	ltx_timer_init;

        if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
                GAME_CONFIG := APP_DEFAULT_CFG;
	if not hiscoreLoad(FILE_SCORES, GAME_SCORES) then
	begin
		hiscoreInit(GAME_SCORES, 5, HS_POINTS);
		hiscoreFill(GAME_SCORES, SCORE_NAMES, 5000, 1000);
	end;
		if (GAME_CONFIG.speed < 1) or (GAME_CONFIG.speed > MAX_LEVELS) then
			GAME_CONFIG.speed := APP_DEFAULT_CFG.speed;
		if GAME_CONFIG.height > BOARD_HEIGHT - 2 then
			GAME_CONFIG.height := APP_DEFAULT_CFG.height;
	SND_ENABLED := GAME_CONFIG.sound;

	GAME_VERSION	:= major shl 8 + minor;
	GAME_BUILD	:= build;
	GAME_COPYRIGHTS	:= 'Tetris (c) 2020-2021 by Infinity Group'#13#10+
			   'Coded by Trinh D.D. Nguyen';

	randomize;
	cursor(CURSOR_OFF);
	blink(BLINK_OFF);
	keyboard_speed(0);

	gameShowVersion;
        gameSelectColorScheme;

	GAME_SCORE  := counterCreate(0, PNL_SCORE_X,  PNL_SCORE_Y, 24, 0, 0, SHADE_RED,     '~08[ ~0FSCORE ~08]' );
	GAME_LEVEL  := counterCreate(GAME_CONFIG.speed, PNL_LEVEL_X, PNL_LEVEL_Y, 24, 0, 0, SHADE_BLUE,    '~08[ ~0FLEVEL ~08]' );
	GAME_PIECES := counterCreate(0, PNL_PIECES_X, PNL_PIECES_Y, 24, 0, 0, SHADE_MAGENTA, '~08[ ~0FPIECES ~08]');
	GAME_LINES  := counterCreate(0, PNL_LINES_X,  PNL_LINES_Y, 24, 0, 0, SHADE_GREEN,   '~08[ ~0FLINES ~08]' );
end;

procedure	gameReset;
begin
	pieceInit(pieces);
	boardInit(board, GAME_CONFIG.height);
	markerInit(marker);

	counterReset(GAME_LEVEL);
	counterReset(GAME_SCORE);
	counterReset(GAME_PIECES);
	counterReset(GAME_LINES);
end;

procedure	gameDone;
begin
	counterDestroy(GAME_SCORE);
	counterDestroy(GAME_LEVEL);
	counterDestroy(GAME_PIECES);
	counterDestroy(GAME_LINES);

        config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));
	hiscoreSave(FILE_SCORES, GAME_SCORES);

	ltx_timer_done;
	ltx_done;
	blink(BLINK_ON);

	writeln(' _______ _______ _______ ______ _______ _______ ');
	writeln('|_     _|    ___|_     _|   __ \_     _|     __|');
	writeln('  |   | |    ___| |   | |      <_|   |_|__     |');
	writeln('  |___| |_______| |___| |___|__|_______|_______|');
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

procedure	gameAbout;
begin
	aboutBox(-1, 14, '~03T~0BE~09T~01R~0DI~05S ~07FOR DOS'+ENDL+
			 'CODED BY ~03TRINH D.D. NGUYEN');
end;

procedure	gameHelp;
var	help	: PHELPBOX;
begin
	help := helpboxCreate(FILE_HELP, '~08[ ~0FHELP ~08]', -1, 9);
	if help <> nil then
	begin
		helpboxExec(help);
		helpboxDestroy(help);
	end
	else	info(SHADE_RED, 'CANNOT OPEN HELP FILE.');
end;

{ the panels at both sides of the board }
procedure	gamePanels(scr, pcs: longint; mv: MOVE_TYPE; redraw: boolean);
const	LOGO		: string[6] = 'TETRIS';
	LOGO_COLORS	: array[1..6] of byte = (12, 14, 10, 11, 9, 13);
var	i, x	: integer;
	name	: string;
begin
	{ left: the name, the counters, the keys }
	for i := 1 to 6 do
	begin
		lores_bar(4 + (i - 1) * 8, 2, 7, 9, LOGO_COLORS[i]);
		lores_text(6 + (i - 1) * 8, 4, LOGO[i], 0);
	end;
	lores_text(2, 17, 'SCORE', 12);
	lores_text(2, 23, lpad(ltoa(scr), 8), 15);
	lores_text(2, 31, 'LEVEL', 9);
	lores_text(2, 37, lpad(ltoa(counterGetValue(GAME_LEVEL)), 8), 15);
	lores_text(2, 45, 'LINES', 10);
	lores_text(2, 51, lpad(ltoa(counterGetValue(GAME_LINES)), 8), 15);
	lores_text(2, 59, 'PIECES', 13);
	lores_text(2, 65, lpad(ltoa(pcs), 8), 15);
	lores_text(2, 76, 'H HINT', 8);
	lores_text(2, 82, 'B COLORS', 8);
	lores_text(2, 88, 'P PAUSE', 8);
	lores_text(2, 94, 'ESC QUIT', 8);

	{ right: the next piece and the leader board }
	x := BOARD_X + BOARD_WIDTH * CELL + 6;
	if GAME_CONFIG.next then
	begin
		lores_text(x, 2, 'NEXT', 14);
		lores_rect(x, 8, 22, 22, 8);
		pieceDraw(pieces, mv, x + 3, 11);
	end;
	lores_text(x, 36, 'BEST', 11);
	for i := 1 to GAME_SCORES.size do
	begin
		name := ltoa(GAME_SCORES.entries[i].score);
		lores_text(x, 36 + i * 7, copy(GAME_SCORES.entries[i].name, 1, 5), 7);
		lores_text(LORES_W - 1 - lores_text_width(name), 36 + i * 7, name, 15);
	end;
end;

function	gameLoop: boolean;
var	mvCurrent,
	mvTry, mvNext	: MOVE_TYPE;
	cx, cy,
	playSpeed,
	lineCount,
	speedCount,
	pieceCount,
	level		: longint;
	gameScore	: longint;
	dropPiece,
	gameOver,
	gameAbort,
	goDown		: boolean;
	key		: word;

	procedure	gameRender;
	begin
		sleep(FRAME_TIME);
		lores_flip;
	end;

	procedure	gameDraw;
	begin
		if GAME_CONFIG.hint then
			boardPieceShadow(board, pieces, mvCurrent, cx, cy);

		lores_clear(0);
		gamePanels(gameScore, pieceCount, mvNext, FALSE);

		boardDraw(board);
		pieceShow(pieces, mvCurrent, cx, cy);
		if markerSize(marker) <> 0 then
		begin
			boardCondense(board, marker);
			gameRender;
			sleep(COLLAPSE_TIME);	{ wait a little bit }
		end;
	end;

	function	gameTryRotate(clockwise: boolean): boolean;
	const
		KICKS: array[1..5] of integer = (0, -1, 1, -2, 2);
	var
		i, delta: integer;
		candidate: MOVE_TYPE;
	begin
		gameTryRotate := FALSE;
		candidate := mvCurrent;
		if clockwise then inc(candidate.rotate)
		else dec(candidate.rotate);
		for i := 1 to 5 do
		begin
			delta := KICKS[i];
			if pieceFit(pieces, board, candidate, cx + delta, cy) then
			begin
				mvCurrent := candidate;
				cx := cx + delta;
				gameTryRotate := TRUE;
				exit;
			end;
		end;
	end;

	procedure	gameHandleEvents;
	var	events: integer;
	begin
		events := 0;
		while keypressed and (events < 4) do
		begin
			inc(events);
			key := getkey;
			if key = GAME_KEYS[MAPPER_DROP] then
			begin
				dropPiece := TRUE;
				goDown := TRUE;
			end;
			if key = GAME_KEYS[MAPPER_QUIT] then
				if lores_ask('QUIT? Y/N', 'YN'#27) = 'Y' then gameAbort := TRUE;
			if key = GAME_KEYS[MAPPER_PAUSE] then
				lores_ask('PAUSED', 'P'#13#27' ');
			if key = GAME_KEYS[MAPPER_SPIN_CW] then
				gameTryRotate(TRUE);
			if key = GAME_KEYS[MAPPER_SPIN_CCW] then
				gameTryRotate(FALSE);
			if key = GAME_KEYS[MAPPER_DOWN] then
				if pieceFit(pieces, board, mvCurrent, cx, cy+1) then inc(cy);
			if key = GAME_KEYS[MAPPER_LEFT] then
				if pieceFit(pieces, board, mvCurrent, cx-1, cy) then dec(cx);
			if key = GAME_KEYS[MAPPER_RIGHT] then
				if pieceFit(pieces, board, mvCurrent, cx+1, cy) then inc(cx);
			if key = GAME_KEYS[MAPPER_HINT] then
			begin
				GAME_CONFIG.hint := not GAME_CONFIG.hint;
				if not GAME_CONFIG.hint then
					boardRowFill(board, BOARD_HEIGHT-1, BOARD_BORDER);
			end;
                        if key = KEY_B then
                        begin
                                GAME_CONFIG.cblind := not GAME_CONFIG.cblind;
                                gameSelectColorScheme;
                        end;
		end;
	end;

	procedure	gamePlay;
	begin
		if goDown then
		begin
			boardShadowClear(board);
			speedCount := 0;
			if pieceCount mod 200 = 0 then
				if (playSpeed >= 20) then dec(playSpeed);

			while dropPiece do
			begin
				while pieceFit(pieces, board, mvCurrent, cx, cy + 1) do
					inc(cy);
				dropPiece := FALSE;
			end;
			if pieceFit(pieces, board, mvCurrent, cx, cy + 1) then
				inc(cy)
			else
			begin
				pieceLock(pieces, board, mvCurrent, cx, cy);
				boardMarkFull(board, marker, cy);

				inc(gameScore, 5);
				if markerSize(marker) = 0 then
					snd_play(SFX_CLICK)
				else
				begin
					level := counterGetValue(GAME_LEVEL);
					counterAccum(GAME_LINES, markerSize(marker));
					inc(gameScore, (1 shl markerSize(marker))*10*counterGetValue(GAME_LEVEL));
					inc(lineCount, markerSize(marker));

					if counterGetValue(GAME_LEVEL) < MAX_LEVELS then
						while lineCount >= LEVEL_LINES[counterGetValue(GAME_LEVEL)] do
						begin
							dec(lineCount, LEVEL_LINES[counterGetValue(GAME_LEVEL)]);
							counterIncrease(GAME_LEVEL);
							if counterGetValue(GAME_LEVEL) = MAX_LEVELS then break;
						end;
						playSpeed := LEVEL_SPEED[counterGetValue(GAME_LEVEL)];
					if (counterGetValue(GAME_LEVEL) <> level) or (markerSize(marker) = 4) then
						snd_play(SFX_LEVEL)
					else if markerSize(marker) > 1 then
						snd_play(SFX_COIN)
					else	snd_play(SFX_BLIP);
				end;

				cx := (BOARD_WIDTH - pieceWidth(pieces, mvCurrent)) shr 1;
				cy := 0;
				mvCurrent := mvNext;
				moveGenerate(mvNext);
				inc(pieceCount);

				gameOver := not pieceFit(pieces, board, mvCurrent, cx, cy);
			end;
		end;
	end;

begin
	gameLoop := TRUE;
	if not lores_init then
	begin
		info(SHADE_RED, 'THE GAME NEEDS AN EGA OR VGA CARD.');
		exit;
	end;
	moveGenerate(mvCurrent);
	moveGenerate(mvNext);

	cx		:= (BOARD_WIDTH - pieceWidth(pieces, mvCurrent)) shr 1;
	cy		:= 0;
	playSpeed	:= LEVEL_SPEED[GAME_CONFIG.speed];
	speedCount	:= 0;
	gameScore	:= 0;
	pieceCount	:= 1;
	lineCount	:= 0;
	goDown		:= FALSE;
	gameOver	:= FALSE;
	gameAbort	:= FALSE;
	dropPiece	:= FALSE;


	while not gameOver and not gameAbort do
	begin
		gameHandleEvents;
		gamePlay;
		gameDraw;
		gameRender;

		inc(speedCount);
		goDown := (speedCount = playSpeed);
	end;

	boardFill(board, -1);
	boardClose(board);

	if gameOver and not gameAbort then
	begin
		snd_play(SFX_DIE);
		lores_ask('GAME OVER', #13#27' ');
	end;

	lores_done;
	gameBackground;
	gameShowVersion;
	hiscoreEnter(GAME_SCORES, gameScore, 'LEVEL ' + ltoa(counterGetValue(GAME_LEVEL)),
		     'NEW HIGH SCORE', SHADE_WHITE);

	gameLoop := gameAbort;
end;

procedure	gamePlay;
var	abort	: boolean;
begin
	case GAME_CONFIG.control of
	KS_WSAD:	GAME_KEYS := CONTROLS_WSAD;
	KS_ARROWS:      GAME_KEYS := CONTROLS_ARROWS;
	end;

	repeat
		gameReset;
		abort := gameLoop;
	until abort;

	info(SHADE_GREEN, '~0ABYE BYE!');
	clear($0F);
end;

procedure	gameMenu;
const
	MENU_KEYS	: array[KEY_SELECTOR] of string [32] =
		('     CONTROL: [W/S/A/D]  ',
		 '     CONTROL: [ARROWS]   ');
	MENU_WSAD	: array[boolean] of string[32] =
		(' ( ) W/S/A/D  ',
		 ' ('#7') W/S/A/D  ');
	MENU_ARROWS	: array[boolean] of string[32] =
		(' ( ) ARROWS  ',
		 ' ('#7') ARROWS  ');
var	main,
	ctrls,
	opt	: PMENU;
	sld	: PSLIDER;
	cmd	: word;
        changed,
	quit	: boolean;
	height,
	speed	: integer;
begin
	quit    := FALSE;
	changed := FALSE;
	opt	:= menuCreate;
	main	:= menuCreate;
	ctrls	:= menuCreate;

	menuAppend(ctrls, MENU_ARROWS[GAME_CONFIG.control = KS_ARROWS], CMD_CTRLS_ARROWS, FALSE, nil);
	menuAppend(ctrls, MENU_WSAD[GAME_CONFIG.control = KS_WSAD], CMD_CTRLS_WASD, FALSE, nil);

	menuAppend(opt, MENU_KEYS[GAME_CONFIG.control], CMD_OPT_CONTROL, TRUE, ctrls);
        menuSeparator(opt);
        menuAppend(opt, ' '+C_ONOFF[GAME_CONFIG.cblind]+' COLOR BLIND BRICKS ', CMD_OPT_CBLIND, FALSE, nil);
	menuAppend(opt, ' '+C_ONOFF[GAME_CONFIG.hint]  +' SHOW HINT          ', CMD_OPT_HINT, FALSE, nil);
	menuAppend(opt, ' '+C_ONOFF[GAME_CONFIG.next]  +' SHOW NEXT PIECE    ', CMD_OPT_NEXT, FALSE, nil);
	menuAppend(opt, ' '+C_ONOFF[GAME_CONFIG.sound] +' SOUND EFFECTS      ', CMD_OPT_SOUND, FALSE, nil);
        menuSeparator(opt);
	menuAppend(opt, '     STARTING SPEED:  ['+lpad(itoa(GAME_CONFIG.speed), 2)+']', CMD_OPT_SPEED, FALSE, nil);
	menuAppend(opt, '     STARTING HEIGHT: ['+lpad(itoa(GAME_CONFIG.height), 2)+']', CMD_OPT_HEIGHT, FALSE, nil);

	menuAppend(main, '   START NEW GAME   ', CMD_GAME_NEW, FALSE, nil);
	menuAppend(main, '   HIGH SCORES      ', CMD_GAME_HOF, FALSE, nil);
	menuAppend(main, '   OPTIONS          ', 0, TRUE, opt);
        menuSeparator(main);
	menuAppend(main, '   ABOUT            ', CMD_GAME_INFO, FALSE, nil);
	menuAppend(main, '   HELP             ', CMD_GAME_HELP, FALSE, nil);
        menuSeparator(main);
	menuAppend(main, '   QUIT GAME        ', CMD_GAME_QUIT, FALSE, nil);

	repeat
        	if changed then
                begin
			menuModify(opt  , CMD_OPT_CONTROL,  MENU_KEYS[GAME_CONFIG.control], TRUE, ctrls);
			menuModify(ctrls, CMD_CTRLS_ARROWS, MENU_ARROWS[GAME_CONFIG.control = KS_ARROWS], FALSE, nil);
			menuModify(ctrls, CMD_CTRLS_WASD,   MENU_WSAD[GAME_CONFIG.control = KS_WSAD], FALSE, nil);

			menuModify(opt, CMD_OPT_SPEED,  '     STARTING SPEED:  ['+lpad(itoa(GAME_CONFIG.speed), 2)+']', FALSE, nil);
			menuModify(opt, CMD_OPT_HEIGHT, '     STARTING HEIGHT: ['+lpad(itoa(GAME_CONFIG.height), 2)+']', FALSE, nil);

		        menuModify(opt, CMD_OPT_CBLIND, ' '+C_ONOFF[GAME_CONFIG.cblind]+' COLOR BLIND BRICKS ', FALSE, nil);
			menuModify(opt, CMD_OPT_HINT,   ' '+C_ONOFF[GAME_CONFIG.hint]  +' SHOW HINT          ', FALSE, nil);
			menuModify(opt, CMD_OPT_NEXT,   ' '+C_ONOFF[GAME_CONFIG.next]  +' SHOW NEXT PIECE    ', FALSE, nil);
			menuModify(opt, CMD_OPT_SOUND,  ' '+C_ONOFF[GAME_CONFIG.sound] +' SOUND EFFECTS      ', FALSE, nil);
                	changed := FALSE;
                end;

		gameBackground;
		gameShowVersion;

		cmd := menuPopup(main, SHADE_WHITE, -1, 14,
				 MENU_TEXT_CLR, MENU_ACCENT_CLR, '');
		case cmd of
		CMD_GAME_QUIT:
			quit := TRUE;
		CMD_GAME_NEW:
                	begin
				gamePlay;
                                changed := TRUE;
                        end;
		CMD_GAME_HOF:
			hiscoreView(GAME_SCORES, 'HALL OF FAME', SHADE_WHITE, 0);
		CMD_GAME_HELP:	gameHelp;
		CMD_GAME_INFO:	gameAbout;
		CMD_OPT_HINT:
			begin
				GAME_CONFIG.hint := not GAME_CONFIG.hint;
                                changed := TRUE;
			end;
		CMD_OPT_CBLIND:
			begin
				GAME_CONFIG.cblind := not GAME_CONFIG.cblind;
				gameSelectColorScheme;
                                changed := TRUE;
			end;
		CMD_OPT_SPEED:
			begin
				sld := sliderCreate('SELECT SPEED', TRUE, 1, 10, -1, 18, 30, SHADE_BLUE);
				sliderSetValue(sld, GAME_CONFIG.speed);
				speed := sliderExec(sld, SHADE_WHITE);
				sliderDestroy(sld);
				if speed <> -1 then
				begin
					GAME_CONFIG.speed := speed;
                                        changed := TRUE;
				end;
			end;
		CMD_OPT_HEIGHT:
			begin
				sld := sliderCreate('HEIGHT DIFFICULTY', TRUE, 0, 16, -1, 18, 30, SHADE_BLUE);
				sliderSetValue(sld, GAME_CONFIG.height);
				height := sliderExec(sld, SHADE_WHITE);
				sliderDestroy(sld);
				if height <> -1 then
				begin
					GAME_CONFIG.height := height;
                                	changed := TRUE;
				end;
			end;

		CMD_OPT_SOUND:
			begin
				GAME_CONFIG.sound := not GAME_CONFIG.sound;
				SND_ENABLED := GAME_CONFIG.sound;
				changed := TRUE;
			end;
		CMD_OPT_NEXT:
			begin
				GAME_CONFIG.next := not GAME_CONFIG.next;
                                changed := TRUE;
			end;
		CMD_CTRLS_ARROWS:
			begin
				GAME_CONFIG.control := KS_ARROWS;
                        	changed := TRUE;
			end;
		CMD_CTRLS_WASD:
			begin
				GAME_CONFIG.control := KS_WSAD;
                        	changed := TRUE;
			end;
		end;
	until quit;
	menuDestroy(opt);
	menuDestroy(ctrls);
	menuDestroy(main);
end;

procedure	gameBackground;
begin
	ANSI_draw(BACKGD_IMG);
end;

{$endif}
