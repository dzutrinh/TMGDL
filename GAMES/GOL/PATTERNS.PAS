{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   HELPER.PAS                                                               }
{   ----------                                                               }
{   Patterns Include file generator for Game of Life                         }
{   Version 1.00                                                             }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2020-2021 by Infinity Group                               }
{                                                                            }
{   Compilers                                                                }
{   ---------                                                                }
{   . Turbo/Borland Pascal 7.0                                               }
{   . TMT Pascal 3.5+                                                        }
{                                                                            }
{   DDK16 | DDK32                                                            }
{									     }
{ ////////////////////////////////////////////////////////////////////////// }

{$define GENPAT}		{ pattern include file generation mode }

uses	libtmgd, libtext;

{ // GENERATOR ///////////////////////////////////////////////////////////// }

{$ifdef GENPAT}

function	getline(var f: TEXT): string;
var	tmp	: string;
begin
	tmp := '';
	repeat
        	readln(f, tmp);
        until (tmp <> '') and (tmp[1] <> ';');
	getline := tmp;
end;

function	token(var s1: string): string;
var	result	: string;
	i	: longint;
begin
	if s1[length(s1)] <> ' ' then s1 := s1 + ' ';
	i := pos(' ', s1);
	if i <> 0 then
	begin
		result := copy(s1, 1, i - 1);
		delete(s1, 1, i + length(' ') - 1);
	end
	else
		result := s1;
	token := result;
end;

function	atoi(const s: string): integer;
var	error	: integer;
	value	: integer;
begin
	val(s, value, error);
	if error <> 0 then value := 0;
	atoi := value;
end;

procedure	block_build(blk: PBLOCK; buf: pointer);
var	p, q	: pchar;
	i	: integer;
        tmp	: string;
begin
	p := buf;
        q := blk^.data;
        for i := 1 to blk^.size do
        begin
               	if p^ <> '0' then
                       	q^ := #1
		else	q^ := #0;
		inc(p);
        	inc(q);
        end;
end;

procedure       block_inc(var f: TEXT; name: string; p: PBLOCK);
var	i, j	: integer;
	q	: pchar;
begin
	writeln(f, #9#9+'(x: 0; y: 0; w: ', p^.w, '; h: ', p^.h, '; size: ', p^.size, '; data:');
	q := p^.data;
        for j := 1 to p^.h do
        begin
	        write  (f, #9#9#32);
	        for i := 1 to p^.w do
                begin
        	        write(f, '#', chr(byte(q^)+byte('0')));
                        inc(q);
                end;
                if j <> p^.h then
                	writeln(f, '+')
                else	write(f, '; attach: FALSE)');
        end;
end;

procedure	makePatterns(filename: string);
var	f, g	: TEXT;
        pname	: string;
        total,
        pw, ph	: integer;
        tmp	: string;
        data	: array[0..1024] of char;
        i, j	: integer;
        pat	: PBLOCK;

begin
        assign(f, filename);
        {$I-}
        reset(f);
        {$I+}
        if ioresult <> 0 then die(-1, 'file open error');

        total := atoi(getline(f));
        writeln('Creating file PATTERNS.INC...');
        assign(g, 'PATTERNS.INC');
        rewrite(g);

	writeln(g, '{$ifndef __PATTERNS_INC__}');
	writeln(g, '{$define __PATTERNS_INC__}');
	writeln(g);
	writeln(g, '{ THIS FILE IS MACHINE GENERATED - PLEASE DO NOT ALTER }');
	writeln(g);
        writeln(g, 'const	PATTERNS_LIST: array[1..',total,'] of BLOCK_TYPE = ');
	writeln(g, #9'(');
        for j := 1 to total do
        begin
                tmp := getline(f);		{ read name }
                if tmp <> '' then
                begin
                        pw := atoi(token(tmp));
                        ph := atoi(token(tmp));
                        write('. Pattern ', j:2, ' (', pw:2, 'x', ph:2, ')...');
                        for i := 1 to ph do	{ read bits }
                        begin
                        	tmp := getline(f);
                                move(tmp[1], data[(i-1)*pw], pw);
                        end;
                        pat := block_create(pw, ph);
                        block_build(pat, @data);
			block_inc(g, pname, pat);
                        block_destroy(pat);
                	writeln('done');
                        if j <> total then
                        	writeln(g, ',')
                        else	writeln(g);
                end;
        end;
        writeln(g, #9');');
	writeln(g);
	writeln(g, '{$endif}');
        close(f);
        close(g);
        writeln('done');
end;

{$else}

{ // PATTERN VIEWER //////////////////////////////////////////////////////// }

{$i globals.inc}
{$i patterns.inc}
{$i engine.inc}

var     c	: integer;
	quit	: boolean;
	key	: word;
	text	: string;
{$endif}

begin
{$ifdef	GENPAT}
        makePatterns('PATTERNS.TXT');
{$else}
	ltx_init;
        quit := FALSE;
        c := 1;
        cursor(CURSOR_OFF);
        repeat
        	clear($0F);
                block_put(@PATTERNS_LIST[c], -1, -1, $0F);
                with PATTERNS_LIST[c] do
                	text := 'pattern ~0F'+itoa(c)+'~07 (~0D'+itoa(w)+'~07, ~0D'+itoa(h)+'~07)';
        	center(24, text);
                key := getkey;
                case key of
                KEY_ESC:	quit := TRUE;
                KEY_LEFT:	if c > 1 then dec(c);
                KEY_RIGHT:      if c < PATTERNS_MAX then inc(c);
                end;
        until quit;
        cursor(CURSOR_ON);
        ltx_done;
{$endif}
end.
