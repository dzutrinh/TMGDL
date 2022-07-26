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

	vscreen := vscreen_alloc;
	if vscreen = nil then
		die(-1, 'Not enough memory for double-buffering');

        if not config_load(FILE_CONFIG, GAME_CONFIG, sizeof(APP_CONFIG)) then
                GAME_CONFIG := APP_DEFAULT_CFG;

	randomize;

	GAME_COLONY := create;
	if not load(GAME_COLONY, FILE_COLONY) then init(GAME_COLONY);

	cursor(CURSOR_OFF);
        blink(BLINK_ON);

        mOpt := menuCreate;
        menuAppend(mOpt, '     ALIVE: '+COLOR_NAMES[GAME_CONFIG.c_alive], CMD_OPT_ALIVE,  FALSE, nil);
        menuAppend(mOpt, '     DEAD : '+COLOR_NAMES[GAME_CONFIG.c_dead],  CMD_OPT_DEAD,   FALSE, nil);
        menuAppend(mOpt, ' '+menuOnOff(GAME_CONFIG.invert)+' INVERT COLORS ',  CMD_OPT_INVERT, FALSE, nil);

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

        marqueeText(30, 23, 20, $03, 5, '   A cellular automaton simulation game   '#4+
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
	vscreen_free(vscreen);

	cursor(CURSOR_ON);
	ltx_done;

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

        case ui of
        UI_MENU:
		begin
	        	labelSetText(lStatus, MSG_READY);
        	end;
        UI_RUN:
		begin
                	labelSetText(lStatus, MSG_RUN);

                	shaded_box(PANEL_X + 13, PANEL_Y + 12,
			           13, 3, SHADE_BLUE, AF_LEFT, 'SIMUL.');

		  	shaded_box(PANEL_X, PANEL_Y + 15, PANEL_W, 9, SHADE_WHITE, AF_LEFT, 'SIMUL. CONTROLS');
                        print(PANEL_X + 2, PANEL_Y + 16, '~07.  ~0BSPACE: ~03STEP~07/~03AUTO');
                        print(PANEL_X + 2, PANEL_Y + 17, '~07.  ~0BENTER: ~03STEPPING');
                        print(PANEL_X + 2, PANEL_Y + 18, '~07.  ~0A    P: ~02PAUSE~07/~02RESUME');
                        print(PANEL_X + 2, PANEL_Y + 19, '~07.  ~0A    R: ~02RANDOMIZE');
                        print(PANEL_X + 2, PANEL_Y + 20, '~07.  ~0D LEFT: ~05FASTER');
                        print(PANEL_X + 2, PANEL_Y + 21, '~07.  ~0DRIGHT: ~05SLOWER');
                        print(PANEL_X + 2, PANEL_Y + 22, '~07.  ~0C  ESC: ~04BACK');
		end;
        UI_EDIT:
		begin
			labelSetText(lStatus, MSG_EDIT);

		  	shaded_box(PANEL_X, PANEL_Y + 15, PANEL_W, 9, SHADE_WHITE, AF_LEFT, 'EDITOR CONTROLS');
                        print(PANEL_X + 2, PANEL_Y + 16, '~07.  ~0BSPACE: ~03TOGGLE CELL');
                        print(PANEL_X + 2, PANEL_Y + 17, '~07.~0CW~07/~0ES~07/~0AA~07/~09D: ~02MOVE CURSOR');
                        print(PANEL_X + 2, PANEL_Y + 18, '~07.      ~0EP: ~06PATTERNS');
                        print(PANEL_X + 2, PANEL_Y + 19, '~07.      ~0DC: ~05CLEAR');
                        print(PANEL_X + 2, PANEL_Y + 20, '~07.      ~0DI: ~05INVERT MAP');
                        print(PANEL_X + 2, PANEL_Y + 21, '~07.      ~0DR: ~05RANDOMIZE');
                        print(PANEL_X + 2, PANEL_Y + 22, '~07.  ~0C  ESC: ~04BACK');
		end;
        UI_PLACE:
        	begin
			labelSetText(lStatus, MSG_PLACE);

		  	shaded_box(PANEL_X, PANEL_Y + 15, PANEL_W, 8, SHADE_WHITE, AF_LEFT, 'EDITOR CONTROLS');
                        print(PANEL_X + 2, PANEL_Y + 16, '~07.   ~0CW~07/~0ES~07/~0AA~07/~09D: ~02MOVE');
                        print(PANEL_X + 2, PANEL_Y + 17, '~07.       ~0BTAB: ~03NEXT');
                        print(PANEL_X + 2, PANEL_Y + 18, '~07. ~0BSHIFT~07-~0BTAB: ~03PREV');
                        print(PANEL_X + 2, PANEL_Y + 19, '~07.         ~0DC: ~05CLEAR');
                        print(PANEL_X + 2, PANEL_Y + 20, '~07.     ~0EENTER: ~06ACCEPT');
                        print(PANEL_X + 2, PANEL_Y + 21, '~07.     ~0C  ESC: ~04CANCEL');
                end;
        end;

        counterShow(cGen);
        labelShow(lStatus);

	gameShowVersion;
end;

procedure	gameRun;
var     quit	: boolean;
	key	: word;
        oldscr	: SCREEN;
	lSimul	: PLABEL;
	gSpeed	: PGAUGE;

	procedure	run_pause;
	begin
		labelSetText(lSimul, MSG_RUN_PAUSED);
		vscreen_flip(vscreen);
		if wait([#27, #13, 'P']) = #27 then quit := TRUE;
	end;

	procedure	run_randomize;
        var	i, j	: integer;
	begin
		labelSetText(lSimul, MSG_RUN_RANDOM);

		init(GAME_COLONY);
                for j:= 0 to FIELD_H-1 do
                begin
                	for i := 0 to FIELD_W-1 do
				cellular(GAME_COLONY, i, j, FALSE);
	                vsync;
        	        vscreen_flip(vscreen);
                end;
		counterReset(cGen);

		vsync;
		vscreen_flip(vscreen);
        	run_pause;
	end;

	procedure	run_render;
	begin
		vsync_sleep(SIMUL_SPEED_MAX-GAME_CONFIG.speed+1);
		vscreen_flip(vscreen);
	end;

        procedure	run_slower;
	begin
		if GAME_CONFIG.speed > SIMUL_SPEED_MIN then
        		dec(GAME_CONFIG.speed);
                gaugeSetValue(gSpeed, GAME_CONFIG.speed);
	end;

        procedure	run_faster;
	begin
		if GAME_CONFIG.speed < SIMUL_SPEED_MAX then
        		inc(GAME_CONFIG.speed);
                gaugeSetValue(gSpeed, GAME_CONFIG.speed);

	end;

begin
        oldscr 		:= vscreen_activate(vscreen);
	quit   		:= FALSE;

        gameInterface(UI_RUN);
        gSpeed  := gaugeCreate(GAME_CONFIG.speed, PANEL_X, PANEL_Y + 12, PANEL_W2 - 1,
        			SIMUL_SPEED_MIN, SIMUL_SPEED_MAX, GSF_HAS_BORDER,
				SHADE_BLUE, SHADE_TRAFFIC, 'SPEED');
	lSimul	:= labelCreate(PANEL_X + 15, PANEL_Y + PANEL_W2, PANEL_W2-4, '');
	labelShow(lSimul);
        gaugeShow(gSpeed);

	render(GAME_COLONY);
	vscreen_flip(vscreen);
	repeat
                render(GAME_COLONY);

                if GAME_CONFIG.step then
                	labelSetText(lSimul, MSG_RUN_STEPPING)
                else	labelSetText(lSimul, MSG_RUN_AUTO);

		run_render;

		if GAME_CONFIG.step then
		begin
                	repeat
				key := getkey;
                        until (key = KEY_ESC) or (key = KEY_SPACE) or
                              (key = KEY_ENTER) or (key = KEY_R);
                        case key of
                        KEY_ENTER:	begin
						evolve(GAME_COLONY);
				                counterIncrease(cGen);
                        		end;
                        KEY_ESC:        quit := TRUE;
			KEY_SPACE:	GAME_CONFIG.step := FALSE;
                        KEY_R:		run_randomize;
                	end;
		end
                else
		begin
			evolve(GAME_COLONY);
			counterIncrease(cGen);
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
        labelDestroy(lSimul);
        gaugeDestroy(gSpeed);
        vscreen_activate(oldscr);
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

procedure	gamePlacePattern;
var	i, j,
	x, y	: integer;
        done	: boolean;
        key	: word;
        p	: pchar;
        c	: byte;
        cCol,
        cRow	: PCOUNTER;

begin
	done	:= FALSE;
        x 	:= 0;
	y	:= 0;
	c	:= 1;

        cCol := counterCreate(x+1, PANEL_X, PANEL_Y + 12, PANEL_W div 2 - 1, 0, PANEL_W2, SHADE_WHITE, 'X');
        cRow := counterCreate(y+1, PANEL_X + PANEL_W2 + 1, PANEL_Y + 12, PANEL_W div 2 - 1, 0, PANEL_W2, SHADE_WHITE, 'Y');
        gameInterface(UI_PLACE);
        counterShow(cCol);
        counterShow(cRow);
        repeat
        	counterSetValue(cCol, x+1);
                counterSetValue(cRow, y+1);

                vsync;
		render(GAME_COLONY);

                { ensure the pattern will not be overdrawn on the panel }
                while x + PATTERNS_LIST[c].w - 1 >= FIELD_W do dec(x);
                while y + PATTERNS_LIST[c].h - 1 >= FIELD_H do dec(y);

               	block_put(@PATTERNS_LIST[c], x shl 1 + 1, y + 1, $8F);
                key := getkey;
                case key of
                KEY_SHIFT_TAB:	if c > 1 then dec(c) else c := PATTERNS_MAX;
                KEY_TAB:	if c < PATTERNS_MAX then inc(c) else c := 1;
                KEY_ESC:	done := TRUE;
                KEY_ENTER:	begin
					p := PATTERNS_LIST[c].data;
                			for j := 0 to PATTERNS_LIST[c].h-1 do
			                for i := 0 to PATTERNS_LIST[c].w-1 do
			                begin
                        			if p^ <> #0 then
				                	GAME_COLONY^.field[cell(x+i, y+j)] := C_ALIVE;
                        			inc(p);
			                end;
                                        done := TRUE;
				end;
		KEY_C: 		empty(GAME_COLONY);
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
        counterDestroy(cCol);
        counterDestroy(cRow);
end;

procedure	gameEdit;
var	change,
	quit	: boolean;
	key	: word;
	c, r	: integer;
        cCol,
        cRow	: PCOUNTER;

begin
	c    := FIELD_W shr 1;
	r    := FIELD_H shr 1;
        cCol := counterCreate(c+1, PANEL_X, PANEL_Y + 12, PANEL_W div 2 - 1, 0, PANEL_W2, SHADE_YELLOW, 'X');
        cRow := counterCreate(r+1, PANEL_X + PANEL_W2 + 1, PANEL_Y + 12, PANEL_W div 2 - 1, 0, PANEL_W2, SHADE_YELLOW, 'Y');
        counterReset(cGen);

        gameInterface(UI_EDIT);
        counterShow(cCol);
        counterShow(cRow);

	quit   := FALSE;
        change := FALSE;
	render(GAME_COLONY);
	repeat
        	counterSetValue(cCol, c+1);
                counterSetValue(cRow, r+1);
        	if change then
		begin
                        gameInterface(UI_EDIT);
                        counterShow(cCol);
                        counterShow(cRow);
			render(GAME_COLONY);
			change := FALSE;
                end;
        	cellular(GAME_COLONY, c, r, TRUE);
		key := getkey;
                cellular(GAME_COLONY, c, r, FALSE);
		case key of
		KEY_SPACE:      toggle(GAME_COLONY, c, r);
                KEY_P	 : 	begin
					gamePlacePattern;
                                        change := TRUE;
                		end;
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
                                        change := TRUE;
                		end;
                KEY_F	: 	begin
                			fill(GAME_COLONY);
                                        change := TRUE;
                		end;
                KEY_I	:	begin
                			invert(GAME_COLONY);
                                        change := TRUE;
                		end;
                KEY_R	: 	begin
					init(GAME_COLONY);
                                        change := TRUE;
                		end;
		end;
	until quit;

        counterDestroy(cCol);
        counterDestroy(cRow);
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
		        menuModify(mOpt, CMD_OPT_INVERT, ' '+menuOnOff(GAME_CONFIG.invert)+' INVERT   ',  FALSE, nil);
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
