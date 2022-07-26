{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   LIBUTIL.PAS                                                              }
{   -----------                                                              }
{   Utility library for text user interface game under DOS                   }
{   Can almost replace the CRT unit                                          }
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

unit    libutil;

interface

uses	libtext
{$ifdef	FPC}
	, go32
{$endif}
        {$ifdef __USE_DOS__}
        , dos
        {$endif}
        ;

{ ///////////////////////////// DEFINITIONS //////////////////////////////// }

const	LIBUTIL_VERSION	= $0100;	{ library version }
        LIBUTIL_BUILD	= '0714';	{ library build   }

const	TEXT_BUFSIZE	= 4096;
	TEXT_SPACES	= [#9, #10, #13, #32];

type    TEXT_FILE = ^TEXT_FILE_TYPE;
	TEXT_FILE_TYPE = record
        	fi	: TEXT;
                buffer	: array[1..TEXT_BUFSIZE] of byte;
        end;

type    STR_NAME	= string[8];	{ the name part of a file name      }
	STR_EXT		= string[4];    { the extension part of a file name }
        STR_BASENAME	= string[12];	{ base name = name + ext            }

const   STR_ELLIPSIS	: array[1..3] of char = '...';
        FOLDER_SEPARATOR: char = '\';	{ DOS folder separator              }
	C_ONOFF		: array[boolean] of array[1..3] of char =
        		('[ ]', '[x]');
        C_YESNO		: array[boolean] of array[1..3] of char =
        		('NO ', 'YES');

const   PATH_TOKEN_MAX	= 256;		{ for path tokenizer API            }

type    PATH_TOKEN_STACK = ^PATH_TOKEN_STACK_TYPE;
	PATH_TOKEN_STACK_TYPE = record
                tokens	: array[0..PATH_TOKEN_MAX-1] of STR_BASENAME;
                top	: integer;
	end;

var     { BIOS timer ticks for INT 1AH }
	ticks		: longint absolute $0040:$006C;

type
{$ifdef	__TMT__}
	CRC32_TYPE = dword;
{$else}
	CRC32_TYPE = longint;
{$endif}

{ ///////////////////////////// UTILITIES API ////////////////////////////// }

function	wait(k: KEYS): char;
function	query(shade: SHADING; mesg: string): boolean;
procedure	info(shade: SHADING; mesg: string);

{ ////////////////////////// CONFIGURATION API ///////////////////////////// }

function	config_load(filename: string; var buf; size: word): boolean;
function	config_save(filename: string; var buf; size: word): boolean;

{ //////////////////////// FILE/FOLDER/DISK API //////////////////////////// }

{$ifndef __USE_DOS__}
procedure	fsplit(filename: string; var dir, name, ext: string);
{$endif}

function	path_norm(dirname: string): string;
function	path_drive(dirname: string): string;
function	path_root(dirname: string): boolean;
function        path_shorten(path: string; width: integer): string;
function        path_folder(filename: string): string;
function        path_basename(filename: string): string;
function        path_filename(filename: string): string;
function        path_extension(filename: string): string;

function	match_wildcard(n1, n2: string): boolean;

function	folder_create(dirname: string): boolean;
function	folder_change(dirname: string): boolean;
function	folder_remove(dirname: string): boolean;
function	folder_current: string;
function	folder_exist(dirname: string): boolean;

function        drive_free(drv: byte): longint;
function        drive_size(drv: byte): longint;
function	drive_valid(drv: byte): boolean;

function	file_size(filename: string): longint;
function	file_exist(filename: string): boolean;
function	file_delete(filename: string): boolean;
function	file_rename(filename, newname: string): boolean;

function        path_tokenizer(dirname: string): PATH_TOKEN_STACK;
procedure	path_token_free(var stk: PATH_TOKEN_STACK);
function	path_token_size(const stk: PATH_TOKEN_STACK): integer;
function	path_token_push(var stk: PATH_TOKEN_STACK; name: STR_BASENAME): boolean;
function	path_token_pop(var stk: PATH_TOKEN_STACK): STR_BASENAME;
function	path_token_at(stk: PATH_TOKEN_STACK; idx: byte): STR_BASENAME;
function	path_token_full(const stk: PATH_TOKEN_STACK): boolean;
function	path_token_empty(const stk: PATH_TOKEN_STACK): boolean;

{ ////////////////////////// EASY TEXT FILE API //////////////////////////// }

function        text_open(filename: string): TEXT_FILE;
procedure	text_close(var tf: TEXT_FILE);
function	text_rewind(var tf: TEXT_FILE): boolean;
function	text_eof(var tf: TEXT_FILE): boolean;
function	text_readln(var tf: TEXT_FILE): string;
function	text_getline(var tf: TEXT_FILE): string;
function	text_gets(var tf: TEXT_FILE; sect, key: string): string;
function	text_getl(var tf: TEXT_FILE; sect, key: string): longint;

{ /////////////////////////////// CRC API ////////////////////////////////// }

function	crc32_init: CRC32_TYPE;
function	crc32_byte(crc: CRC32_TYPE; c: byte): CRC32_TYPE;
function	crc32_block(crc: CRC32_TYPE; var buf; len: longint): CRC32_TYPE;
function	crc32_final(crc: CRC32_TYPE): CRC32_TYPE;

implementation

const	CRC32_TABLE	: array [0..255] Of CRC32_TYPE = (
		$00000000,$77073096,$ee0e612c,$990951ba,$076dc419,$706af48f,$e963a535,$9e6495a3,
   		$0edb8832,$79dcb8a4,$e0d5e91e,$97d2d988,$09b64c2b,$7eb17cbd,$e7b82d07,$90bf1d91,
		$1db71064,$6ab020f2,$f3b97148,$84be41de,$1adad47d,$6ddde4eb,$f4d4b551,$83d385c7,
		$136c9856,$646ba8c0,$fd62f97a,$8a65c9ec,$14015c4f,$63066cd9,$fa0f3d63,$8d080df5,
   		$3b6e20c8,$4c69105e,$d56041e4,$a2677172,$3c03e4d1,$4b04d447,$d20d85fd,$a50ab56b,
   		$35b5a8fa,$42b2986c,$dbbbc9d6,$acbcf940,$32d86ce3,$45df5c75,$dcd60dcf,$abd13d59,
   		$26d930ac,$51de003a,$c8d75180,$bfd06116,$21b4f4b5,$56b3c423,$cfba9599,$b8bda50f,
  		$2802b89e,$5f058808,$c60cd9b2,$b10be924,$2f6f7c87,$58684c11,$c1611dab,$b6662d3d,
   		$76dc4190,$01db7106,$98d220bc,$efd5102a,$71b18589,$06b6b51f,$9fbfe4a5,$e8b8d433,
   		$7807c9a2,$0f00f934,$9609a88e,$e10e9818,$7f6a0dbb,$086d3d2d,$91646c97,$e6635c01,
   		$6b6b51f4,$1c6c6162,$856530d8,$f262004e,$6c0695ed,$1b01a57b,$8208f4c1,$f50fc457,
   		$65b0d9c6,$12b7e950,$8bbeb8ea,$fcb9887c,$62dd1ddf,$15da2d49,$8cd37cf3,$fbd44c65,
   		$4db26158,$3ab551ce,$a3bc0074,$d4bb30e2,$4adfa541,$3dd895d7,$a4d1c46d,$d3d6f4fb,
  	 	$4369e96a,$346ed9fc,$ad678846,$da60b8d0,$44042d73,$33031de5,$aa0a4c5f,$dd0d7cc9,
   		$5005713c,$270241aa,$be0b1010,$c90c2086,$5768b525,$206f85b3,$b966d409,$ce61e49f,
   		$5edef90e,$29d9c998,$b0d09822,$c7d7a8b4,$59b33d17,$2eb40d81,$b7bd5c3b,$c0ba6cad,
   		$edb88320,$9abfb3b6,$03b6e20c,$74b1d29a,$ead54739,$9dd277af,$04db2615,$73dc1683,
   		$e3630b12,$94643b84,$0d6d6a3e,$7a6a5aa8,$e40ecf0b,$9309ff9d,$0a00ae27,$7d079eb1,
   		$f00f9344,$8708a3d2,$1e01f268,$6906c2fe,$f762575d,$806567cb,$196c3671,$6e6b06e7,
   		$fed41b76,$89d32be0,$10da7a5a,$67dd4acc,$f9b9df6f,$8ebeeff9,$17b7be43,$60b08ed5,
   		$d6d6a3e8,$a1d1937e,$38d8c2c4,$4fdff252,$d1bb67f1,$a6bc5767,$3fb506dd,$48b2364b,
   		$d80d2bda,$af0a1b4c,$36034af6,$41047a60,$df60efc3,$a867df55,$316e8eef,$4669be79,
   		$cb61b38c,$bc66831a,$256fd2a0,$5268e236,$cc0c7795,$bb0b4703,$220216b9,$5505262f,
   		$c5ba3bbe,$b2bd0b28,$2bb45a92,$5cb36a04,$c2d7ffa7,$b5d0cf31,$2cd99e8b,$5bdeae1d,
   		$9b64c2b0,$ec63f226,$756aa39c,$026d930a,$9c0906a9,$eb0e363f,$72076785,$05005713,
   		$95bf4a82,$e2b87a14,$7bb12bae,$0cb61b38,$92d28e9b,$e5d5be0d,$7cdcefb7,$0bdbdf21,
   		$86d3d2d4,$f1d4e242,$68ddb3f8,$1fda836e,$81be16cd,$f6b9265b,$6fb077e1,$18b74777,
   		$88085ae6,$ff0f6a70,$66063bca,$11010b5c,$8f659eff,$f862ae69,$616bffd3,$166ccf45,
   		$a00ae278,$d70dd2ee,$4e048354,$3903b3c2,$a7672661,$d06016f7,$4969474d,$3e6e77db,
   		$aed16a4a,$d9d65adc,$40df0b66,$37d83bf0,$a9bcae53,$debb9ec5,$47b2cf7f,$30b5ffe9,
   		$bdbdf21c,$cabac28a,$53b39330,$24b4a3a6,$bad03605,$cdd70693,$54de5729,$23d967bf,
   		$b3667a2e,$c4614ab8,$5d681b02,$2a6f2b94,$b40bbe37,$c30c8ea1,$5a05df1b,$2d02ef8d
	);

        { configuration file header }
	CONFIG_MAGIC	: array[1..2] of char = 'CF';

{ wait for a set of keys to be input from keyboard }
function	wait(k: KEYS): char;
var	ch	: char;
begin
	repeat
		ch := upcase(readkey);
	until ch in k;
	wait := ch;
end;

{ open a Y/N-based query message box }
function	query(shade: SHADING; mesg: string): boolean;
var	value	: boolean;
	dlg	: PDIALOG;
begin
	dlg := dialog_open(-1, -1, -1, -1, shade, '', mesg);
	value := wait(['Y','N',#13,#27]) in ['Y',#13];
        dialog_close(dlg);
	query := value;
end;

{ open a information message box }
procedure	info(shade: SHADING; mesg: string);
var	dlg	: PDIALOG;
begin
        dlg := dialog_open(-1, -1, -1, -1, shade, '', mesg);
	wait([#13,#27,#32]);
	dialog_close(dlg);
end;

{ load configuration from file }
function	config_load(filename: string; var buf; size: word): boolean;
var	f	: FILE;
	nr	: word;
        hdr	: array[1..2] of char;
begin
	config_load := FALSE;
        assign(f, filename);
        {$I-}
        reset(f, 1);
        {$I+}
        if ioresult <> 0 then exit;

        blockread(f, hdr, sizeof(CONFIG_MAGIC), nr);
        if (nr <> sizeof(CONFIG_MAGIC)) or (hdr <> CONFIG_MAGIC) then
        begin
        	close(f);
                exit;
        end;

        blockread(f, buf, size, nr);
        if nr <> size then
        begin
        	close(f);
                exit;
        end;

        close(f);
        config_load := TRUE;
end;

{ save configuration block to file }
function	config_save(filename: string; var buf; size: word): boolean;
var	f	: FILE;
        nw	: word;
begin
	config_save := FALSE;
        assign(f, filename);
        {$I-}
        rewrite(f, 1);
        {$I+}
        if ioresult <> 0 then exit;

        blockwrite(f, CONFIG_MAGIC, sizeof(CONFIG_MAGIC), nw);
        if nw <> sizeof(CONFIG_MAGIC) then
        begin
        	close(f);
                exit;
        end;

        blockwrite(f, buf, size, nw);
        if nw <> size then
        begin
        	close(f);
                exit;
        end;

        close(f);
        config_save := TRUE;
end;

{$ifndef __USE_DOS__}

{ DOS alternative version of fsplit() }
procedure	fsplit(filename: string; var dir, name, ext: string);
var	i, count: integer;
        temp	: string;
        ps	: PATH_TOKEN_STACK;
begin
	dir   := '';
        name  := '';
        ext   := '';

        temp := filename;
        ps   := path_tokenizer(temp);

        if filename[length(filename)] = '\' then
		dir := filename
        else
	begin
        	count := path_token_size(ps);
        	for i := 0 to count-2 do
			dir := dir + path_token_at(ps, i) + '\';
		name := path_token_at(ps, count-1);
                i := pos('.', name);
                if i <> 0 then
                begin
                	ext := copy(name, i, length(name));
                        delete(name, i, length(name));
                end;
        end;
	path_token_free(ps);
end;
{$endif}

{ return the drive part of a path }
function	path_drive(dirname: string): string;
begin
	if (upcase(dirname[1]) in ['A'..'Z']) and
           (dirname[2] = ':') then
        	path_drive := upcase(dirname[1]) + dirname[2]
        else	path_drive := '';
end;

{ normalize a path string }
function	path_norm(dirname: string): string;
begin
	if dirname[length(dirname)] = '\' then dec(dirname[0]);
        path_norm := dirname;
end;

{ TRUE if the path is root }
function	path_root(dirname: string): boolean;
begin
	path_root := (dirname[0] = #03) and
		     ((dirname[1] >= 'A') and (dirname[1] <= 'Z')) and
		     (dirname[2] = ':') and
		     (dirname[3] = '\');
end;

{ fit the path into 'width' characters }
function        path_shorten(path: string; width: integer): string;
var	first_f	: boolean;
        tmp,
	first,
        last	: string;
	first_i,
        last_i,
        idx, i,
	tokens	: integer;
        ps	: PATH_TOKEN_STACK;
begin
	path_shorten := path;
	if length(path) < width then exit;
        if width <= 3 then
	begin
		path_shorten := STR_ELLIPSIS;
                exit;
        end;

        first_f:= true;
        first	:= '';
        last	:= '';
        first_i	:= 0;
        last_i	:= 0;
        tokens	:= 0;
	ps	:= path_tokenizer(path);
        tokens	:= path_token_size(ps);

        for i := 0 to tokens-1 do
        begin
        	if first_f then
                begin
                	tmp   := path_token_at(ps, first_i) + FOLDER_SEPARATOR;
                        if length(first) + length(last) + length(tmp) + 3 > width then break;
			first := first + tmp;
                        if tmp <> FOLDER_SEPARATOR then first_f := FALSE;
                        inc(first_i);
                end
                else
                begin
                	idx := tokens - last_i - 1;
                        tmp := FOLDER_SEPARATOR + path_token_at(ps, idx);
                        if length(first) + length(last) + length(tmp) + 3 > width then break;
                        last := tmp + last;
                        if tmp <> FOLDER_SEPARATOR then first_f := TRUE;
                        inc(last_i);
                end;
        end;

        if last = '' then
        begin
        	last := path_token_at(ps, tokens-1);
                last := copy(last,
			     length(last)+3+length(first)-width,
			     width-3-length(first));
        end;
        path_token_free(ps);
	path_shorten := first + STR_ELLIPSIS + last;
end;

{ return the folder part from the path }
function        path_folder(filename: string): string;
var
{$ifdef __USE_DOS__}
	ds	: DIRSTR;
        ns	: NAMESTR;
        es	: EXTSTR;
{$else}
	ds, ns, es	: string;
{$endif}
begin
	fsplit(filename, ds, ns, es);
        path_folder := ds;
end;

{ return the base name part from the path }
function        path_basename(filename: string): string;
var
{$ifdef __USE_DOS__}
	ds	: DIRSTR;
        ns	: NAMESTR;
        es	: EXTSTR;
{$else}
	ds, ns, es	: string;
{$endif}
begin
	fsplit(filename, ds, ns, es);
        path_basename := ns + es;
end;

{ return the file name part only from the path }
function        path_filename(filename: string): string;
var
{$ifdef __USE_DOS__}
	ds	: DIRSTR;
        ns	: NAMESTR;
        es	: EXTSTR;
{$else}
	ds, ns, es	: string;
{$endif}
begin
	fsplit(filename, ds, ns, es);
        path_filename := ns;
end;

{ return the extension part from the path }
function        path_extension(filename: string): string;
var
{$ifdef __USE_DOS__}
	ds	: DIRSTR;
        ns	: NAMESTR;
        es	: EXTSTR;
{$else}
	ds, ns, es	: string;
{$endif}
begin
	fsplit(filename, ds, ns, es);
        path_extension := es;
end;

{ wildcard matching }
function	match_wildcard(n1, n2: string): boolean;
var	p1, p2	: integer;
	match	: boolean;
begin
	p1    := 1;
	p2    := 1;
	match := TRUE;

	if (length(n1) = 0) and (length(n2) = 0) then match := TRUE
	else
	if length(n1) = 0 then
		if n2[1] = '*' then
			match := TRUE
		else	match := FALSE
	else
	if length(n2) = 0 then
		if n1[1] = '*' then
			match := TRUE
		else	match := FALSE;

	while (match = TRUE) and (p1 <= length(n1)) and (p2 <= length(n2)) do
	      	if (n1[p1] = '?') or (n2[p2] = '?') then
		begin
			inc(p1);
			inc(p2);
		end
		else
			if n1[p1] = '*' then
			begin
				inc(p1);
				if p1 <= length(n1) then
				begin
					while (p2 <= length(n2)) and
					      not match_wildcard(copy(n1, p1, length(n1)-p1+1),
					                         copy(n2, p2, length(n2)-p2+1)) do
					      inc(p2);
					if p2 > length(n2) then match := FALSE
					else
					begin
						p1 := succ(length(n1));
						p2 := succ(length(n2));
					end;
				end
				else	p2 := succ(length(n2));
			end
			else
			if n2[p2] = '*' then
			begin
				inc(p2);
				if p2 <= length(n2) then
				begin
					while (p1 <= length(n1)) and
					      not match_wildcard(copy(n1, p1, length(n1)-p1+1),
					                         copy(n2, p2, length(n2)-p2+1)) do
					      inc(p1);
					if p1 > length(n1) then match := FALSE
					else
					begin
						p1 := succ(length(n1));
						p2 := succ(length(n2));
					end;
				end
				else	p1 := succ(length(n1));
			end
			else
			if upcase(n1[p1]) = upcase(n2[p2]) then
			begin
				inc(p1);
				inc(p2);
			end
			else	match := FALSE;

	if p1 > length(n1) then
	begin
		while (p2 <= length(n2)) and (n2[p2] = '*') do inc(p2);
		if p2 <= length(n2) then match := FALSE;
	end;

	if p2 > length(n2) then
	begin
		while (p1 <= length(n1)) and (n1[p1] = '*') do inc(p1);
		if p1 <= length(n1) then match := FALSE;
	end;

	match_wildcard := match;
end;

{ recursively create a series of folders }
function	folder_create(dirname: string): boolean;
var	i, start,
	count	: integer;
        ret	: boolean;
        ps	: PATH_TOKEN_STACK;
begin
	folder_create := FALSE;
        if dirname = '' then exit;

	dirname := path_norm(dirname);
        ps	:= path_tokenizer(dirname);
        count	:= path_token_size(ps);

        dirname := '';
        if pos(':', path_token_at(ps, 0)) <> 0 then
        begin
        	dirname := path_token_at(ps, 0) + FOLDER_SEPARATOR;
                start := 1;
        end
        else	start := 0;

        for i := start to count-1 do
	begin
		dirname := dirname + path_token_at(ps, i);
                if dirname <> '' then
                begin
                	{$I-}
                        mkdir(dirname);
                        {$I+}
                        ret := ioresult = 0;
                end;
                dirname := dirname + FOLDER_SEPARATOR;
        end;
        path_token_free(ps);
	folder_create := ret;
end;

{ change folder and drive if needed }
function	folder_change(dirname: string): boolean;
begin
        {$I-}
        chdir(dirname);
        {$I+}
        folder_change := ioresult = 0;
end;

{ remove an empty folder }
function	folder_remove(dirname: string): boolean;
begin
	{$I-}
        rmdir(dirname);
        {$I+}
        folder_remove := ioresult = 0;
end;

{ return the current folder }
function	folder_current: string;
var	folder	: string;
begin
        getdir(0, folder);
        folder_current := folder;
end;

{ check if a folder is already existed }
function	folder_exist(dirname: string): boolean;
var     old	: string;
begin
	old := folder_current;
        folder_exist := folder_change(dirname);
        folder_change(old);
end;

{ get disk free space }
function        drive_free(drv: byte): longint;
var
	a, b, c	: word;
        size	: longint;
begin
	asm
		mov	ah, $36
                mov	dl, drv
                int	$21

                mov	a, ax
                mov	b, bx
                mov	c, cx
        end;

	if a = $FFFF then
		size := -1
	else	size := longint(a) * b * c;
        drive_free := size;
end;

{ get disk capacity }
function        drive_size(drv: byte): longint;
var	a, c, d	: word;
        size	: longint;
begin
	asm
		mov	ah, $36
                mov	dl, drv
                int	$21

                mov	a, ax
                mov	c, cx
                mov	d, dx
        end;

	if a = $FFFF then
		size := -1
	else	size := longint(a) * c * d;
        drive_size := size;
end;

{ check if a drive is valid }
function	drive_valid(drv: byte): boolean;
begin
	drive_valid := drive_free(drv) > 0;
end;

{ return the file size in bytes }
function	file_size(filename: string): longint;
var	f	: FILE;
	size	: longint;
begin
	{$I-}
	assign(f, filename);
	filemode := 0;
	reset(f, 1);
        if ioresult <> 0 then size := -1 else size := filesize(f);
	close(f);
	{$I+}
	file_size := size;
end;

{ check for the existing file }
function	file_exist(filename: string): boolean;
var	f	: FILE;
begin
	{$I-}
	assign(f, filename);
	filemode := 0;
	reset(f);
	close(f);
	{$I+}
	file_exist := (ioresult = 0) and (filename <> '');
end;

{ delete a file using given file name }
function	file_delete(filename: string): boolean;
var	f	: FILE;
begin
	assign(f, filename);
        {$I-}
        erase(f);
        {$I+}
        file_delete := ioresult = 0;
end;

{ rename or move a file }
function	file_rename(filename, newname: string): boolean;
var	f	: FILE;
begin
	assign(f, filename);
        {$I-}
        rename(f, newname);
        {$I+}
        file_rename := ioresult = 0;
end;

{ convert a path into tokens, must call path_token_free() after use }
function        path_tokenizer(dirname: string): PATH_TOKEN_STACK;
var	stk	: PATH_TOKEN_STACK;
begin
	dirname := path_norm(dirname);
	new(stk);
        stk^.top := 0;
        while (dirname <> '') and not path_token_full(stk) do
                path_token_push(stk, tokenize(dirname, FOLDER_SEPARATOR));
	path_tokenizer := stk;
end;

function	path_token_full(const stk: PATH_TOKEN_STACK): boolean;
begin
	path_token_full := stk^.top >= PATH_TOKEN_MAX;
end;

function	path_token_empty(const stk: PATH_TOKEN_STACK): boolean;
begin
        path_token_empty := stk^.top = 0;
end;

function	path_token_push(var stk: PATH_TOKEN_STACK; name: STR_BASENAME): boolean;
begin
	path_token_push := FALSE;
        if stk = nil then exit;
        stk^.tokens[stk^.top] := name;
        inc(stk^.top);
        path_token_push := TRUE;
end;

function	path_token_pop(var stk: PATH_TOKEN_STACK): STR_BASENAME;
begin
	path_token_pop := '';
	if stk = nil then exit;
        dec(stk^.top);
        path_token_pop := stk^.tokens[stk^.top];
end;

function	path_token_at(stk: PATH_TOKEN_STACK; idx: byte): STR_BASENAME;
begin
	path_token_at := '';
        if stk = nil then exit;
        if idx >= stk^.top then exit;
	path_token_at := stk^.tokens[idx];
end;

function	path_token_size(const stk: PATH_TOKEN_STACK): integer;
begin
        path_token_size := stk^.top;
end;

procedure	path_token_free(var stk: PATH_TOKEN_STACK);
begin
        dispose(stk);
end;

{ text file reader }

function        text_open(filename: string): TEXT_FILE;
var	tf	: TEXT_FILE;
begin
	text_open := nil;
        if ltx_memavail < sizeof(TEXT_FILE_TYPE) then exit;
        new(tf);
        assign(tf^.fi, filename);
        settextbuf(tf^.fi, tf^.buffer);
        {$I-}
        reset(tf^.fi);
        {$I+}
        if ioresult <> 0 then
        begin
        	dispose(tf);
                exit;
        end;
	text_open := tf;
end;

procedure	text_close(var tf: TEXT_FILE);
begin
        close(tf^.fi);
        dispose(tf);
end;

function	text_rewind(var tf: TEXT_FILE): boolean;
begin
	if tf = nil then exit;
        {$I-}
        reset(tf^.fi);
        {$I+}
        if ioresult <> 0 then text_rewind := FALSE else text_rewind := TRUE;
end;

function	text_eof(var tf: TEXT_FILE): boolean;
begin
	text_eof := eof(tf^.fi);
end;

function	text_readln(var tf: TEXT_FILE): string;
var     s	: string;
begin
	if tf = nil then exit;
        readln(tf^.fi, s);
        text_readln := s;
end;

function	text_getline(var tf: TEXT_FILE): string;
var	tmp, s	: string;
	i, epos	: integer;
begin
	if tf = nil then exit;
	repeat
		s := text_readln(tf);
	       	if s <> '' then while s[1] in TEXT_SPACES do delete(s, 1, 1);
	until	(s[1] <> ';') and (s <> '');

	if s[1] = '[' then
	begin
        	while s[1] in TEXT_SPACES do delete(s, 1, 1);
		if s[length(s)] <> ']' then s := s + ']';
	end
	else
	begin
		epos := pos('=', s);
		if epos <> 0 then
		begin
			tmp := '';
			for i := 1 to epos do
				if not (s[i] in TEXT_SPACES) then
					tmp := tmp + s[i];
			inc(epos);
			while s[epos] in TEXT_SPACES do inc(epos);
			tmp := tmp + copy(s, epos, length(s));
			s := tmp;
		end;
	end;
	text_getline := s;
end;

function	text_gets(var tf: TEXT_FILE; sect, key: string): string;
var	value, s,
	lhs, rhs: string;
	epos, i	: longint;
	sfound,
	kfound	: boolean;
begin
	value	:= '';
	sfound	:= FALSE;
	kfound	:= FALSE;

        text_rewind(tf);

        for i := 1 to length(sect) do sect[i] := upcase(sect[i]);
        for i := 1 to length(key) do key[i] := upcase(key[i]);
	while not sfound and not text_eof(tf) do
	begin
		s := text_getline(tf);
                repeat
                        i := pos(' ', s); if i <> 0 then delete(s, i, 1);
                until i = 0;
                repeat
                        i := pos(#9, s); if i <> 0 then delete(s, i, 1);
                until i = 0;
                for i := 1 to length(s) do s[i] := upcase(s[i]);
		if s = '['+sect+']' then sfound := TRUE;
	end;

	if sfound then
	repeat
		s := text_getline(tf);
		epos := pos('=', s);
		lhs := copy(s, 1, epos-1);
                for i := 1 to length(lhs) do lhs[i] := upcase(lhs[i]);
		if lhs = key then
		begin
			rhs := copy(s, epos+1, length(s));
			kfound := TRUE;
		end;
	until kfound or text_eof(tf);
	if sfound and kfound then value := rhs;

	text_gets := value;
end;

function	text_getl(var tf: TEXT_FILE; sect, key: string): longint;
begin
	text_getl := atol(text_gets(tf, sect, key));
end;

function	crc32_init: CRC32_TYPE;
begin
	crc32_init := -1;	 { $FFFFFFFF }
end;

function	crc32_byte(crc: CRC32_TYPE; c: byte): CRC32_TYPE;
begin
	crc32_byte := CRC32_TABLE[byte(crc xor c)] xor ((crc shr 8) and $00FFFFFF);
end;

function	crc32_block(crc: CRC32_TYPE; var buf; len: longint): CRC32_TYPE;
var     p	: ^byte;
        i	: longint;
        sum	: CRC32_TYPE;
begin
        sum	:= crc;
        p	:= @buf;
        for i := 0 to len-1 do
        begin
                sum := crc32_byte(sum, p^);
        	inc(p);
        end;
	crc32_block := sum;
end;

function	crc32_final(crc: CRC32_TYPE): CRC32_TYPE;
begin
	crc32_final := not crc;
end;

end.
