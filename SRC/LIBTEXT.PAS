{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   LIBTEXT.PAS                                                              }
{   -----------                                                              }
{   Core library for text user interface game under DOS                      }
{   Version 1.3                                                              }
{                                                                            }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2016-2021 by Infinity Group                               }
{                                                                            }
{   Compilers supported:                                                     }
{   --------------------                                                     }
{   . Turbo/Borland Pascal 7.0                                               }
{   . Free Pascal 1.0                                                        }
{   . TMT Pascal 3.5+                                                        }
{                                                                            }
{ ////////////////////////////////////////////////////////////////////////// }

{$i preps.inc}

unit    libtext;

interface

{$ifdef	FPC}
uses    go32;
{$endif}

{ ///////////////////////////// DEFINITIONS //////////////////////////////// }

const	LIBTEXT_VERSION	= $0104;	{ library version }
        LIBTEXT_BUILD	= '0714';	{ library build   }
	LIBTEXT_INIT	: boolean = FALSE;

	SCR_WIDTH	= 80;		{ text-mode screen width }
	SCR_HEIGHT	= 25;		{ text-mode screen height }
	SCR_SIZE	= SCR_WIDTH * SCR_HEIGHT * 2;
        SCR_LINEW	= SCR_WIDTH * 2;{ real width of the screen on memory }

	BW80		= 2;		{ black-white }
	CO80		= 3;		{ text-mode number }
	MONO		= 7;		{ monochrome }

	MONO_ADAPTER	: boolean = FALSE;
        VSEG_ADDR       : word = $B800;

	CURSOR_OFF	= $2000;	{ turn off cursor }
	CURSOR_ON	= $0607;	{ restore cursor }
	CURSOR_BLOCK	= $0107;	{ block cursor }

	BLINK_OFF	= FALSE;	{ no high-intensity backgrounds }
	BLINK_ON	= TRUE;		{ enable high-intensity backgrounds }

	CLEAR_COLOR	: byte = 0;	{ default background color }

	{ some popular keyboard scancodes }
	KEY_A		= $0041;
	KEY_B		= $0042;
	KEY_C		= $0043;
	KEY_D		= $0044;
	KEY_E		= $0045;
	KEY_F		= $0046;
	KEY_G		= $0047;
	KEY_H		= $0048;
	KEY_I		= $0049;
	KEY_J		= $004A;
	KEY_K		= $004B;
	KEY_L		= $004C;
	KEY_M		= $004D;
	KEY_N		= $004E;
	KEY_O		= $004F;
	KEY_P		= $0050;
	KEY_Q		= $0051;
	KEY_R		= $0052;
	KEY_S		= $0053;
	KEY_T		= $0054;
	KEY_U		= $0055;
	KEY_V		= $0056;
	KEY_W		= $0057;
	KEY_X		= $0058;
	KEY_Y		= $0059;
	KEY_Z		= $005A;
	KEY_SLASH	= $002F;
        KEY_QUESTION	= $003F;
	KEY_EXCLAIM	= $0021;

	KEY_F1		= $3B00;
	KEY_F2		= $3C00;
	KEY_F3		= $3D00;
	KEY_F4		= $3E00;
	KEY_F5		= $3F00;
	KEY_F6		= $4000;
	KEY_F7		= $4100;
	KEY_F8		= $4200;
	KEY_F9		= $4300;
	KEY_F10		= $4400;
	KEY_F11		= $8500;
	KEY_F12		= $8600;

	KEY_SHIFT_TAB	= $0F00;
        KEY_BACKSPACE	= $0008;
	KEY_TAB		= $0009;
	KEY_ENTER	= $000D;
	KEY_SPACE	= $0020;
	KEY_LEFT	= $4B00;
	KEY_RIGHT	= $4D00;
	KEY_UP		= $4800;
	KEY_DOWN	= $5000;
	KEY_ESC		= $001B;
	KEY_HOME	= $4700;
	KEY_END		= $4F00;
        KEY_DELETE	= $5300;

	KEY_PGUP	= $4900;
	KEY_PGDN	= $5100;
	KEY_CTRL_LEFT	= $7300;
	KEY_CTRL_RIGHT	= $7400;
	KEY_CTRL_UP	= $8D00;
	KEY_CTRL_DOWN	= $9100;
	KEY_CTRL_BSLASH	= $001C;

	KEY_CTRL_A	= $0001;
	KEY_CTRL_B	= $0002;
	KEY_CTRL_C	= $0003;
	KEY_CTRL_D	= $0004;
	KEY_CTRL_E	= $0005;
	KEY_CTRL_F	= $0006;
	KEY_CTRL_G	= $0007;
	KEY_CTRL_H	= $0008;
	KEY_CTRL_I	= $0009;
	KEY_CTRL_J	= $000A;
	KEY_CTRL_K	= $000B;
	KEY_CTRL_L	= $000C;
	KEY_CTRL_M	= $000D;
	KEY_CTRL_N	= $000E;
	KEY_CTRL_O	= $000F;
	KEY_CTRL_P	= $0010;
	KEY_CTRL_Q	= $0011;
	KEY_CTRL_R	= $0012;
	KEY_CTRL_S	= $0013;
	KEY_CTRL_T	= $0014;
	KEY_CTRL_U	= $0015;
	KEY_CTRL_V	= $0016;
	KEY_CTRL_W	= $0017;
	KEY_CTRL_X	= $0018;
	KEY_CTRL_Y	= $0019;
	KEY_CTRL_Z	= $001A;

const	MAX_DLG_LINE	= 22;		{ maximum allowed text lines for }
        				{ displaying in a dialog box     }

type	pword = ^word;  		{ word-based pointer }

	SCREEN	= pchar;		{ for screen memory accessing }

	{ represent an ANSI image }
	ANSIDATA = array[1..4000] of char;

	KEYS = set of char;		{ represent a set of keys }

	SHADING	= array[1..4] of byte;	{ shading of colors }

	FRAME_SET = string[6];		{ frame character set }

	ALIGNMENT = (AF_LEFT,		{ text alignment }
		     AF_CENTER,
		     AF_RIGHT);

	PREGION = ^TREGION;		{ represent a screen block }
	TREGION	= record
		l, t,
		w, h	: word;
		size	: word;
		data	: pchar;
	end;

	PBLOCK = ^BLOCK_TYPE;		{ text-based bit block }
	BLOCK_TYPE = record
                x, y	: integer;
                w, h	: word;
		size	: word;
                data	: pchar;
                attach	: boolean;	{ attached block or not }
	end;

        PDIALOG = ^DIALOG_TYPE;
        DIALOG_TYPE = record
                x, y	: integer;
                w, h,
		oldc	: word;
                lines,
                mlen	: integer;
		shade	: SHADING;
                title	: string[60];
                text	: array[1..MAX_DLG_LINE] of string;
                save	: PREGION;
                show	: boolean;
                oldscr	: SCREEN;
        end;

const	{ use combine with SHADING type }
	SHADE_BRIGHT	= 1;
	SHADE_MEDIUM	= 2;
	SHADE_DARK	= 3;
        SHADE_TEXT	= 4;

	{ pre-defined monotone color shades }
	SHADE_MONO	: SHADING = (15,  7,  7, 7);
	SHADE_BLACK	: SHADING = ( 0,  0,  0, 0);
	SHADE_RED	: SHADING = (15, 12,  4, 7);
	SHADE_YELLOW	: SHADING = (15, 14,  6, 7);
	SHADE_GREEN	: SHADING = (15, 10,  2, 7);
	SHADE_CYAN	: SHADING = (15, 11,  3, 7);
	SHADE_BLUE	: SHADING = (15,  9,  1, 7);
	SHADE_MAGENTA	: SHADING = (15, 13,  5, 7);
	SHADE_WHITE	: SHADING = (15,  7,  8, 7);

        { pre-defined gradients }
        SHADE_TRAFFIC	: SHADING = (12, 14, 10, 7);
        SHADE_INFERNO	: SHADING = (14, 12,  4, 7);
        SHADE_DAWN	: SHADING = (13,  9,  1, 7);
        SHADE_OCEAN	: SHADING = (11,  9,  1, 7);
        SHADE_POISON	: SHADING = (10,  2,  3, 7);
        SHADE_PLASMA	: SHADING = (15, 11,  9, 7);
        SHADE_FLARE	: SHADING = (15, 14, 12, 7);
        SHADE_FOREST	: SHADING = (11, 10,  6, 7);
        SHADE_BERRY	: SHADING = (15, 12,  9, 7);
        SHADE_RGB	: SHADING = ( 4,  2,  1, 7);
        SHADE_CMY	: SHADING = (14, 13, 11, 7);

        COLOR_NAMES	: array[0..15] of string[11] =
		('BLACK'   , 'BLUE'       , 'GREEN'    , 'CYAN'    ,
		 'RED'     , 'MAGENTA'    , 'ORANGE'   , 'GRAY'    ,
		 'DK. GRAY', 'LT. BLUE'   , 'LT. GREEN', 'LT. CYAN',
		 'LT. RED' , 'LT. MAGENTA', 'YELLOW'   , 'WHITE'   );

	{ frame types }
	FRAME_SINGLE	: FRAME_SET = #$DA#$C4#$BF#$B3#$C0#$D9;
	FRAME_DOUBLE	: FRAME_SET = #$C9#$CD#$BB#$BA#$C8#$BC;
	FRAME_TEXT	: FRAME_SET = '+-+|++';

	{ pointer to video RAM, must be filled by ltx_init() }
	VIDEO_RAM	: SCREEN = nil;
	CURRENT_SCREEN	: SCREEN = nil;	{ pointer to current screen buffer }

        ENDL		= #10;

type	STR_ATTR	= string[3];	{ for storing attribute sequence }
	STR8		= string[8];
	STR10		= string[10];
	STR20		= string[20];
	STR30		= string[30];
        STR40		= string[40];
        STR80		= string[80];
        STR128		= string[128];

type    CONTROL_STATE	= (CS_NEXT,	{ indicate select next control     }
			   CS_PREV,	{ indicate select previous control }
                           CS_DONE,	{ terminate control selection loop }
                           CS_ABORT);	{ abort control selection loop     }

const	MULTI_CONTROLS	: boolean = FALSE;

var     { BIOS timer ticks for INT 1AH }
	ticks		: longint absolute $0040:$006C;

{ ///////////////////////////// LOW-LEVEL API ////////////////////////////// }

{$ifndef FPC}	{ retain FPC compatibility }

function	inportb(px: word): byte;
procedure	outportb(px: word; v: byte);
function	inportw(px: word): word;
procedure	outportw(px: word; v: word);

{$endif}

procedure       ltx_init;
procedure	ltx_done;
function	ltx_ready: boolean;
function	ltx_mono: boolean;
procedure	ltx_setmode(mode: byte);
function	ltx_getmode: byte;
function	ltx_version: string;
function        ltx_bit: SCREEN;
function	ltx_malloc(size: longint): pointer;
procedure	ltx_free(var pt);
function	ltx_memavail: longint;
procedure	ltx_timer_init;
procedure	ltx_timer_done;
function	ltx_timer: longint;
function	ltx_offset(x, y: word): word;
function	ltx_makeb(l, h: byte): byte;
function	ltx_makew(c: char; a: byte): word;
function	ltx_wtoa(w: word): byte;
function	ltx_wtoc(w: word): char;
procedure	ltx_setw(x, y: word; data: word);
procedure	ltx_seta(x, y: word; a: byte);
procedure	ltx_setc(x, y: word; c: char);
function	ltx_getw(x, y: word): word;
function	ltx_geta(x, y: word): byte;
function	ltx_getc(x, y: word): char;
procedure	ltx_fillw(x, y, w, h: integer; v: word);
procedure	ltx_filla(x, y, w, h: integer; a: byte);
procedure	ltx_fillc(x, y, w, h: integer; c: char);
function	ltx_min(a, b: longint): longint;
function	ltx_max(a, b: longint): longint;
function	ltx_randin(a, b: longint): longint;
function	ltx_vram: SCREEN;
function	ltx_centerx(w: longint): longint;
function	ltx_centery(h: longint): longint;
procedure	ltx_validate_rect(var x, y: integer; w, h: integer);

{ /////////////////////////////// TIMING API /////////////////////////////// }

procedure	vsync;
procedure	vsync_sleep(n: longint);
procedure	sleep(duration: longint);

{ ////////////////////////////// KEYBOARD API ////////////////////////////// }

function	readkey: char;
function	keypressed: boolean;
function	getkey: longint;
procedure	flushkeys;
procedure	keyboard_speed(speed: word);

{ /////////////////////////// VIRTUAL SCREEN API /////////////////////////// }

function	vscreen_vram: SCREEN;
function        vscreen_alloc: SCREEN;
procedure	vscreen_free(var s: SCREEN);
function	vscreen_activate(s: SCREEN): SCREEN;
procedure	vscreen_flip(s: SCREEN);

{ ///////////////////// SCREEN ATTTRIBUTE MODIFIERS //////////////////////// }

procedure	blink(flag: boolean);
procedure	cursor(size: word);
function	cursor_shape: word;
procedure	gotoxy(x, y: byte);
function	wherex: byte;
function	wherey: byte;
function        make_attr(b: byte): STR_ATTR;
function	get_clear_color: byte;

{ ///////////////////////////// SCREEN I/O API ///////////////////////////// }

procedure	clear(attr: byte);

procedure	frame_type(const f: FRAME_SET);
procedure	frame(x, y, w, h: integer; color: byte);
procedure       shaded_box(l, t, w, h: longint;
			   shade: SHADING; align: ALIGNMENT;
			   title: string);

procedure	ANSI_draw(var buffer);
procedure	printc(x, y, attr: longint; c: char);
procedure       prints(x, y, attr: longint; text: string);
function	inputs(x, y, w, a: integer; itext: string): string;
function	text_length(text: string): longint;
procedure       print(x, y: longint; st: string);
procedure       center(y: longint; text: string);
function	input(x, y, w, c: integer; var s: string): boolean;

function        region_size(w, h: integer): word;
function	region_get(l, t, w, h: integer): PREGION;
procedure	region_put(var r: PREGION);
procedure	region_dump(r: PREGION; filename: string);
function	region_read(filename: string): PREGION;

{ ////////////////////////// TEXT-BASED BIT BLOCK ////////////////////////// }

function        block_create       (w, h: integer): PBLOCK;
procedure	block_destroy      (var blk: PBLOCK);
procedure	block_blit         (blk: PBLOCK; x, y, attr: integer; trans: boolean);
procedure	block_membit       (blk: PBLOCK; var buffer);
function	block_attach       (buffer: pointer): PBLOCK;
procedure	block_mapper       (blk: PBLOCK; buf: pchar; c: char);
function	block_save         (blk: PBLOCK; filename: string): boolean;
function	block_load         (filename: string): PBLOCK;

{ //////////////////////////// DIALOG BOX API ////////////////////////////// }

function	dialog_create (x, y, w, h: integer; shade: SHADING;
			       title, text: string): PDIALOG;
function	dialog_open   (x, y, w, h: integer; shade: SHADING;
			       title, text: string): PDIALOG;
procedure	dialog_show   (var dlg: PDIALOG);
procedure	dialog_hide   (var dlg: PDIALOG);
procedure	dialog_destroy(var dlg: PDIALOG);
procedure	dialog_close  (var dlg: PDIALOG);

{ ////////////////////////// STRING MANIPULATION /////////////////////////// }

function        lpad(text: string; width: longint): string;
function        rpad(text: string; width: longint): string;
function	repl(ch: char; n: longint): string;
function        ltoa(v: longint): string;
function        itoa(v: integer): string;
function	ftoa(f: real; n: integer): string;
function	atol(v: string): longint;
function	tohex(n, w: longint): string;
function	toupper(s: string): string;
function	tolower(s: string): string;
function        tokenize(var st: string; sep: char): string;
function        make_center(text: string; width: longint): string;
function        unitize(v: longint): string;

{ ///////////////////////// FOR DEBUGGING PURPOSES ///////////////////////// }

procedure	die(code: integer; msg: string);
function	screen_dump(filename: string): boolean;
function	screen_view(filename: string): boolean;
function        vtos(v: longint; b: string): string;

procedure       memstateInit;
procedure	memstateDone;

implementation

const	LTX_INITIALIZED	: boolean = FALSE;

	{ to make a chrome look of the shades }
	SHADE_PERCENTAGE: SHADING = (15, 40, 30, 0);

	FRAME_CURRENT	: FRAME_SET = '';

	{ for hexa-decimal conversion }
	HEXA_DIGITS	: array[0..15] of char = '0123456789ABCDEF';

	CONTROL_CHAR	= '~';		{ control sequence character }

	SIZE_KB	= 1000;
	SIZE_MB	= 1000 * SIZE_KB;
        SIZE_GB = 1000 * SIZE_MB;       { just in case you're on FreeDOS }

type    MEMSTATE = record		{ for memory leak checking }
		membf,
		memaf 	: longint;
	end;

var     g_memstate	: MEMSTATE;

{ ////////////////////////////////////////////////////////////////////////// }
{ //                      FP/TP/TMT COMPATIBLE CODE                       // }
{ ////////////////////////////////////////////////////////////////////////// }

{ before logging memory state }
procedure       memstateInit;
begin
	g_memstate.membf := ltx_memavail;
	g_memstate.memaf := 0;
end;

{ finish logging memory state and output result }
procedure	memstateDone;
var	delta	: longint;
begin
	g_memstate.memaf := ltx_memavail;
	delta	 := g_memstate.memaf - g_memstate.membf;
	writeln('==========================================');
{$ifdef	__TMT__}
        writeln('COMPILER     : ', 'TMT PASCAL');
{$else}
        writeln('COMPILER     : ', 'BORLAND PASCAL');
{$endif}
	writeln('------------------------------------------');
	writeln('MEMORY BEFORE: ', g_memstate.membf:12, ' bytes (', unitize(g_memstate.membf), ')');
	writeln('MEMORY AFTER : ', g_memstate.memaf:12, ' bytes (', unitize(g_memstate.memaf), ')');
	writeln('------------------------------------------');
	writeln('CHANGES      : ', delta:12, ' bytes');
	writeln('==========================================');
end;

{$ifndef FPC}
function	inportb(px: word): byte;
begin
	inportb := port[px];
end;

procedure	outportb(px: word; v: byte);
begin
	port[px] := v;
end;

function	inportw(px: word): word;
begin
	inportw := portw[px];
end;

procedure	outportw(px: word; v: word);
begin
	portw[px] := v;
end;
{$endif}

var     SCR_SAVE	: record	{ old screen data }
		data	: array[0..SCR_SIZE-1] of byte;
                cx, cy	: word;
	end;

{ initialize the LIBTEXT library - must be called before any other API calls }
procedure       ltx_init;
begin
	if ltx_getmode = MONO then
	begin
{$ifdef __TMT__}
		VIDEO_RAM := pchar  ($B0000);
{$else}
	{$ifdef	FPC}
		VIDEO_RAM := pointer($B000);
	{$else}
		VIDEO_RAM := ptr    ($B000, 0);
        {$endif}
{$endif}
                VSEG_ADDR := $B000;
	end
	else
	begin
{$ifdef __TMT__}
		VIDEO_RAM := pchar  ($B8000);
{$else}
	{$ifdef	FPC}
		VIDEO_RAM := pointer($B800);
	{$else}
		VIDEO_RAM := ptr    ($B800, 0);
        {$endif}
{$endif}
                VSEG_ADDR := $B800;
	end;
	CURRENT_SCREEN := VIDEO_RAM;

        with SCR_SAVE do
        begin
{$ifndef FPC}
       	        move(VIDEO_RAM^, data, SCR_SIZE);
{$else}
        	dosmemget(VSEG_ADDR, 0, data, SCR_SIZE);
{$endif}
        	cx := wherex;
	        cy := wherey;
        end;

        clear($07);
        LTX_INITIALIZED := TRUE;

	LIBTEXT_INIT := TRUE;
end;

{ shutdown the LIBTEXT library }
procedure	ltx_done;
begin
	if not LIBTEXT_INIT then exit;
	ltx_setmode(CO80);

        with SCR_SAVE do
        begin
{$ifndef FPC}
        	move(data, VIDEO_RAM^, SCR_SIZE);
{$else}
		dosmemput(VSEG_ADDR, 0, data, SCR_SIZE);
{$endif}
	        gotoxy(cx, cy);
	end;

	LTX_INITIALIZED := FALSE;
end;

function	ltx_ready: boolean;
begin
        ltx_ready := LTX_INITIALIZED;
end;

{ return TRUE if the adapter is monochrome }
function	ltx_mono: boolean;
begin
	ltx_mono := MONO_ADAPTER;
end;

{ return the pointer to current screen buffer }
function        ltx_bit: SCREEN;
begin
	ltx_bit := CURRENT_SCREEN;
end;

{ allocate a block of memory }
function	ltx_malloc(size: longint): pointer;
var	p	: pointer;
begin
	ltx_malloc	:= nil;
        size	:= size + 4;
        if ltx_memavail < size then exit;

	getmem(p, size);
        move(size, p^, 4);
        p := pchar(p) + 4;
        ltx_malloc := p;
end;

{ return the memory block of the given pointer back to the heap }
procedure	ltx_free(var pt);
var     size	: longint;
	p	: pchar;
begin
	p := pointer(pt);
	if p = nil then exit;
        p := pchar(p) - 4;
        move(p^, size, 4);
        freemem(p, size);
        p := nil;
end;

function	ltx_memavail: longint;
begin
{$ifndef FPC}
	ltx_memavail := memavail;
{$else}
        ltx_memavail := high(longint);
{$endif}
end;

{ switch video mode }
procedure	ltx_setmode(mode: byte); assembler;
asm
	mov	ah, $00
	mov	al, mode
	int	$10
end;

{ get current video mode }
function	ltx_getmode: byte; assembler;
asm
	mov	ah, $0F
	int	$10
end;

function	ltx_version: string;
begin
	ltx_version := vtos(LIBTEXT_VERSION, LIBTEXT_BUILD);
end;

{ initialize high precision timer in milliseconds }
procedure	ltx_timer_init; assembler;
asm
	mov     dx, $43			{ 8524 timer control word }
	mov	al, $36
	out	dx, al

	mov	dx, $40			{ new timer constant for 1.19318 Mhz }
	mov	al, $A9                 { 04A9h = 1193 }
	out	dx, al
	mov	al, $04
	out	dx, al
end;

function	ltx_timer: longint;
begin
	ltx_timer := ticks;
end;

{ restore old timer frequency }
procedure	ltx_timer_done; assembler;
asm
	mov	dx, $43			{ 8253 control word }
	mov	al, $36 		{ old timer 0 constant = 0 }
	out	dx, al

	mov	dx, $40
	xor	al, al
	out	dx, al
	out	dx, al
end;

{ determine the offset of (x, y) inside a screen buffer, (1-80,1-25) }
function	ltx_offset(x, y: word): word;
begin
	ltx_offset := (y-1) * SCR_LINEW + (x-1) shl 1;
end;

{ make a byte from two nibbles }
function	ltx_makeb(l, h: byte): byte;
begin
	ltx_makeb := l or (h shl 4);
end;

{ make a word containing both character and its attribute }
function	ltx_makew(c: char; a: byte): word;
begin
	ltx_makew := ord(c) or a shl 8;
end;

{ put a word to a specified coordinate (x, y) in the current buffer }
procedure	ltx_setw(x, y: word; data: word);
begin
{$ifndef FPC}
        pword(ltx_bit + ltx_offset(x, y))^ := data;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                MEMW[word(ltx_bit):ltx_offset(x, y)] := data
        else    pword(ltx_bit+ltx_offset(x, y))^ := data;
{$endif}
end;

{ return a word at the specified coordinate (x, y) in the current buffer }
function	ltx_getw(x, y: word): word;
begin
{$ifndef FPC}
        ltx_getw := pword(ltx_bit + ltx_offset(x, y))^;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                ltx_getw := MEMW[word(ltx_bit):ltx_offset(x, y)]
        else
                ltx_getw := pword(ltx_bit+ltx_offset(x, y))^;
{$endif}
end;

{ modify the attribute at coordinate (x, y) }
procedure	ltx_seta(x, y: word; a: byte);
begin
{$ifndef FPC}
	(ltx_bit + ltx_offset(x, y) + 1)^ := chr(a);
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                MEM[word(ltx_bit):ltx_offset(x, y) + 1] := a
        else    (ltx_bit + ltx_offset(x, y) + 1)^ := char(a);
{$endif}
end;

{ modify the character at coordinate (x, y) }
procedure	ltx_setc(x, y: word; c: char);
begin
{$ifndef FPC}
	(ltx_bit + ltx_offset(x, y))^ := c;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                MEM[word(ltx_bit):ltx_offset(x, y)] := byte(c)
        else    (ltx_bit + ltx_offset(x, y) + 1)^ := c;
{$endif}
end;

{ retrieve the attribute at coordinate (x, y) }
function	ltx_geta(x, y: word): byte;
begin
{$ifndef FPC}
        ltx_geta := byte((ltx_bit + ltx_offset(x, y) + 1)^);
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                ltx_geta := MEM[word(ltx_bit):ltx_offset(x, y) + 1]
        else    ltx_geta := byte((ltx_bit + ltx_offset(x, y) + 1)^);
{$endif}
end;

{ retrieve the character at coordinate (x, y) }
function	ltx_getc(x, y: word): char;
begin
{$ifndef FPC}
	ltx_getc := (ltx_bit + ltx_offset(x, y))^;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
                ltx_getc := char(MEM[word(ltx_bit):ltx_offset(x, y)])
        else    ltx_getc := (ltx_bit + ltx_offset(x, y))^;
{$endif}
end;

{ fill a rectangular area with specified attribute }
procedure	ltx_filla(x, y, w, h: integer; a: byte);
var	p	: pchar;
	i, j, o : word;
	v	: char;
begin
	if (w <= 0) or (h <= 0) then exit;
	v := char(a);
{$ifndef FPC}
        p := ltx_bit + ltx_offset(x, y) + 1;
        for j := 0 to h - 1 do
	begin
		for i := 0 to w - 1 do (p+(i shl 1))^ := v;
		inc(p, SCR_LINEW);
	end;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
        begin
                o := ltx_offset(x, y) + 1;
                for j := 0 to h - 1 do
                begin
                        for i := 0 to w - 1 do
                                MEM[VSEG_ADDR:o+i shl 1] := a;
                        inc(o, SCR_LINEW);
	        end;
        end
        else
        begin
                p := ltx_bit + ltx_offset(x, y) + 1;
                for j := 0 to h - 1 do
                begin
		        for i := 0 to w - 1 do (p+(i shl 1))^ := v;
        	inc(p, SCR_LINEW);
	        end;
        end;
{$endif}
end;

{ fill a rectangular area with specified character }
procedure	ltx_fillc(x, y, w, h: integer; c: char);
var	p	: pchar;
	i, j, o : word;
        v       : byte;
begin
	if (w <= 0) or (h <= 0) then exit;
{$ifndef FPC}
	p := ltx_bit + ltx_offset(x, y);
	for j := 0 to h - 1 do
	begin
		for i := 0 to w - 1 do
			(p+(i shl 1))^ := c;
		inc(p, SCR_LINEW)
	end;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
        begin
                v := byte(c);
                o := ltx_offset(x, y);
                for j := 0 to h - 1 do
                begin
                        for i := 0 to w - 1 do
                                MEM[VSEG_ADDR:o+i shl 1] := v;
                        inc(o, SCR_LINEW);
	        end;
        end
        else
        begin
                p := ltx_bit + ltx_offset(x, y);
	        for j := 0 to h - 1 do
                begin
                       for i := 0 to w - 1 do
                                (p+(i shl 1))^ := c;
		        inc(p, SCR_LINEW)
                end;
        end;
{$endif}
end;

{ fill a rectangular area with a pair of character and attribute }
procedure	ltx_fillw(x, y, w, h: integer; v: word);
var	p	: pword;
	i, j,
        r, o    : integer;
begin
	if (w <= 0) or (h <= 0) then exit;
        CLEAR_COLOR := v shr 12;
	r := SCR_WIDTH-w;
{$ifndef FPC}
        p := pword(ltx_bit + ltx_offset(x, y));
        for j := 0 to h - 1 do
	begin
		for i := 0 to w-1 do
		begin
			p^ := v;
			inc(p);
		end;
		inc(p, r);
	end;
{$else}
        if word(ltx_bit) = VSEG_ADDR then
        begin
                o := ltx_offset(x, y);
                for j := 0 to h-1 do
                begin
                        for i := 0 to w-1 do
                                MEMW[VSEG_ADDR:o+(i shl 1)] := v;
                        inc(o, SCR_LINEW);
                end;
        end
        else
        begin
                p := pword(ltx_bit + ltx_offset(x, y));
	        for j := 0 to h - 1 do
                begin
		        for i := 0 to w-1 do
		        begin
			        p^ := v;
			        inc(p);
		        end;
		        inc(p, r);
                end;
	end;
{$endif}
end;

{ extract the attribute part of a word }
function	ltx_wtoa(w: word): byte;
begin
	ltx_wtoa := w shr 8;
end;

{ extract the character part of a word }
function	ltx_wtoc(w: word): char;
begin
	ltx_wtoc := char(w and $FF);
end;

{ return the pointer to the video RAM }
function	ltx_vram: SCREEN;
begin
	ltx_vram := VIDEO_RAM;
end;

{ determine the x-centered coordinate using the given width (w) }
function	ltx_centerx(w: longint): longint;
begin
	ltx_centerx := (SCR_WIDTH - w) shr 1 + 1;
end;

{ determine the y-centered coordinate using the given height (h) }
function	ltx_centery(h: longint): longint;
begin
	ltx_centery := (SCR_HEIGHT - h) shr 1 + 1;
end;

{ ensure a rectangle always inside the visible screen space }
procedure	ltx_validate_rect(var x, y: integer; w, h: integer);
begin
	if (x < 0) then x := ltx_centerx(w);
	if (y < 0) then y := ltx_centery(h);
	while (x + w - 1 > SCR_WIDTH) do if (x > 0) then dec(x);
	while (y + h - 1 > SCR_HEIGHT) do if (y > 0) then dec(y);
end;

{ smallest value of a and b }
function	ltx_min(a, b: longint): longint;
begin
	if a < b then ltx_min := a else ltx_min := b;
end;

{ largest value of a and b }
function	ltx_max(a, b: longint): longint;
begin
	if a >= b then ltx_max := a else ltx_max := b;
end;

{ generate random number in the range [a,b] }
function	ltx_randin(a, b: longint): longint;
begin
	ltx_randin := random(b-a+1)+a;
end;

{ wait for veritcal retrace, avoid snowy screen updates }
procedure	vsync;
begin
	if MONO_ADAPTER then exit;
	while (inportb($3DA) and 8) <> 0 do;
	while (inportb($3DA) and 8) =  0 do;
end;

{ VSYNC-based delay }
procedure	vsync_sleep(n: longint);
begin
	while n > 0 do
	begin
		vsync;
		dec(n);
	end;
end;

{ timer-based delay }
procedure	sleep(duration: longint);
var	timer	: longint;
begin
	timer := ticks;
	while abs(ticks-timer) < duration do;
end;

{ read a character }
function	readkey: char; assembler;
asm
	mov  ah, $07
	int  $21
end;

{ wait until a key is hit, the character is not removed from the buffer }
function	keypressed: boolean; assembler;
asm
	mov  ah, $0B
	int  $21
end;

{ read extended keyboard scancode }
function	getkey: longint;
var	k	: longint;
	c	: char;
begin
	c := upcase(readkey);
	if c = #0 then	k := (ord(readkey) shl 8) + ord(c)
	else		k := ord(c);
	getkey := k;
end;

{ discard any available keys in the buffer }
procedure	flushkeys;
begin
	while keypressed do readkey;
end;

{ set keyboard typematic speed }
procedure	keyboard_speed(speed: word);
begin
	outportb($60, $F3);
	sleep(2);
	outportb($60, speed);
end;

{ return the pointer to the video RAM }
function	vscreen_vram: SCREEN;
begin
	vscreen_vram := VIDEO_RAM;
end;

{ virtual screen allocation }
function        vscreen_alloc: SCREEN;
var	scr	: SCREEN;
begin
        scr := ltx_malloc(SCR_SIZE);
	fillchar(scr^, SCR_SIZE, 0);
	vscreen_alloc := scr;
end;

{ release memory occupied by a virtual screen }
procedure	vscreen_free(var s: SCREEN);
begin
	if s <> nil then ltx_free(s);
end;

{ activate a virtual screen, subsequent drawings will happen on this screen }
function	vscreen_activate(s: SCREEN): SCREEN;
var	old	: SCREEN;
begin
	old := CURRENT_SCREEN;
	CURRENT_SCREEN := s;
	vscreen_activate := old;
end;

{ page flipping }
procedure	vscreen_flip(s: SCREEN);
begin
{$ifndef FPC}
	move(s^, vscreen_vram^, SCR_SIZE);
{$else}
        dosmemput(VSEG_ADDR, 0, s^, SCR_SIZE);
{$endif}
end;

{ control text blinking }
procedure	blink(flag: boolean); assembler;
asm
	mov	ax, $1003
	cmp	[flag], 0
	jz	@off

	mov	bl, 1
	jmp	@done

@off:
	mov	bl, 0

@done:
	int	10h
end;

{ cursor size control }
procedure	cursor(size: word); assembler;
asm
	mov	ah, $01
	mov	bh, 0
	mov	cx, size
	int	$10
end;

{ return the shape of the cursor }
function	cursor_shape: word; assembler;
asm
	mov	ah, $03
	mov	bh, 0
	int	$10
	mov	ax, cx
end;

{ move the cursor to coordinate (x, y), ranged in (1-80,1-25) }
procedure	gotoxy(x, y: byte); assembler;
asm
	dec	x
	dec	y
	mov     ah, $02
	mov	bh, 0
	mov	dh, y
	mov	dl, x
	int	$10
end;

{ return the x-coordinate of the cursor }
function	wherex: byte; assembler;
asm
	mov	ah, $03
	mov	bh, 0
	int	$10
	inc	dl
	mov	al, dl
end;

{ return the y-coordinate of the cursor }
function	wherey: byte; assembler;
asm
	mov	ah, $03
	mov	bh, 0
	int	$10
	inc	dh
	mov	al, dh
end;

{ convert an attribute byte to control sequence }
function        make_attr(b: byte): STR_ATTR;
var	s	: STR_ATTR;
begin
	s := CONTROL_CHAR + tohex(b, 2);
	make_attr := s;
end;

{ return the current clear color, or as known as background color }
function	get_clear_color: byte;
begin
	get_clear_color := CLEAR_COLOR;
end;

{ draw an ANSI buffer to video RAM, the screen is created using tools  }
{ such as: TheDraw, ACIDDraw, Pablo, etc. and exported as Pascal-based }
{ array to be included directly into the program                       }
procedure       ANSI_draw(var buffer);
begin
{$ifndef FPC}
	move(buffer, vscreen_vram^, SCR_SIZE);
{$else}
        dosmemput(VSEG_ADDR, 0, buffer, SCR_SIZE);
{$endif}
end;

{ print a character on screen buffer }
procedure       printc(x, y, attr: longint; c: char);
begin
	ltx_setw(x, y, ltx_makew(c, attr));
end;

{ print a text string on screen buffer }
procedure       prints(x, y, attr: longint; text: string);
var	i	: longint;
{$ifndef FPC}
	p	: pword;
{$endif}
begin
{$ifndef FPC}
	p := pword(ltx_bit+ltx_offset(x, y));
	for i := 1 to length(text) do
	begin
		p^ := ltx_makew(text[i], attr);
		inc(p);
	end;
{$else}
        for i := 1 to length(text) do
                printc(x+i-1, y, attr, text[i]);
{$endif}
end;

{ input/editing a string }
function	inputs(x, y, w, a: integer; itext: string): string;
var	temp	: string;
	done 	: boolean;
        pt	: byte;
        ch	: char;
        oc	: word;
        ox, oy	: byte;
begin
        temp := itext;
        if length(temp) > w then delete(temp, w+1, length(temp));
        pt   := length(temp);
	ox   := wherex;
        oy   := wherey;
        oc   := cursor_shape;
	done := FALSE;
        gotoxy(x, y+pt);
        flushkeys;
        vsync;
        cursor(CURSOR_ON);
        repeat
	       	prints(x, y, a, rpad(temp, w));
                gotoxy(x+pt, y);
                ch := readkey;
                case ch of
                #00:	begin
                		ch := readkey;
                                case ch of
                                #71:	pt := 0;
                                #79:	pt := length(temp);
                                #75:	if pt > 0 then dec(pt);
                                #77:	if pt < length(temp) then inc(pt);
                                #83:	if (pt >= 0) then delete(temp, pt+1, 1)
                                end;
			end;
                #27:	begin
				done := TRUE;
                                temp := '';
                	end;
                #13:	done := TRUE;
                #08:	if pt > 0 then
			begin
				delete(temp, pt, 1);
                                dec(pt);
                	end;
                #32..#254:
                	if length(temp) < w then
                        begin
                                insert(ch, temp, pt+1);
        	                inc(pt);
                        end;
                end;
        until done;
        cursor(oc);
        gotoxy(ox, oy);
        inputs := temp;
end;

{ clear screen buffer }
procedure	clear(attr: byte);
begin
	CLEAR_COLOR := attr shr 4;
	ltx_fillw(1, 1, SCR_WIDTH, SCR_HEIGHT, ltx_makew(' ', attr));
end;

{ colorful text printing using control sequences ~XY, as follows. }
{ whereas: X = background color                                   }
{          Y = foreground color                                   }
{ example: print(10, 8, '~0FWhite ~0AGreen ~1BGuess it.');        }
{ note   : the control squence must contain both X and Y          }
procedure       print(x, y: longint; st: string);
var     i, attr,
	fg, bk	: longint;
	v	: char;
begin
	attr := $07; i := 1;
	while i <= length(st) do
	begin
		v := st[i];
		if (v = CONTROL_CHAR) and
		   (st[i+1] in ['0'..'9', 'A'..'F', 'a'..'f']) and
		   (st[i+2] in ['0'..'9', 'A'..'F', 'a'..'f']) then
		begin
			inc(i); v := upcase(st[i]);
			case v of
			'0'..'9': bk := ord(v)-48;
			'A'..'F': bk := ord(v)-55;
			else	  bk := 0;
			end;
			inc(i); v := upcase(st[i]);
			case v of
			'0'..'9': fg := ord(v)-48;
			'A'..'F': fg := ord(v)-55;
			else	  fg := 0;
			end;
			attr := bk shl 4 + fg;
		end
		else
		begin
			printc(x, y, attr, v);
			inc(x);
		end;
		inc(i);
	end;
end;

{ read a string at coordinate (x, y), using w-width field and color c}
function	input(x, y, w, c: integer; var s: string): boolean;
var	done	: boolean;
	key	: char;
	i	: integer;
	oldc	: word;
	text	: string;
begin
	done := FALSE;
	text := s;
	oldc := cursor_shape;
	cursor(CURSOR_ON);
	gotoxy(x+length(text), y);
	repeat
		for i := 1 to w do
			if i <= length(text) then
				printc(x+i-1, y, c, text[i])
			else	printc(x+i-1, y, c, ' ');
		gotoxy(x+length(text), y);
		key := readkey;
		if key <> #0 then case key of
		#13:	begin
				done := TRUE;
				input := TRUE;
				s := text;
			end;
		#27:	begin
				done := TRUE;
				input := FALSE;
			end;
		#8:
			begin
				delete(text, length(text), 1);
			end;
		else
			if length(text) < w then
			begin
				text := text + key;
			end;
		end
		else	readkey;
	until done;
	cursor(oldc);
end;

{ return the length of the text, exlcuding the control sequences }
function	text_length(text: string): longint;
var	i, c, l	: longint;
begin
	c := 0;
	l := length(text);
	for i := 1 to l do
		if text[i] = CONTROL_CHAR then inc(c, 3);
	text_length := l-c;
end;

{ centering the text }
procedure       center(y: longint; text: string);
begin
	print(ltx_centerx(text_length(text)), y, text);
end;

function	dialog_create(x, y, w, h: integer; shade: SHADING;
			      title, text: string): PDIALOG;
var	dlg	: PDIALOG;
	tl	: integer;
begin
	dialog_create := nil;
        if maxavail < sizeof(DIALOG_TYPE) then exit;
	new(dlg);

        dlg^.lines  := 0;
        dlg^.mlen   := 0;

        while (text <> '') and (dlg^.lines < MAX_DLG_LINE) do
        begin
        	inc(dlg^.lines);
                dlg^.text[dlg^.lines] := tokenize(text, ENDL);
                tl := text_length(dlg^.text[dlg^.lines]);
                if dlg^.mlen < tl then dlg^.mlen := tl;
        end;

	if w <= 0 then w := dlg^.mlen + 6  else w := ltx_max(dlg^.mlen + 6, w);
	if h <= 0 then h := dlg^.lines + 4 else h := ltx_max(dlg^.lines + 4, h);

        ltx_validate_rect(x, y, w, h);

        dlg^.x := x;	dlg^.y := y;
        dlg^.w := w;	dlg^.h := h;
        dlg^.show  := FALSE;
        dlg^.shade := shade;
	dlg^.title := title;

	dialog_create := dlg;
end;

procedure	dialog_show(var dlg: PDIALOG);
var	i	: integer;
begin
	if dlg = nil then exit;
        if dlg^.show then exit;

	dlg^.oldscr := vscreen_activate(vscreen_vram);
        dlg^.show   := TRUE;
	dlg^.save   := region_get(dlg^.x, dlg^.y, dlg^.w, dlg^.h);
	dlg^.oldc   := cursor_shape;

	cursor(CURSOR_OFF);
	shaded_box(dlg^.x, dlg^.y, dlg^.w, dlg^.h, dlg^.shade, AF_LEFT, dlg^.title);
        for i := 1 to dlg^.lines do
		print(dlg^.x + 3, dlg^.y + i + 1, make_center(dlg^.text[i], dlg^.mlen));

	vscreen_activate(dlg^.oldscr);
end;

procedure	dialog_hide(var dlg: PDIALOG);
begin
        if dlg = nil then exit;
        if not dlg^.show then exit;

	dlg^.oldscr := vscreen_activate(vscreen_vram);

        dlg^.show := FALSE;
        region_put(dlg^.save);
	cursor(dlg^.oldc);

	vscreen_activate(dlg^.oldscr);
end;

procedure	dialog_destroy(var dlg: PDIALOG);
begin
        if dlg^.show then dialog_hide(dlg);
        dispose(dlg);
end;

function	dialog_open(x, y, w, h: integer; shade: SHADING;
			    title, text: string): PDIALOG;
var	dlg	: PDIALOG;
begin
	dialog_open := nil;
        dlg := dialog_create(x, y, w, h, shade, title, text);
        if dlg = nil then exit;
        dialog_show(dlg);
        dialog_open := dlg;
end;

procedure	dialog_close(var dlg: PDIALOG);
begin
        dialog_destroy(dlg);
end;

{ left padding a string }
function        lpad(text: string; width: longint): string;
var	tmp	: string;
	d, l	: longint;
begin
	l := length(text);
	d := abs(l-width);
	if l < width then
	begin
		fillchar(tmp[1], d, ' ');
		move(text[1], tmp[d+1], l);
		tmp[0] := chr(width);
		lpad := tmp;
	end
	else	lpad := text;
end;

{ right padding a string }
function        rpad(text: string; width: longint): string;
var	tmp	: string;
	d, l	: longint;
begin
	l := length(text);
	d := abs(l-width);
	if l < width then
	begin
		move(text[1], tmp[1], l);
		fillchar(tmp[l+1], d, ' ');
		tmp[0] := chr(width);
		rpad := tmp;
	end
	else	rpad := text;
end;

{ replicate a character n times to form a string }
function	repl(ch: char; n: longint): string;
var	st	: string;
begin
	fillchar(st[1], n, ch);
	st[0] := chr(n);
	repl := st;
end;

{ long to string conversion }
function        ltoa(v: longint): string;
var	text	: string;
begin
	text := '';
	if v = 0 then text := '-' else str(v, text);
	ltoa := text;
end;

{ integer to string conversion }
function        itoa(v: integer): string;
var	text	: string;
begin
	str(v, text);
	itoa := text;
end;

{ float to string conversion }
function	ftoa(f: real; n: integer): string;
var	s	: string;
begin
	str(f:0:n, s);
	ftoa := s;
end;

{ convert string to longint }
function	atol(v: string): longint;
var     r	: longint;
        e	: integer;
begin
	val(v, r, e);
        atol := r;
end;


{ convert a number to hexa-decimal format, using w characters }
function	tohex(n, w: longint): string;
var	s	: string;
begin
	s := '';
        if n < 0 then n := not n;
	while n <> 0 do
	begin
		s := HEXA_DIGITS[n mod 16] + s;
		n := n shr 4;
	end;
	while length(s) < w do s := '0'+s;
	tohex := s;
end;

{ make a string upper-case }
function	toupper(s: string): string;
var	i	: integer;
begin
	for i := 1 to length(s) do s[i] := upcase(s[i]);
	toupper := s;
end;

{ convert a string into lower case }
function	tolower(s: string): string;
var	i	: integer;
begin
	for i := 1 to length(s) do
		if s[i] in ['A'..'Z'] then s[i] := chr(ord(s[i]) + 32);
	tolower := s;
end;

{ split a string into tokens using given separator character }
function        tokenize(var st: string; sep: char): string;
var	temp	: string;
	p	: integer;
begin
	p := pos(sep, st);
        if p <> 0 then
        begin
                temp := copy(st, 1, p-1);
                delete(st, 1, p);
        end
        else
	begin
		temp := st;
                st := '';
        end;
        tokenize := temp;
end;

{ make a centerized text string }
function        make_center(text: string; width: longint): string;
var	bound,
	len, i	: longint;
	pad	: string;
begin
	len := text_length(text);
	if width < len then width := len;
	bound := (width-len) shr 1;
	pad   := '';
	for i := 1 to bound do pad := pad+' ';
	text := pad+text+pad;
	while text_length(text) < width do text := ' '+text;
	make_center := text;
end;

{ convert a value into memory space unit-based text }
function        unitize(v: longint): string;
var	s_unit	: string[3];
begin
	if v > SIZE_GB then
        begin
		s_unit := ' GB';
                v      := v div SIZE_GB;
        end
	else
        	if v > SIZE_MB then
                begin
                	s_unit := ' MB';
                        v      := v div SIZE_MB;
                end
                else
                begin
                	s_unit := ' KB';
                        v      := v div SIZE_KB;
                end;
                unitize := ltoa(v) + s_unit;
end;


{ set frame characters }
procedure	frame_type(const f: FRAME_SET);
begin
	FRAME_CURRENT := f;
end;

{ draw a frame using characters given in FRAME_CURRENT }
procedure	frame(x, y, w, h: integer; color: byte);
var	i	: integer;
begin
	if FRAME_CURRENT = '' then FRAME_CURRENT := FRAME_SINGLE;
	printc(x    , y    , color, FRAME_CURRENT[1]);
	printc(x+w-1, y    , color, FRAME_CURRENT[3]);
	printc(x    , y+h-1, color, FRAME_CURRENT[5]);
	printc(x+w-1, y+h-1, color, FRAME_CURRENT[6]);
	for i := 1 to w-2 do
	begin
		printc(x+i, y    , color, FRAME_CURRENT[2]);
		printc(x+i, y+h-1, color, FRAME_CURRENT[2]);
	end;
	for i := 1 to h-2 do
	begin
		printc(x    , y+i, color, FRAME_CURRENT[4]);
		printc(x+w-1, y+i, color, FRAME_CURRENT[4]);
	end;
end;

{ open a shaded box, use -1 at (l, t) to make it center }
procedure       shaded_box(l, t, w, h: longint;
			   shade: SHADING; align: ALIGNMENT;
			   title: string);
var	vw, hw,
	vg, hg,
	vd, hd,
	i	: longint;
begin
	if title <> '' then
		title := #32+title+#32;
	w := ltx_max(w, text_length(title)+4);
	h := ltx_max(h, 3);

	if l < 0 then l := ltx_centerx(w);
	if t < 0 then t := ltx_centery(h);

	vw := (w-1) * SHADE_PERCENTAGE[SHADE_BRIGHT] div 100;
	vg := (w-1) * SHADE_PERCENTAGE[SHADE_MEDIUM] div 100;
	vd := (w-1) * SHADE_PERCENTAGE[SHADE_DARK]   div 100;
	hw := (h-1) * SHADE_PERCENTAGE[SHADE_BRIGHT] div 100;
	hg := (h-1) * SHADE_PERCENTAGE[SHADE_MEDIUM] div 100;
	hd := (h-1) * SHADE_PERCENTAGE[SHADE_DARK]   div 100;

	vg := ltx_max(vg, 1);
	hg := ltx_max(hg, 1);

	{ filling }
	for i := 1 to h do prints(l, t+i-1, $0F, lpad(' ', w));

	if ltx_mono then shade := SHADE_MONO;
	{ framing }
	printc(l    , t    , shade[SHADE_BRIGHT], 'Ú');
	printc(l+w-1, t+h-1, shade[SHADE_BRIGHT], 'Ù');
	for i := 1 to vw do
	begin
		printc(l+i       , t    , shade[SHADE_BRIGHT], #196);
		printc(l+w-i-1   , t+h-1, shade[SHADE_BRIGHT], #196);
	end;
	for i := 1 to vg do
	begin
		printc(l+i+vw    , t    , shade[SHADE_MEDIUM], #196);
		printc(l+w-i-vw-1, t+h-1, shade[SHADE_MEDIUM], #196);
	end;
	for i := 1 to vd do
	begin
		printc(l+i+vw+vg    , t    , shade[SHADE_DARK], #196);
		printc(l+w-i-vw-vg-1, t+h-1, shade[SHADE_DARK], #196);
	end;
	for i := 1 to hw do
	begin
		printc(l    , t+i    , shade[SHADE_BRIGHT], #179);
		printc(l+w-1, t+h-i-1, shade[SHADE_BRIGHT], #179);
	end;
	for i := 1 to hg do
	begin
		printc(l    , t+i+hw    , shade[SHADE_MEDIUM], #179);
		printc(l+w-1, t+h-i-hw-1, shade[SHADE_MEDIUM], #179);
	end;
	for i := 1 to hd do
	begin
		printc(l    , t+i+hw+hg    , shade[SHADE_DARK], #179);
		printc(l+w-1, t+h-i-hw-hg-1, shade[SHADE_DARK], #179);
	end;

	if title <> '' then
		case align of
		AF_LEFT:   print(l+2, t, title);
		AF_CENTER: print(l+(w-text_length(title)) shr 1 + 1, t, title);
		AF_RIGHT:  print(l+w-text_length(title)-2, t+h-1, title);
		end;
end;

{ calculate the memory size needed of a region with dimensions (w x h) }
function        region_size(w, h: integer): word;
var	size	: word;
begin
	size := w * h shl 1;
	region_size := size;
end;

{ copy a region from video RAM to memory buffer }
function	region_get(l, t, w, h: integer): PREGION;
var	r	: PREGION;
{$ifndef FPC}
	p,
{$endif}
	q	: pchar;
	w2, i,
	size	: longint;
begin
	new(r);
	size := region_size(w, h);
	w2   := w shl 1;
        r^.data := ltx_malloc(size);

	r^.l := l;	r^.t := t;
	r^.w := w;	r^.h := h;
	r^.size := size;

{$ifndef FPC}
        p := VIDEO_RAM+ltx_offset(l, t);
	q := r^.data;
	for i := t to t+h-1 do
	begin
		move(p^, q^, w2);
		inc(p, 160);
		inc(q, w2);
	end;
{$else}
	q := r^.data;
	for i := t to t+h-1 do
	begin
		dosmemget(VSEG_ADDR, ltx_offset(l, i), q^, w2);
		inc(q, w2);
	end;
{$endif}

	region_get := r;
end;

procedure	region_put(var r: PREGION);
var
{$ifndef FPC}
	q,
{$endif}
	p	: pchar;
	i, w2	: longint;
begin
	w2 := r^.w shl 1;
	p := r^.data;
{$ifndef FPC}
	q := VIDEO_RAM+ltx_offset(r^.l, r^.t);
	for i := r^.t to r^.t+r^.h-1 do
	begin
		move(p^, q^, w2);
		inc(p, w2);
		inc(q, 160);
	end;
{$else}
	for i := r^.t to r^.t+r^.h-1 do
	begin
		dosmemput(VSEG_ADDR, ltx_offset(r^.l, i), p^, w2);
		inc(p, w2);
	end;
{$endif}
        ltx_free(r^.data);
	dispose(r);
end;

{ dump the region's memory buffer onto disk }
procedure	region_dump(r: PREGION; filename: string);
var	f	: FILE;
begin
	assign(f, filename);
	rewrite(f, 1);
	blockwrite(f, r^.l, sizeof(r^.l));
	blockwrite(f, r^.t, sizeof(r^.t));
	blockwrite(f, r^.w, sizeof(r^.h));
	blockwrite(f, r^.h, sizeof(r^.h));
	blockwrite(f, r^.data^, r^.size);
	close(f);
end;

{ read up a the region's data to memory buffer  }
function	region_read(filename: string): PREGION;
var     r	: PREGION;
	f	: FILE;
begin
	new(r);
	assign(f, filename);
	reset(f, 1);

	blockread(f, r^.l, sizeof(r^.l));
	blockread(f, r^.t, sizeof(r^.t));
	blockread(f, r^.w, sizeof(r^.h));
	blockread(f, r^.h, sizeof(r^.h));

	r^.size := region_size(r^.w, r^.h);
        r^.data := ltx_malloc(r^.size);
	blockread(f, r^.data^, r^.size);

	close(f);
	region_read := r;
end;

{ create a text-based bit block }
function        block_create(w, h: integer): PBLOCK;
var	blk	: PBLOCK;
begin
	if (w < 1) or (h < 1) then exit;
        new(blk);
        blk^.x	    := 0;
        blk^.y	    := 0;
        blk^.w	    := w;
        blk^.h	    := h;
        blk^.size   := w * h;
	blk^.attach := FALSE;

        if ltx_memavail < blk^.size then
        begin
                dispose(blk);
                block_create := nil;
                exit;
        end;

        blk^.data := ltx_malloc(blk^.size);
        block_create := blk;
end;

{ free a text-based bit block }
procedure	block_destroy(var blk: PBLOCK);
begin
        if (blk^.size <> 0) and not blk^.attach then
                ltx_free(blk^.data);
        dispose(blk);
end;

{ blit a text-based bit block to screen at (x, y) using given attr }
procedure	block_blit(blk: PBLOCK; x, y, attr: integer; trans: boolean);
var	i, j,
        cw, ch	: integer;	{ clipped width and height }
{$ifndef FPC}
	r	: integer;
	p	: SCREEN;
        a, v	: char;
{$endif}
        q	: pchar;
begin
        if (blk = nil) or (x < 1) or (y < 1) or
	   (x > 80) or (y > 25) then exit;

        { clipping }
        if (x+blk^.w > 80) then cw := 80 - x + 1 else cw := blk^.w;
        if (y+blk^.h > 25) then ch := 25 - y + 1 else ch := blk^.h;

	blk^.x	:= x;
        blk^.y	:= y;
        q := blk^.data;

{$ifndef FPC}
        a := char(attr);
        r := 160 - cw shl 1;
        p := ltx_bit + ltx_offset(x, y);
        if not trans then               { opaque blitting }
        begin
	        for j := 1 to ch do
        	begin
        		for i := 1 to cw do
	                begin
		                p^ := (q+i-1)^;
                	        (p+1)^ := a;
                        	inc(p, 2);
	                end;
			inc(p, r);
                	inc(q, blk^.w);
                end;
        end
        else
        begin                           { transparent blitting }
	        for j := 1 to ch do
        	begin
        		for i := 1 to cw do
                        begin
                        	v := (q+i-1)^;
                        	if v <> #0 then
                                begin
			                p^ := v;
        	        	        (p+1)^ := a;
                                end;
				inc(p, 2);
	                end;
			inc(p, r);
                	inc(q, blk^.w);
	        end;
        end;
{$else}
        if not trans then		{ opaque blitting }
        begin
	        for j := 1 to ch do
        	for i := 1 to cw do
                begin
                        printc(x+i-1, y+j-1, attr, q^);
                        inc(q);
                end;
        end
        else
        begin                           { transparent blitting }
	        for j := 1 to ch do
        	for i := 1 to cw do
                begin
                        if q^ <> #0 then printc(x+i-1, y+j-1, attr, q^);
                        inc(q);
                end;
        end;
{$endif}
end;

{ save a text-based bit block to disk }
function	block_save(blk: PBLOCK; filename: string): boolean;
var	f	: FILE;
begin
        assign(f, filename);
	{$I-}
        rewrite(f, 1);
        {$I+}
        if ioresult <> 0 then
        begin
                block_save := FALSE;
                exit;
        end;
        blockwrite(f, blk^.w, sizeof(word));
        blockwrite(f, blk^.h, sizeof(word));
        blockwrite(f, blk^.data^, blk^.size);
        close(f);
        block_save := TRUE;
end;

{ load a text-based bit block from disk }
function	block_load(filename: string): PBLOCK;
var	blk     : PBLOCK;
	f	: FILE;
	w, h	: word;
begin
	assign(f, filename);
        {$I-}
        reset(f, 1);
        if ioresult <> 0 then
        begin
        	block_load := nil;
                exit;
        end;
        {$I+}
        blockread(f, w, sizeof(word));
        blockread(f, h, sizeof(word));
        blk := block_create(w, h);
        if (blk = nil) then
        begin
        	dispose(blk);
	        close(f);
                block_load := nil;
                exit;
        end;
        blockread(f, blk^.data^, blk^.size);
        close(f);
        block_load := blk;
end;

{ initialize data for a text-based bit block from a specific memory block }
procedure	block_membit(blk: PBLOCK; var buffer);
begin
	if blk = nil then exit;
	move(buffer, blk^.data^, blk^.size);
end;

{ create a text-based bit block from internal resource data }
function	block_attach(buffer: pointer): PBLOCK;
var	blk	: PBLOCK;
        w, h    : word;
        p	: pchar;
begin
	p := pchar(buffer);

        w := word((p+0)^) + word((p+1)^) shl 8;	{ get width }
        inc(p, 2);

        h := word((p+0)^) + word((p+1)^) shl 8;	{ get height }
        inc(p, 2);

        blk := block_create(w, h);
        blk^.attach := TRUE;		{ notice }
        blk^.data   := p;		{ attach pointer to data }
	block_attach := blk;
end;

{ map a bitmap into text-based bit block, using singled width }
procedure	block_mapper(blk: PBLOCK; buf: pchar; c: char);
var	p, q	: pchar;
	i	: integer;
begin
	p := buf;
        q := blk^.data;
        for i := 1 to blk^.size do
        begin
               	if p^ <> #0 then
                       	q^ := c
		else	q^ := #0;
		inc(p);
        	inc(q);
        end;
end;

{ dump the video RAM onto disk }
function	screen_dump(filename: string): boolean;
var	f	: FILE;
begin
	assign(f, filename);
	{$I-}
	rewrite(f, 1);
	{$I+}
	if ioresult <> 0 then
		screen_dump := FALSE
	else
	begin
		blockwrite(f, VIDEO_RAM^, SCR_SIZE);
		close(f);
		screen_dump := TRUE;
	end;
end;

{ load the screen-dump file onto video RAM  }
function	screen_view(filename: string): boolean;
var	f	: FILE;
begin
	assign(f, filename);
	{$I-}
	reset(f, 1);
	{$I+}
	if ioresult <> 0 then
		screen_view := FALSE
	else
	begin
		blockread(f, VIDEO_RAM^, SCR_SIZE);
		close(f);
		screen_view := TRUE;
	end;
end;

{ display an error message and terminate the application }
procedure	die(code: integer; msg: string);
begin
	writeln(msg);
	halt(code);
end;

{ construct a version string }
function        vtos(v: longint; b: string): string;
var	tmp	: string;
begin
	tmp :=  itoa((v shr 8)) + '.' +
		itoa((v and $FF)) + '.' + b;
	vtos := tmp;
end;

end.
