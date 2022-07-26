{$ifndef	__TETRIS_ENGINE__}
{$define	__TETRIS_ENGINE__}

{$V-}

{ ////////////////////////////////////////////////////////////////////////// }

procedure	moveGenerate (var m: MOVE_TYPE);								forward;
procedure	moveSet      (var m: MOVE_TYPE; p, r: longint);							forward;

procedure	boardInit    (var b: BOARD_TYPE; level: byte);							forward;
procedure	boardDraw    (const b: BOARD_TYPE);								forward;
procedure	boardClose   (const b: BOARD_TYPE);								forward;
procedure	boardFill    (var b: BOARD_TYPE; v: integer);							forward;
procedure	boardCondense(var b: BOARD_TYPE; var mk: MARKER_TYPE);						forward;
procedure	boardPieceShadow(var b: BOARD_TYPE; const p: PIECE_TYPE; m: MOVE_TYPE; x, y: integer);		forward;

function	boardOffset  (x, y: longint): longint;								forward;
procedure	boardRowGen  (var b: BOARD_TYPE; row: byte);							forward;
procedure	boardRowDraw (const b: BOARD_TYPE; y: longint);							forward;
procedure	boardRowFill (var b: BOARD_TYPE; y, v: longint);						forward;
function	boardRowFull (const b: BOARD_TYPE; y: longint): boolean;					forward;

function	pieceColor   (p: longint): longint;									forward;
function	pieceRotate  (px, py, r: longint): longint;								forward;
function	pieceFit     (const p: PIECE_TYPE; const b: BOARD_TYPE; const m: MOVE_TYPE; x, y: longint): boolean;	forward;
procedure	pieceDraw    (const p: PIECE_TYPE; const m: MOVE_TYPE; x, y: longint);					forward;
procedure	pieceShow    (const p: PIECE_TYPE; const m: MOVE_TYPE; x, y: longint);					forward;
procedure	pieceLock    (const p: PIECE_TYPE; var b: BOARD_TYPE; const m: MOVE_TYPE; x, y: longint);		forward;
function	pieceWidth   (const p: PIECE_TYPE; const m: MOVE_TYPE): longint;					forward;

{ markers for full row checking }
procedure	markerAdd    (var mk: MARKER_TYPE; value: byte);						forward;
procedure	markerReset  (var mk: MARKER_TYPE);								forward;
function	markerSize   (const mk: MARKER_TYPE): longint;							forward;
function	markerGet    (const mk: MARKER_TYPE; idx: longint): byte;					forward;

{ score management }
procedure	scoreShow(x, y: integer);									forward;
procedure	scoreView;                                                                                      forward;
procedure	scoreUpdate;	                                                           			forward;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	moveGenerate(var m: MOVE_TYPE);
begin
	m.piece  := random(PIECE_MAX)+1;
{$ifdef	RANDOM_ROTATION}
	m.rotate := random(4);	{ increased difficulty }
{$else}
	m.rotate := 0;
{$endif}
end;

procedure	moveSet(var m: MOVE_TYPE; p, r: longint);
begin
	m.piece  := p;
	m.rotate := r;
end;

procedure	boardRowGen(var b: BOARD_TYPE; row: byte);
var	x, o,
	p, c	: longint;
begin
	o := boardOffset(1, row);

	{ pick a random number of blocks to generate, minimum three blocks  }
	{ and maximum at BOARD_WIDTH-3 block, where as minus two blocks at  }
	{ the borders and one more block to avoid generate a full row       }
	c := random(BOARD_WIDTH-3);
	if c < 3 then c := 3;

	for x := 0 to c-1 do
	begin
		p := random(BOARD_WIDTH-2);     { at random column }
		b[o+p] := random(PIECE_MAX)+1;	{ and random piece }
	end;
end;

procedure	boardInit(var b: BOARD_TYPE; level: byte);
var	i, j, o	: longint;
begin
	for j := 0 to BOARD_HEIGHT-1 do
	begin
		o := boardOffset(0, j);
		for i := 0 to BOARD_WIDTH-1 do
		begin
			if (i = 0) or (i = BOARD_WIDTH-1) or
			   (j = BOARD_HEIGHT-1) then
				b[o+i] := BOARD_BORDER
			else	b[o+i] := BOARD_BLANK;
		end;
	end;
	if level > 0 then
		for i := 1 to level do boardRowGen(b, BOARD_HEIGHT-i-1);
end;

function	boardOffset(x, y: longint): longint;
begin
	boardOffset := y * BOARD_WIDTH + x;
end;

procedure	boardDraw(const b: BOARD_TYPE);
var	y	: longint;
begin
	for y := 0 to BOARD_HEIGHT-1 do boardRowDraw(b, y);
end;

procedure	boardRowDraw(const b: BOARD_TYPE; y: longint);
var	x, o	: longint;
	block	: PIECE_SHAPE;
begin
	o := boardOffset(0, y);
	for x := 0 to BOARD_WIDTH-1 do
	begin
		case b[o] of
		BOARD_BLANK	: block := BOARD_TEXT_BLANK;
		BOARD_BORDER	: block := BOARD_TEXT_BORDER;
		BOARD_WIPE	: block := BOARD_TEXT_WIPE;
		BOARD_SHADOW	: block := BOARD_TEXT_BORDER;
		else		  block := BOARD_TEXT_PIECE;
		end;
		prints(BOARD_X + (x shl 1),
		       BOARD_Y + y, pieceColor(b[o]), block);
		inc(o);
	end;
end;

procedure	boardFill(var b: BOARD_TYPE; v: integer);
var	x, y, o	: longint;
begin
	for y := 0 to BOARD_HEIGHT-2 do
	begin
		o := boardOffset(1, y);
		for x := 0 to BOARD_WIDTH-3 do
			if v >= 0 then
				b[o+x] := v
			else	b[o+x] := 1+random(7);
	end;
end;

procedure	boardClose(const b: BOARD_TYPE);	{ closing animation }
var	y	: longint;
	oldscr	: pchar;
begin
	oldscr := vscreen_activate(vscreen_vram);
	for y := 0 to BOARD_HEIGHT-1 do
	begin
		vsync_sleep(2);
		boardRowDraw(b, y);
	end;
	vscreen_activate(oldscr);
end;

procedure	boardRowFill(var b: BOARD_TYPE; y, v: longint);
var	o, i	: longint;
begin
	o := boardOffset(1, y);
	for i := 0 to BOARD_WIDTH-3 do b[o+i] := v;
end;

function	boardRowFull(const b: BOARD_TYPE; y: longint): boolean;
var	o, i	: longint;
begin
	o := boardOffset(1, y);
	for i := 0 to BOARD_WIDTH-3 do
		if b[o+i] = 0 then
		begin
			boardRowFull := FALSE;
			exit;
		end;
	boardRowFull := TRUE;
end;

procedure	boardMarkFull(var b: BOARD_TYPE; var mk: MARKER_TYPE; y: longint);
var	i	: longint;
begin
	for i := 0 to PIECE_SIZE-1 do
		if (y + i < BOARD_HEIGHT-1) then
		begin
			if boardRowFull(b, y + i) then
			begin
				boardRowFill(b, y + i, BOARD_WIPE);
				markerAdd(mk, y + i);
			end;
		end;
end;

procedure	boardCondense(var b: BOARD_TYPE; var mk: MARKER_TYPE);
var	x, y, k	: longint;
begin
	for k := 1 to markerSize(mk) do
		for x := 1 to BOARD_WIDTH-2 do
		begin
			for y := markerGet(mk, k) downto 1 do
				b[y*BOARD_WIDTH+x] := b[(y-1)*BOARD_WIDTH+x];
			b[x] := BOARD_BLANK;
		end;
	markerReset(mk);
end;

procedure	boardPieceShadow(var b: BOARD_TYPE; const p: PIECE_TYPE; m: MOVE_TYPE; x, y: integer);
var	i, j, pi: integer;
	s, e, o,
	min, max: integer;
begin
	min := BOARD_WIDTH-1; max := 0;
	for i := 0 to PIECE_SIZE-1 do
	for j := 0 to PIECE_SIZE-1 do
	begin
		pi := pieceRotate(i, j, m.rotate);
		if (x + i >= 0) and (x + i < BOARD_WIDTH) then
		if (y + j >= 0) and (y + j < BOARD_HEIGHT) then
		begin
			if (p[m.piece][pi] <> 0) then
			begin
				s := x+i;
				e := s;
				if min > s then min := s;
				if max < e then max := e;
			end;
		end;
	end;

	o := boardOffset(0, BOARD_HEIGHT-1);
	for i := 1 to BOARD_WIDTH-1 do
		if (i >= min) and (i <= max) then
			b[o+i] := BOARD_SHADOW
		else	b[o+i] := BOARD_BORDER;
end;

procedure	pieceInit(var p: PIECE_TYPE);
var	i	: longint;
begin
	for i := 1 to PIECE_MAX do
		p[i] := TETRIS_PIECES[i];
end;

function	pieceRotate(px, py, r: longint): longint;
var	pi	: longint;
begin
	pi := 0;
	case r and 3 of
	0: pi :=      (py shl 2) + px;		{ 0 deg }
	1: pi := 12 +  py        - (px shl 2);	{ 90 deg }
	2: pi := 15 - (py shl 2) - px;		{ 180 deg }
	3: pi :=  3 -  py        + (px shl 2);	{ 270 deg }
	end;
	pieceRotate := pi;
end;

function	pieceFit(const p: PIECE_TYPE; const b: BOARD_TYPE; const m: MOVE_TYPE; x, y: longint): boolean;
var	i, j,
	pi, fi	: longint;
begin
	for i := 0 to PIECE_SIZE-1 do
	for j := 0 to PIECE_SIZE-1 do
	begin
		pi := pieceRotate(i, j, m.rotate);
		fi := boardOffset(i + x, j + y);
		if (x + i >= 0) and (x + i < BOARD_WIDTH) then
		if (y + j >= 0) and (y + j < BOARD_HEIGHT) then
		begin
			if (p[m.piece][pi] <> 0) and (b[fi] <> 0) then
			begin
				pieceFit := FALSE;
				exit;
			end;
		end;
	end;
	pieceFit := TRUE;
end;

function	pieceColor(p: longint): longint;
begin
	pieceColor := GAME_CONFIG.scheme[p];
end;

procedure	pieceDraw(const p: PIECE_TYPE; const m: MOVE_TYPE; x, y: longint);
var	i, j,
	cx, cy	: longint;
	block	: string[2];
begin
	block := PIECE_TEXT;
	for i := 0 to PIECE_SIZE-1 do
	for j := 0 to PIECE_SIZE-1 do
	begin
		cx := x + i shl 1;
		cy := y + j;
		if p[m.piece, pieceRotate(i, j, m.rotate)] <> 0 then
			prints(cx, cy, pieceColor(m.piece), block);
	end;
end;

procedure	pieceShow(const p: PIECE_TYPE; const m: MOVE_TYPE; x, y: longint);
begin
	pieceDraw(p, m, BOARD_X + (x shl 1), BOARD_Y + y);
end;

procedure	pieceLock(const p: PIECE_TYPE; var b: BOARD_TYPE; const m: MOVE_TYPE; x, y: longint);
var	i, j	: longint;
begin
	for i := 0 to PIECE_SIZE-1 do
	for j := 0 to PIECE_SIZE-1 do
		if p[m.piece][pieceRotate(i, j, m.rotate)] <> 0 then
			b[boardOffset(x + i, y + j)] := m.piece;
end;

function	pieceWidth(const p: PIECE_TYPE; const m: MOVE_TYPE): longint;
var	i, j,
	v, mv	: longint;
begin
	mv := 0;
	for j := 0 to PIECE_SIZE-1 do
	begin
		v := 0;
		for i := 0 to PIECE_SIZE-1 do
		begin
			if (p[m.piece][pieceRotate(i, j, m.rotate)] <> 0) then inc(v);
			if mv < v then mv := v;
		end;
	end;
	pieceWidth := mv;
end;

procedure	markerInit(var mk: MARKER_TYPE);
var	i	: longint;
begin
	mk.size := 0;
	for i := 1 to BOARD_HEIGHT do mk.list[i] := 0;
end;

procedure	markerAdd(var mk: MARKER_TYPE; value: byte);
begin
	if (mk.size < BOARD_HEIGHT) then
	begin
		inc(mk.size);
		mk.list[mk.size] := value;
	end;
end;

procedure	markerReset(var mk: MARKER_TYPE);
begin
	mk.size := 0;
end;

function	markerSize(const mk: MARKER_TYPE): longint;
begin
	markerSize := mk.size;
end;

function	markerGet(const mk: MARKER_TYPE; idx: longint): byte;
begin
	if (idx > 0) and (idx <= BOARD_HEIGHT) then markerGet := mk.list[idx];
end;

const	player	: SCORE_ENTRY = (name: ''; score: 0);

procedure	scoreInit;
begin
	player.name := '';
	player.score := 0;
end;

procedure	scoreSet(value: longint);
begin
	player.score := value;
end;

function	scoreRank(const player: SCORE_ENTRY): integer;
var	i	: integer;
begin
	i := 1;
        {$R-}
	while (i <= MAX_SCORE_TABLE) and
	      (player.score <= GAME_CONFIG.scores[i].score) do inc(i);
        {$R+}
	if i <= MAX_SCORE_TABLE then
		scoreRank := i
	else	scoreRank := 0;
end;

procedure	scoreAdd(rank: integer; player: SCORE_ENTRY);
begin
	if (rank > 0) and (rank <= MAX_SCORE_TABLE) then
	begin
		GAME_CONFIG.scores[rank].name := toupper(player.name);
		GAME_CONFIG.scores[rank].score := player.score;
	end;
end;

procedure	scoreShow(x, y: integer);
var	i	: integer;
begin
	for i := 1 to MAX_SCORE_TABLE do
	begin
		print(x, y+i,
		      '~01' + itoa(i) + ' ~09' +
		      rpad(GAME_CONFIG.scores[i].name, 8) + ' ~0B' +
		      lpad(ltoa(GAME_CONFIG.scores[i].score), 8));
	end;
end;

procedure	scoreView;
var	x, y,
	w, h	: integer;
        dlg	: PDIALOG;
begin
	w := 30;	h := 9;
        x := -1;	y := -1;
	ltx_validate_rect(x, y, w, h);
        y := y + 7;
        dlg := dialog_open(x, y, w, h, SHADE_WHITE, '~08[ ~0FLEADER BOARD ~08]', '');
	scoreShow(x + 6, y + 1);
	wait([#27, #13]);
        dialog_close(dlg);
end;

procedure	scoreUpdate;
var	x, y,
	py, w, h: integer;
        dlg	: PDIALOG;
        oldscr	: SCREEN;
begin
	player.score := counterGetValue(GAME_SCORE);
	py := scoreRank(player);
	if py = 0 then exit;

	w := 25;
	h := 9;
	x := ltx_centerx(w);
	y := ltx_centery(h);

        oldscr := vscreen_activate(vscreen_vram);
        dlg := dialog_open(x, y, w, h, SHADE_WHITE,
			   '~08[ ~0FNEW HIGH SCORE ~08]', '');
	scoreShow(x + 3, y + 1);
	print(x + 3, y+py+1,
	      '~01' + itoa(py) + ' ~09' +
	      rpad('', 8) + ' ~0B' +
	      lpad(ltoa(player.score), 8));
	if input(x + 5, y + py + 1, 8, $3F, player.name) then
		scoreAdd(py, player);

	scoreShow(x + 3, y);
        dialog_close(dlg);
        vscreen_activate(oldscr);
end;

{$endif}

