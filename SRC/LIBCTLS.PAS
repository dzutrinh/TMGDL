{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   LIBCTLS.PAS                                                              }
{   -----------                                                              }
{   Library for text-based common controls                                   }
{   Version 1.1                                                              }
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
{   Available controls:                                                      }
{   -------------------                                                      }
{   . Label             | . About dialog       | . Color picker              }
{   . Life counter      | . Life counter       | . Generic counter           }
{   . Menu pop-up       | . Menu pull-down     | . Help box                  }
{   . Progress bar      | . Input box          | . Drive picker              }
{   . List view/picker  | . Scroll bar         | . Log view                  }
{   . ASCII char map    |                                                    }
{                                                                            }
{ ////////////////////////////////////////////////////////////////////////// }

{$i preps.inc}

unit	libctls;

interface

uses	libtext;

const	LIBCTLS_VERSION	= $0102;
	LIBCTLS_BUILD	= '0714';

{ //////////////////////////////// LABEL API /////////////////////////////// }

type    PLABEL = ^LABEL_TYPE;
	LABEL_TYPE = record
		x, y	: longint;
		w	: word;
                autox,
		show	: boolean;
		text	: string;
	end;

function	labelCreate(x, y, w: integer; text: string): PLABEL;
procedure	labelDestroy(var lb: PLABEL);
procedure       labelShow(var lb: PLABEL);
procedure	labelHide(var lb: PLABEL);
procedure	labelSetText(var lb: PLABEL; text: string);

{ //////////////////////////// COLOR PICKER API //////////////////////////// }

type	COLOR_PICKER	= (CPF_ALL, CPF_BACK);

function        colorPicker(flag: COLOR_PICKER;
			    x, y, default: integer;
			    caption: string): integer;

{ ////////////////////////////// ABOUT BOX API ///////////////////////////// }

procedure	aboutBox(x, y: integer; text: string);

{ /////////////////////////// CHARACTER MAP API //////////////////////////// }

function	characterMap(x, y: integer; title: string): integer;

{ ////////////////////////////// INPUT BOX API ///////////////////////////// }

function        inputBox(x, y, w, a: integer;
			 shade: SHADING; itext, title: string): string;

{ //////////////////////////// DRIVE PICKER API //////////////////////////// }

function        drivePicker(x, y, norm, hilight: integer;
			    shade: SHADING; title: string): string;

{ //////////////////////////// TEXT MARQUEE API //////////////////////////// }

procedure	marqueeText(x, y, w, attr, speed: integer; text: string);

{ ///////////////////////////// SCROLLBAR API ////////////////////////////// }

type    SCROLLBAR_STYLE = (SCS_VERT, SCS_HORZ);
	SCROLLBAR_CHARS	= array[1..4] of char;
	PSCROLLBAR	= ^SCROLLBAR_TYPE;
	SCROLLBAR_TYPE	= record
                x, y,
                len	: integer;
                value,
                min,
                max	: longint;
                shade	: SHADING;
                chars	: SCROLLBAR_CHARS;
                style	: SCROLLBAR_STYLE;
	end;

const	VSCROLL_CHARS	: SCROLLBAR_CHARS = #179#186#30#31;
	HSCROLL_CHARS	: SCROLLBAR_CHARS = #196#205#17#16;

function	scrollbarCreate(style: SCROLLBAR_STYLE; shade: SHADING;
			        x, y, len, min, max: longint): PSCROLLBAR;
procedure	scrollbarDestroy(var scr: PSCROLLBAR);
procedure	scrollbarSetRange(var scr: PSCROLLBAR; min, max: longint);
procedure	scrollbarSetValue(var scr: PSCROLLBAR; value: longint);
procedure	scrollbarUpdateValue(var scr: PSCROLLBAR; value: longint);
procedure	scrollbarDraw(const scr: PSCROLLBAR);
function	scrollbarMin(const scr: PSCROLLBAR): longint;
function	scrollbarMax(const scr: PSCROLLBAR): longint;
function	scrollbarValue(const scr: PSCROLLBAR): longint;

{ /////////////////////////////// SLIDER API /////////////////////////////// }

type	PSLIDER = ^SLIDER_TYPE;		{ represent a slider }
	SLIDER_TYPE = record
		title	: string;       { title of the slider }
                sx, sy,
		x, y	: longint;	{ coordinates }
		w, h	: integer;	{ width }
                shades	: SHADING;	{ slider shading }
		frame	: boolean;	{ has frame flag }
		{ private attributes }
		fwidth	: integer;	{ fill width }
                scroll	: PSCROLLBAR;	{ scrollbar object }
	end;

function	sliderCreate(title: string; frame: boolean;
			     min, max, x, y, len: integer;
			     shades: SHADING): PSLIDER;
procedure	sliderDestroy(var sld: PSLIDER);
procedure	sliderSetValue(sld: PSLIDER; value: longint);
procedure	sliderDraw(const sld: PSLIDER);
function	sliderExec(sld: PSLIDER; shade: SHADING): integer;

{ //////////////////////////////// MENU API //////////////////////////////// }

const	MENU_MAX	= 20;		{ maximum menu entries allowed }

type    { type of menus }
	MENU_STYLE	= (MSF_POP, MSF_PULL);

	{ exposed menu type }
	PMENU		= ^MENU_TYPE;   { represent a menu, both pull-down }

	{ core menu types }
	MENU_ITEM	= record	{ and pop-up style }
		name	: string[70];	{ item name }
		cmd	: word;		{ item command ID }
                sep	: boolean;	{ separator }
		case pop: boolean of	{ TRUE if there's a sub-menu }
		TRUE	: (sub: PMENU);	{ pointer to the sub-menu }
	end;
	MENU_LIST	= array[1..MENU_MAX] of MENU_ITEM;
	MENU_TYPE	= record
		list	: MENU_LIST;	{ list of menu items }
		{ private attributes }
		last,			{ last selection }
		size	: byte;		{ total items }
	end;

const   { internal used menu flags }
	MENU_PULLED	: boolean = FALSE;
	MENU_PULL_LEFT	: boolean = FALSE;
	MENU_PULL_RIGHT	: boolean = FALSE;

function	menuCreate: PMENU;
procedure	menuDestroy(var m: PMENU);
procedure	menuAppend (m: PMENU; name: string; cmd: word;
			    pop: boolean; sub: PMENU);
procedure	menuModify (m: PMENU; id: word;
			    name: string; pop: boolean; sub: PMENU);
procedure	menuSeparator(m: PMENU);

function	menuPopup(m: PMENU; shade: SHADING;
			  x, y, txt, sel: integer;
			  title: string): word;
function	menuPulldown(m: PMENU; shade: SHADING;
			     x, y, txt, sel: integer;
			     title: string): word;
function	menuExec   (m: PMENU; style: MENU_STYLE;
			    shade: SHADING; x, y, txt, sel: integer;
			    title: string): word;
function	menuOnOff(state: boolean): string;

{ ////////////////////////////// LIFE-BOX API ////////////////////////////// }

type    LIFE_TYPE = record
		x, y,
		w, h	: integer;
		row	: integer;
		count	: integer;
		max	: integer;
		syma	: integer;
		lpr	: integer;
		show	: boolean;
		shade	: SHADING;
		name	: STR80;
		sym	: char;
	end;

procedure	lifeCreate(var lf: LIFE_TYPE;
			   x, y, w, row, max, syma: integer;
			   shade: SHADING;
			   name: STR80; symbol: char);
procedure       lifeShow(var lf: LIFE_TYPE);
procedure	lifeHide(var lf: LIFE_TYPE);
function	lifeLast(const lf: LIFE_TYPE): boolean;
function	lifeOver(const lf: LIFE_TYPE): boolean;
procedure	lifeLost(var lf: LIFE_TYPE);
procedure	lifeBonus(var lf: LIFE_TYPE);
function	lifeVisible(const lf: LIFE_TYPE): boolean;

{ ///////////////////////////// COUNTER-BOX API //////////////////////////// }

type    PCOUNTER = ^COUNTER_TYPE;
	COUNTER_TYPE = record
		text	: STR80;
		shade	: SHADING;
		show	: boolean;
		x, y,
		w, vw	: integer;
		base,
		value,
		min, max: longint;
	end;

function	counterCreate(ival, x, y, w: integer;
			      min, max: longint;
			      shade: SHADING; name: STR80): PCOUNTER;
procedure	counterDestroy(var cnt: PCOUNTER);
procedure	counterShow(var cnt: PCOUNTER);
procedure	counterHide(var cnt: PCOUNTER);
procedure	counterSetValue(var cnt: PCOUNTER; value: integer);
function	counterGetValue(var cnt: PCOUNTER): integer;
procedure	counterIncrease(var cnt: PCOUNTER);
procedure	counterDecrease(var cnt: PCOUNTER);
procedure	counterReset(var cnt: PCOUNTER);
procedure       counterLimitDecrease(var cnt: PCOUNTER);
procedure	counterLimitIncrease(var cnt: PCOUNTER);
procedure	counterAdd(var cnt: PCOUNTER; incr: integer);
procedure	counterAccum(var cnt: PCOUNTER; value: integer);
function	counterLast(var cnt: PCOUNTER): boolean;
function	counterVisible(cnt: PCOUNTER): boolean;

{ /////////////////////////////// HELP-BOX API ///////////////////////////// }

const   HELPTEXT_MAX	= 300;
	HELPTEXT_LEN	= 50;
	HELPWIND_W	= HELPTEXT_LEN + 5;
	HELPWIND_H	= 16;
	HELPCLNT_W	= HELPTEXT_LEN;
	HELPCLNT_H	= HELPWIND_H - 2;

type    HELPSTR	= STR128;
	HELPTEXT = array[1..HELPTEXT_MAX] of HELPSTR;

	PHELPBOX = ^HELPBOX_TYPE;
	HELPBOX_TYPE = record
		x, y	: integer;
		title	: string[20];
		text	: HELPTEXT;
		lines	: integer;
	end;

function        helpboxCreate(source, title: string; x, y: integer): PHELPBOX;
procedure	helpboxExec(hb: PHELPBOX);
procedure       helpboxDestroy(var hb: PHELPBOX);

{ //////////////////////////////// GAUGE API /////////////////////////////// }

const   GSF_HAS_BORDER	= $0001;
        GSF_HAS_PERCENT	= $0002;
        GSF_DEFAULT	= GSF_HAS_BORDER + GSF_HAS_PERCENT;

        GLV_LIMIT_LOW	: real = 0.5;
        GLV_LIMIT_HIGH	: real = 0.8;

        GLC_CHARSET	: array[1..3] of char = #219#221#249;

type    PGAUGE = ^GAUGE_TYPE;
	GAUGE_TYPE = record
                x, y,
		w, h	: integer;
        	shadef,
		shadeg	: SHADING;
                cval,
                min, max: longint;
                style	: word;
                title	: STR80;

                { private }
                gw	: integer;
                gx, gy	: integer;
                r_low,
		r_hi	: longint;
                ival,
		diff	: longint;
                pct	: real;
	end;

function	gaugeCreate(ival, x, y, w, min, max, style: longint;
			    shadef, shadeg: SHADING; title: string): PGAUGE;
procedure	gaugeDestroy(var gg: PGAUGE);
procedure	gaugeDraw(var gg: PGAUGE);
procedure	gaugeHide(var gg: PGAUGE);
procedure	gaugeShow(var gg: PGAUGE);
procedure	gaugeSetValue(var gg: PGAUGE; value: longint);
function	gaugeGetValue(var gg: PGAUGE): longint;
procedure	gaugeDecrease(var gg: PGAUGE);
procedure	gaugeIncrease(var gg: PGAUGE);
procedure	gaugeAdd(var gg: PGAUGE; value: longint);
procedure	gaugeSub(var gg: PGAUGE; value: longint);
procedure	gaugeReset(var gg: PGAUGE);

{ ////////////////////////////// LIST VIEW API ///////////////////////////// }

type	LISTBOX_MODE	= (LVM_PICK, LVM_VIEW);
	PLISTBOX	= ^LISTBOX_TYPE;

	LISTBOX_ITEM_FUNC	= function(item: longint): string;
        LISTBOX_DRAW_PROC	= procedure(var lv: PLISTBOX);

	LISTBOX_TYPE	= record
                norm,
		active	: byte;
                x, y,
                w, h,
                count,
                range,
                citem,
                delta,
                last	: longint;
                mode	: LISTBOX_MODE;
                chd	: LISTBOX_DRAW_PROC;
                chi	: LISTBOX_ITEM_FUNC;
        end;

function	listCreate(mode: LISTBOX_MODE; x, y, w, h, nrm, sel, count: integer): PLISTBOX;
procedure	listDestroy(var lv: PLISTBOX);
function	listCurrent(const lv: PLISTBOX): longint;
procedure	listResize(var lv: PLISTBOX; count: longint);
procedure	listHandleEvent(var lv: PLISTBOX; key: word);
function	listGetString(i: longint): string;
procedure       listDraw(var lv: PLISTBOX);
function	listExec(var lv: PLISTBOX): longint;

procedure	listHandleDraw(var lv: PLISTBOX; handler: LISTBOX_DRAW_PROC);
procedure	listHandleItem(var lv: PLISTBOX; handler: LISTBOX_ITEM_FUNC);

{ /////////////////////////////// LOGVIEW API ////////////////////////////// }

type    PLOGVIEW = ^LOGVIEW_TYPE;
	LOGVIEW_TYPE = record
                x, y,
                w, h	: integer;
                attr	: byte;
                count	: integer;
                log	: array[0..MAX_DLG_LINE-1] of STR80;
	end;

function	logviewCreate(x, y, w, h, attr: integer): PLOGVIEW;
procedure	logviewDestroy(var log: PLOGVIEW);
procedure	logviewDraw(const log: PLOGVIEW);
procedure	logviewAdd(var log: PLOGVIEW; text: STR80);

implementation

uses	libtmgd, libutil;

{ ////////////////////////////////////////////////////////////////////////// }

{ //////////////////////////////// LABEL API /////////////////////////////// }

function	labelCreate(x, y, w: integer; text: string): PLABEL;
var	lb	: PLABEL;
begin
	new(lb);
	lb^.w	 := w;
	lb^.text := text;
	lb^.show := FALSE;

	if x < 0 then
	begin
		x := ltx_centerx(w);
                lb^.autox := TRUE;
        end
        else	lb^.autox := FALSE;

	if y < 0 then	y := ltx_centery(1);

	lb^.x	 := x;
	lb^.y	 := y;

        labelCreate := lb;
end;

procedure	labelDestroy(var lb: PLABEL);
begin
	dispose(lb);
end;

procedure       labelShow(var lb: PLABEL);
var	tmp	: string;
begin
        if lb = nil then exit;

	lb^.show := TRUE;
        prints(lb^.x, lb^.y, (CLEAR_COLOR shl 4) or 0, rpad(' ', lb^.w));
        if lb^.autox then
        	tmp := make_center(lb^.text, lb^.w)
        else	tmp := lb^.text;
	print (lb^.x, lb^.y, tmp);
end;

procedure	labelHide(var lb: PLABEL);
begin
        if lb = nil then exit;
	lb^.show := FALSE;
	prints(lb^.x, lb^.y, (CLEAR_COLOR shl 4) or 0, repl(' ', lb^.w));
end;

procedure	labelSetText(var lb: PLABEL; text: string);
var	vis	: boolean;
begin
        if lb = nil then exit;

	vis := lb^.show;
	if not vis then labelHide(lb);
	lb^.text := text;
	if lb^.show then labelShow(lb);
end;

{ //////////////////////////// COLOR PICKER API //////////////////////////// }

function        colorPicker(flag: COLOR_PICKER; x, y, default: integer; caption: string): integer;
const   CW	: byte = 3;		{ cell width, should be odd and > 1 }
	CH	: byte = 1;		{ cell height, should be 1          }
        CPR	: byte = 4;		{ colors per row, must be 4         }
        CPC	: byte = 4;		{ colors per column, can be 2 or 4  }

var	c, r,                  	 	{ column, row indices         }
	k,				{ current selection           }
	w, h	: integer;		{ width and height of the box }
        done	: boolean;		{ loop control flag           }
        key	: word;			{ input key                   }
	cell	: string[13];		{ for storing a color cell    }
        dlg	: PDIALOG;

        { draw the color palette }
        procedure	board(k: integer);
        var	i, j,			{ col, row indices         }
		c, a	: integer;	{ current colors           }
        begin
        	c := 0;			{ starting from color 0    }
        	for j := 0 to CPC-1 do	{ 2 or 4 colors per column }
	        for i := 0 to CPR-1 do 	{ 4 colors per row         }
        	begin
                	{ invert color if it is black              }
                	if c = 0 then a := $70 else a := c;
                	if c = k then	{ highlight current color  }
                        	cell[CW shr 1 + 1] := #8
                        else	cell[CW shr 1 + 1] := cell[1];

                        { draw the color block }
                	prints(x+3+(i*CW), y+2+(j*CH), a, cell);
	                inc(c);		{ next color               }
        	end;
        	prints(x+3, y+CPC+2, $07, rpad(COLOR_NAMES[k], 12));
        end;

begin
	{ CPF_ALL: 4 color rows, CPF_BACK: 2 color rows }
        if flag = CPF_ALL then CPC := 4 else CPC := 2;
	w := CPR * CW + 6;		{ calculate the box width  }
	h := CPC * CH + 4;		{ calculate the box height }

        cell := repl(#219, CW);

        { centering the box if needed }
        ltx_validate_rect(x, y, w, h);

        { k points to the default color if needed }
        if (default < 0) or (default > 15) then k := 0 else k := default;
        r := k div CPR;			{ calculate the column and    }
        c := k mod CPR;			{ row indices for selection   }

        done := FALSE;
        dlg := dialog_open(x, y, w, h, SHADE_WHITE, caption, '');
        repeat
        	k := r * CPR + c;	{ current color selection     }
        	board(k);		{ draw the palette            }
                key := getkey;		{ get an input key event      }
                { handle the keys }
                case key of
                KEY_LEFT:	if c > 0 then dec(c) else c := CPR-1;
                KEY_RIGHT:	if c < CPR-1 then inc(c) else c := 0;
                KEY_UP:		if r > 0 then dec(r) else r := CPC-1;
                KEY_DOWN:	if r < CPC-1 then inc(r) else r := 0;
                KEY_ESC:	begin
                			{ return default if ESC is pressed }
                        		done := TRUE;
                                	k    := default;
                        	end;
                KEY_ENTER:	done := TRUE;
                end;
        until done;
        dialog_close(dlg);
        colorPicker := k;		{ return the current color    }
end;

{ ////////////////////////////// ABOUT BOX API ///////////////////////////// }

{ display an about box }
procedure	aboutBox(x, y: integer; text: string);
var	w, h, t,
	mlen,
	lines	: integer;
        list	: array[1..MAX_DLG_LINE] of STR80;
        save	: PREGION;
        oldscr	: SCREEN;
begin
        { extracts each line into a list }
        lines  := 0;
        mlen   := 0;
        while (text <> '') and (lines < MAX_DLG_LINE) do
        begin
        	inc(lines);
                list[lines] := tokenize(text, ENDL);
                t := text_length(list[lines]);  { determine max length to  }
                if mlen < t then mlen := t;	{ compute the dialog width }
        end;

        { setting up the dialog }
        w := ltx_max(mlen + 8, 32);
        h := lines + 8;
        ltx_validate_rect(x, y, w, h);

        oldscr := vscreen_activate(vscreen_vram);
        save := region_get(x, y, w, h);
        shaded_box(x, y, w, h, SHADE_WHITE, AF_LEFT, '');

        { display the given texts }
        t := 1;
        while t <= lines do
        begin
        	print(x + 4, y + t + 1, list[t]);
                inc(t);
        end;

        { plus game version and build }
        mlen := w - 8;
        print(x + 4, y + t + 2, repl(#196, mlen));
        print(x + 4, y + t + 3, #7' VERSION      ~0F'+vtos(GAME_VERSION, GAME_BUILD));
        print(x + 4, y + t + 4, #7' CORE LIBRARY ~0F'+ltx_version);

	{ wait for user inputs }
        wait([#13, #27]);

        region_put(save);
        vscreen_activate(oldscr);
end;

{ /////////////////////////// CHARACTER MAP API //////////////////////////// }

const	CHARMAP_LASTSEL	: integer = -1;

function	characterMap(x, y: integer; title: string): integer;
const	BRD_W	= 36;
	BRD_H	= 14;
	CLNT_W	= 32;
	CLNT_H	= 8;
        C_NORM	= $08;
        C_SEL	= $0F;

var	sx, sy,
	cx, cy,
	cc, i, j: integer;
	quit	: boolean;
        oldc,
	key	: word;
	text	: string;
        dlg	: PDIALOG;

        procedure       update(act: boolean);
        begin
		cc := cy * CLNT_W + cx;
		text := '~0FCHR: ~07[~0B'+chr(cc)          +'~07]  '+
			'~0FHEX: ~07[~0E'+tohex(cc, 2)     +'~07]  '+
			'~0FDEC: ~07[~0D'+lpad(itoa(cc), 3)+'~07]';
		print(x + 3, y + 11, text);

		gotoxy(sx + cx, sy + cy);
                if act then
	                ltx_seta(sx + cx, sy + cy, C_SEL)
                else	ltx_seta(sx + cx, sy + cy, C_NORM);
        end;

begin
        ltx_validate_rect(x, y, BRD_W, BRD_H);
	sx	:= x + 2;
	sy	:= y + 2;
	quit	:= FALSE;
        oldc	:= cursor_shape;
	dlg	:= dialog_open(x, y, BRD_W, BRD_H, SHADE_WHITE, title, '');
        if CHARMAP_LASTSEL <> -1 then
	begin
		cc := CHARMAP_LASTSEL;
                cx := CHARMAP_LASTSEL mod CLNT_W;
                cy := CHARMAP_LASTSEL div CLNT_W;
	end
	else
	begin
		cc := 0;
	        cx := 0;
        	cy := 0;
        end;
	cursor(CURSOR_ON);
	for i := 0 to CLNT_H-1 do
	for j := 0 to CLNT_W-1 do
		prints(sx+j, sy+i, C_NORM, char(i * CLNT_W + j));
	repeat
                update(TRUE);
		key := getkey;
                update(FALSE);

		case key of
		KEY_A,
		KEY_LEFT:	if cx > 0 then dec(cx);
		KEY_D,
		KEY_RIGHT:	if cx < CLNT_W-1 then inc(cx);
		KEY_W,
		KEY_UP:		if cy > 0 then dec(cy);
		KEY_S,
		KEY_DOWN:	if cy < CLNT_H-1 then inc(cy);
		KEY_CTRL_LEFT:	cx := 0;
		KEY_CTRL_RIGHT:	cx := CLNT_W-1;
		KEY_CTRL_UP:	cy := 0;
		KEY_CTRL_DOWN:	cy := CLNT_H-1;
                KEY_ENTER,
		KEY_ESC:	quit := TRUE;
		end;
	until quit;
        cursor(oldc);
        dialog_close(dlg);
        if key = KEY_ESC then cc := -1 else CHARMAP_LASTSEL := cc;
        characterMap := cc;
end;

{ ////////////////////////////// INPUT BOX API ///////////////////////////// }

{ open an box for text inputing }
function        inputBox(x, y, w, a: integer; shade: SHADING; itext, title: string): string;
var	bw, h	: integer;
        dlg	: PDIALOG;
begin
        if w < 2 then w := 2;
	bw := w + 4;
        h  := 5;
        if length(title) >= w then title := '';
        ltx_validate_rect(x, y, bw, h);
        dlg := dialog_open(x, y, bw, h, shade, title, '');
        inputBox := inputs(x + 2, y + 2, w, a, itext);
        dialog_close(dlg);
end;

{ //////////////////////////// DRIVE PICKER API //////////////////////////// }

var	DRIVE_LIST	: record
		list	: array[1..26] of record
	        	drive	: char;
			free,
                	size	: longint;
                end;
                count	: integer;
	end;

procedure	drivePopulate;
var	i, f, s	: longint;
begin
	DRIVE_LIST.count := 0;
        for i := 1 to 26 do
	begin
        	f := drive_free(i);
	        if f >= 0 then
        	begin
                	inc(DRIVE_LIST.count);
                        s := drive_size(i);
                        DRIVE_LIST.list[DRIVE_LIST.count].drive := chr(i + 64);
                        DRIVE_LIST.list[DRIVE_LIST.count].free  := f;
                        DRIVE_LIST.list[DRIVE_LIST.count].size  := s;
        	end;
        end;
end;

function	driveGetItemString(i: longint): string;
begin
	with DRIVE_LIST do
	        driveGetItemString := 	'  '+list[i].drive+': '+
                                	lpad(unitize(list[i].free), 12) + ' '+
                                        lpad(unitize(list[i].size), 12);
end;

function        drivePicker(x, y, norm, hilight: integer; shade: SHADING; title: string): string;
var	w, h	: integer;
	dlg	: PDIALOG;
        lpicker	: PLISTBOX;
        ret	: longint;

begin
	drivePicker := '';
        w   := ltx_max(length(title), 35);
        h   := 8;
	ltx_validate_rect(x, y, w, h);
	drivePopulate;
        if DRIVE_LIST.count = 0 then
        begin
                info(SHADE_RED, 'NO DRIVE AVAILABLE');
                listDestroy(lpicker);
                exit;
        end;

        dlg := dialog_open(x, y, w, h, SHADE_WHITE, title, '');
        prints(x + 1, y + 1, shade[SHADE_BRIGHT], '  DISK'+lpad('FREE', 8)+lpad('SIZE', 12));

        lpicker := listCreate(LVM_PICK, x + 1, y + 2, w - 1, h-3, norm, hilight, DRIVE_LIST.count);
	listHandleItem(lpicker, driveGetItemString);
        ret := listExec(lpicker);
        listDestroy(lpicker);
        dialog_close(dlg);
        if ret <> -1 then drivePicker := DRIVE_LIST.list[ret].drive+':';
end;

{ //////////////////////////// TEXT MARQUEE API //////////////////////////// }

{ run a text marquee until a key is hit }
procedure	marqueeText(x, y, w, attr, speed: integer; text: string);
var	tmp	: string;
	st, at,
        len, i	: integer;
begin
	if text = '' then exit;
	if (w < 1) then w := 1;
        if (w > 80) then w := 80;
	st  := 0;
        len := length(text);
        repeat
                tmp := '';
		for i := 0 to w-1 do
                begin
                	at := (st + i) mod len;
                        tmp := tmp + text[at+1];
                end;
                inc(st);
	        prints(x, y, attr, tmp);
                sleep(speed);
        until keypressed;
end;

{ ///////////////////////////// SCROLLBAR API ////////////////////////////// }

function	scrollbarCreate(style: SCROLLBAR_STYLE; shade: SHADING;
			        x, y, len, min, max: longint): PSCROLLBAR;
var     scr	: PSCROLLBAR;
	t	: longint;
begin
	if max < min then
        begin
                t   := max;
                max := min;
                min := t;
        end;

        new(scr);
        scr^.x       := x;
        scr^.y	     := y;
        scr^.len     := len;
        scr^.min     := min;
        scr^.max     := max;
        scr^.value   := 0;
        scr^.style   := style;
        scr^.shade   := shade;

        if style = SCS_VERT then
		scr^.chars := VSCROLL_CHARS
        else	scr^.chars := HSCROLL_CHARS;

        scrollbarCreate := scr;
end;

procedure	scrollbarDestroy(var scr: PSCROLLBAR);
begin
	dispose(scr);
end;

procedure	scrollbarSetRange(var scr: PSCROLLBAR; min, max: longint);
begin
        if scr = nil then exit;
	scr^.min := min;
        scr^.max := max;
end;

procedure	scrollbarSetValue(var scr: PSCROLLBAR; value: longint);
begin
	if (value >= scr^.min) and (value <= scr^.max) then
		scr^.value := value;
end;

procedure	scrollbarUpdateValue(var scr: PSCROLLBAR; value: longint);
begin
	scrollbarSetValue(scr, value);
        scrollbarDraw(scr);
end;

procedure	scrollbarDraw(const scr: PSCROLLBAR);
var     start, stop, i,
	fill, d, s, c	: integer;
begin
	fill  := scr^.len - 2;
        s     := scr^.value - scr^.min;
        d     := scr^.max - scr^.min;
        if d = 0 then d := 1;
	if scr^.style = SCS_VERT then
        begin
	        c     := s*(fill-1) div d;
	        start := scr^.y;
                stop  := start + fill + 1;
        	prints(scr^.x, start    , scr^.shade[SHADE_MEDIUM], scr^.chars[3]);
        	prints(scr^.x, stop     , scr^.shade[SHADE_MEDIUM], scr^.chars[4]);
                for i := 0 to fill-1 do
                	prints(scr^.x, start+i+1, scr^.shade[SHADE_DARK], scr^.chars[1]);
		prints(scr^.x, start+c+1, scr^.shade[SHADE_BRIGHT], scr^.chars[2]);
        end
        else
        begin
	        c     := (s*(fill-2) div d);
	        start := scr^.x;
        	stop  := start + fill + 1;
                prints(start    , scr^.y, scr^.shade[SHADE_MEDIUM], scr^.chars[3]);
                prints(stop     , scr^.y, scr^.shade[SHADE_MEDIUM], scr^.chars[4]);
                prints(start+1  , scr^.y, scr^.shade[SHADE_DARK], repl(scr^.chars[1], fill));
		prints(start+c+1, scr^.y, scr^.shade[SHADE_BRIGHT], scr^.chars[2]+scr^.chars[2]);
        end;
end;

function	scrollbarMin(const scr: PSCROLLBAR): longint;
begin
        scrollbarMin := scr^.min;
end;

function	scrollbarMax(const scr: PSCROLLBAR): longint;
begin
        scrollbarMax := scr^.max;
end;

function	scrollbarValue(const scr: PSCROLLBAR): longint;
begin
        scrollbarValue := scr^.value;
end;

{ /////////////////////////////// SLIDER API /////////////////////////////// }

const	SLIDER_DEFWD	= 10;
	SLIDER_DEFHT	= 5;

function	sliderCreate(title: string; frame: boolean;
			     min, max, x, y, len: integer;
			     shades: SHADING): PSLIDER;
var	sld	: PSLIDER;
begin
	if ltx_memavail < sizeof(SLIDER_TYPE) then
	begin
		sliderCreate := nil;
		exit;
	end;
	
	if (max < 2) then exit;

	len := ltx_max(len, SLIDER_DEFWD);
        ltx_validate_rect(x, y, len, SLIDER_DEFHT);

	new(sld);
	sld^.title	:= title;
	sld^.x		:= x;
	sld^.y		:= y;
	sld^.w		:= len;
        sld^.shades	:= shades;
	sld^.frame	:= frame;

	if frame then
	begin
		sld^.sx := x + 2; sld^.sy := y + 1;
                sld^.fwidth := len-4;
                sld^.h := SLIDER_DEFHT;
	end
	else
	begin
		sld^.sx := x + 1; sld^.sy := y + 1;
                sld^.fwidth := len-2;
                sld^.h := 3;
	end;

        sld^.scroll	:= scrollbarCreate(SCS_HORZ, shades,
					   sld^.sx, sld^.sy+1, sld^.fwidth,
					   min, max);
	sliderCreate	:= sld;
end;

procedure	sliderDestroy(var sld: PSLIDER);
begin
	dispose(sld);
end;

procedure	sliderDraw(const sld: PSLIDER);
begin
	with sld^ do
	begin
		vsync;
		prints(	sx, sy, shades[SHADE_BRIGHT], make_center(title, fwidth));
		prints(	sx, sy+2, shades[SHADE_BRIGHT], make_center(ltoa(scrollbarValue(scroll)), fwidth));
		scrollbarDraw(scroll);
	end;
end;

procedure	sliderSetValue(sld: PSLIDER; value: longint);
begin
        scrollbarSetValue(sld^.scroll, value);
end;

function	sliderExec(sld: PSLIDER; shade: SHADING): integer;
var	done	: boolean;
	key	: word;
        dlg	: PDIALOG;
        min,
        max,
        step,
	current	: longint;
begin
	if sld^.frame then
	        dlg := dialog_open(sld^.x, sld^.y, sld^.w, sld^.h, shade, '', '');
	done    := FALSE;
        step    := scrollbarMax(sld^.scroll) div 10;
        current := scrollbarValue(sld^.scroll);
        min     := scrollbarMin(sld^.scroll);
        max	:= scrollbarMax(sld^.scroll);
        sliderDraw(sld);
	repeat
		sliderSetValue(sld, current);
                sliderDraw(sld);
		key := getkey;

		with sld^ do
		case key of
		KEY_ESC,
		KEY_ENTER:	done := TRUE;
		KEY_LEFT:	if current > min then dec(current);
		KEY_RIGHT:      if current < max then inc(current);
		KEY_HOME:	current := min;
		KEY_END:	current := max;
		KEY_CTRL_RIGHT:	begin
					if current < max then inc(current, step);
					if current > max then current := max;
				end;
		KEY_CTRL_LEFT:	begin
					if current > min then dec(current, step);
					if current < min then current := min;
				end;
		end;
	until done;
        if sld^.frame then dialog_close(dlg);
	if key = KEY_ESC then sliderExec := -1 else sliderExec := current;
end;

{ //////////////////////////////// MENU API //////////////////////////////// }

{ create a menu object }
function	menuCreate: PMENU;
var	i	: byte;
	m	: PMENU;
begin
	if ltx_memavail < sizeof(MENU_TYPE) then
	begin
		menuCreate := nil;
		exit;
	end;
	
	new(m);
	m^.size := 0;
	m^.last := 0;
	for i := 1 to MENU_MAX do
	begin
		m^.list[i].name := '';
		m^.list[i].cmd	:= 0;
		m^.list[i].pop  := FALSE;
	end;
	menuCreate := m;
end;

{ destroy a menu object }
procedure	menuDestroy(var m: PMENU);
begin
	dispose(m);
end;

{ add a new item into the menu }
procedure	menuAppend(m: PMENU; name: string; cmd: word; pop: boolean; sub: PMENU);
begin
	if m^.size < MENU_MAX then
	begin
		inc(m^.size);
		m^.list[m^.size].name := name;
		m^.list[m^.size].cmd  := cmd;
		m^.list[m^.size].pop  := pop;
                m^.list[m^.size].sep  := FALSE;
		if pop then
		begin
			m^.list[m^.size].sub  := sub;
		end;
	end;
end;

{ modify an existed menu item }
procedure	menuModify(m: PMENU; id: word; name: string; pop: boolean; sub: PMENU);
var	p, i	: integer;
	found	: boolean;
begin
	i     := 1;
	found := FALSE;
	while (i <= m^.size) and not found do
	begin
		if id = m^.list[i].cmd then
		begin
			found := TRUE;
			p := i;
		end;
		inc(i);
	end;
	if found then
	begin
		m^.list[p].name := name;
		m^.list[p].pop  := pop;
		m^.list[p].sub  := sub;
	end;
end;

procedure	menuSeparator(m: PMENU);
begin
	if m^.size < MENU_MAX then
	begin
		inc(m^.size);
		m^.list[m^.size].name := '-';
		m^.list[m^.size].cmd  := 0;
		m^.list[m^.size].pop  := FALSE;
                m^.list[m^.size].sep  := TRUE;
	end;
end;

{ execute a menu pop-up, return the command ID of the selected item }
function	menuPopup(m: PMENU; shade: SHADING; x, y, txt, sel: integer; title: string): word;

var	w, h,
	iw, k	: integer;
	quit	: boolean;
	id, key	: word;
        dlg	: PDIALOG;

	function	menu_maxwidth: integer;
	var	i, w, mw: integer;
	begin
		mw := 0;
		for i := 1 to m^.size do
		begin
			w := length(m^.list[i].name);
			if mw < w then mw := w;
		end;
		menu_maxwidth := mw;
	end;

	function	menu_maxheight: integer;
	begin
		menu_maxheight := m^.size;
	end;

	function	menu_haspop: boolean;
	var	value	: boolean;
		i	: byte;
	begin
		for i := 1 to m^.size do
			value := value or m^.list[i].pop;
		menu_haspop := value;
	end;

	procedure	menu_item(k: integer; act: boolean);
	var	ix, iy, c: integer;
	begin
		ix := x + 1;
		iy := y + k;
		if act then c := sel else c := txt;
                if m^.list[k].sep then
                        prints(ix, iy, shade[SHADE_MEDIUM], repl(#196, iw))
                else
		if m^.list[k].pop then
			prints(ix, iy, c, rpad(m^.list[k].name, iw-2)+#16#32)
		else	prints(ix, iy, c, rpad(m^.list[k].name, iw));
	end;

	procedure	menu_table;
	var	i	: integer;
	begin
		for i := 1 to m^.size do
			menu_item(i, false);
	end;

        procedure	move_home;
        begin
		k := 1;
                while (k < m^.size) and m^.list[k].sep do inc(k);
        end;

        procedure	move_end;
        begin
		k := m^.size;
                while (k > 0) and m^.list[k].sep do dec(k);
        end;

        procedure       move_next;
        begin
                if k < m^.size then
                begin
	                while (k < m^.size) and (m^.list[k+1].sep) do inc(k);
                        inc(k);
	                if k >= m^.size then move_end;
                end
                else	move_home;
	end;

        procedure	move_prev;
        begin
		if k > 1 then
		begin
			while (k > 1) and m^.list[k-1].sep do dec(k);
			dec(k);
                        if k <= 1 then move_home;
		end
		else	move_end;
	end;

begin
	if title <> '' then
		w := ltx_max(text_length(title) + 2, menu_maxwidth)
	else	w := menu_maxwidth;
	inc(w, 4);
	if menu_haspop then inc(w, 2);
	iw := w - 2;
	h  := menu_maxheight + 2;

        ltx_validate_rect(x, y, w, h);

        dlg  := dialog_open(x, y, w, h, shade, title, '');
	quit := FALSE;
	id   := 0;
	if m^.last <> 0 then k := m^.last else move_home;
	menu_table;
	repeat
		MENU_PULL_LEFT := FALSE;
		MENU_PULL_RIGHT := FALSE;

		menu_item(k, TRUE);
		key := getkey;
		menu_item(k, FALSE);
		case key of
		KEY_ESC:	begin
					quit := TRUE;
					id := 0;
				end;
		KEY_ENTER:	begin
					if m^.list[k].pop then
					begin
						id := menuPopup(m^.list[k].sub, shade, x+4, y+k+1, txt, sel, '');
						if id <> 0 then quit := TRUE;
					end
					else
					begin
						id := m^.list[k].cmd;
						quit := TRUE;
					end;
				end;
		KEY_UP:         move_prev;
		KEY_DOWN:	move_next;
		KEY_HOME:	move_home;
		KEY_END:        move_end;
		KEY_LEFT:	if MENU_PULLED then
				begin
					MENU_PULL_LEFT := TRUE;
					quit := TRUE;
					id := 0;
				end;
		KEY_RIGHT:	if MENU_PULLED then
				begin
					MENU_PULL_RIGHT := TRUE;
					quit := TRUE;
					id := 0;
				end;
		end;
	until quit;
	m^.last := k;
        dialog_close(dlg);
	menuPopup := id;
end;

{ execute a menu pull-down, return the command ID of the selected item }
function	menuPulldown(m: PMENU; shade: SHADING; x, y, txt, sel: integer; title: string): word;
var	mx, my,
	w, h,
	k	: integer;
	quit    : boolean;
	key,
	sid, id	: word;
	r	: PREGION;
	oldscr	: SCREEN;

	function	menu_pos(k: integer): integer;
	var	i, p	: integer;
	begin
		p := 0;
		for i := 1 to k-1 do
			inc(p, length(m^.list[i].name));
		menu_pos := x + p;
	end;

	procedure	menu_item(k: integer; act: boolean);
	var	ix, iy, c: integer;
	begin
		ix := mx + menu_pos(k);
		iy := my;
		if act then c := sel else c := txt;
		prints(ix, iy, c, m^.list[k].name);
	end;

	procedure	menu_table;
	var	i	: integer;
	begin
		if title = '' then
			prints(1, y, txt, rpad('', SCR_WIDTH))
		else	shaded_box(x, y, w, h, shade, AF_LEFT, title);
		for i := 1 to m^.size do menu_item(i, FALSE);
	end;

	function	menu_width: integer;
	var	i, ww	: integer;
	begin
		ww := 0;
		for i := 1 to m^.size do
			inc(ww, length(m^.list[i].name));
		menu_width := ww + 4;
	end;

	function	menu_height: integer;
	begin
		menu_height := 1;
	end;

begin
        oldscr := vscreen_activate(vscreen_vram);

	if title <> '' then
	begin
		mx := x + 1;	my := y + 1;
		w  := menu_width;
		h  := menu_height + 2;
	end
	else
	begin
		mx := x;	my := y;
		w  := SCR_WIDTH;
		h  := menu_height;
	end;

	r := region_get(x, y, w, h);
	menu_table;

	MENU_PULLED     := FALSE;
	quit            := FALSE;
	id              := 0;

	if m^.last <> 0 then k := m^.last else k := 1;
	repeat
		sid := 0;
		menu_item(k, TRUE);

		if MENU_PULLED and m^.list[k].pop and (m^.list[k].sub <> nil) then
			sid := menuPopup(m^.list[k].sub, shade,
					  menu_pos(k), my+1, txt, sel, '');
		if (sid = 0) and (not MENU_PULL_LEFT) and (not MENU_PULL_RIGHT) then
			MENU_PULLED := FALSE;

		if MENU_PULLED and m^.list[k].pop and (m^.list[k].sub <> nil) then
		begin
			if (MENU_PULL_LEFT or MENU_PULL_RIGHT) and MENU_PULLED then
				sid := 0;
			if MENU_PULL_LEFT then
			begin
				key := KEY_LEFT;
				sid := 0;
			end;
			if MENU_PULL_RIGHT then
			begin
				key := KEY_RIGHT;
				sid := 0;
			end;
			menu_item(k, FALSE);
		end
		else
		begin
			key := getkey;
			menu_item(k, FALSE);
		end;
		case key of
		KEY_LEFT:	if k > 1 then dec(k) else k := m^.size;
		KEY_RIGHT:	if k < m^.size then inc(k) else k := 1;
		KEY_HOME:	k := 1;
		KEY_END:	k := m^.size;
		KEY_ESC:	begin
					quit := TRUE;
					id := 0;
					sid := 0;
				end;
		KEY_DOWN,
		KEY_ENTER:	if m^.list[k].pop then
					MENU_PULLED := TRUE
				else
				begin
					id := m^.list[k].cmd;
					quit := TRUE;
				end;
		end;
	until quit or (sid <> 0);
	m^.last := k;
	region_put(r);
        vscreen_activate(oldscr);
	if sid = 0 then
		menuPulldown := id
	else	menuPulldown := sid;
end;

{ execute a generic menu object, both pop-up or pull-down }
function	menuExec(m: PMENU; style: MENU_STYLE; shade: SHADING; x, y, txt, sel: integer; title: string): word;
var	value	: word;
begin
	case style of
	MSF_POP : value := menuPopup   (m, shade, x, y, txt, sel, title);
	MSF_PULL: value := menuPulldown(m, shade, x, y, txt, sel, title);
	end;
	menuExec := value;
end;

{ on/off symbol }
function	menuOnOff(state: boolean): string;
begin
	if state then menuOnOff := '[x]' else menuOnOff := '[ ]';
end;

{ ////////////////////////////// LIFE-BOX API ////////////////////////////// }

procedure	lifeCreate(var lf: LIFE_TYPE;
			   x, y, w, row, max, syma: integer;
			   shade: SHADING;
			   name: STR80; symbol: char);
begin
	lf.x	:= x;
	lf.y	:= y;
	lf.w	:= w;
	lf.h	:= row+2;
	lf.row	:= row;
	lf.count:= max;
	lf.max  := max;
	lf.name := name;
	lf.sym	:= symbol;
	lf.syma	:= syma;
	lf.show := FALSE;
	lf.shade:= shade;
	lf.lpr  := max div row;
	if odd(max) then inc(lf.lpr);
	if lf.w < lf.lpr + 4 then lf.w := lf.lpr + 4;
end;

procedure	lifeShowLives(var lf: LIFE_TYPE);
var	text	: string;
	i, j, c	: integer;
begin
	with lf do
	begin
		if not show then exit;
		c    := 1;
		for i := 1 to row do
		begin
			text := '';
			for j := 1 to lpr do
			begin
				if c <= count then
					text := text + sym
				else	text := text + #32;
				inc(c);
			end;
			print(x + 2, y + i, make_attr(syma)+make_center(text, w-2));
		end;
	end;
end;

procedure       lifeShow(var lf: LIFE_TYPE);
begin
	with lf do
	begin
		show := TRUE;
		shaded_box(x, y, w, h, shade, AF_LEFT, name);
	end;
	lifeShowLives(lf);
end;

procedure	lifeHide(var lf: LIFE_TYPE);
begin
	with lf do
	begin
		show := FALSE;
		shaded_box(x, y, w, h, SHADE_BLACK, AF_LEFT, '');
	end;
end;

function	lifeLast(const lf: LIFE_TYPE): boolean;
begin
	lifeLast := lf.count = 1;
end;

function	lifeOver(const lf: LIFE_TYPE): boolean;
begin
	lifeOver := lf.count = 0;
end;

procedure	lifeLost(var lf: LIFE_TYPE);
begin
	if lf.count > 0 then dec(lf.count);
	lifeShowLives(lf);
end;

procedure	lifeBonus(var lf: LIFE_TYPE);
begin
	if lf.count < lf.max then
	begin
		inc(lf.count);
		lifeShowLives(lf);
	end;
end;

function	lifeVisible(const lf: LIFE_TYPE): boolean;
begin
	lifeVisible := lf.show;
end;

{ ///////////////////////////// COUNTER-BOX API //////////////////////////// }

function	counterCreate(ival, x, y, w: integer; min, max: longint;
			      shade: SHADING; name: STR80): PCOUNTER;
var     cnt	: PCOUNTER;
	tlen	: integer;
begin
	if ltx_memavail < sizeof(COUNTER_TYPE) then
	begin
		counterCreate := nil;
		exit;
	end;
	
	new(cnt);

	tlen := text_length(name)+6;
	if (w < tlen) then w := tlen;
	cnt^.x		:= x;
	cnt^.y		:= y;
	cnt^.w		:= w;
	cnt^.vw		:= w-4;
	cnt^.min	:= min;
	cnt^.max	:= max;
	cnt^.show	:= FALSE;
	cnt^.text	:= name;
	cnt^.shade	:= shade;
	cnt^.base	:= ival;
	cnt^.value	:= ival;

	counterCreate 	:= cnt;
end;

procedure	counterDestroy(var cnt: PCOUNTER);
begin
	dispose(cnt);
end;

procedure	counterShow(var cnt: PCOUNTER);
begin
	with cnt^ do
	begin
		show := TRUE;
		shaded_box(x, y, w, 3, shade, AF_LEFT, text);
		print(x + 2, y + 1, lpad(ltoa(value), vw));
	end;
end;

procedure	counterHide(var cnt: PCOUNTER);
begin
	with cnt^ do
	begin
		show := FALSE;
		shaded_box(x, y, w, 3, SHADE_BLACK, AF_LEFT, '');
	end;
end;

procedure	counterShowValue(var cnt: PCOUNTER);
begin
	with cnt^ do
	begin
		show := TRUE;
		print(x + 2, y + 1, lpad(ltoa(value), vw));
	end;
end;

procedure	counterHideValue(cnt: PCOUNTER);
begin
	with cnt^ do
	begin
		show := FALSE;
		print(x + 2, y + 1, repl(' ', vw));
	end;
end;

procedure	counterSetValue(var cnt: PCOUNTER; value: integer);
var	vis	: boolean;
begin
	vis := cnt^.show;
	if vis then counterHideValue(cnt);
	cnt^.value := value;
	if vis then counterShowValue(cnt);
end;

function	counterGetValue(var cnt: PCOUNTER): integer;
begin
	counterGetValue := cnt^.value;
end;

procedure	counterIncrease(var cnt: PCOUNTER);
begin
	counterSetValue(cnt, counterGetValue(cnt) + 1);
end;

procedure	counterDecrease(var cnt: PCOUNTER);
begin
	counterSetValue(cnt, counterGetValue(cnt) - 1);
end;

procedure	counterReset(var cnt: PCOUNTER);
begin
	counterSetValue(cnt, cnt^.base);
end;

procedure       counterLimitDecrease(var cnt: PCOUNTER);
begin
	if cnt^.value > cnt^.min then counterDecrease(cnt);
end;

procedure	counterLimitIncrease(var cnt: PCOUNTER);
begin
	if cnt^.value < cnt^.max then counterIncrease(cnt);
end;

procedure	counterAccum(var cnt: PCOUNTER; value: integer);
begin
	counterSetValue(cnt, cnt^.value + value);
end;

procedure	counterAdd(var cnt: PCOUNTER; incr: integer);
var	tmp	: integer;
begin
	tmp := cnt^.value + incr;
	if (tmp >= cnt^.min) and (tmp <= cnt^.max) then
		counterSetValue(cnt, tmp);
end;

function	counterLast(var cnt: PCOUNTER): boolean;
begin
	counterLast := cnt^.value = cnt^.min;
end;

function	counterVisible(cnt: PCOUNTER): boolean;
begin
	counterVisible := cnt^.show;
end;

{ /////////////////////////////// HELP-BOX API ///////////////////////////// }

function        helpboxCreate(source, title: string; x, y: integer): PHELPBOX;
var	hb	: PHELPBOX;
	s	: HELPSTR;
	f	: TEXT;
begin
	if ltx_memavail < sizeof(HELPBOX_TYPE) then
	begin
		helpboxCreate := nil;
		exit;
	end;

	new(hb);

        ltx_validate_rect(x, y, HELPWIND_W, HELPWIND_H);

	hb^.x	    := x;
	hb^.y	    := y;
	hb^.title   := title;
	hb^.lines   := 0;

	assign(f, source);
	{$I-}
	reset(f);
	{$I+}
	if ioresult <> 0 then
	begin
		helpboxCreate := nil;
		exit;
	end;

	while not eof(f) do
	begin
		readln(f, s);
		while text_length(s) > HELPTEXT_LEN do
			delete(s, length(s), 1);
		inc(hb^.lines);
		hb^.text[hb^.lines] := s;
	end;
	close(f);

	helpboxCreate := hb;
end;

procedure	helpboxExec(hb: PHELPBOX);
var	k, rows	: longint;
        changed,
	done	: boolean;
	key	: word;
	dlg	: PDIALOG;
	scroll	: PSCROLLBAR;

	procedure       draw(start: longint);
	var	i, k	: longint;
	begin
        	vsync;
		ltx_fillw(hb^.x + 2, hb^.y + 1, HELPCLNT_W, HELPCLNT_H, $0720);

		for i := 1 to HELPCLNT_H do
		begin
                	k := i + start - 1;
			if (k <= hb^.lines) and (k >= 1) then
				print(hb^.x + 2, hb^.y + i, hb^.text[k]);
                end;
	end;

begin
	if (hb = nil) then exit;
	if (hb^.lines = 0) then exit;

        changed:= TRUE;
	done   := FALSE;
	rows   := hb^.lines - HELPCLNT_H + 1;
        if rows < 0 then rows := hb^.lines;
	k      := 1;
        dlg    := dialog_open(hb^.x, hb^.y, HELPWIND_W, HELPWIND_H, SHADE_WHITE, hb^.title, '');
        scroll := scrollbarCreate(SCS_VERT, SHADE_WHITE,
				  hb^.x + HELPWIND_W - 2, hb^.y + 1, HELPCLNT_H,
				  0, rows);
        {draw(k);
        scrollbarDraw(scroll);}
	repeat
        	if changed then
                begin
			draw(k);
                        scrollbarUpdateValue(scroll, k);
                        changed := FALSE;
                end;
		key := getkey;
		case key of
                KEY_ENTER,
		KEY_ESC:	done := TRUE;
		KEY_DOWN:	begin
					if k < rows then inc(k);
                                        changed := TRUE;
                		end;
		KEY_UP:		begin
					if k > 1 then dec(k);
                                        changed := TRUE;
                		end;
		KEY_HOME:	begin
					k := 1;
                                        changed := TRUE;
                		end;
		KEY_END:	begin
					k := rows-1;
                                        changed := TRUE;
                		end;
		KEY_PGDN:	begin
					inc(k, HELPCLNT_H);
					if k > rows then k := rows;
                                        changed := TRUE;
				end;
		KEY_PGUP:       begin
					dec(k, HELPCLNT_H);
					if k < 1 then k := 1;
                                        changed := TRUE;
				end;
		end;
	until done;
        scrollbarDestroy(scroll);
        dialog_close(dlg);
end;

procedure       helpboxDestroy(var hb: PHELPBOX);
begin
	dispose(hb);
end;

{ //////////////////////////////// GAUGE API /////////////////////////////// }

function	gaugeStyle(flag, style: word): boolean;
begin
        gaugeStyle := flag and style = style;
end;

function	gaugeCreate(ival, x, y, w, min, max, style: longint; shadef, shadeg: SHADING; title: string): PGAUGE;
var	gg	: PGAUGE;
begin
        if ltx_memavail < sizeof(GAUGE_TYPE) then
        begin
                gaugeCreate := nil;
                exit;
        end;

	new(gg);
        gg^.x     := x;
        gg^.y     := y;
        gg^.w     := w;
        gg^.h	  := 1;
        gg^.min   := min;
        gg^.max	  := max;
        gg^.cval  := ival;
        gg^.shadef:= shadef;
        gg^.shadeg:= shadeg;
        gg^.title := title;
	gg^.style := style;
        gg^.gw	  := gg^.w;
        gg^.gx	  := x;
        gg^.gy	  := y;

        if gaugeStyle(style, GSF_HAS_BORDER) then
	begin
        	dec(gg^.gw, 4);
		inc(gg^.h, 2);
                gg^.gx := gg^.x + 2;
                gg^.gy := gg^.y + 1;
        end
        else
        begin
                gg^.gx := gg^.x;
                gg^.gy := gg^.y;
        end;

	if gaugeStyle(style, GSF_HAS_PERCENT) then
        	inc(gg^.h);

        gg^.ival  := ival;
        gg^.diff  := max - min;
	gg^.r_low  := round(GLV_LIMIT_LOW*gg^.gw);
        gg^.r_hi  := round(GLV_LIMIT_HIGH*gg^.gw);

        gaugeCreate := gg;
end;

procedure	gaugeDestroy(var gg: PGAUGE);
begin
        with gg^ do
        	ltx_fillw(x, y, w, h, ltx_makew(#0, CLEAR_COLOR shl 4));
        dispose(gg);
end;

procedure	gaugeDraw(var gg: PGAUGE);
var	i, k, ip,
	clr	: longint;
        c	: char;
begin
	with gg^ do
        begin
	        pct := (1.0 * cval - min) / diff * gw;
                ip  := round(pct);
                k   := 0;
	        clr := shadeg[SHADE_DARK];
		prints(gx+ip, gy, shadef[SHADE_DARK], repl(GLC_CHARSET[3], gw-ip));
	        for i := 0 to ip-1 do
        	begin
        		if i < r_low then clr := shadeg[SHADE_DARK]
			else	if (i >= r_low) and (i <= r_hi) then
				clr := shadeg[SHADE_MEDIUM]
			else	clr := shadeg[SHADE_BRIGHT];
	                printc(gx+i, gy, clr, GLC_CHARSET[1]);
                        k := i;
        	end;
	        if (pct > 0) and (pct < gw) then
        	begin
        		if (pct-i <= 0.8) then
				c := GLC_CHARSET[2]
			else	c := GLC_CHARSET[1];
		       	printc(gx+k, gy, clr, c);
        	end;
                pct := pct / gw * 100;
        	if gaugeStyle(style, GSF_HAS_PERCENT) then
                begin
                        prints(	gx, gy + 1, shadeg[SHADE_TEXT],
				make_center(itoa(round(pct))+'%', gw));
                end;
	end;
end;

procedure	gaugeShow(var gg: PGAUGE);
begin
        with gg^ do
		if gaugeStyle(style, GSF_HAS_BORDER) then
        		shaded_box(x, y, w, h, shadef, AF_LEFT, title);
	gaugeDraw(gg);
end;

procedure	gaugeHide(var gg: PGAUGE);
begin
        with gg^ do
        	ltx_fillw(x, y, w, h, ltx_makew(#0, (CLEAR_COLOR shl 4) + 0));
end;

procedure	gaugeSetValue(var gg: PGAUGE; value: longint);
begin
	if (value < gg^.min) then value := gg^.min;
	if (value > gg^.max) then value := gg^.max;

        gg^.cval := value;		{ update the value }
	gaugeDraw(gg);
end;

function	gaugeGetValue(var gg: PGAUGE): longint;
begin
        gaugeGetValue := gg^.cval;
end;

procedure	gaugeDecrease(var gg: PGAUGE);
begin
        gaugeSetValue(gg, gg^.cval - 1);
end;

procedure	gaugeIncrease(var gg: PGAUGE);
begin
        gaugeSetValue(gg, gg^.cval + 1);
end;

procedure	gaugeAdd(var gg: PGAUGE; value: longint);
var	tmp	: longint;
begin
        tmp := gg^.cval + value;
        if tmp > gg^.max then tmp := gg^.max;
        gaugeSetValue(gg, tmp);
end;

procedure	gaugeSub(var gg: PGAUGE; value: longint);
var	tmp	: longint;
begin
        tmp := gg^.cval - value;
        if tmp < gg^.min then tmp := gg^.min;
        gaugeSetValue(gg, tmp);
end;

procedure	gaugeReset(var gg: PGAUGE);
begin
        gaugeSetValue(gg, gg^.ival);
end;

{ ////////////////////////////// LIST VIEW API ///////////////////////////// }

function	listCreate(mode: LISTBOX_MODE; x, y, w, h, nrm, sel, count: integer): PLISTBOX;
var	lv	: PLISTBOX;
begin
	listCreate := nil;
        if ltx_memavail < sizeof(LISTBOX_TYPE) then exit;
        if (w = 0) or (h = 0) then exit;

        ltx_validate_rect(x, y, w, h);

        new(lv);
        lv^.mode   := mode;
        lv^.x      := x;
	lv^.y      := y;
        lv^.w      := w;
	lv^.h      := h;
        lv^.norm   := nrm;
        lv^.active := sel;
      	lv^.count  := count;
        lv^.citem  := 1;
        lv^.delta  := 1;
        lv^.last   := -1;
        lv^.chd	   := listDraw;
        lv^.chi    := listGetString;

        if mode = LVM_VIEW then
        	if count < h then lv^.range := 0 else lv^.range := count-h+1
        else	lv^.range := count;

        listCreate := lv;
end;

procedure	listDestroy(var lv: PLISTBOX);
begin
        dispose(lv);
end;

function	listCurrent(const lv: PLISTBOX): longint;
begin
	listCurrent := lv^.citem;
end;

procedure	listResize(var lv: PLISTBOX; count: longint);
begin
        lv^.count := count;
        if lv^.mode = LVM_VIEW then
        	if lv^.count < lv^.h then
			lv^.range := 0
		else 	lv^.range := lv^.count-lv^.h+1
        else	lv^.range := count;
        lv^.citem := 1;
        lv^.delta := 1;
end;

procedure	listHandleDraw(var lv: PLISTBOX; handler: LISTBOX_DRAW_PROC);
begin
        {LISTBOX_HANDLER_DRAW}lv^.chd := handler;
end;

procedure	listHandleItem(var lv: PLISTBOX; handler: LISTBOX_ITEM_FUNC);
begin
	{LISTBOX_HANDLER_ITEM}lv^.chi := handler;
end;

procedure	listHandleEvent(var lv: PLISTBOX; key: word);
var	newc	: longint;
begin
        if lv^.mode = LVM_PICK then
        begin
		lv^.last := -1;
        	case key of
	        KEY_UP:		if lv^.citem > 1 then
        	        	begin
                	        	lv^.last := lv^.delta;
        	                        if lv^.delta > 1 then dec(lv^.delta);
	                                dec(lv^.citem);
	                        end;
        	KEY_DOWN:	if lv^.citem < lv^.count then
	        		begin
        	                        lv^.last := lv^.delta;
                	                if lv^.delta < lv^.h then inc(lv^.delta);
	                                inc(lv^.citem);
        	                end;
	        KEY_PGUP:	if lv^.citem > 1 then
        			begin
                	        	newc := lv^.citem - (lv^.h - 1);
	                                if newc - lv^.delta > 0 then
        	                        	lv^.citem := newc
                	                else
                        	        begin
	                                	lv^.citem := 1;
        	                                lv^.delta := 1;
                	                end;
                        	        lv^.last := 0;
	                        end;
        	KEY_PGDN:	if lv^.citem < lv^.count then
	        		begin
        	                	newc := lv^.citem + (lv^.h-1);
                	                if newc + (lv^.h - lv^.delta) <= lv^.count then
                                	lv^.citem := newc
	                                else
        	                        begin
                	                	lv^.citem := lv^.count;
	                                        lv^.delta := ltx_min(lv^.h, lv^.count);
        	                        end;
                	                lv^.last := 0;
                        	end;
	        KEY_HOME:	if lv^.citem > 1 then
        			begin
                	        	lv^.citem := 1;
                        	        lv^.delta := 1;
	                                lv^.last  := 0;
        	                end;
	        KEY_END:        if lv^.citem < lv^.count then
        			begin
                	        	lv^.citem := lv^.count;
	                                lv^.delta := ltx_min(lv^.h, lv^.count);
        	                        lv^.last  := 0;
                	        end;
	        end;
        	if lv^.last <> -1 then	lv^.chd(lv);
        end
        else
        begin
                prints(1, 1, $0E, itoa(lv^.count)+'-'+itoa(lv^.h)+'='+itoa(lv^.range));
                prints(1, 2, $0A, lpad(itoa(lv^.citem), 4));
                case key of
                KEY_DOWN:       if (lv^.citem <= lv^.range) then inc(lv^.citem);
                KEY_UP:		if lv^.citem > 1 then dec(lv^.citem);
                KEY_HOME:	lv^.citem := 1;
                KEY_END:	lv^.citem := lv^.range;
                KEY_PGUP:	begin
                			if lv^.citem > 1 then
                                        	dec(lv^.citem, lv^.h);
                                        if lv^.citem < 1 then lv^.citem := 1;
				end;
                KEY_PGDN:	begin
					if lv^.citem <= lv^.range then
						inc(lv^.citem, lv^.h);
					if lv^.citem > lv^.range then lv^.citem := lv^.range;
                		end;
                end;
        	lv^.chd(lv);
        end;
	key := 0;
end;

function	listGetString(i: longint): string;
begin
	listGetString := '['+ltoa(i)+']';
end;

procedure       listDraw(var lv: PLISTBOX);
var	i, c,
	offset	: longint;
        text	: string;
begin
	if lv^.mode = LVM_PICK then
        begin
		offset := lv^.citem - lv^.delta;
        	for i := 1 to lv^.h do
	        begin
        		if i = lv^.delta then c := lv^.active else c := lv^.norm;
                	if i + offset <= lv^.count then
				text := rpad(lv^.chi(i + offset), lv^.w-1)
                        else	text := rpad(' ', lv^.w-1);
			prints(	lv^.x, lv^.y+i-1, c, text);
	        end;
        end
        else
        begin
        	for i := 1 to lv^.h do
			prints(	lv^.x, lv^.y+i-1, lv^.norm,
				rpad(lv^.chi(lv^.citem+i-1), lv^.w-1));
        end;
end;

function	listExec(var lv: PLISTBOX): longint;
var	key	: word;
	done	: boolean;
        scrl	: PSCROLLBAR;
begin
        done := FALSE;

        lv^.chd(lv);
	scrl := scrollbarCreate(SCS_VERT, SHADE_WHITE,
				lv^.x + lv^.w - 1, lv^.y, lv^.h, 1, lv^.range);
        repeat
                scrollbarUpdateValue(scrl, lv^.citem);
        	key := getkey;
                if (key = KEY_ESC) or (key = KEY_ENTER) then done := TRUE
                else	listHandleEvent(lv, key);
        until done;
        scrollbarDestroy(scrl);

        if (lv^.mode = LVM_PICK) and (key = KEY_ENTER) then
        	listExec := lv^.citem
        else	listExec := -1;
end;

{ /////////////////////////////// LOGVIEW API ////////////////////////////// }

function	logviewCreate(x, y, w, h, attr: integer): PLOGVIEW;
var	log	: PLOGVIEW;
begin
        if (h = 0) or (w = 0) then exit;
        w := ltx_min(w, SCR_WIDTH);
        h := ltx_min(h, MAX_DLG_LINE);
        ltx_validate_rect(x, y, w, h);

        new(log);
        log^.x	  := x;
        log^.y    := y;
        log^.w	  := w;
        log^.h	  := h;
        log^.attr := attr;
        log^.count:= 0;

        logviewCreate := log;
end;

procedure	logviewDestroy(var log: PLOGVIEW);
begin
        if log = nil then exit;
        dispose(log);
        log := nil;
end;

procedure	logviewDraw(const log: PLOGVIEW);
var	i	: integer;
	text	: STR80;
begin
        for i := 0 to log^.h-1 do
	begin
        	if i < log^.count then
	        	text := rpad(log^.log[i], log^.w)
                else	text := rpad(' ', log^.w);
                prints(log^.x, log^.y + i, log^.attr, text);
	end;
end;

procedure	logviewAdd(var log: PLOGVIEW; text: STR80);
var	i	: integer;
begin
        if length(text) >= log^.w then text[0] := chr(log^.w);
        if log^.count < log^.h then
        begin
                log^.log[log^.count] := text;
                inc(log^.count);
        end
        else
        begin
                for i := 1 to log^.h-1 do
                        log^.log[i-1] := log^.log[i];
                log^.log[log^.h-1] := text;
        end;
	logviewDraw(log);
end;

end.
