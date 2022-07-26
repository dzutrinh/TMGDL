{$ifndef __CONNECT3_ENGINE__}
{$define __CONNECT3_ENGINE__}

{ // GAME BOARD //////////////////////////////////////////////////////////// }

function	boardWidth: longint;                                                            forward;
function	boardHeight: longint;                                                           forward;
function        boardGet(const board: GAME_BOARD; c, r: INDEX_TYPE): TILE_TYPE;                 forward;
procedure	boardSet(var board: GAME_BOARD; c, r: INDEX_TYPE; v: COLOR_TYPE);               forward;
function        boardAvail(const board: GAME_BOARD): boolean;                                   forward;
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
function	aiMinimax (var board: GAME_BOARD; depth: integer; max: boolean): integer;	forward;
procedure	aiBestMove(var board: GAME_BOARD; var bc, br: INDEX_TYPE);				forward;

{ ////////////////////////////////////////////////////////////////////////// }

function	equals(a, b, c: COLOR_TYPE): boolean;
begin
	equals := (a = b) and (b = c);
end;

function	boardWidth: longint;
begin
	boardWidth := length(BOARD_FRAME[1]);
end;

function	boardHeight: longint;
begin
	boardHeight := high(BOARD_FRAME);
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
        prints(x, y, attr, 'ßßßßßßß');
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

{ // AI - MINIMAX ////////////////////////////////////////////////////////// }

type	GAME_LEVEL	= array[AI_LEVEL] of byte;

const	AI_DEPTH	: GAME_LEVEL = (5, 1, 0);
	AI_DIFFICULTY	: AI_LEVEL = HARD;
	AI_INFTY 	= 1000;
	AI_SCORE_CROSS	: integer = +1;
	AI_SCORE_NOUGHT	: integer = -1;
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
	if equals(board.board[1,1], board.board[2,2], board.board[3,3]) then
        begin
		if board.board[1,1] = CROSS then
			aiEvaluate := AI_SCORE_CROSS
		else
		if board.board[1,1] = NOUGHT then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	if equals(board.board[1,3], board.board[2,2], board.board[3,1]) then
        begin
		if board.board[1,3] = CROSS then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[1,3] = NOUGHT then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	{ scan rows }
	for r := 1 to BOARD_SIZE do
	if equals(board.board[r,1], board.board[r,2], board.board[r,3]) then
        begin
		if board.board[r,1] = CROSS then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[r,1] = NOUGHT then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;

	{ scan columns }
	for c := 1 to BOARD_SIZE do
	if equals(board.board[1,c], board.board[2,c], board.board[3,c]) then
        begin
		if board.board[1,c] = CROSS then
			aiEvaluate := AI_SCORE_CROSS
                else
		if board.board[1,c] = NOUGHT then
			aiEvaluate := AI_SCORE_NOUGHT;
		exit;
	end;
	{$R+}
end;

{ MINIMAX ALGORITHM }
function	aiMinimax(var board: GAME_BOARD; depth: integer; max: boolean): integer;
var	score,
	best	: integer;
	c, r	: INDEX_TYPE;
begin
	score := aiEvaluate(board);

	{ if a side has won the game, return the score }
	if (score <> AI_SCORE_TIE) then
	begin
		aiMinimax := score;
		exit;
	end;

	{ if there are no more moves and no winner then it is a tie }
	if boardFull(board) then
	begin
		aiMinimax := AI_SCORE_TIE;
		exit;
	end;

	if max then			{ AI: maximizer's move }
	begin
		best := -AI_INFTY;
		for r := 1 to BOARD_SIZE do	{ scan all cells }
		for c := 1 to BOARD_SIZE do
			if board.board[r, c] = NULL then
			begin
				board.board[r, c] := COMPUTER;
				score := aiMinimax(board, depth+1, FALSE)+
					 ltx_randin(-AI_DEPTH[AI_DIFFICULTY],
					             AI_DEPTH[AI_DIFFICULTY]);
				board.board[r, c] := NULL;
				best := ltx_max(score, best);
			end;
		aiMinimax := best;
	end
	else				{ HUMAN: minimizer move }
	begin
		best := AI_INFTY;
		for r := 1 to BOARD_SIZE do	{ scan all cells }
		for c := 1 to BOARD_SIZE do
			if board.board[r, c] = NULL then
			begin
				board.board[r, c] := HUMAN;
				score := aiMinimax(board, depth+1, TRUE)+
                                	 ltx_randin(-AI_DEPTH[AI_DIFFICULTY],
					             AI_DEPTH[AI_DIFFICULTY]);
				board.board[r, c] := NULL;
				best := ltx_min(score, best);
			end;
		aiMinimax := best;
	end;
end;

procedure	aiBestMove(var board: GAME_BOARD; var bc, br: INDEX_TYPE);
var     score,
	best	: integer;
	c, r	: INDEX_TYPE;
begin
	best	:= -AI_INFTY;

	{ scan all cells, evaluate minimax function for all empty cells }
	{ return the cell with optimal value                            }
	for r := 1 to BOARD_SIZE do
	for c := 1 to BOARD_SIZE do
		if board.board[r, c] = NULL then	{ cell is empty }
		begin
			board.board[r, c] := COMPUTER;	{ make the move }
			{ evaluate the move }
			score := aiMinimax(board, 0, FALSE);
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
