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

{ a key and what it does, in the left panel }
procedure	gameKey(y: longint; key, text: string);
begin
	lores_text(1, y, key, 15);
	lores_text(1 + length(key) * 4, y, ' ' + text, 7);
end;

{ the play screen in the 160 x 100 mode, with the board as loBoard draws it }
procedure	gameScreen;
const	LOGO_C	: array[1..4] of byte = (12, 14, 10, 11);
var	i	: longint;
begin
	lores_text_big(1, 3, '2', LOGO_C[1], 2, 2);
	for i := 2 to 4 do lores_text_big(1 + (i-1)*8, 3, copy('2048', i, 1), LOGO_C[i], 2, 2);
	gameKey(20, 'WASD', '');
	lores_text(1, 27, 'SLIDE', 7);
	gameKey(38, 'R', 'NEW');
	gameKey(46, 'H', 'BEST');
	gameKey(54, 'M', 'SOUND');
	gameKey(62, '?', 'HELP');
	gameKey(70, '!', 'INFO');
	gameKey(78, 'ESC', 'QUIT');
	if GAME_CONFIG.sound then
		lores_text(1, 90, 'SND ON', 10)
	else	lores_text(1, 90, 'SND OFF', 8);

	lores_text(128, 5, 'SCORE', 13);
	lores_text(128, 13, ltoa(GAME_SCORE), 15);
	lores_text(128, 27, 'BEST', 12);
	lores_text(128, 35, ltoa(GAME_HISCORE), 15);
	lores_text(128, 49, 'MOVES', 14);
	lores_text(128, 57, ltoa(GAME_MOVES), 15);
	lores_text(128, 71, 'GOAL', 8);
	lores_text(128, 79, ltoa(longint(1) shl GAME_WINCOND), 7);
end;

{ the whole screen, board as it is }
procedure	gameShow;
var	big	: BIG_TYPE;
begin
	fillchar(big, sizeof(big), FALSE);
	lores_clear(0);
	gameScreen;
	loBoard(board, big, -1, -1, 0);
	lores_flip;
end;

{ after a slide: the merged tiles pop, then the new tile grows }
procedure	gameSettle(const anim: ANIM_TYPE);
var	big	: BIG_TYPE;
	before	: BOARD_TYPE;
	k, n,
	x, y,
	nx, ny	: longint;
begin
	fillchar(big, sizeof(big), FALSE);
	for k := 0 to anim.count-1 do
	begin
		n := 0;
		for x := 0 to anim.count-1 do
			if (anim.tiles[x].tx = anim.tiles[k].tx) and (anim.tiles[x].ty = anim.tiles[k].ty) then inc(n);
		if n > 1 then big[anim.tiles[k].tx, anim.tiles[k].ty] := TRUE;
	end;
	lores_clear(0);
	gameScreen;
	loBoard(board, big, -1, -1, 0);
	lores_flip;
	sleep(50);

	before := board;
	boardPlace(board);
	nx := -1;
	ny := -1;
	for y := 0 to BOARD_DIM-1 do
	for x := 0 to BOARD_DIM-1 do
		if board[x, y] <> before[x, y] then
		begin
			nx := x;
			ny := y;
		end;
	fillchar(big, sizeof(big), FALSE);
	for k := 1 to 3 do
	begin
		loBoard(board, big, nx, ny, k * 6);
		lores_flip;
		sleep(FRAME_TIME);
	end;
	gameShow;
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
	lores_suspend;
	ANSI_draw(BACKGD_IMG);
	hiscoreEnter(GAME_SCORES, GAME_SCORE, 'BEST TILE ' + ltoa(longint(1) shl best), 'NEW HIGH SCORE', SHADE_RED);
	scoreSave;
	lores_resume;
end;

procedure       gamePlay;
var	c       : char;
	quit,
	next	: boolean;
	anim	: ANIM_TYPE;
	score,
	k	: longint;
begin
	if not lores_init then
	begin
		info(SHADE_RED, 'THE GAME NEEDS AN EGA OR VGA CARD.');
		exit;
	end;
	quit := FALSE; next := FALSE;
	anim.count := 0;
	if not stateLoad(board) then stateRestart(board);
	repeat
		gameShow;
		{ one key at a time: repeats piled up during a slide are dropped }
		repeat
			k := getkey_single;
			case k of
			KEY_UP:		c := 'W';
			KEY_DOWN:	c := 'S';
			KEY_LEFT:	c := 'A';
			KEY_RIGHT:	c := 'D';
			else		c := upcase(chr(k and $FF));
			end;
		until (k < 256) and (c in ['W','A','S','D','R','H','M',#27,'?','!']) or
		      (k = KEY_UP) or (k = KEY_DOWN) or (k = KEY_LEFT) or (k = KEY_RIGHT);
		next := FALSE;
		score := GAME_SCORE;
		case c of
		#27:	quit := lores_ask('QUIT AND SAVE THE GAME? Y/N', 'YN'#27) = 'Y';
		'R':	if lores_ask('START A NEW GAME? Y/N', 'YN'#27) = 'Y' then
			begin
				gameRecord;
				stateRestart(board);
				scoreSave;
			end;
		'A':	next := boardMove(board, DIR_LEFT , anim);
		'D':	next := boardMove(board, DIR_RIGHT, anim);
		'W':	next := boardMove(board, DIR_UP   , anim);
		'S':	next := boardMove(board, DIR_DOWN , anim);
		'?',
		'!',
		'H':	begin
				lores_suspend;
				ANSI_draw(BACKGD_IMG);
				case c of
				'?':	gameHelp;
				'!':	gameAbout;
				'H':	hiscoreView(GAME_SCORES, 'HIGH SCORES', SHADE_RED, 0);
				end;
				lores_resume;
			end;
		'M':	begin
				GAME_CONFIG.sound := not GAME_CONFIG.sound;
				SND_ENABLED := GAME_CONFIG.sound;
				snd_play(SFX_BLIP);
			end;
		end;
		if next then
		begin
			if GAME_SCORE > score then snd_play(SFX_BLIP) else snd_play(SFX_CLICK);
			boardAnimate(anim);
			inc(GAME_MOVES);
			gameSettle(anim);
			if stateCheckWin(board) then
			begin
				snd_play(SFX_WIN);
				lores_ask('YOU MADE ' + ltoa(longint(1) shl GAME_WINCOND) + '! YOU WIN!', #13#27' ');
				gameRecord;
				stateRestart(board);
			end
			else
			if stateCheckOver(board) then
			begin
				snd_play(SFX_DIE);
				lores_ask('GAME OVER', #13#27' ');
				gameRecord;
				stateRestart(board);
			end;
		end;
	until quit;
	lores_done;
	scoreSave;
	stateSave(board);
end;

{$endif}
