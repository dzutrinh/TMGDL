{$ifndef	__ENGINE_2048__}
{$define	__ENGINE_2048__}

{ ////////////////////////////////////////////////////////////////////////// }

{ exposed game engine API }
procedure	cellDraw(i, j: longint; c: byte; const mid: string);	forward;
procedure	tileDraw(i, j, v: longint);					forward;
procedure	emptyDraw(x, y: longint);					forward;
procedure       boardShow(const board: BOARD_TYPE); 				forward;
function        boardFind(const v: VEC4_TYPE; x, stop: longint): longint;	forward;
function        boardSlide(var v, dest: VEC4_TYPE): boolean;			forward;
procedure       boardRotate(var board: BOARD_TYPE);				forward;
procedure	boardLineCell(dir: DIR_TYPE; l, p: longint; var x, y: longint);	forward;
function	boardMove(var board: BOARD_TYPE; dir: DIR_TYPE;
			  var anim: ANIM_TYPE): boolean;			forward;
procedure	boardAnimate(const anim: ANIM_TYPE);				forward;
function        boardCheckPair(board: BOARD_TYPE): boolean;			forward;
function        boardCountBlank(board: BOARD_TYPE): longint;			forward;
function        boardPlace(var board: BOARD_TYPE): boolean;			forward;

procedure	stateInit(var board: BOARD_TYPE);				forward;
procedure	stateRestart(var board: BOARD_TYPE);				forward;
function        stateSave(var board: BOARD_TYPE): boolean;			forward;
function        stateLoad(var board: BOARD_TYPE): boolean;			forward;
function        stateCheckOver(board: BOARD_TYPE): boolean;			forward;
function	stateCheckWin(board: BOARD_TYPE): boolean;			forward;

function        scoreLoad: boolean;						forward;
function	scoreSave: boolean;						forward;

{ ////////////////////////////////////////////////////////////////////////// }

{ Draws a 3-row board cell at screen position (i, j) }
procedure	cellDraw(i, j: longint; c: byte; const mid: string);
begin
	prints(i, j  , c, BOARD_FILLER);
	prints(i, j+1, c, mid);
	prints(i, j+2, c, BOARD_FILLER);
end;

{ Draws a numbered tile (v = log2 of its value) at screen position (i, j) }
procedure	tileDraw(i, j, v: longint);
begin
	cellDraw(i, j, CELL_COLORS[v+1], make_center(ltoa(1 shl v), BOARD_W));
end;

{ Draws the empty board cell (x, y) in its checkerboard color }
procedure	emptyDraw(x, y: longint);
begin
	cellDraw(BOARD_X+x*BOARD_W, BOARD_Y+y*BOARD_H,
		 CELL_COLORS[(x+y) and 1], BOARD_FILLER);
end;

{ Displays the game board }
procedure       boardShow(const board: BOARD_TYPE);
var	x, y	: longint;
begin
	for y := 0 to BOARD_DIM-1 do
	for x := 0 to BOARD_DIM-1 do
		if board[x, y] <> 0 then
			tileDraw(BOARD_X+x*BOARD_W, BOARD_Y+y*BOARD_H, board[x, y])
		else	emptyDraw(x, y);
end;

{ Finds the proper cell to slide to }
function        boardFind(const v: VEC4_TYPE; x, stop: longint): longint;
var	t: longint;
begin
	if x = 0 then
	begin
		boardFind := x;
		exit;
	end;
        t := x-1;
        while t >= 0 do
        begin
                if v[t] <> 0 then
                begin
			if v[t] <> v[x] then
			begin
				boardFind := t+1;
				exit;
			end;
			boardFind := t;
			exit;
		end
                else
		if t = stop then
		begin
			boardFind := t;
			exit;
		end;
                dec(t);
        end;
end;

{ Slides a line towards index 0; dest[x] receives where the tile at x went }
function        boardSlide(var v, dest: VEC4_TYPE): boolean;
var     success : boolean;
	x, t,
	stop    : longint;
begin
	stop := 0;
	success := FALSE;
	for x := 0 to BOARD_DIM-1 do
        begin
        	dest[x] := x;
                if v[x] <> 0 then
                begin
			t := boardFind(v, x, stop);
                        if (t <> x) then
                        begin
                                if v[t] = 0 then
                                        v[t] := v[x]
                                else
                                if v[t] = v[x] then
                                begin
                                        inc(v[t]);
					inc(GAME_SCORE, 1 shl v[t]);
					if GAME_HISCORE < GAME_SCORE then
						GAME_HISCORE := GAME_SCORE;
                                        stop := t+1;
                                end;
                                v[x] := 0;
                                dest[x] := t;
                                success := TRUE;
                        end;
                end;
        end;
	boardSlide := success;
end;

{ Rotates the board CW }
procedure       boardRotate(var board: BOARD_TYPE);
var     i, j,
	t, n : longint;
begin
	{ inspace square CW matrix rotation, from geeksforgeeks.com }
	n := BOARD_DIM;
	for i := 0 to (n shr 1)-1 do
	for j := i to (n-i-1)-1 do
	begin
				  t := board[j    , i    ];
		board[j    , i    ] := board[i    , n-1-j];
		board[i    , n-1-j] := board[n-1-j, n-1-i];
		board[n-1-j, n-1-i] := board[n-1-i, j    ];
		board[n-1-i, j    ] := t;
	end;
end;

{ Board cell of position p (0 = the edge tiles slide to) in line l }
procedure	boardLineCell(dir: DIR_TYPE; l, p: longint; var x, y: longint);
begin
	case dir of
	DIR_UP:		begin x := l;             y := p;             end;
	DIR_DOWN:	begin x := l;             y := BOARD_DIM-1-p; end;
	DIR_LEFT:	begin x := p;             y := l;             end;
	DIR_RIGHT:	begin x := BOARD_DIM-1-p; y := l;             end;
	end;
end;

{ Slides the board in a direction, TRUE if anything moved; anim receives }
{ the path of every tile for boardAnimate                                }
function	boardMove(var board: BOARD_TYPE; dir: DIR_TYPE;
			  var anim: ANIM_TYPE): boolean;
var	l, p,
	x, y	: longint;
	v, org,
	dest	: VEC4_TYPE;
	ok	: boolean;
begin
	ok := FALSE;
	anim.count := 0;
	for l := 0 to BOARD_DIM-1 do
	begin
		for p := 0 to BOARD_DIM-1 do
		begin
			boardLineCell(dir, l, p, x, y);
			v[p] := board[x, y];
		end;
		org := v;
		if boardSlide(v, dest) then ok := TRUE;
		for p := 0 to BOARD_DIM-1 do
		begin
			boardLineCell(dir, l, p, x, y);
			board[x, y] := v[p];
			if org[p] <> 0 then
			with anim.tiles[anim.count] do
			begin
				value := org[p];
				fx := x;
				fy := y;
				boardLineCell(dir, l, dest[p], tx, ty);
				inc(anim.count);
			end;
		end;
	end;
	boardMove := ok;
end;

{ Plays a slide: every tile glides from its old cell to its new one. Frames }
{ are composed off-screen and copied on the vertical retrace (no flicker)   }
procedure	boardAnimate(const anim: ANIM_TYPE);
var	scr,
	old	: SCREEN;
	f, k,
	x, y,
	i, j	: longint;
	moving	: boolean;
begin
	scr := vscreen_alloc;
	if scr = nil then exit;			{ no memory: skip the animation }
	vscreen_grab(scr);
	old := vscreen_activate(scr);
	for f := 1 to ANIM_FRAMES do
	begin
		for y := 0 to BOARD_DIM-1 do
		for x := 0 to BOARD_DIM-1 do
			emptyDraw(x, y);
		{ resting tiles first, so moving ones slide over them }
		for moving := FALSE to TRUE do
		for k := 0 to anim.count-1 do
		with anim.tiles[k] do
		if ((fx <> tx) or (fy <> ty)) = moving then
		begin
			i := BOARD_X + (fx*BOARD_W + (tx-fx)*BOARD_W*f div ANIM_FRAMES);
			j := BOARD_Y + (fy*BOARD_H + (ty-fy)*BOARD_H*f div ANIM_FRAMES);
			tileDraw(i, j, value);
		end;
		if ltx_mono then sleep(14) else vsync;
		vscreen_flip(scr);
	end;
	vscreen_activate(old);
	vscreen_free(scr);
end;

{ Checks for mergeable pair }
function        boardCheckPair(board: BOARD_TYPE): boolean;
var     x, y    : longint;
begin
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-2 do
		if board[x, y] = board[x, y+1] then
		begin
			boardCheckPair := TRUE;
			exit;
		end;
	boardCheckPair := FALSE;
end;

{ Counts the number of empty cells }
function        boardCountBlank(board: BOARD_TYPE): longint;
var     x, y, c	: longint;
begin
	c := 0;
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-1 do
		if board[x, y] = 0 then inc(c);
	boardCountBlank := c;
end;

{ Place a random cell into the board }
function        boardPlace(var board: BOARD_TYPE): boolean;
var     x, y,
	r, n,
	len	: longint;
	list    : array[0..BOARD_SIZE-1] of record
			x, y: longint;
		  end;
begin
        len := 0;
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-1 do
		if board[x, y] = 0 then
		begin
			list[len].x := x;
			list[len].y := y;
                        inc(len);
                end;
	if (len > 0) then
	begin
                r := random(len);
		n := (random(10) div 9)+1;
		board[list[r].x, list[r].y] := n;
		boardPlace := TRUE;
        end
	else    boardPlace := FALSE;
end;

{ Check for game over state }
function        stateCheckOver(board: BOARD_TYPE): boolean;
var     ended   : boolean;
begin
	ended := TRUE;
	if boardCountBlank(board) > 0 then
	begin
		stateCheckOver := FALSE;
		exit;
	end;
	if boardCheckPair(board) then
	begin
		stateCheckOver := FALSE;
		exit;
	end;

	boardRotate(board);
	if boardCheckPair(board) then
		ended := FALSE;
	boardRotate(board);
	boardRotate(board);
	boardRotate(board);
	stateCheckOver := ended;
end;

function	stateCheckWin(board: BOARD_TYPE): boolean;
var     x, y	: longint;
begin
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-1 do
		if board[x, y] = GAME_WINCOND then
		begin
			stateCheckWin := TRUE;
			exit;
		end;
	stateCheckWin := FALSE;
end;

{ Puts the game into initial state }
procedure	stateInit(var board: BOARD_TYPE);
begin
	fillchar(board, sizeof(BOARD_TYPE), 0);
	GAME_MOVES := 0;
	GAME_SCORE := 0;
end;

{ Restarts the game }
procedure	stateRestart(var board: BOARD_TYPE);
begin
	stateInit(board);
	boardPlace(board);
	boardPlace(board);
	boardShow(board);
end;

{ Saves current game state to the save game file, returns TRUE if succeed }
function        stateSave(var board: BOARD_TYPE): boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SAVE);
	{$i-}
	rewrite(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockwrite(f, GAME_SCORE, sizeof(longint));
		blockwrite(f, GAME_MOVES, sizeof(longint));
		blockwrite(f, board, sizeof(BOARD_TYPE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	stateSave := ret;
end;

{ Loads the game state from the save game file, returns TRUE if succeed }
function        stateLoad(var board: BOARD_TYPE): boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SAVE);
	{$i-}
	reset(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockread(f, GAME_SCORE, sizeof(longint));
		blockread(f, GAME_MOVES, sizeof(longint));
		blockread(f, board, sizeof(BOARD_TYPE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	stateLoad := ret;
end;

{ Loads the hi-score value from the score file, returns TRUE if succeed }
function        scoreLoad: boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SCORE);
	{$i-}
	reset(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockread(f, GAME_HISCORE, sizeof(GAME_HISCORE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	scoreLoad := ret;
end;

{ Saves the hi-score value into the score file, returns TRUE if succeed }
function	scoreSave: boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SCORE);
	{$i-}
	rewrite(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockwrite(f, GAME_HISCORE, sizeof(GAME_HISCORE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	scoreSave := ret;
end;

{$endif}
