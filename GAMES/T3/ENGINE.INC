{$ifndef __CONNECT3_ENGINE__}
{$define __CONNECT3_ENGINE__}

{ // GAME BOARD //////////////////////////////////////////////////////////// }

function	boardWidth: longint;                                                            forward;
function	boardHeight: longint;                                                           forward;
function	boardGet(const board: GAME_BOARD; c, r: INDEX_TYPE): TILE_TYPE;                 forward;
procedure	boardSet(var board: GAME_BOARD; c, r: INDEX_TYPE; v: COLOR_TYPE);               forward;
function	boardAvail(const board: GAME_BOARD): boolean;                                   forward;
function	boardFull(const board: GAME_BOARD): boolean;                                    forward;
procedure	boardDraw(const board: GAME_BOARD; cc, cr: longint);                             forward;
procedure	boardPut(var board: GAME_BOARD; c, r: INDEX_TYPE; piece: COLOR_TYPE);           forward;
procedure	boardClear(var board: GAME_BOARD);                                              forward;
function	boardOpponent(player: COLOR_TYPE): COLOR_TYPE;                                  forward;
procedure	boardInit(var board: GAME_BOARD; x, y: longint);                                forward;

procedure	aiSetLevel(level: AI_LEVEL);							forward;
function	aiGetLevel: AI_LEVEL;                                           		forward;
function	aiEvaluate(const board: GAME_BOARD): integer;                                   forward;
function	aiAlphaBeta(var board: GAME_BOARD; depth: integer; max: boolean;
				alpha, beta: integer): integer;	forward;
procedure	aiBestMove(var board: GAME_BOARD; var bc, br: INDEX_TYPE);				forward;

{ ////////////////////////////////////////////////////////////////////////// }

function	equals(a, b, c: COLOR_TYPE): boolean;
begin
	equals := (a = b) and (b = c);
end;

function	boardWidth: longint;
begin
	boardWidth := BOARD_FRAME_WIDTH;
end;

function	boardHeight: longint;
begin
	boardHeight := BOARD_FRAME_HEIGHT;
end;

function        boardGet(const board: GAME_BOARD; c, r: INDEX_TYPE): TILE_TYPE;
begin
	boardGet := board.board[r, c];
end;

procedure	boardSet(var board: GAME_BOARD; c, r: INDEX_TYPE; v: COLOR_TYPE);
begin
	board.board[r, c] := v;
end;

function        boardAvail(const board: GAME_BOARD): boolean;
var	c, r	: INDEX_TYPE;
begin
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do
                if boardGet(board, c, r) = NULL then
		begin
			boardAvail := TRUE;
			exit;
		end;
	boardAvail := FALSE;
end;

function	boardFull(const board: GAME_BOARD): boolean;
begin
	boardFull := not boardAvail(board);
end;

{ top left pixel of a square }
function	cellX(c: longint): longint;
begin
	cellX := BX + (c-1) * PITCH;
end;

function	cellY(r: longint): longint;
begin
	cellY := BY + (r-1) * PITCH;
end;

{ the first n points of a line, with a brush of 2 x 2 pixels }
procedure	brushLine(x0, y0, x1, y1, n: longint; color: byte);
var	dx, dy,
	sx, sy,
	err, e2	: longint;
begin
	dx := abs(x1 - x0);
	dy := -abs(y1 - y0);
	if x0 < x1 then sx := 1 else sx := -1;
	if y0 < y1 then sy := 1 else sy := -1;
	err := dx + dy;
	while n > 0 do
	begin
		lores_bar(x0, y0, 2, 2, color);
		dec(n);
		if (x0 = x1) and (y0 = y1) then break;
		e2 := 2 * err;
		if e2 >= dy then
		begin
			inc(err, dy);
			inc(x0, sx);
		end;
		if e2 <= dx then
		begin
			inc(err, dx);
			inc(y0, sy);
		end;
	end;
end;

{ the angle of (x, y) from twelve o'clock, clockwise, 0 to 1023: the }
{ 'diamond angle', which needs no trigonometry                        }
function	clockAngle(x, y: longint): longint;
var	u, v	: longint;
begin
	u := -y;		{ turned a quarter: twelve o'clock is 0 }
	v := x;
	if (u = 0) and (v = 0) then
		clockAngle := 0
	else if v >= 0 then
		if u >= 0 then
			clockAngle := 256 * v div (u + v)
		else	clockAngle := 256 + 256 * (-u) div (-u + v)
	else	if u < 0 then
			clockAngle := 512 + 256 * (-v) div (-u - v)
		else	clockAngle := 768 + 256 * u div (u - v);
end;

{ a mark in square (c, r), drawn up to step of PIECE_STEPS: the cross }
{ stroke by stroke, the nought and the heart round the clock          }
procedure	pieceDraw(c, r: longint; piece: TILE_TYPE; step: longint);
var	x, y,
	i, j, n,
	dx, dy	: longint;
	inside	: boolean;
begin
	x := cellX(c);
	y := cellY(r);
	case piece of
	CROSS:	begin
			n := 2 * 17 * step div PIECE_STEPS;
			brushLine(x + 4, y + 4, x + 20, y + 20, n, GAME_CONFIG.clrX);
			if n > 17 then
				brushLine(x + 20, y + 4, x + 4, y + 20, n - 17, GAME_CONFIG.clrX);
		end;
	NOUGHT:	for j := 0 to CELL - 1 do
			for i := 0 to CELL - 1 do
			begin
				dx := 2 * i - (CELL - 1);
				dy := 2 * j - (CELL - 1);
				if GAME_CONFIG.isHeart then
					inside := (j >= 6) and (j < 6 + HEART_H) and (i >= 4) and (i < 4 + HEART_W) and
						  (HEART_IMG[j - 6][i - 3] = 'X')
				else	inside := (dx * dx + dy * dy >= 225) and (dx * dx + dy * dy <= 361);
				if inside and (clockAngle(dx, dy) < 1024 * step div PIECE_STEPS) then
					lores_set(x + i, y + j, GAME_CONFIG.clrO);
			end;
	end;
	{ a shine on the heart }
	if (piece = NOUGHT) and GAME_CONFIG.isHeart and (step = PIECE_STEPS) then
		lores_set(x + 6, y + 8, 15);
end;

{ the grid, the marks and the cursor on (cc, cr) unless cc is 0 }
procedure	boardDraw(const board: GAME_BOARD; cc, cr: longint);
var	c, r	: INDEX_TYPE;
begin
	lores_bar(BX + CELL, BY, LINE, 3 * CELL + 2 * LINE, GRID_C);
	lores_bar(BX + 2 * CELL + LINE, BY, LINE, 3 * CELL + 2 * LINE, GRID_C);
	lores_bar(BX, BY + CELL, 3 * CELL + 2 * LINE, LINE, GRID_C);
	lores_bar(BX, BY + 2 * CELL + LINE, 3 * CELL + 2 * LINE, LINE, GRID_C);
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do
	begin
		if (c = cc) and (r = cr) then
			lores_bar(cellX(c), cellY(r), CELL, CELL, COLOR_HILITE)
		else	lores_bar(cellX(c), cellY(r), CELL, CELL, 0);
		pieceDraw(c, r, board.board[r, c], PIECE_STEPS);
	end;
end;

{ the line through three in a row, from square (c1, r1) to (c2, r2), }
{ drawn up to step of PIECE_STEPS                                    }
procedure	winLine(c1, r1, c2, r2, step: longint);
var	x0, y0,
	x1, y1	: longint;
begin
	x0 := cellX(c1) + 12 - (c2 - c1) * 5;
	y0 := cellY(r1) + 12 - (r2 - r1) * 5;
	x1 := cellX(c2) + 12 + (c2 - c1) * 5;
	y1 := cellY(r2) + 12 + (r2 - r1) * 5;
	brushLine(x0, y0, x1, y1, (ltx_max(abs(x1 - x0), abs(y1 - y0)) + 1) * step div PIECE_STEPS, WIN_C);
end;

{ a small mark, 7 pixels wide, for the panels }
procedure	markMini(x, y: longint; piece: COLOR_TYPE);
var	i, j	: longint;
begin
	if piece = CROSS then
		lores_text_big(x, y, 'X', GAME_CONFIG.clrX, 2, 1)
	else if not GAME_CONFIG.isHeart then
		lores_text_big(x, y, 'O', GAME_CONFIG.clrO, 2, 1)
	else	for j := 0 to 4 do
			for i := 0 to 6 do
				if MINI_HEART[j][i + 1] = 'X' then lores_set(x + i, y + j, GAME_CONFIG.clrO);
end;

procedure	boardPut(var board: GAME_BOARD; c, r: INDEX_TYPE; piece: COLOR_TYPE);
begin
	boardSet(board, c, r, piece);
end;

procedure	boardClear(var board: GAME_BOARD);
var	r, c	: INDEX_TYPE;
begin
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do board.board[r, c] := NULL;
end;

function	boardOpponent(player: COLOR_TYPE): COLOR_TYPE;
begin
	if player = NOUGHT then
		boardOpponent := CROSS
	else	boardOpponent := NOUGHT;
end;

procedure	boardInit(var board: GAME_BOARD; x, y: longint);
begin
	board.x		:= x;
	board.y		:= y;
	board.cc	:= 1;
	board.cr	:= 1;
	boardClear(board);
end;

{ // AI - ALPHA-BETA /////////////////////////////////////////////////////// }

type	GAME_LEVEL	= array[AI_LEVEL] of byte;

	const	AI_DEPTH	: GAME_LEVEL = (0, 1, 9);
	AI_DIFFICULTY	: AI_LEVEL = HARD;
	AI_INFTY 	= 1000;
	AI_SCORE_CROSS	: integer = +100;
	AI_SCORE_NOUGHT	: integer = -100;
	AI_SCORE_TIE	=  0;

procedure	aiSetLevel(level: AI_LEVEL);
begin
        AI_DIFFICULTY := level;
end;

function	aiGetLevel: AI_LEVEL;
begin
	aiGetLevel := AI_DIFFICULTY;
end;

function	aiEvaluate(const board: GAME_BOARD): integer;
var	c, r    : INDEX_TYPE;
begin
	aiEvaluate := AI_SCORE_TIE;
	{$R-}
	{ scan diagonals }
	if equals(board.board[1,1], board.board[2,2], board.board[3,3]) and
	   (board.board[1,1] <> NULL) then
        begin
		if board.board[1,1] = TILE_TYPE(CROSS) then
			aiEvaluate := AI_SCORE_CROSS
		else
		if board.board[1,1] = TILE_TYPE(NOUGHT) then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	if equals(board.board[1,3], board.board[2,2], board.board[3,1]) and
	   (board.board[1,3] <> NULL) then
        begin
		if board.board[1,3] = TILE_TYPE(CROSS) then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[1,3] = TILE_TYPE(NOUGHT) then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	{ scan rows }
	for r := 1 to BOARD_SIZE do
	if equals(board.board[r,1], board.board[r,2], board.board[r,3]) and
	   (board.board[r,1] <> NULL) then
        begin
		if board.board[r,1] = TILE_TYPE(CROSS) then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[r,1] = TILE_TYPE(NOUGHT) then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	{ scan columns }
	for c := 1 to BOARD_SIZE do
	if equals(board.board[1,c], board.board[2,c], board.board[3,c]) and
	   (board.board[1,c] <> NULL) then
        begin
		if board.board[1,c] = TILE_TYPE(CROSS) then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[1,c] = TILE_TYPE(NOUGHT) then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;
	{$R+}
end;

function	aiHeuristic(const board: GAME_BOARD): integer;
var	i		: INDEX_TYPE;
	cross,
	nought,
	value		: integer;
begin
	{ Open winning lines are a useful deterministic frontier evaluation. }
	value := 0;
	for i := 1 to BOARD_SIZE do
	begin
		cross := 0; nought := 0;
		if board.board[i,1] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[i,2] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[i,3] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[i,1] = TILE_TYPE(NOUGHT) then inc(nought);
		if board.board[i,2] = TILE_TYPE(NOUGHT) then inc(nought);
		if board.board[i,3] = TILE_TYPE(NOUGHT) then inc(nought);
		if nought = 0 then inc(value, cross * cross);
		if cross = 0 then dec(value, nought * nought);
	end;
	for i := 1 to BOARD_SIZE do
	begin
		cross := 0; nought := 0;
		if board.board[1,i] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[2,i] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[3,i] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[1,i] = TILE_TYPE(NOUGHT) then inc(nought);
		if board.board[2,i] = TILE_TYPE(NOUGHT) then inc(nought);
		if board.board[3,i] = TILE_TYPE(NOUGHT) then inc(nought);
		if nought = 0 then inc(value, cross * cross);
		if cross = 0 then dec(value, nought * nought);
	end;
	cross := 0; nought := 0;
	for i := 1 to BOARD_SIZE do
	begin
		if board.board[i,i] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[i,i] = TILE_TYPE(NOUGHT) then inc(nought);
	end;
	if nought = 0 then inc(value, cross * cross);
	if cross = 0 then dec(value, nought * nought);
	cross := 0; nought := 0;
	for i := 1 to BOARD_SIZE do
	begin
		if board.board[i,BOARD_SIZE+1-i] = TILE_TYPE(CROSS) then inc(cross);
		if board.board[i,BOARD_SIZE+1-i] = TILE_TYPE(NOUGHT) then inc(nought);
	end;
	if nought = 0 then inc(value, cross * cross);
	if cross = 0 then dec(value, nought * nought);
	aiHeuristic := value;
end;

{ Alpha-beta search with deterministic depth-limited difficulty. }
function	aiAlphaBeta(var board: GAME_BOARD; depth: integer; max: boolean;
				alpha, beta: integer): integer;
var	score, best, nextAlpha, nextBeta: integer;
	c, r: INDEX_TYPE;
begin
	score := aiEvaluate(board);
	if score = AI_SCORE_CROSS then
	begin
		aiAlphaBeta := score - depth;
		exit;
	end;
	if score = AI_SCORE_NOUGHT then
	begin
		aiAlphaBeta := score + depth;
		exit;
	end;
	if boardFull(board) then
	begin
		aiAlphaBeta := AI_SCORE_TIE;
		exit;
	end;
	if depth >= AI_DEPTH[AI_DIFFICULTY] then
	begin
		aiAlphaBeta := aiHeuristic(board);
		exit;
	end;

	if max then
	begin
		best := -AI_INFTY;
		nextAlpha := alpha;
		for r := 1 to BOARD_SIZE do
		for c := 1 to BOARD_SIZE do
			if board.board[r, c] = NULL then
			begin
				board.board[r, c] := COMPUTER;
				score := aiAlphaBeta(board, depth+1, FALSE, nextAlpha, beta);
				board.board[r, c] := NULL;
				best := ltx_max(score, best);
				if best > nextAlpha then nextAlpha := best;
				if beta <= nextAlpha then break;
			end;
		aiAlphaBeta := best;
	end
	else
	begin
		best := AI_INFTY;
		nextBeta := beta;
		for r := 1 to BOARD_SIZE do
		for c := 1 to BOARD_SIZE do
			if board.board[r, c] = NULL then
			begin
				board.board[r, c] := HUMAN;
				score := aiAlphaBeta(board, depth+1, TRUE, alpha, nextBeta);
				board.board[r, c] := NULL;
				best := ltx_min(score, best);
				if best < nextBeta then nextBeta := best;
				if nextBeta <= alpha then break;
			end;
		aiAlphaBeta := best;
	end;
end;

procedure	aiBestMove(var board: GAME_BOARD; var bc, br: INDEX_TYPE);
var     score,
	best	: integer;
	c, r	: INDEX_TYPE;
begin
	bc := 1;
	br := 1;
	best	:= -AI_INFTY;

	{ scan all cells, evaluate minimax function for all empty cells }
	{ return the cell with optimal value                            }
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do
		if board.board[r, c] = NULL then	{ cell is empty }
		begin
			board.board[r, c] := COMPUTER;	{ make the move }
			{ evaluate the move }
			score := aiAlphaBeta(board, 0, FALSE, -AI_INFTY, AI_INFTY);
			board.board[r, c] := NULL;	{ undo the move }
			{ if the evaluated value is higher than }
			{ the best value, then update it        }
			if best < score then
			begin
				bc := c;
				br := r;
				best  := score;
			end;
		end;
end;

{$endif}
