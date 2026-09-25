{$i preps.inc}

unit	libpack;

interface

uses	libtext, libutil;

const   PKGFILE_MAX	= 1024;
	PKGFILE_BUFSIZE	= 1024;

type	PKGFILE_SIGNATURE = array[0..3] of char;
        PKGFILE_BUFFER  = array[1..PKGFILE_BUFSIZE] of byte;

{ on-disk records are packed so every compiler writes the same layout }
type	PKGFILE_ENTRY	= packed record	{ a file table entry, consists of }
		name	: string[12];	{ file name }
                size	: longint;	{ its size in bytes }
                offset	: longint;	{ offset inside package file }
	end;

        PKGFILE_HEADER = packed record
        	magic	: PKGFILE_SIGNATURE;
                count   : word;
                crc32	: CRC32_TYPE;
        end;

        { appended after the package in a self-extracting executable }
        PKGFILE_SFX_TRAILER = packed record
        	magic	: PKGFILE_SIGNATURE;
        	base	: longint;	{ where the package starts in the file }
        end;

        PKGFILE_LIST = ^PKGFILE_ARRAY;
        PKGFILE_ARRAY = array[1..PKGFILE_MAX] of PKGFILE_ENTRY;
        PKGFILE_SOURCES = array[1..PKGFILE_MAX] of longint;

        PKGFILE_MODE = (PFM_CREATE,	{ create package file }
			PFM_OPEN);	{ use package file    }

        PACKAGE = ^PKGFILE_TABLE;
        PKGFILE_TABLE	= record
                hdr	  : PKGFILE_HEADER;
                case mode : PKGFILE_MODE of
                PFM_CREATE: (table  : PKGFILE_ARRAY;
                	     { data of entry i: a disk file when source[i] < 0, }
                	     { else source[i] bytes into package srcname       }
                	     source : PKGFILE_SOURCES;
                	     srcname: string[79];
                	     srcbase: longint);
                PFM_OPEN  : (list  : PKGFILE_LIST;
			     size  : longint;
			     handle: FILE;
			     base  : longint);	{ package start inside the file }
        end;

type	PKGFILE_CB_MODE = (CB_FILE, CB_DISK);
	PKGFILE_CB_FILE	= procedure(const entry: PKGFILE_ENTRY);
	PKGFILE_CB_DISK = procedure(current, total: longint);

{ package file creator and packer }
function        package_init: PACKAGE;
function	package_load(filename: string): PACKAGE;
function	package_add(var pkg: PACKAGE; filename: string): boolean;
function	package_delete(var pkg: PACKAGE; filename: string): boolean;
function	package_pack(var pkg: PACKAGE; pkgname: string): boolean;

{ package file utilizations }
function	package_valid(filename: string): boolean;
function        package_open(filename: string): PACKAGE;
function        package_open_at(filename: string; base, len: longint): PACKAGE;
function	package_lookup(const pkg: PACKAGE; filename: string): longint;
function	package_search(const pkg: PACKAGE; filename: string;
			       var entry: PKGFILE_ENTRY): boolean;
function        package_extract(var pkg: PACKAGE;
			        filename, destdir: string): boolean;
function        package_verify(const pkg: PACKAGE): CRC32_TYPE;
function	package_checksum(const pkg: PACKAGE): CRC32_TYPE;
function	package_aborted: boolean;

{ self-extracting packages: a stub executable with the package appended }
function	package_make_sfx(pkgname, stubname, exename: string): boolean;
function	package_open_sfx(exename: string): PACKAGE;

{ package file clean up }
procedure	package_shutdown(var pkg: PACKAGE);

{ helpers }
procedure	package_callback_assign(mode: PKGFILE_CB_MODE; cb: pointer);
procedure	package_callback_file(const entry: PKGFILE_ENTRY);
procedure	package_callback_disk(current, total: longint);
procedure	package_callback_detach(mode: PKGFILE_CB_MODE);

implementation

{*--------------------------------------------------------------------------*}
const   CALLBACK_FILE	: PKGFILE_CB_FILE = nil;
	CALLBACK_DISK	: PKGFILE_CB_DISK = nil;

	PKGFILE_MAGIC	: PKGFILE_SIGNATURE = 'PACK';
	PKGFILE_SFX_MAGIC: PKGFILE_SIGNATURE = 'PKSX';

	{ set when the user cancelled the last package_pack/package_verify }
	PKGFILE_ABORTED	: boolean = FALSE;


{ an entry name may not leave the destination folder or name a device }
function	package_safe_name(const name: string): boolean;
var	i	: integer;
begin
	package_safe_name := FALSE;
	if (name = '') or (name = '.') or (name = '..') then exit;
	for i := 1 to length(name) do
		if name[i] in ['\', '/', ':', #0..#31] then exit;
	package_safe_name := TRUE;
end;

{ index of an entry in a package being built, 0 if absent }
function	package_find(const pkg: PACKAGE; filename: string): integer;
var	i	: integer;
begin
	package_find := 0;
	filename := toupper(filename);
	for i := 1 to pkg^.hdr.count do
		if toupper(pkg^.table[i].name) = filename then
		begin
			package_find := i;
			exit;
		end;
end;

{ open a file read-only, whatever FileMode the caller left }
function	package_reset(var f: FILE; filename: string): boolean;
var	oldmode	: longint;
begin
	assign(f, filename);
	oldmode  := filemode;
	filemode := 0;
	{$I-}
	reset(f, 1);
	{$I+}
	filemode := oldmode;
	package_reset := ioresult = 0;
end;

function        package_init: PACKAGE;
var	pkg	: PACKAGE;
begin
	package_init := nil;
	if ltx_maxavail < sizeof(PKGFILE_TABLE) then exit;
	new(pkg);

        with pkg^ do
        begin
        	hdr.magic := PKGFILE_MAGIC;
        	hdr.count := 0;
                hdr.crc32 := crc32_init;
                mode      := PFM_CREATE;
                fillchar(table, sizeof(PKGFILE_ARRAY), 0);
                srcname   := '';
                srcbase   := 0;
        end;

        package_init := pkg;
end;

{ a package being built that starts with the content of an existing one, }
{ so files can be added, replaced or deleted and the result packed again }
function	package_load(filename: string): PACKAGE;
var	src,
	pkg	: PACKAGE;
	i	: integer;
begin
	package_load := nil;
	src := package_open(filename);
	if src = nil then exit;
	pkg := package_init;
	if pkg <> nil then
	with pkg^ do
	begin
		hdr.count := src^.hdr.count;
		for i := 1 to hdr.count do
		begin
			table[i]  := src^.list^[i];
			source[i] := src^.list^[i].offset;
		end;
		srcname := filename;
		srcbase := src^.base;
	end;
	package_shutdown(src);
	package_load := pkg;
end;

function	package_valid(filename: string): boolean;
var	f	: FILE;
	sig	: PKGFILE_SIGNATURE;
	nr	: word;
begin
        package_valid := FALSE;
        if not package_reset(f, filename) then exit;
        {$I-}
        blockread(f, sig, sizeof(PKGFILE_SIGNATURE), nr);
        package_valid := (ioresult = 0) and
        		 (nr = sizeof(PKGFILE_SIGNATURE)) and (sig = PKGFILE_MAGIC);
        close(f);
        if ioresult <> 0 then ;
        {$I+}
end;

function        package_open(filename: string): PACKAGE;
begin
	package_open := package_open_at(filename, 0, -1);
end;

{ open a package stored 'base' bytes into a file, 'len' bytes long }
{ (len < 0: up to the end of the file)                              }
function        package_open_at(filename: string; base, len: longint): PACKAGE;
var	pkg	: PACKAGE;
	i	: integer;
	nr	: word;
	ok	: boolean;
begin
	package_open_at := nil;

	if ltx_maxavail < sizeof(PKGFILE_TABLE) then exit;
	new(pkg);
	pkg^.mode := PFM_OPEN;
	pkg^.list := nil;
	pkg^.base := base;
	if not package_reset(pkg^.handle, filename) then
	begin
        	dispose(pkg);
                exit;
        end;

        {$I-}
        if len < 0 then len := filesize(pkg^.handle) - base;
        ok := (base >= 0) and (len >= sizeof(PKGFILE_HEADER)) and
              (base + len <= filesize(pkg^.handle));
        if ok then
        begin
	        seek(pkg^.handle, base);
	        blockread(pkg^.handle, pkg^.hdr, sizeof(PKGFILE_HEADER), nr);
	        ok := (ioresult = 0) and (nr = sizeof(PKGFILE_HEADER)) and
		      (pkg^.hdr.magic = PKGFILE_MAGIC) and
		      (pkg^.hdr.count <= PKGFILE_MAX);
        end;
        if ok then
        begin
	        pkg^.size := sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
	        ok := pkg^.size <= len - sizeof(PKGFILE_HEADER);
        end;
        if ok then
        begin
	        pkg^.list := ltx_malloc(pkg^.size);
	        ok := (pkg^.list <> nil) or (pkg^.hdr.count = 0);
        end;
        i := 1;
        while ok and (i <= pkg^.hdr.count) do
        begin
        	blockread(pkg^.handle, pkg^.list^[i], sizeof(PKGFILE_ENTRY), nr);
        	{ offsets are relative to the package start }
        	ok := (ioresult = 0) and (nr = sizeof(PKGFILE_ENTRY)) and
        	      package_safe_name(pkg^.list^[i].name) and
        	      (pkg^.list^[i].size >= 0) and
        	      (pkg^.list^[i].offset >= sizeof(PKGFILE_HEADER) + pkg^.size) and
        	      (pkg^.list^[i].offset <= len) and
        	      (pkg^.list^[i].size <= len - pkg^.list^[i].offset);
                inc(i);
        end;
        if ioresult <> 0 then ok := FALSE;
        if not ok then
        begin
        	if pkg^.list <> nil then ltx_free(pkg^.list);
        	close(pkg^.handle);
        	if ioresult <> 0 then ;
                dispose(pkg);
                exit;
        end;
        {$I+}

        package_open_at := pkg;
end;

procedure	package_shutdown(var pkg: PACKAGE);
begin
	if pkg = nil then exit;
        if pkg^.mode = PFM_OPEN then
	begin
		if pkg^.list <> nil then ltx_free(pkg^.list);
		{$I-}
                close(pkg^.handle);
                {$I+}
                if ioresult <> 0 then ;
        end;
	dispose(pkg);
        pkg := nil;
end;

{ add a file of the current folder; an entry with the same name is replaced }
function	package_add(var pkg: PACKAGE; filename: string): boolean;
var	size	: longint;
	idx	: integer;
begin
	package_add := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.mode = PFM_OPEN) then exit;
        if length(filename) > 12 then exit;
        if not package_safe_name(filename) then exit;

	size := file_size(filename);
	if size < 0 then exit;

	idx := package_find(pkg, filename);
	if idx = 0 then
	begin
		if (pkg^.hdr.count >= PKGFILE_MAX) then exit;	{ table full }
		inc(pkg^.hdr.count);
		idx := pkg^.hdr.count;
	end;
	pkg^.table[idx].name   := filename;
	pkg^.table[idx].size   := size;
	pkg^.table[idx].offset := 0;
	pkg^.source[idx]       := -1;
	package_callback_file(pkg^.table[idx]);

        package_add := TRUE;
end;

{ remove an entry from a package being built }
function	package_delete(var pkg: PACKAGE; filename: string): boolean;
var	i, idx	: integer;
begin
	package_delete := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.mode = PFM_OPEN) then exit;
	idx := package_find(pkg, filename);
	if idx = 0 then exit;
	with pkg^ do
	begin
		for i := idx to hdr.count-1 do
		begin
			table[i]  := table[i+1];
			source[i] := source[i+1];
		end;
		dec(hdr.count);
	end;
	package_delete := TRUE;
end;

{ write the package; when some entries come from an existing package the }
{ result goes to a temporary file first, since pkgname may be that file   }
function	package_pack(var pkg: PACKAGE; pkgname: string): boolean;
var	i	: integer;
	csize,
	remain,
	lastoffs: longint;
        fi, fo,
        fs	: FILE;
        outname	: string;
        buffer	: PKGFILE_BUFFER;
        nr, nw,
        chunk	: word;
        abort,
        usesrc,
        opened,
        ok	: boolean;
begin
	package_pack := FALSE;
	PKGFILE_ABORTED := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.hdr.count = 0) or (pkg^.mode = PFM_OPEN) then exit;

        { files may have changed since package_add: size them again so the }
        { table written below matches the data that follows it             }
        usesrc   := FALSE;
        lastoffs := sizeof(PKGFILE_HEADER) +
	            sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
        with pkg^ do for i := 1 to hdr.count do
        begin
        	if source[i] < 0 then
        	begin
	                table[i].size := file_size(table[i].name);
	                if table[i].size < 0 then exit;
        	end
        	else	usesrc := TRUE;
                table[i].offset := lastoffs;
                inc(lastoffs, table[i].size);
        end;
        pkg^.hdr.crc32 := crc32_init;

        if usesrc then
        begin
        	if not package_reset(fs, pkg^.srcname) then exit;
        	outname := path_folder(pkgname) + 'PACK$$$$.TMP';
        end
        else	outname := pkgname;

        assign(fo, outname);
        {$I-}
        rewrite(fo, 1);
        if ioresult <> 0 then
        begin
        	if usesrc then close(fs);
        	if ioresult <> 0 then ;
        	exit;
        end;

        { header (checksum is filled in at the end) and file table }
        blockwrite(fo, pkg^.hdr, sizeof(PKGFILE_HEADER), nw);
        ok := (ioresult = 0) and (nw = sizeof(PKGFILE_HEADER));
        i := 1;
	while ok and (i <= pkg^.hdr.count) do
        begin
        	blockwrite(fo, pkg^.table[i], sizeof(PKGFILE_ENTRY), nw);
        	ok := (ioresult = 0) and (nw = sizeof(PKGFILE_ENTRY));
        	inc(i);
	end;

        { file data, from disk or from the source package }
        abort := FALSE;
        i := 1;
	while ok and not abort and (i <= pkg^.hdr.count) do
	with pkg^ do
        begin
                package_callback_file(table[i]);
                opened := FALSE;
                if source[i] < 0 then
                begin
                	opened := package_reset(fi, table[i].name);
                	ok := opened;
                end
                else
                begin
                	seek(fs, srcbase + source[i]);
                	ok := ioresult = 0;
                end;

                csize := 0;
                while ok and not abort and (csize < table[i].size) do
                begin
                	remain := table[i].size - csize;
                	if remain > sizeof(buffer) then
                		chunk := sizeof(buffer)
                	else	chunk := remain;
                	if source[i] < 0 then
                		blockread(fi, buffer, chunk, nr)
                	else	blockread(fs, buffer, chunk, nr);
                	ok := (ioresult = 0) and (nr = chunk);	{ fails if the file shrank }
                	if ok then
                	begin
	                        hdr.crc32 := crc32_block(hdr.crc32, buffer, nr);
	                        blockwrite(fo, buffer, nr, nw);
	                        ok := (ioresult = 0) and (nw = nr);
	                        inc(csize, nw);
                	end;

                        if keypressed then if getkey = KEY_ESC then
                                abort := TRUE;

	                package_callback_disk(csize, table[i].size);
                end;
                if opened then
                begin
	                close(fi);
	                if ioresult <> 0 then ;		{ read-only, nothing lost }
                end;
                inc(i);
        end;

        { rewrite the header with the final checksum }
        if ok and not abort then
        begin
	        pkg^.hdr.crc32 := crc32_final(pkg^.hdr.crc32);
	        seek(fo, 0);
	        blockwrite(fo, pkg^.hdr, sizeof(PKGFILE_HEADER), nw);
	        ok := (ioresult = 0) and (nw = sizeof(PKGFILE_HEADER));
        end;

	flushkeys;
        close(fo);
        ok := (ioresult = 0) and ok;
        if usesrc then
        begin
        	close(fs);
        	if ioresult <> 0 then ;
        end;

        { replace the target with the temporary file }
        if ok and not abort and usesrc then
        begin
        	assign(fs, pkgname);
        	erase(fs);
        	if ioresult <> 0 then ;		{ it may not exist yet }
        	rename(fo, pkgname);
        	ok := ioresult = 0;
        end;

        if abort or not ok then
        begin
        	erase(fo);
        	if ioresult <> 0 then ;
        end;
        {$I+}

        { the entries now live in the new package: packing again (after more }
        { add/delete calls) copies them from there                           }
        if ok and not abort then
        with pkg^ do
        begin
        	for i := 1 to hdr.count do source[i] := table[i].offset;
        	srcname := pkgname;
        	srcbase := 0;
        end;

        PKGFILE_ABORTED := abort;
        package_pack := ok and not abort;
end;

function	package_lookup(const pkg: PACKAGE; filename: string): longint;
var	i	: integer;
begin
        package_lookup := 0;
	if (pkg = nil) or (filename = '') then exit;
        if pkg^.mode = PFM_CREATE then exit;

	filename := toupper(filename);
        for i := 1 to pkg^.hdr.count do
        	if filename = toupper(pkg^.list^[i].name) then
                begin
                	package_lookup := i;
                        exit;
                end;
end;

function	package_search(const pkg: PACKAGE; filename: string; var entry: PKGFILE_ENTRY): boolean;
var	idx	: longint;
begin
	package_search := FALSE;
        if (pkg = nil) or (filename = '') then exit;
        idx := package_lookup(pkg, filename);
        if idx <> 0 then
        begin
                entry := pkg^.list^[idx];
        	package_search := TRUE;
        end
end;

function        package_extract(var pkg: PACKAGE; filename, destdir: string): boolean;
var	fo	: FILE;
	buffer	: PKGFILE_BUFFER;
        idx	: longint;
        nr, nw,
        chunk	: word;
        csize,
        total	: longint;
        ok	: boolean;
begin
	package_extract := FALSE;
        if (pkg = nil) then exit;
        if pkg^.mode = PFM_CREATE then exit;
	if not package_safe_name(filename) then exit;

        idx := package_lookup(pkg, filename);
        if idx = 0 then exit;

        destdir := path_norm(destdir);
        assign(fo, path_join(destdir, pkg^.list^[idx].name));
        {$I-}
        rewrite(fo, 1);
        if ioresult <> 0 then exit;

{$ifdef __DEBUG__}
        writeln('Extracting file ', filename, ' to ', destdir, '...');
{$endif}

        total := pkg^.list^[idx].size;
        csize := 0;
        seek(pkg^.handle, pkg^.base + pkg^.list^[idx].offset);
        ok := ioresult = 0;
        while ok and (csize < total) do
        begin
        	if total - csize > PKGFILE_BUFSIZE then
        		chunk := PKGFILE_BUFSIZE
        	else	chunk := total - csize;
        	blockread(pkg^.handle, buffer, chunk, nr);
        	ok := (ioresult = 0) and (nr = chunk);
        	if ok then
        	begin
	                blockwrite(fo, buffer, nr, nw);
	                ok := (ioresult = 0) and (nw = nr);
        	end;
        	if ok then
        	begin
	                inc(csize, nr);
	                package_callback_disk(csize, total);
        	end;
        end;
        close(fo);
        ok := (ioresult = 0) and ok;
        if not ok then
        begin
{$ifdef __DEBUG__}
		writeln('Error extracting ', filename, ' at ', csize, ' bytes');
{$endif}
        	erase(fo);
        	if ioresult <> 0 then ;
        end;
        {$I+}

        package_extract := ok;
end;

{ checksum of the file data, 0 when cancelled (see package_aborted) }
function        package_verify(const pkg: PACKAGE): CRC32_TYPE;
var	sum	: CRC32_TYPE;
	buffer	: PKGFILE_BUFFER;
        csize,
        total	: longint;
        i	: integer;
        nr,
        chunk	: word;
	abort,
	ok	: boolean;
begin
	package_verify := 0;
	PKGFILE_ABORTED := FALSE;
        if (pkg = nil) then exit;
        if pkg^.mode = PFM_CREATE then exit;
	if pkg^.hdr.count = 0 then exit;

	{ the data of all entries is stored back to back, in table order }
	total := 0;
	for i := 1 to pkg^.hdr.count do inc(total, pkg^.list^[i].size);

        sum   := crc32_init;
	csize := 0;
	abort := FALSE;
	{$I-}
        seek(pkg^.handle, pkg^.base + pkg^.list^[1].offset);
        ok := ioresult = 0;
        while ok and not abort and (csize < total) do
        begin
        	if total - csize > PKGFILE_BUFSIZE then
        		chunk := PKGFILE_BUFSIZE
        	else	chunk := total - csize;
        	blockread(pkg^.handle, buffer, chunk, nr);
        	ok := (ioresult = 0) and (nr = chunk);
                sum := crc32_block(sum, buffer, nr);
                inc(csize, nr);
	        package_callback_disk(csize, total);
                if keypressed then if getkey = KEY_ESC then abort := TRUE;
        end;
        {$I+}
        sum := crc32_final(sum);

        PKGFILE_ABORTED := abort;
        if abort then sum := 0;
	package_verify := sum;
end;

{ append the whole content of file 'name' to the open file fo }
function	package_append_file(var fo: FILE; name: string): boolean;
var	fi	: FILE;
	buffer	: PKGFILE_BUFFER;
	nr, nw	: word;
	ok	: boolean;
begin
	package_append_file := FALSE;
	if not package_reset(fi, name) then exit;
	{$I-}
	repeat
		blockread(fi, buffer, sizeof(buffer), nr);
		ok := ioresult = 0;
		if ok and (nr > 0) then
		begin
			blockwrite(fo, buffer, nr, nw);
			ok := (ioresult = 0) and (nw = nr);
		end;
	until not ok or (nr = 0);
	close(fi);
	if ioresult <> 0 then ;
	{$I+}
	package_append_file := ok;
end;

{ build exename = stub executable + package + trailer; the stub finds the }
{ package through package_open_sfx(paramstr(0))                           }
function	package_make_sfx(pkgname, stubname, exename: string): boolean;
var	fo	: FILE;
	trailer	: PKGFILE_SFX_TRAILER;
	nw	: word;
	ok	: boolean;
begin
	package_make_sfx := FALSE;
	if not package_valid(pkgname) then exit;
	if (toupper(exename) = toupper(pkgname)) or
	   (toupper(exename) = toupper(stubname)) then exit;

	trailer.magic := PKGFILE_SFX_MAGIC;
	trailer.base  := file_size(stubname);
	if trailer.base <= 0 then exit;

	assign(fo, exename);
	{$I-}
	rewrite(fo, 1);
	if ioresult <> 0 then exit;
	ok := package_append_file(fo, stubname) and
	      package_append_file(fo, pkgname);
	if ok then
	begin
		blockwrite(fo, trailer, sizeof(trailer), nw);
		ok := (ioresult = 0) and (nw = sizeof(trailer));
	end;
	close(fo);
	ok := (ioresult = 0) and ok;
	if not ok then
	begin
		erase(fo);
		if ioresult <> 0 then ;
	end;
	{$I+}
	package_make_sfx := ok;
end;

{ open the package appended to a self-extracting executable }
function	package_open_sfx(exename: string): PACKAGE;
var	f	: FILE;
	trailer	: PKGFILE_SFX_TRAILER;
	fsize	: longint;
	nr	: word;
	ok	: boolean;
begin
	package_open_sfx := nil;
	if not package_reset(f, exename) then exit;
	{$I-}
	fsize := filesize(f);
	ok := fsize > sizeof(trailer);
	if ok then
	begin
		seek(f, fsize - sizeof(trailer));
		blockread(f, trailer, sizeof(trailer), nr);
		ok := (ioresult = 0) and (nr = sizeof(trailer)) and
		      (trailer.magic = PKGFILE_SFX_MAGIC) and
		      (trailer.base > 0) and
		      (trailer.base < fsize - sizeof(trailer));
	end;
	close(f);
	if ioresult <> 0 then ok := FALSE;
	{$I+}
	if ok then
		package_open_sfx := package_open_at(exename, trailer.base,
						    fsize - sizeof(trailer) - trailer.base);
end;

procedure	package_callback_assign(mode: PKGFILE_CB_MODE; cb: pointer);
begin
	case mode of
	CB_FILE: CALLBACK_FILE := PKGFILE_CB_FILE(cb);
        CB_DISK: CALLBACK_DISK := PKGFILE_CB_DISK(cb);
        end;
end;

function	package_checksum(const pkg: PACKAGE): CRC32_TYPE;
begin
	package_checksum := 0;
	if pkg = nil then exit;
	package_checksum := pkg^.hdr.crc32;
end;

{ TRUE if the last package_pack or package_verify was cancelled with ESC }
function	package_aborted: boolean;
begin
	package_aborted := PKGFILE_ABORTED;
end;

procedure	package_callback_file(const entry: PKGFILE_ENTRY);
begin
	if @CALLBACK_FILE <> nil then CALLBACK_FILE(entry);
end;

procedure	package_callback_disk(current, total: longint);
begin
	if @CALLBACK_DISK <> nil then CALLBACK_DISK(current, total);
end;

procedure	package_callback_detach(mode: PKGFILE_CB_MODE);
begin
	case mode of
	CB_FILE: CALLBACK_FILE := nil;
        CB_DISK: CALLBACK_DISK := nil;
        end;
end;

end.
