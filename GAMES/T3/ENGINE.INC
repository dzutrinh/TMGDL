{$ifndef __CONNECT3_ENGINE__}
{$define __CONNECT3_ENGINE__}

{ // GAME BOARD //////////////////////////////////////////////////////////// }

function	boardWidth: longint;                                                            forward;
function	boardHeight: longint;                                                           forward;
function	boardGet(const board: GAME_BOARD; c, r: INDEX_TYPE): TILE_TYPE;                 forward;
procedure	boardSet(var board: GAME_BOARD; c, r: INDEX_TYPE; v: COLOR_TYPE);               forward;
function	boardAvail(const board: GAME_BOARD): boolean;                                   forward;
procedure	boardDrawPiece(const board: GAME_BOARD; c, r: INDEX_TYPE);                      forward;
function	boardFull(const board: GAME_BOARD): boolean;                                    forward;
procedure	boardDraw(const board: GAME_BOARD);                                             forward;
procedure	boardJump(var board: GAME_BOARD; c, r: INDEX_TYPE; active: boolean);            forward;
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

procedure	boardDrawPiece(const board: GAME_BOARD; c, r: INDEX_TYPE);
var	x, y	: integer;
begin
        x := board.x + 2 + (c-1) * CELL_WIDTH;
        y := board.y + 1 + (r-1) * (CELL_HEIGHT+1);
        case board.board[r, c] of
        CROSS:  block_blit(BLOCK_CROSS , x, y, GAME_CONFIG.clrX, FALSE);
        NOUGHT: block_blit(BLOCK_NOUGHT, x, y, GAME_CONFIG.clrO, FALSE);
        end;
end;

procedure	boardDraw(const board: GAME_BOARD);
var	c, r	: INDEX_TYPE;
	i	: longint;
begin
	for i := 1 to high(BOARD_FRAME) do
		print(board.x, board.y + i-1, BOARD_FRAME[i]);
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do boardDrawPiece(board, c, r);
end;

procedure	boardJump(var board: GAME_BOARD; c, r: INDEX_TYPE; active: boolean);
var	attr,
	x, y	: integer;
begin
	boardDrawPiece(board, c, r);

        x := board.x + 1 + (c-1) * CELL_WIDTH;
        y := board.y + 0 + (r-1) * (CELL_HEIGHT+1);

        if active then attr := COLOR_HILITE else attr := CLEAR_COLOR;
        attr := attr shl 4;
		prints(x, y, attr, #223#223#223#223#223#223#223);
        case board.board[r, c] of
        CROSS:	attr := attr + GAME_CONFIG.clrX;
        NOUGHT:	attr := attr + GAME_CONFIG.clrO;
        end;
        ltx_filla(x, y+1, CELL_WIDTH-1, CELL_HEIGHT-1, attr);
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
