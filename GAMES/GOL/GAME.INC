{$ifndef __GAME_INC__}
{$define __GAME_INC__}

{ ////////////////////////////////////////////////////////////////////////// }

procedure       gameInit(major, minor: longint; build: string);		forward;
procedure	gameRun;                                                forward;
procedure	gameDone;						forward;
procedure	gameEdit;						forward;
procedure	gameAbout;                                              forward;
procedure	gameHelp;                                               forward;
procedure	gameInterface(ui: UI_MODE);				forward;
procedure	gameShowVersion;					forward;
procedure	gamePlacePattern;                                       forward;

{ ////////////////////////////////////////////////////////////////////////// }

var     mOpt,
	mMain	: PMENU;
	cGen	: PCOUNTER;
        lStatus	: PLABEL;

procedure       gameInit(major, minor: longint; build: string);
begin
	GAME_VERSION  := major shl 8 + minor;
	GAME_BUILD    := build;

        if not file_exist(FILE_HELP) then
        	die(-1, 'Missing game file. Please reinstall.');

	ltx_init;
        if ltx_mono then
        	die(-1, 'EGA/VGA adapter is required');

        if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
                GAME_CONFIG := APP_DEFAULT_CFG;
	SND_ENABLED := GAME_CONFIG.sound;

	randomize;

	GAME_COLONY := create;
	if not load(GAME_COLONY, FILE_COLONY) then init(GAME_COLONY);

	cursor(CURSOR_OFF);
        blink(BLINK_OFF);	{ bright colors for both cells of a text cell }

        mOpt := menuCreate;
        menuAppend(mOpt, '     ALIVE: '+COLOR_NAMES[GAME_CONFIG.c_alive], CMD_OPT_ALIVE,  FALSE, nil);
        menuAppend(mOpt, '     DEAD : '+COLOR_NAMES[GAME_CONFIG.c_dead],  CMD_OPT_DEAD,   FALSE, nil);
        menuAppend(mOpt, ' '+menuOnOff(GAME_CONFIG.invert)+' INVERT COLORS ',  CMD_OPT_INVERT, FALSE, nil);
        menuAppend(mOpt, ' '+menuOnOff(GAME_CONFIG.sound)+' SOUND EFFECTS ',  CMD_OPT_SOUND,  FALSE, nil);

        mMain := menuCreate;
        menuAppend(mMain, '   START SIMULATION ', CMD_START,  FALSE, nil);
        menuAppend(mMain, '   COLONY EDITOR    ', CMD_EDIT,   FALSE, nil);
        menuSeparator(mMain);
        menuAppend(mMain, '   OPTIONS          ', CMD_OPTION, TRUE,  mOpt);
        menuSeparator(mMain);
        menuAppend(mMain, '   ABOUT            ', CMD_ABOUT,  FALSE, nil);
        menuAppend(mMain, '   HELP             ', CMD_HELP,   FALSE, nil);
        menuSeparator(mMain);
        menuAppend(mMain, '   EXIT             ', CMD_EXIT,   FALSE, nil);

        cGen    := counterCreate(0, PANEL_X, PANEL_Y + 9, PANEL_W, 0, 0, SHADE_GREEN, 'GENERATION');
        lStatus := labelCreate(PANEL_X + 2, PANEL_Y + 7, PANEL_W-4, '');

        ANSI_draw(SPLASH_IMG);

        marqueeText(30, 23, 20, $03, 275,'   A cellular automaton simulation game   '#4+
					'   Hit any key to continue   '#4);
	flushkeys;

	gameInterface(UI_MENU);
end;

procedure	gameDone;
begin
        labelDestroy(lStatus);
        counterDestroy(cGen);
	menuDestroy(mMain);
        menuDestroy(mOpt);

        save(GAME_COLONY, FILE_COLONY);
        config_save(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG));

	destroy(GAME_COLONY);

	cursor(CURSOR_ON);
	ltx_done;
	blink(BLINK_ON);

	writeln(' _______    _______    _____   ');
	writeln('|     __|  |       |  |     |_ ');
	writeln('|    |  |__|   -   |__|       |');
	writeln('|_______|__|_______|__|_______|');
	write  ('v');
	write  (vtos(GAME_VERSION, GAME_BUILD));
	writeln(' [', COMPILER, ']');
	writeln('Thanks for playing!');
	writeln;
end;

procedure	gameInterface(ui: UI_MODE);
var     i	: integer;
begin
        vsync;

        ltx_fillw(BOARD_X + BOARD_W, 1, SCR_WIDTH-BOARD_W, SCR_HEIGHT, ltx_makew(#0, $0F));
	for i := 1 to 4 do print(PANEL_X, PANEL_Y + i, GAME_LOGO[i]);
        shaded_box(PANEL_X, PANEL_Y + 6, PANEL_W, 3, SHADE_CYAN, AF_LEFT, 'STATUS');

	labelSetText(lStatus, MSG_READY);

        counterShow(cGen);
        labelShow(lStatus);

	gameShowVersion;
end;

{ the panel beside the colony on the 160 x 100 screen: the name, what is }
{ going on, the generation or the cursor, and the keys                   }
procedure	lpanel(ui: UI_MODE; state: string; gen: longint; x, y: integer);
const	NAME		: string[4] = 'LIFE';
	NAME_COLORS	: array[1..4] of byte = (15, 14, 10, 2);
	RUN_KEYS	: array[1..7] of string[15] =
			  ('SPC STEP/AUTO', 'ENTER 1 STEP', 'P PAUSE', 'R RANDOM',
			   '<- SLOWER', '-> FASTER', 'ESC BACK');
	EDIT_KEYS	: array[1..8] of string[15] =
			  ('SPC TOGGLE', 'ARROWS MOVE', 'P PATTERNS', 'C CLEAR',
			   'F FILL', 'I INVERT', 'R RANDOM', 'ESC BACK');
	PLACE_KEYS	: array[1..6] of string[15] =
			  ('ARROWS MOVE', 'TAB NEXT', 'SHIFT-TAB BACK', 'C CLEAR',
			   'ENTER PUT', 'ESC CANCEL');
var	i	: integer;
begin
	lores_bar(FIELD_W, 0, LORES_W - FIELD_W, LORES_H, 0);
	lores_bar(FIELD_W, 0, 1, LORES_H, 8);
	for i := 1 to 4 do
	begin
		lores_bar(LPANEL_X + (i - 1) * 9, 2, 8, 9, NAME_COLORS[i]);
		lores_text(LPANEL_X + (i - 1) * 9 + 3, 4, NAME[i], 0);
	end;
	lores_text(LPANEL_X, 16, state, 11);
	case ui of
	UI_RUN:
		begin
			lores_text(LPANEL_X, 25, 'GENERATION', 7);
			lores_text(LPANEL_X, 31, ltoa(gen), 15);
			lores_text(LPANEL_X, 40, 'SPEED', 7);
			for i := 1 to SIMUL_SPEED_MAX do
				if i <= GAME_CONFIG.speed then
					lores_bar(LPANEL_X + 22 + (i - 1) * 3, 40, 2, 5, 10)
				else	lores_bar(LPANEL_X + 22 + (i - 1) * 3, 40, 2, 5, 8);
			for i := 1 to 7 do lores_text(LPANEL_X, 52 + (i - 1) * 6, RUN_KEYS[i], 8);
		end;
	UI_EDIT, UI_PLACE:
		begin
			lores_text(LPANEL_X, 25, 'X ' + itoa(x + 1), 15);
			lores_text(LPANEL_X + 28, 25, 'Y ' + itoa(y + 1), 15);
			if ui = UI_EDIT then
				for i := 1 to 8 do lores_text(LPANEL_X, 40 + (i - 1) * 6, EDIT_KEYS[i], 8)
			else	for i := 1 to 6 do lores_text(LPANEL_X, 40 + (i - 1) * 6, PLACE_KEYS[i], 8);
		end;
	end;
end;

procedure	gameRun;
var     quit	: boolean;
	key	: word;
	gen	: longint;
	state	: string;

	procedure	run_show;
	begin
		fieldDraw(GAME_COLONY);
		lpanel(UI_RUN, state, gen, 0, 0);
		lores_flip;
	end;

	procedure	run_pause;
	begin
		state := 'PAUSED';
		run_show;
		if wait([#27, #13, 'P']) = #27 then quit := TRUE;
	end;

	procedure	run_randomize;
	begin
		state := 'RANDOMIZE';
		snd_play(SFX_JUMP);
		init(GAME_COLONY);
		gen := 0;
        	run_pause;
	end;

        procedure	run_slower;
	begin
		if GAME_CONFIG.speed > SIMUL_SPEED_MIN then
		begin
        		dec(GAME_CONFIG.speed);
			snd_play(SFX_CLICK);
		end;
	end;

        procedure	run_faster;
	begin
		if GAME_CONFIG.speed < SIMUL_SPEED_MAX then
		begin
        		inc(GAME_CONFIG.speed);
			snd_play(SFX_CLICK);
		end;
	end;

begin
	if not lores_init then
	begin
		info(SHADE_RED, 'THE SIMULATION NEEDS AN EGA OR VGA CARD.');
		exit;
	end;
	quit := FALSE;
	gen  := counterGetValue(cGen);
	repeat
                if GAME_CONFIG.step then state := 'STEPPING' else state := 'RUNNING';
		run_show;
		sleep((SIMUL_SPEED_MAX-GAME_CONFIG.speed+1) * 16);

		if GAME_CONFIG.step then
		begin
                	repeat
				key := getkey_single;
                        until (key = KEY_ESC) or (key = KEY_SPACE) or
                              (key = KEY_ENTER) or (key = KEY_R);
                        case key of
                        KEY_ENTER:	begin
						snd_play(SFX_CLICK);
						evolve(GAME_COLONY);
				                inc(gen);
                        		end;
                        KEY_ESC:        quit := TRUE;
			KEY_SPACE:	GAME_CONFIG.step := FALSE;
                        KEY_R:		run_randomize;
                	end;
		end
                else
		begin
			evolve(GAME_COLONY);
			inc(gen);
                end;

		if keypressed then
		begin
			key := getkey;
			case key of
			KEY_ESC	 :	quit := TRUE;
                        KEY_SPACE:	GAME_CONFIG.step := TRUE;
                        KEY_P    :	run_pause;
			KEY_R	 :	run_randomize;
                        KEY_LEFT :	run_slower;
                        KEY_RIGHT:	run_faster;
			end;
		end;
	until quit;
	lores_done;
	counterSetValue(cGen, gen);
end;

procedure	gameAbout;
begin
	aboutBox(-1, -1, '~0AGAME OF LIFE ~07FOR DOS'+ENDL+
			 'CODED BY ~03TRINH D.D. NGUYEN');
end;

procedure	gameHelp;
var	hb	: PHELPBOX;
begin
        hb := helpboxCreate(FILE_HELP, 'HELP', -1, -1);
        helpboxExec(hb);
        helpboxDestroy(hb);
end;

procedure	gameShowVersion;
begin
	prints(72, 25, $08, 'v' + vtos(GAME_VERSION, GAME_BUILD));
end;

{ put one of the patterns on the colony; the 160 x 100 screen is on }
procedure	gamePlacePattern;
var	i, j,
	x, y	: integer;
        done	: boolean;
        key	: word;
        p	: pchar;
        c	: byte;
begin
	done	:= FALSE;
        x 	:= 0;
	y	:= 0;
	c	:= 1;
        repeat
                { keep the pattern inside the colony }
                while x + PATTERNS_LIST[c].w - 1 >= FIELD_W do dec(x);
                while y + PATTERNS_LIST[c].h - 1 >= FIELD_H do dec(y);

		fieldDraw(GAME_COLONY);
               	patternDraw(@PATTERNS_LIST[c], x, y);
		lpanel(UI_PLACE, 'PATTERN ' + itoa(c), 0, x, y);
		lores_flip;
                key := getkey_single;
                case key of
                KEY_SHIFT_TAB:	if c > 1 then dec(c) else c := PATTERNS_MAX;
                KEY_TAB:	if c < PATTERNS_MAX then inc(c) else c := 1;
                KEY_ESC:	done := TRUE;
                KEY_ENTER:	begin
					p := PATTERNS_LIST[c].data;
					snd_play(SFX_COIN);
                			for j := 0 to PATTERNS_LIST[c].h-1 do
			                for i := 0 to PATTERNS_LIST[c].w-1 do
			                begin
                        			if p^ <> #0 then
				                	GAME_COLONY^.field[cell(x+i, y+j)] := C_ALIVE;
                        			inc(p);
			                end;
                                        done := TRUE;
				end;
		KEY_C: 		begin
					empty(GAME_COLONY);
					snd_play(SFX_FALL);
				end;
                KEY_A,
                KEY_LEFT:	if x > 0 then dec(x);
                KEY_D,
                KEY_RIGHT:	if x <= FIELD_W - PATTERNS_LIST[c].w - 1 then inc(x);
                KEY_W,
                KEY_UP:		if y > 0 then dec(y);
                KEY_S,
                KEY_DOWN:	if y <= FIELD_H - PATTERNS_LIST[c].h - 1 then inc(y);
                end;
        until done;
end;

procedure	gameEdit;
var	quit	: boolean;
	key	: word;
	c, r	: integer;
begin
	if not lores_init then
	begin
		info(SHADE_RED, 'THE EDITOR NEEDS AN EGA OR VGA CARD.');
		exit;
	end;
	c    := FIELD_W shr 1;
	r    := FIELD_H shr 1;
	quit := FALSE;
	repeat
		fieldDraw(GAME_COLONY);
        	cellular(GAME_COLONY, c, r, TRUE);
		lpanel(UI_EDIT, 'EDITING', 0, c, r);
		lores_flip;
		key := getkey_single;
		case key of
		KEY_SPACE:      begin
					toggle(GAME_COLONY, c, r);
					snd_play(SFX_CLICK);
				end;
                KEY_P	 : 	gamePlacePattern;
		KEY_ESC	:       quit := TRUE;
                KEY_CTRL_LEFT:  c := 0;
                KEY_CTRL_RIGHT:	c := FIELD_W-1;
                KEY_CTRL_UP:	r := 0;
                KEY_CTRL_DOWN:	r := FIELD_H-1;
		KEY_A,
		KEY_LEFT:       if c > 0 then dec(c) else c := FIELD_W-1;
		KEY_D,
		KEY_RIGHT:	if c < FIELD_W-1 then inc(c) else c := 0;
		KEY_W,
		KEY_UP	:	if r > 0 then dec(r) else r := FIELD_H-1;
		KEY_S,
		KEY_DOWN:	if r < FIELD_H-1 then inc(r) else r := 0;
		KEY_C	: 	begin
					empty(GAME_COLONY);
					snd_play(SFX_FALL);
                		end;
                KEY_F	: 	begin
                			fill(GAME_COLONY);
					snd_play(SFX_BLIP);
                		end;
                KEY_I	:	begin
                			invert(GAME_COLONY);
					snd_play(SFX_BLIP);
                		end;
                KEY_R	: 	begin
					init(GAME_COLONY);
					snd_play(SFX_JUMP);
                		end;
		end;
	until quit;
	lores_done;
        counterSetValue(cGen, 0);
end;

procedure	gameMenu;
var	choice	: word;		{ selected menu item }
	change	: boolean;	{ UI invalidate flag }
        clr, tmp: byte;		{ for color changing }
begin
        { paint the user interface }
	change := FALSE;
        gameInterface(UI_MENU);
        render(GAME_COLONY);

        { menu loop }
	repeat
        	if change then	{ force UI refresh }
                begin
                	gameInterface(UI_MENU);
		        menuModify(mOpt, CMD_OPT_ALIVE,  '     ALIVE: '+COLOR_NAMES[GAME_CONFIG.c_alive], FALSE, nil);
        		menuModify(mOpt, CMD_OPT_DEAD,   '     DEAD : '+COLOR_NAMES[GAME_CONFIG.c_dead],  FALSE, nil);
		        menuModify(mOpt, CMD_OPT_INVERT, ' '+menuOnOff(GAME_CONFIG.invert)+' INVERT COLORS ', FALSE, nil);
		        menuModify(mOpt, CMD_OPT_SOUND,  ' '+menuOnOff(GAME_CONFIG.sound)+' SOUND EFFECTS ',  FALSE, nil);
	                render(GAME_COLONY);
                        change := FALSE;
                end;

                { update status }
        	labelSetText(lStatus, MSG_READY);

                { execute the popup menu }
	        choice := menuPopup(mMain, SHADE_WHITE,
				    PANEL_X, PANEL_Y + 12,
				    MENU_TEXT_CLR, MENU_ACCENT_CLR, '');
                case choice of
                CMD_START:      begin
					gameRun;
                                        change := TRUE;
                		end;
                CMD_EDIT:	begin
					gameEdit;
                                        change := TRUE;
                		end;
                CMD_ABOUT:	gameAbout;
                CMD_HELP:	gameHelp;
                CMD_OPT_ALIVE:	begin
                			clr := colorPicker(CPF_ALL, PANEL_X + 4, PANEL_Y + 16,
							   GAME_CONFIG.c_alive, 'ALIVE');
                			if clr <> GAME_CONFIG.c_dead then
                                        begin
	                			GAME_CONFIG.c_alive := clr;
        	                                change := TRUE;
                                        end
                                        else	info(SHADE_RED, 'COLOR CONFLICTED');
                		end;
                CMD_OPT_DEAD:	begin
                			clr := colorPicker(CPF_ALL, PANEL_X + 4, PANEL_Y + 16,
							   GAME_CONFIG.c_dead, 'DEAD');
                			if clr <> GAME_CONFIG.c_alive then
                                        begin
	                			GAME_CONFIG.c_dead := clr;
        	                                change := TRUE;
                                        end
                                        else	info(SHADE_RED, 'COLOR CONFLICTED');
                		end;
                CMD_OPT_SOUND:	begin
                                        GAME_CONFIG.sound := not GAME_CONFIG.sound;
                                        SND_ENABLED := GAME_CONFIG.sound;
                                        change := TRUE;
				end;
                CMD_OPT_INVERT:	begin
                                        GAME_CONFIG.invert := not GAME_CONFIG.invert;
                                        tmp := GAME_CONFIG.c_alive;
                                        GAME_CONFIG.c_alive := GAME_CONFIG.c_dead;
                                        GAME_CONFIG.c_dead := tmp;
                                        change := TRUE;
				end;
                end;
        until choice = CMD_EXIT;
end;

{$endif}
