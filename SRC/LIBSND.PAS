{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   LIBSND.PAS                                                               }
{   ----------                                                               }
{   Library for PC speaker sound and music                                   }
{   Version 1.0                                                              }
{                                                                            }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2026 by Infinity Group                                    }
{                                                                            }
{   Compilers supported:                                                     }
{   --------------------                                                     }
{   . Turbo/Borland Pascal 7.0                                               }
{   . Free Pascal 3.2 (go32v2)                                               }
{   . TMT Pascal 3.5+                                                        }
{                                                                            }
{   The speaker is driven by channel 2 of the timer chip; the millisecond    }
{   timer of LIBTEXT (channel 0) is not touched.                             }
{                                                                            }
{   snd_play() plays in the background: the music goes on whenever the     }
{   program waits in LIBTEXT (ltx_idle, sleep, readkey); a game loop that   }
{   waits otherwise calls snd_update.                                       }
{                                                                            }
{   Music is written like the PLAY statement of BASIC:                       }
{     A..G   a note, then # or + (sharp), - (flat), a length, dots           }
{     N n    note number n (1..84, 0 is a rest)                              }
{     O n    octave n (0..6), < and > one octave down and up                 }
{     L n    default length (1 whole, 2 half, 4 quarter .. 64)               }
{     T n    tempo, quarter notes a minute (32..255)                         }
{     P n    a pause (R n works too)                                         }
{     MS MN ML  staccato, normal, legato                                     }
{                                                                            }
{ ////////////////////////////////////////////////////////////////////////// }

{$i preps.inc}

unit	libsnd;

interface

uses	{$ifdef FPC} go32, {$endif} libtext;

const	LIBSND_VERSION	= $0100;
	LIBSND_BUILD	= '0925';

	{ sound effects, to play with snd_play or snd_play_wait }
	SFX_CLICK	= 'T255L64O6C';
	SFX_BLIP	= 'T255L64O5CEG';
	SFX_COIN	= 'T255L32O6B>E';
	SFX_HIT		= 'T255L64O2CC#DD#';
	SFX_JUMP	= 'T255L64O4CEG>CEG';
	SFX_FALL	= 'T255L64O5GFEDC<BAG';
	SFX_BOOM	= 'T255L64O1CDC#<BA#AG#G';
	SFX_DIE		= 'T200L16O4GF#FEL8D#MLL4D';
	SFX_LEVEL	= 'T220L16O5CEGL8>C<GL4>C';
	SFX_WIN		= 'T180L8O5CCCL4ECL8EL2G';
	SFX_ERROR	= 'T255L16O2CP32C';

	{ FALSE: no sound at all (a game setting) }
	SND_ENABLED	: boolean = TRUE;

procedure	snd_tone(freq: word);			{ a tone until snd_off   }
procedure	snd_off;
procedure	snd_beep(freq, ms: word);		{ a tone, waits for it   }

procedure	snd_play(music: string);		{ starts in background   }
procedure	snd_update;				{ call it in game loops  }
function	snd_playing: boolean;
procedure	snd_stop;
procedure	snd_wait;				{ plays the rest, waits  }
procedure	snd_play_wait(music: string);		{ plays all, then back   }

implementation

const	PIT_CLOCK	= 1193182;
	{ octave 4, from C: the other octaves are shifted from it }
	NOTE_FREQ	: array[0..11] of word =
			  (262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494);
	NOTE_STEP	: array['A'..'G'] of byte = (9, 11, 0, 2, 4, 5, 7);

var	music_text	: string;
	music_pos	: integer;
	music_on	: boolean;
	music_next,			{ time of the next note           }
	music_quiet	: longint;	{ time the note stops sounding    }
	music_octave,
	music_length,
	music_tempo,
	music_style	: integer;	{ eighths of a note that sound    }
	old_exit	: pointer;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	snd_tone(freq: word);
var	div_	: longint;
begin
	if (freq < 20) or not SND_ENABLED then
	begin
		snd_off;
		exit;
	end;
	div_ := PIT_CLOCK div freq;
	if div_ > $FFFF then div_ := $FFFF;
	outportb($43, $B6);			{ channel 2, square wave  }
	outportb($42, lo(word(div_)));
	outportb($42, hi(word(div_)));
	outportb($61, inportb($61) or 3);	{ speaker on              }
end;

procedure	snd_off;
begin
	outportb($61, inportb($61) and $FC);
end;

procedure	snd_beep(freq, ms: word);
begin
	snd_stop;
	snd_tone(freq);
	sleep(ms);
	snd_off;
end;

{ ////////////////////////////////////////////////////////////////////////// }

function	note_hertz(n: integer): word;
var	o, f	: integer;
begin
	o := n div 12;
	f := NOTE_FREQ[n mod 12];
	if o > 4 then note_hertz := f shl (o - 4)
	else	note_hertz := f shr (4 - o);
end;

{ a number in the music, or def when there is none }
function	music_number(def: integer): integer;
var	v	: integer;
	any	: boolean;
begin
	v := 0;
	any := FALSE;
	while (music_pos <= length(music_text)) and (music_text[music_pos] in ['0'..'9']) do
	begin
		if v < 1000 then v := v * 10 + ord(music_text[music_pos]) - ord('0');
		inc(music_pos);
		any := TRUE;
	end;
	if any then music_number := v else music_number := def;
end;

{ ms of a note of length l (4: a quarter), with dots after it }
function	music_time(l: integer): longint;
var	t	: longint;
	d	: longint;
begin
	if l < 1 then l := 1;
	t := 240000 div (longint(music_tempo) * l);
	d := t;
	while (music_pos <= length(music_text)) and (music_text[music_pos] = '.') do
	begin
		d := d div 2;
		inc(t, d);
		inc(music_pos);
	end;
	music_time := t;
end;

{ read the music up to the next note or rest and start it; FALSE at the end }
function	music_step(now: longint): boolean;
var	c	: char;
	n, l	: integer;
	t	: longint;
	sound	: boolean;
begin
	music_step := FALSE;
	while music_pos <= length(music_text) do
	begin
		c := upcase(music_text[music_pos]);
		inc(music_pos);
		n := -1;
		sound := FALSE;
		case c of
		'A'..'G':
			begin
				n := music_octave * 12 + NOTE_STEP[c];
				if music_pos <= length(music_text) then
					case music_text[music_pos] of
					'#', '+':	begin inc(n); inc(music_pos); end;
					'-':		begin dec(n); inc(music_pos); end;
					end;
				sound := TRUE;
			end;
		'N':	begin
				n := music_number(0) - 1;
				sound := n >= 0;
				if n < 0 then n := 0;
			end;
		'P', 'R':
			n := 0;
		'O':	begin
				music_octave := music_number(4);
				if music_octave > 6 then music_octave := 6;
			end;
		'<':	if music_octave > 0 then dec(music_octave);
		'>':	if music_octave < 6 then inc(music_octave);
		'L':	begin
				music_length := music_number(4);
				if music_length < 1 then music_length := 1;
				if music_length > 64 then music_length := 64;
			end;
		'T':	begin
				music_tempo := music_number(120);
				if music_tempo < 32 then music_tempo := 32;
				if music_tempo > 255 then music_tempo := 255;
			end;
		'M':	if music_pos <= length(music_text) then
			begin
				case upcase(music_text[music_pos]) of
				'S':	music_style := 6;
				'N':	music_style := 7;
				'L':	music_style := 8;
				end;
				inc(music_pos);
			end;
		end;
		if n >= 0 then
		begin
			if c in ['A'..'G', 'P', 'R'] then l := music_number(music_length) else l := music_length;
			t := music_time(l);
			if sound and (n >= 0) and (n < 84) then snd_tone(note_hertz(n)) else snd_off;
			music_next  := now + t;
			music_quiet := now + t * music_style div 8;
			music_step  := TRUE;
			exit;
		end;
	end;
end;

procedure	snd_play(music: string);
begin
	snd_stop;
	if not SND_ENABLED then exit;
	music_text   := music;
	music_pos    := 1;
	music_octave := 4;
	music_length := 4;
	music_tempo  := 120;
	music_style  := 7;
	music_on     := TRUE;
	music_next   := ltx_timer;
	music_quiet  := music_next;
	snd_update;
end;

procedure	snd_update;
var	now	: longint;
begin
	if not music_on then exit;
	now := ltx_timer;
	if now >= music_quiet then snd_off;
	if now < music_next then exit;
	if not music_step(now) then snd_stop;
end;

function	snd_playing: boolean;
begin
	snd_playing := music_on;
end;

procedure	snd_stop;
begin
	music_on := FALSE;
	snd_off;
end;

procedure	snd_wait;
begin
	while music_on do
	begin
		snd_update;
		ltx_idle;
	end;
end;

procedure	snd_play_wait(music: string);
begin
	snd_play(music);
	snd_wait;
end;

{ ////////////////////////////////////////////////////////////////////////// }

{ never leave the speaker on when the program ends }
procedure	snd_exit;
begin
	ExitProc := old_exit;
	snd_off;
end;

begin
	music_on := FALSE;
	music_text := '';
	{ the music goes on whenever the program waits (LIBTEXT idle hook) }
	ltx_idle_hook(snd_update);
	old_exit := ExitProc;
	ExitProc := @snd_exit;
end.
