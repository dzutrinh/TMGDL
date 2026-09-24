{$i preps.inc}

unit libtimer;

interface

const
        LIBTIMER_VERSION = $0100;
        LIBTIMER_BUILD   = '0924';

procedure timerInit;
procedure timerDone;
function  timerNow: longint;
procedure timerDelay(milliseconds: longint);

implementation

uses	libtext
{$ifdef FPC}
		, go32
{$endif}
		;

function timerCounter: word;
var
        lo, hi: byte;
begin
        { Latch PIT channel 0 without changing its BIOS configuration. }
        outportb($43, $00);
        lo := inportb($40);
        hi := inportb($40);
        timerCounter := lo or (hi shl 8);
end;

function timerNow: longint;
var
        before, after: longint;
        counter: word;
        fraction: longint;
begin
        repeat
                before := ticks;
                counter := timerCounter;
                after := ticks;
        until before = after;

        { BIOS ticks are about 55 ms apart; PIT supplies the sub-tick part. }
        fraction := (65535 - counter) * 55 div 65536;
        timerNow := before * 55 + fraction;
end;

procedure timerInit;
begin
end;

procedure timerDone;
begin
end;

procedure timerIdle; assembler;
asm
        int     $28
end;

procedure timerDelay(milliseconds: longint);
var
        start: longint;
begin
        if milliseconds <= 0 then exit;
        start := timerNow;
        repeat
                timerIdle;
        until timerNow - start >= milliseconds;
end;

end.
