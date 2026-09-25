{$i preps.inc}

unit libtimer;

interface

const
        LIBTIMER_VERSION = $0101;
        LIBTIMER_BUILD   = '0925';

procedure timerInit;
procedure timerDone;
function  timerNow: longint;
procedure timerDelay(milliseconds: longint);

implementation

uses    libtext;

{ milliseconds, monotonic; the clock itself lives in LIBTEXT (ltx_timer) }
function timerNow: longint;
begin
        timerNow := ltx_timer;
end;

procedure timerInit;
begin
        ltx_timer_init;
end;

procedure timerDone;
begin
        ltx_timer_done;
end;

procedure timerDelay(milliseconds: longint);
begin
        sleep(milliseconds);
end;

end.
