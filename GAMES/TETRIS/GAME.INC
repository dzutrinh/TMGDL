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

const   FRAME_TIME	= 16;		{ timer tick per frame              }
	COLLAPSE_TIME	= 30;		{ timer tick per collapse animation }

var	board	: BOARD_TYPE;		{ global board game                 }
	pieces	: PIECE_TYPE;		{ represent current tetromino       }
	marker	: MARKER_TYPE;		{ for counting filled rows          }
	vscreen	: pchar;		{ double-buffering                  }

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

	GAME_VERSION	:= major shl 8 + minor;
	GAME_BUILD	:= build;
	GAME_COPYRIGHTS	:= 'Tetris (c) 2020-2021 by Infinity Group'#13#10+
			   'Coded by Trinh D.D. Nguyen';

	vscreen := vscreen_alloc;
	if vscreen = nil then die(-1, 'Not enough memory for double-buffering');

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
	scoreInit;

	counterReset(GAME_LEVEL);
	counterReset(GAME_SCORE);
	counterReset(GAME_PIECES);
	counterReset(GAME_LINES);
end;

procedure	gameDone;
begin
	vscreen_free(vscreen);

	counterDestroy(GAME_SCORE);
	counterDestroy(GAME_LEVEL);
	counterDestroy(GAME_PIECES);
	counterDestroy(GAME_LINES);

        config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));

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

procedure	gamePanels(scr, pcs: longint; mv: MOVE_TYPE; redraw: boolean);
var	i	: integer;
begin
	if redraw then
	begin
		for i := 1 to 4 do print(6, i, GAME_LOGO[i]);

		shaded_box(PNL_CONTROL_X, PNL_CONTROL_Y, 24, 12, SHADE_WHITE, AF_LEFT, '~08[ ~0FCONTROL ~08]');
		if GAME_CONFIG.control = KS_WSAD then
		begin
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+1, '~08.     ~0CQ: ~04SPIN CCW');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+2, '~08.     ~0CW: ~04SPIN CW');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+3, '~07.     ~0EA: ~06LEFT');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+4, '~0F.     ~0ES: ~06DOWN');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+5, '~0F.     ~0ED: ~06RIGHT');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+6, '~07. ~0ASPACE: ~02DROP');
		end
		else
		begin
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+1, '~08.     ~0C/: ~04SPIN CW');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+2, '~08.     ~0C'#24': ~04SPIN CW');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+3, '~07.     ~0E'#27': ~06LEFT');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+4, '~0F.     ~0E'#25': ~06DOWN');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+5, '~0F.     ~0E'#26': ~06RIGHT');
			print(PNL_CONTROL_X+2, PNL_CONTROL_Y+6, '~07. ~0AENTER: ~02DROP');
		end;
		print(PNL_CONTROL_X+2, PNL_CONTROL_Y+7 , '~07.     ~0BB: ~03COLOR-BLIND');
		print(PNL_CONTROL_X+2, PNL_CONTROL_Y+8 , '~07.     ~0BH: ~03HINT');
		print(PNL_CONTROL_X+2, PNL_CONTROL_Y+9 , '~08.     ~09P: ~01PAUSE');
		print(PNL_CONTROL_X+2, PNL_CONTROL_Y+10, '~08.   ~0DESC: ~05QUIT');

		counterShow(GAME_SCORE);
		counterShow(GAME_LEVEL);
		counterShow(GAME_PIECES);
		counterShow(GAME_LINES);

		shaded_box(PNL_HOF_X, PNL_HOF_Y, 24, 7, SHADE_WHITE, AF_LEFT, '~08[ ~0FLEADER BOARD ~08]');
		scoreShow(PNL_HOF_X + 2, PNL_HOF_Y);
	end;

	if GAME_CONFIG.next then
	begin
		shaded_box(PNL_NEXT_X, PNL_NEXT_Y, 24, 6, SHADE_YELLOW, AF_LEFT, '~08[ ~0FNEXT ~08]');
		pieceDraw(pieces, mv, PNL_NEXT_X+8, PNL_NEXT_Y+1);
	end;

	counterSetValue(GAME_PIECES, pcs);
	counterSetValue(GAME_SCORE,  scr);
end;

function	gameLoop: boolean;
var	mvCurrent,
	mvTry, mvNext	: MOVE_TYPE;
	cx, cy,
	playSpeed,
	lineCount,
	speedCount,
	pieceCount	: longint;
	gameScore	: longint;
	dropPiece,
	gameOver,
	gameAbort,
	goDown		: boolean;
	key		: word;

	procedure	gameRender;
	begin
		sleep(FRAME_TIME);
		vscreen_flip(vscreen);
	end;

	procedure	gameDraw;
	begin
		if GAME_CONFIG.hint then
			boardPieceShadow(board, pieces, mvCurrent, cx, cy);

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

	procedure	gameHandleEvents;
	begin
		if keypressed then
		begin
			key := getkey;
			if key = GAME_KEYS[MAPPER_DROP] then dropPiece := TRUE;
			if key = GAME_KEYS[MAPPER_QUIT] then
				if query(SHADE_RED, '~0CABORT CURRENT GAME (~0FY~0C/~0FN~0C)?') then
					gameAbort := TRUE;
			if key = GAME_KEYS[MAPPER_PAUSE] then
				info(SHADE_YELLOW, '~0EGAME PAUSED. ~0FENTER ~0ETO CONTINUE...');
			if key = GAME_KEYS[MAPPER_SPIN_CW] then
			begin
				mvTry := mvCurrent;
				inc(mvTry.rotate);
				if pieceFit(pieces, board, mvTry, cx, cy) then
					inc(mvCurrent.rotate);
			end;
			if key = GAME_KEYS[MAPPER_SPIN_CCW] then
			begin
				mvTry := mvCurrent;
				inc(mvTry.rotate);
				if pieceFit(pieces, board, mvTry, cx, cy) then
					dec(mvCurrent.rotate);
			end;
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
			{ secret key to take a screen shot }
			if key = KEY_F12 then screen_dump(FILE_SNAPSHOT);
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
				if markerSize(marker) <> 0 then
				begin
					counterAccum(GAME_LINES, markerSize(marker));
					inc(gameScore, (1 shl markerSize(marker))*10*counterGetValue(GAME_LEVEL));
					inc(lineCount);

					if counterGetValue(GAME_LEVEL) < MAX_LEVELS then
						if lineCount > LEVEL_LINES[counterGetValue(GAME_LEVEL)] then
						begin
							dec(lineCount, LEVEL_LINES[counterGetValue(GAME_LEVEL)]);
							counterIncrease(GAME_LEVEL);
						end;
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
	vscreen_activate(vscreen);
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

	gamePanels(gameScore, pieceCount, mvNext, TRUE);
	vscreen_flip(vscreen);

	while not gameOver and not gameAbort do
	begin
		gamePlay;
		gameDraw;
		gameHandleEvents;
		gameRender;

		inc(speedCount);
		goDown := (speedCount = playSpeed);
	end;

	boardFill(board, -1);
	boardClose(board);

	if gameOver and not gameAbort then
		info(SHADE_MAGENTA, '~0D GAME OVER! ');

	scoreSet(gameScore);
	scoreUpdate;

	vscreen_activate(vscreen_vram);

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
			scoreView;
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
