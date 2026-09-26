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

	if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
		GAME_CONFIG := APP_DEFAULT_CFG;
	SND_ENABLED := GAME_CONFIG.sound;
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
	config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));

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

{ the M line of the key box shows whether the sound is on }
procedure	gameSoundKey;
const	STATE	: array[boolean] of string[3] = ('OFF', 'ON ');
begin
	print(HELP_X+2, HELP_Y+9, '~07.   ~0EM ~06SOUND ' + STATE[GAME_CONFIG.sound]);
end;

procedure	gameLogo;
begin
	ANSI_draw(BACKGD_IMG);

	prints(72, 25, $08, 'v' + vtos(GAME_VERSION, GAME_BUILD));

	shaded_box(HELP_X, HELP_Y, 18, 12, SHADE_WHITE, AF_LEFT, '~0FCONTROL');
	print(HELP_X+2, HELP_Y+1, '~08.   ~0AW ~02UP   ');
	print(HELP_X+2, HELP_Y+2, '~07.   ~0AS ~02DOWN ');
	print(HELP_X+2, HELP_Y+3, '~0F.   ~0AA ~02LEFT ');
	print(HELP_X+2, HELP_Y+4, '~0F.   ~0AD ~02RIGHT');
	print(HELP_X+2, HELP_Y+5, '~07.   ~0ER ~06RESET');
	print(HELP_X+2, HELP_Y+6, '~07.   ~0E? ~06HELP');
	print(HELP_X+2, HELP_Y+7, '~07.   ~0E! ~06ABOUT');
	print(HELP_X+2, HELP_Y+8, '~07.   ~0EH ~06SCORES');
	gameSoundKey;
	print(HELP_X+2, HELP_Y+10, '~07. ~0CESC ~04QUIT ');

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
	ch := 0;
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

{ a score good enough goes in the table of the best scores }
procedure	gameRecord;
var	i, j, best	: longint;
begin
	if hiscoreRank(GAME_SCORES, GAME_SCORE) = 0 then exit;
	best := 0;
	for j := 0 to BOARD_DIM - 1 do
		for i := 0 to BOARD_DIM - 1 do
			if board[j][i] > best then best := board[j][i];
	hiscoreEnter(GAME_SCORES, GAME_SCORE, 'BEST TILE ' + ltoa(longint(1) shl best), 'NEW HIGH SCORE', SHADE_RED);
	scoreSave;
	gameLogo;
end;

procedure       gamePlay;
var	c       : char;
	quit,
	next	: boolean;
	anim	: ANIM_TYPE;
	score	: longint;
begin
	quit := FALSE; next := FALSE;
	anim.count := 0;
	gameLogo;
	if stateLoad(board) then
	begin
		gameUpdate;
	end
	else	stateRestart(board);
	repeat
		gameUpdate;
		c := wait(['W','A','S','D','R','H','M',#27,'?','!']);
		gameUpdate;
		next := FALSE;
		score := GAME_SCORE;
		case c of
		#27:	begin
				if query(SHADE_RED, '~0CQUIT AND SAVE GAME (~0FY~0C/~0FN~0C) ?') then
					quit := TRUE;
				next := FALSE;
			end;
		'R':	if query(SHADE_CYAN, '~03START A NEW GAME (~0FY~03/~0FN~03) ?') then
			begin
				gameRecord;
				stateRestart(board);
				scoreSave;
				next := FALSE;
			end;
		'A':	next := boardMove(board, DIR_LEFT , anim);
		'D':	next := boardMove(board, DIR_RIGHT, anim);
		'W':	next := boardMove(board, DIR_UP   , anim);
		'S':	next := boardMove(board, DIR_DOWN , anim);
                '?':    begin
                		gameHelp;
                                next := FALSE;
                	end;
                '!':    gameAbout;
		'H':	hiscoreView(GAME_SCORES, 'HIGH SCORES', SHADE_RED, 0);
		'M':	begin
				GAME_CONFIG.sound := not GAME_CONFIG.sound;
				SND_ENABLED := GAME_CONFIG.sound;
				gameSoundKey;
				snd_play(SFX_BLIP);
			end;
		end;
		if next then
		begin
			if GAME_SCORE > score then snd_play(SFX_BLIP) else snd_play(SFX_CLICK);
			boardAnimate(anim);
			inc(GAME_MOVES);
			boardPlace(board);
			boardShow(board);
			if stateCheckWin(board) then
			begin
				snd_play(SFX_WIN);
				info(SHADE_GREEN, '~02!! ~0AYOU WIN ~02!!');
				gameRecord;
				stateRestart(board);
				next := FALSE;
			end
			else
			if stateCheckOver(board) then
			begin
				snd_play(SFX_DIE);
				info(SHADE_YELLOW, '~06!! ~0EGAME OVER~06 !!');
				gameRecord;
				stateRestart(board);
				next := FALSE;
			end;
		end;
	until quit;
	scoreSave;
	stateSave(board);
end;

{$endif}
