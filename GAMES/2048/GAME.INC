{$ifndef	__GAME_2048__}
{$define	__GAME_2048__}

{ ////////////////////////////////////////////////////////////////////////// }

{ exposed API }
procedure       gameInit(major, minor: longint; build: string);		forward;
procedure	gameDone;						forward;
function        gameDemo: boolean;					forward;
procedure       gamePlay;						forward;

{ private API }
procedure	gameLogo;						forward;
procedure       gameUpdate;						forward;
procedure	gameHelp;						forward;
procedure	gameAbout;                                              forward;

{ ////////////////////////////////////////////////////////////////////////// }

var	board	: BOARD_TYPE;
	cScore,
	cHiScore,
	cMoves	: PCOUNTER;

procedure       gameInit(major, minor: longint; build: string);
begin
        if not file_exist(FILE_HELP) then
        	die(-1, 'Missing game file. Please reinstall.');

	GAME_VERSION := major shl 8 + minor;
	GAME_BUILD   := build;

	ltx_init;
	ltx_timer_init;
	randomize;

	cursor(CURSOR_OFF);
	blink(BLINK_OFF);

	scoreLoad;
	stateInit(board);

	cScore	:= counterCreate(GAME_SCORE  , INFO_X, INFO_SCORE,   16, 0, 0, SHADE_MAGENTA, '~0FSCORE');
	cHiScore:= counterCreate(GAME_HISCORE, INFO_X, INFO_HISCORE, 16, 0, 0, SHADE_RED,     '~0FHI-SCORE');
	cMoves	:= counterCreate(GAME_MOVES  , INFO_X, INFO_MOVES,   16, 0, 0, SHADE_YELLOW,  '~0FMOVES');
end;

procedure	gameDone;
begin
	counterDestroy(cScore);
	counterDestroy(cHiScore);
	counterDestroy(cMoves);

	ltx_timer_done;
	ltx_done;
	writeln(' ______ ______ _____  ______ ');
	writeln('|__    |      |  |  ||  __  |');
	writeln('|    __|  --  |__    |  __  |');
	writeln('|______|______|  |__||______|');
	write  ('v');
	write  (vtos(GAME_VERSION, GAME_BUILD));
	writeln(' [', COMPILER, ']');
	writeln('Thanks for playing!');
	writeln;
end;

procedure	gameLogo;
begin
	ANSI_draw(BACKGD_IMG);

	prints(72, 25, $08, 'v' + vtos(GAME_VERSION, GAME_BUILD));

	shaded_box(HELP_X, HELP_Y, 16, 10, SHADE_WHITE, AF_LEFT, '~0FCONTROL');
	print(HELP_X+2, HELP_Y+1, '~08.   ~0AW ~02UP   ');
	print(HELP_X+2, HELP_Y+2, '~07.   ~0AS ~02DOWN ');
	print(HELP_X+2, HELP_Y+3, '~0F.   ~0AA ~02LEFT ');
	print(HELP_X+2, HELP_Y+4, '~0F.   ~0AD ~02RIGHT');
	print(HELP_X+2, HELP_Y+5, '~07.   ~0ER ~06RESET');
	print(HELP_X+2, HELP_Y+6, '~07.   ~0E? ~06HELP');
	print(HELP_X+2, HELP_Y+7, '~07.   ~0E! ~06ABOUT');
	print(HELP_X+2, HELP_Y+8, '~07. ~0CESC ~04QUIT ');

	counterShow(cScore);
	counterShow(cHiScore);
	counterShow(cMoves);
end;

procedure       gameUpdate;
begin
	counterSetValue(cScore, GAME_SCORE);
	counterSetValue(cHiScore, GAME_HISCORE);
	counterSetValue(cMoves, GAME_MOVES);
	boardShow(board);
end;

procedure	gameHelp;
var	hb	: PHELPBOX;
begin
	hb := helpboxCreate(FILE_HELP, 'HELP', -1, 9);
        helpboxExec(hb);
	helpboxDestroy(hb);
end;

procedure	gameAbout;
begin
	aboutBox(-1, 12, '~0C2~0E0~0A4~0B8 ~07FOR DOS'+ENDL+
			 'CODED BY ~05TRINH D.D. NGUYEN');
end;

function        gameDemo: boolean;
var     x, y, c : longint;
	tmp	: BOARD_TYPE;
	stop	: boolean;
	ch	: word;
begin
	gameLogo;
	gameUpdate;
	fillchar(tmp, sizeof(BOARD_TYPE), 0);
	c := 0;
	stop := FALSE;
	center(24, '~09ENTER~01:START  ~08-  ~09ESC~01:QUIT');
	repeat
		x := c mod BOARD_DIM; y := c div BOARD_DIM;
		tmp[x, y] := (c mod 11)+1;
		boardShow(tmp);
		tmp[x, y] := 0;
		c := (c + 1) mod BOARD_SIZE;
		if keypressed then
		begin
			ch := getkey;
			case ch of
			KEY_ENTER,
			KEY_SPACE,
			KEY_ESC:	stop := TRUE;
                        KEY_H:		gameHelp;
                        KEY_EXCLAIM:	gameAbout;
                        end;
		end;
		sleep(200);
	until stop;
        flushkeys;
	gameDemo := (ch = KEY_ENTER) or (ch = KEY_SPACE);
end;

procedure       gamePlay;
var	c       : char;
	quit,
	next	: boolean;
begin
	quit := FALSE; next := FALSE;
	gameLogo;
	if stateLoad(board) then
	begin
		gameUpdate;
	end
	else	stateRestart(board);
	repeat
		gameUpdate;
		c := wait(['W','A','S','D','R',#27,'?','!']);
		gameUpdate;
		case c of
		#27:	begin
				if query(SHADE_RED, '~0CQUIT AND SAVE GAME (~0FY~0C/~0FN~0C) ?') then
					quit := TRUE;
				next := FALSE;
			end;
		'R':	if query(SHADE_CYAN, '~03START A NEW GAME (~0FY~03/~0FN~03) ?') then
			begin
				stateRestart(board);
				scoreSave;
				next := FALSE;
			end;
		'A':	next := boardSlideLeft (board);
		'D':	next := boardSlideRight(board);
		'W':	next := boardSlideUp   (board);
		'S':	next := boardSlideDown (board);
                '?':    begin
                		gameHelp;
                                next := FALSE;
                	end;
                '!':    gameAbout;
		end;
		if next then
		begin
			inc(GAME_MOVES);
			boardPlace(board);
			boardShow(board);
			if stateCheckWin(board) then
			begin
				info(SHADE_GREEN, '~02!! ~0AYOU WIN ~02!!');
				stateRestart(board);
				next := FALSE;
			end
			else
			if stateCheckOver(board) then
			begin
				info(SHADE_YELLOW, '~06!! ~0EGAME OVER~06 !!');
				stateRestart(board);
				next := FALSE;
			end;
		end;
	until quit;
	scoreSave;
	stateSave(board);
end;

{$endif}
