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

        PKGFILE_LIST = ^PKGFILE_ARRAY;
        PKGFILE_ARRAY = array[1..PKGFILE_MAX] of PKGFILE_ENTRY;

        PKGFILE_MODE = (PFM_CREATE,	{ create package file }
			PFM_OPEN);	{ use package file    }

        PACKAGE = ^PKGFILE_TABLE;
        PKGFILE_TABLE	= record
                hdr	  : PKGFILE_HEADER;
                case mode : PKGFILE_MODE of
                PFM_CREATE: (table : PKGFILE_ARRAY);
                PFM_OPEN  : (list  : PKGFILE_LIST;
			     size  : longint;
			     handle: FILE);
        end;

type	PKGFILE_CB_MODE = (CB_FILE, CB_DISK);
	PKGFILE_CB_FILE	= procedure(const entry: PKGFILE_ENTRY);
	PKGFILE_CB_DISK = procedure(current, total: longint);

{ package file creator and packer }
function        package_init: PACKAGE;
function	package_add(var pkg: PACKAGE; filename: string): boolean;
function	package_pack(var pkg: PACKAGE; pkgname: string): boolean;

{ package file utilizations }
function	package_valid(filename: string): boolean;
function        package_open(filename: string): PACKAGE;
function	package_lookup(const pkg: PACKAGE; filename: string): longint;
function	package_search(const pkg: PACKAGE; filename: string;
			       var entry: PKGFILE_ENTRY): boolean;
function        package_extract(var pkg: PACKAGE;
			        filename, destdir: string): boolean;
function        package_verify(const pkg: PACKAGE): CRC32_TYPE;
function	package_checksum(const pkg: PACKAGE): CRC32_TYPE;
function	package_aborted: boolean;

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
        end;

        package_init := pkg;
end;

function	package_valid(filename: string): boolean;
var	f	: FILE;
	sig	: PKGFILE_SIGNATURE;
	nr	: word;
begin
        package_valid := FALSE;
        assign(f, filename);
        {$I-}
        reset(f, 1);
        {$I+}
        if ioresult <> 0 then exit;
        blockread(f, sig, sizeof(PKGFILE_SIGNATURE), nr);
        if (nr <> sizeof(PKGFILE_SIGNATURE)) or (sig <> PKGFILE_MAGIC) then
                package_valid := FALSE
        else	package_valid := TRUE;
        close(f);
end;

function        package_open(filename: string): PACKAGE;
var	pkg	: PACKAGE;
	i	: integer;
	nr	: word;
        fsize	: longint;
begin
	package_open := nil;

	if ltx_maxavail < sizeof(PKGFILE_TABLE) then exit;
	new(pkg);
	pkg^.mode := PFM_OPEN;
	assign(pkg^.handle, filename);
        {$I-}
        reset(pkg^.handle, 1);
        if ioresult <> 0 then
        begin
        	dispose(pkg);
                exit;
        end;

        blockread(pkg^.handle, pkg^.hdr, sizeof(PKGFILE_HEADER), nr);
        if (nr <> sizeof(PKGFILE_HEADER)) or
	   (pkg^.hdr.magic <> PKGFILE_MAGIC) or
	   (pkg^.hdr.count > PKGFILE_MAX) then
        begin
                close(pkg^.handle);
                dispose(pkg);
                exit;
        end;

        pkg^.size := sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
        fsize := filesize(pkg^.handle);
        if pkg^.size > fsize - sizeof(PKGFILE_HEADER) then
        begin
                close(pkg^.handle);
                dispose(pkg);
                exit;
        end;
        pkg^.list := ltx_malloc(pkg^.size);
        if pkg^.list = nil then
        begin
        	close(pkg^.handle);
                dispose(pkg);
                exit;
        end;
        for i := 1 to pkg^.hdr.count do
        begin
        	blockread(pkg^.handle, pkg^.list^[i], sizeof(PKGFILE_ENTRY), nr);
                if nr <> sizeof(PKGFILE_ENTRY) then
                begin
                        close(pkg^.handle);
                        ltx_free(pkg^.list);
                        dispose(pkg);
                        exit;
                end;
                if not package_safe_name(pkg^.list^[i].name) or
                   (pkg^.list^[i].size < 0) or
                   (pkg^.list^[i].offset < sizeof(PKGFILE_HEADER) + pkg^.size) or
                   (pkg^.list^[i].offset > fsize) or
                   (pkg^.list^[i].size > fsize - pkg^.list^[i].offset) then
                begin
                        close(pkg^.handle);
                        ltx_free(pkg^.list);
                        dispose(pkg);
                        exit;
                end;
        end;
        {$I+}

        package_open := pkg;
end;

procedure	package_shutdown(var pkg: PACKAGE);
begin
	if pkg = nil then exit;
        if pkg^.mode = PFM_OPEN then
	begin
		ltx_free(pkg^.list);
                close(pkg^.handle);
        end;
	dispose(pkg);
        pkg := nil;
end;

function	package_add(var pkg: PACKAGE; filename: string): boolean;
var	size	: longint;
begin
	package_add := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.mode = PFM_OPEN) then exit;
        if length(filename) > 12 then exit;
        if not package_safe_name(filename) then exit;
	if (pkg^.hdr.count >= PKGFILE_MAX) then exit;	{ table full }

	size := file_size(filename);
	if size < 0 then exit;

	inc(pkg^.hdr.count);
	pkg^.table[pkg^.hdr.count].name := filename;
	pkg^.table[pkg^.hdr.count].size := size;
	pkg^.table[pkg^.hdr.count].offset := 0;
	package_callback_file(pkg^.table[pkg^.hdr.count]);

        package_add := TRUE;
end;

function	package_pack(var pkg: PACKAGE; pkgname: string): boolean;
var	i	: integer;
	csize,
	remain,
	lastoffs: longint;
        fi, fo	: FILE;
        buffer	: PKGFILE_BUFFER;
        nr, nw,
        chunk	: word;
        abort,
        opened,
        ok	: boolean;
begin
	package_pack := FALSE;
	PKGFILE_ABORTED := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.hdr.count = 0) or (pkg^.mode = PFM_OPEN) then exit;

        { files may have changed since package_add: size them again so the }
        { table written below matches the data that follows it             }
        lastoffs := sizeof(PKGFILE_HEADER) +
	            sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
        with pkg^ do for i := 1 to hdr.count do
        begin
                table[i].size := file_size(table[i].name);
                if table[i].size < 0 then exit;
                table[i].offset := lastoffs;
                inc(lastoffs, table[i].size);
        end;
        pkg^.hdr.crc32 := crc32_init;

        assign(fo, pkgname);
        {$I-}
        rewrite(fo, 1);
        if ioresult <> 0 then exit;

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

        { file data }
        abort := FALSE;
        i := 1;
	while ok and not abort and (i <= pkg^.hdr.count) do
	with pkg^ do
        begin
                package_callback_file(table[i]);
        	assign(fi, table[i].name);
                reset(fi, 1);
                opened := ioresult = 0;
                ok := opened;

                csize := 0;
                while ok and not abort and (csize < table[i].size) do
                begin
                	remain := table[i].size - csize;
                	if remain > sizeof(buffer) then
                		chunk := sizeof(buffer)
                	else	chunk := remain;
                	blockread(fi, buffer, chunk, nr);
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
        if abort or not ok then
        begin
        	erase(fo);
        	if ioresult <> 0 then ;
        end;
        {$I+}

        PKGFILE_ABORTED := abort;
        package_pack := ok and not abort;
end;

function	package_lookup(const pkg: PACKAGE; filename: string): longint;
var	i, index: integer;
	found	: boolean;
begin
        package_lookup := 0;
	if (pkg = nil) or (filename = '') then exit;
        if pkg^.mode = PFM_CREATE then exit;

	found := FALSE;
        i     := 1;
        index := 0;
        while (i <= pkg^.hdr.count) and not found do
        begin
        	if toupper(filename) = toupper(pkg^.list^[i].name) then
                begin
                	found := TRUE;
                        index := i;
                        break;
                end;
                inc(i);
        end;
        package_lookup := index;
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
        assign(fo, path_join(destdir, filename));
        {$I-}
        rewrite(fo, 1);
        if ioresult <> 0 then exit;

{$ifdef __DEBUG__}
        writeln('Extracting file ', filename, ' to ', destdir, '...');
{$endif}

        total := pkg^.list^[idx].size;
        csize := 0;
        seek(pkg^.handle, pkg^.list^[idx].offset);
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

function        package_verify(const pkg: PACKAGE): CRC32_TYPE;
var	sum	: CRC32_TYPE;
	buffer	: PKGFILE_BUFFER;
        csize,
        fsize	: longint;
        nr	: word;
	abort	: boolean;
begin
	package_verify := 0;
        if (pkg = nil) then exit;
        if pkg^.mode = PFM_CREATE then exit;
	if pkg^.hdr.count = 0 then exit;

        sum   := crc32_init;
	csize := sizeof(PKGFILE_HEADER) +
		 pkg^.hdr.count * sizeof(PKGFILE_ENTRY);
        fsize := filesize(pkg^.handle);
	abort := FALSE;
        seek(pkg^.handle, pkg^.list^[1].offset);
        repeat
        	blockread(pkg^.handle, buffer, PKGFILE_BUFSIZE, nr);
                sum := crc32_block(sum, buffer, nr);
                inc(csize, nr);
	        package_callback_disk(csize, fsize);
                if keypressed then if getkey = KEY_ESC then abort := TRUE;
        until (nr = 0) or abort;
        sum := crc32_final(sum);

        PKGFILE_ABORTED := abort;
        if abort then sum := 0;
	package_verify := sum;
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
