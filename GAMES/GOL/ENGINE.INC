{$ifndef __ENGINE_INC__}
{$define __ENGINE_INC__}

{ ////////////////////////////////////////////////////////////////////////// }

function	dead   (const ctx: COLONY; c, r: integer): boolean;       	forward;
function	alive  (const ctx: COLONY; c, r: integer): boolean;       	forward;
function	cell   (c, r: integer): integer;				forward;
procedure	empty  (var ctx: COLONY);					forward;
procedure	fill   (var ctx: COLONY);					forward;
procedure	invert (var ctx: COLONY);					forward;
procedure	init   (var ctx: COLONY);					forward;
procedure	render (const ctx: COLONY);					forward;
function	create: COLONY;							forward;
procedure	destroy(var ctx: COLONY);					forward;
function	load   (var ctx: COLONY; src: string): boolean;			forward;
function	save   (const ctx: COLONY; dst: string): boolean;		forward;
function        next(var ctx: COLONY; x, y: integer): STATE;			forward;
procedure	evolve (var ctx: COLONY);                               	forward;

{ ////////////////////////////////////////////////////////////////////////// }

function	cell(c, r: integer): integer;
begin
        { return the correct offset of the cell in the colony }
	cell := r * FIELD_W + c;
end;

function	dead(const ctx: COLONY; c, r: integer): boolean;
begin
        dead := ctx^.field[cell(c, r)] = C_DEAD;
end;

function	alive(const ctx: COLONY; c, r: integer): boolean;
begin
        alive := ctx^.field[cell(c, r)] = C_ALIVE;
end;

procedure	toggle(var ctx: COLONY; c, r: integer);
var	o	: integer;
begin
	o := cell(c, r);
	if ctx^.field[o] = C_ALIVE then
        	ctx^.field[o] := C_DEAD
        else	ctx^.field[o] := C_ALIVE;
end;

function        next(var ctx: COLONY; x, y: integer): STATE;
var	n, i, j,
        c, r	: integer;
        s	: STATE;
begin
	s := ctx^.field[cell(x, y)];
	n := 0;
        for i := -1 to +1 do
        for j := -1 to +1 do
        begin
                c := (x + j + FIELD_W) mod FIELD_W;
                r := (y + i + FIELD_H) mod FIELD_H;
	        inc(n, ctx^.field[cell(c, r)]);
        end;
        dec(n, ctx^.field[cell(x, y)]);

        if (s = C_DEAD) and (n = 3) then
        	next := C_ALIVE
        else	if (s = C_ALIVE) and ((n < 2) or (n > 3)) then
        		next := C_DEAD
        	else	next := s;
end;

procedure	empty(var ctx: COLONY);
begin
	fillchar(ctx^.field, sizeof(FIELD_TYPE), C_DEAD);
	fillchar(ctx^.next , sizeof(FIELD_TYPE), C_DEAD);
end;

procedure	fill   (var ctx: COLONY);
begin
	fillchar(ctx^.field, sizeof(FIELD_TYPE), C_ALIVE);
	fillchar(ctx^.next , sizeof(FIELD_TYPE), C_DEAD);
end;

procedure	invert (var ctx: COLONY);
var	i, j, o	: integer;
begin
	for j := 0 to FIELD_H-1 do
        for i := 0 to FIELD_W-1 do
        begin
        	o := cell(i, j);
        	if ctx^.field[o] = C_ALIVE then
                        ctx^.field[o] := C_DEAD
                else	ctx^.field[o] := C_ALIVE;
        end;
	fillchar(ctx^.next , sizeof(FIELD_TYPE), C_DEAD);
end;

procedure	init(var ctx: COLONY);
var	k	: integer;
begin
	if ctx = nil then exit;
	empty(ctx);
	for k := 0 to FIELD_W * FIELD_H-1 do
		ctx^.field[k] := STATE(random(2));
end;

function	create: COLONY;
var	ctx	: COLONY;
begin
	if maxavail < sizeof(COLONY_TYPE) then
	begin
		create := nil;
		exit;
	end;

	new(ctx);
	empty(ctx);
	create := ctx;
end;

procedure	destroy(var ctx: COLONY);
begin
	if (ctx <> nil) then
	begin
		dispose(ctx);
		ctx := nil;
	end;
end;

procedure       cellular(const ctx: COLONY; x, y: integer; active: boolean);
var	clr	: integer;
begin
        clr := GAME_CONFIG.c_dead;
	if ctx^.field[cell(x, y)] = C_ALIVE then clr := GAME_CONFIG.c_alive;
        if active then clr := 8 shl 4 + clr;
	prints(BOARD_X + (x shl 1), BOARD_Y + y, clr, FIELD_C);
end;

procedure	render(const ctx: COLONY);
var	x, y	: integer;
begin
	if ctx = nil then exit;

	for y := 0 to FIELD_H-1 do
	for x := 0 to FIELD_W-1 do	cellular(ctx, x, y, FALSE);
end;

procedure	evolve(var ctx: COLONY);
var	x, y	: integer;
	n	: STATE;
begin
	if ctx = nil then exit;

	fillchar(ctx^.next, sizeof(FIELD_TYPE), C_DEAD);
	for y := 0 to FIELD_H-1 do
	for x := 0 to FIELD_W-1 do
	begin
		n := next(ctx, x, y);		{ evaluate the cell       }
		ctx^.next[cell(x, y)] := n;	{ update to the next gen. }
	end;
        ctx^.field := ctx^.next;
end;

function	load(var ctx: COLONY; src: string): boolean;
var	f	: FILE;
	nr	: word;
	buffer	: FIELD_TYPE;
	hdr	: MAP_MAGIC;
begin
	load := FALSE;
	assign(f, src);
	{$I-}
	reset(f, 1);
	{$I+}
	if ioresult <> 0 then exit;

        blockread(f, hdr, sizeof(MAP_MAGIC));
        if hdr <> MAP_SIGNATURE then
        begin
		close(f);
                exit;
        end;

	blockread(f, buffer, sizeof(FIELD_TYPE), nr);
	if nr <> sizeof(FIELD_TYPE) then
	begin
		close(f);
		exit;
	end;
	ctx^.field := buffer;

	close(f);
        load := TRUE;
end;

function	save(const ctx: COLONY; dst: string): boolean;
var	f	: FILE;
	nw	: word;
begin
        save := FALSE;
	assign(f, dst);
	{$I-}
	rewrite(f, 1);
	{$I+}
	if ioresult <> 0 then exit;

        blockwrite(f, MAP_SIGNATURE, sizeof(MAP_MAGIC));
	blockwrite(f, ctx^.field, sizeof(FIELD_TYPE), nw);
	if nw <> sizeof(FIELD_TYPE) then
	begin
		close(f);
		erase(f);
		exit;
	end;

	close(f);
        save := TRUE;
end;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	block_put(blk: PBLOCK; x, y, attr: integer);
var	i, j	: integer;
        q	: pchar;
begin
        if (blk = nil) then exit;

        if x < 0 then x := (SCR_WIDTH - blk^.w) shr 1;
        if y < 0 then y := (SCR_HEIGHT - blk^.h) shr 1;

        q := blk^.data;
	for j := 1 to blk^.h do
        begin
        	for i := 1 to blk^.w do
                begin
                	if q^ <> #0 then
                                prints(x+(i-1)*2, y+j-1, attr, #177#177);
                        inc(q);
                end;
        end;
end;

{$endif}
