{$i preps.inc}

unit	libpack;

interface

uses	libtext, libutil;

const   PKGFILE_MAX	= 1024;
	PKGFILE_BUFSIZE	= 1024;

type	PKGFILE_SIGNATURE = array[0..3] of char;
        PKGFILE_BUFFER  = array[1..PKGFILE_BUFSIZE] of byte;

type	PKGFILE_ENTRY	= record	{ a file table entry, consists of }
		name	: string[12];	{ file name }
                size	: longint;	{ its size in bytes }
                offset	: longint;	{ offset inside package file }
	end;

        PKGFILE_HEADER = record
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


function        package_init: PACKAGE;
var	pkg	: PACKAGE;
begin
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
        {$I-}
        if ioresult <> 0 then exit;
        {$I+}
        blockread(f, sig, sizeof(PKGFILE_SIGNATURE), nr);
        if (nr <> sizeof(PKGFILE_SIGNATURE)) or (sig <> PKGFILE_MAGIC) then
                package_valid := FALSE
        else	package_valid := TRUE;
        {$I+}
        close(f);
end;

function        package_open(filename: string): PACKAGE;
var	pkg	: PACKAGE;
	i	: integer;
	nr	: word;
begin
	package_open := nil;

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

        blockread(pkg^.handle, pkg^.hdr, sizeof(PKGFILE_HEADER));
        if pkg^.hdr.magic <> PKGFILE_MAGIC then
        begin
                close(pkg^.handle);
                dispose(pkg);
                exit;
        end;

        pkg^.size := sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
        if ltx_memavail < pkg^.size then
        begin
        	close(pkg^.handle);
                dispose(pkg);
                exit;
        end;

        pkg^.list := ltx_malloc(pkg^.size);
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
var	f	: FILE;
	size	: longint;
begin
	package_add := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.mode = PFM_OPEN) then exit;
	if not file_exist(filename) then exit;

	if (pkg^.hdr.count < PKGFILE_MAX) then
        begin
        	assign(f, filename);
                {$I-}
                reset(f, 1);
                {$I+}
                if ioresult <> 0 then exit;
		size := filesize(f);
                close(f);

        	inc(pkg^.hdr.count);
                pkg^.table[pkg^.hdr.count].name := filename;
                pkg^.table[pkg^.hdr.count].size := size;
                pkg^.table[pkg^.hdr.count].offset := 0;
        	package_callback_file(pkg^.table[pkg^.hdr.count]);
        end;

        package_add := TRUE;
end;

function	package_pack(var pkg: PACKAGE; pkgname: string): boolean;
var	i	: integer;
	csize,
	total,
	lastoffs: longint;
        fi, fo	: FILE;
        buffer	: PKGFILE_BUFFER;
        nr, nw	: word;
        abort	: boolean;
begin
	package_pack := FALSE;
	if (pkg = nil) then exit;
        if (pkg^.hdr.count = 0) or (pkg^.mode = PFM_OPEN) then exit;

        total    := sizeof(PKGFILE_HEADER) +
	            sizeof(PKGFILE_ENTRY) * pkg^.hdr.count;
        lastoffs := total;

        with pkg^ do for i := 1 to hdr.count do
        begin
                table[i].offset := lastoffs;
                inc(lastoffs, table[i].size);
        end;

        { write header }
        assign(fo, pkgname);
        {$I-}
        rewrite(fo, 1);
        {$I+}
        if ioresult <> 0 then exit;

        { write header }
        blockwrite(fo, pkg^.hdr, sizeof(PKGFILE_HEADER), nw);
        if nw <> sizeof(PKGFILE_HEADER) then
        begin
                close(fo);
                erase(fo);
                exit;
        end;

        { write partial file table }
	for i := 1 to pkg^.hdr.count do
        begin
        	blockwrite(fo, pkg^.table[i], sizeof(PKGFILE_ENTRY), nw);
		if nw <> sizeof(PKGFILE_ENTRY) then
	        begin
        	        close(fo);
			erase(fo);
	                exit;
	        end;
	end;

        { start packing }
        abort := FALSE;
	with pkg^ do for i := 1 to hdr.count do
        begin
                package_callback_file(table[i]);
        	assign(fi, table[i].name);
                reset(fi, 1);

                csize := 0;
                repeat
                	blockread(fi, buffer, sizeof(buffer), nr);
                        hdr.crc32 := crc32_block(hdr.crc32, buffer, nr);

                        blockwrite(fo, buffer, nr, nw);
                        inc(csize, nw);

                        if keypressed then if getkey = KEY_ESC then
                                abort := TRUE;

	                package_callback_disk(csize, table[i].size);
                until (nr = 0) or abort;
                close(fi);

                if abort then break;

                inc(total, table[i].size);
        end;
        pkg^.hdr.crc32 := crc32_final(pkg^.hdr.crc32);

        seek(fo, 0);
        { write full header with checksum }
        blockwrite(fo, pkg^.hdr, sizeof(PKGFILE_HEADER), nw);
        if nw <> sizeof(PKGFILE_HEADER) then
        begin
                close(fo);
                erase(fo);
                exit;
        end;

	flushkeys;
        close(fo);

        if abort then erase(fo);
        package_pack := not abort;
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
        	if filename = pkg^.list^[i].name then
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
        i, idx	: longint;
        nr, nw	: word;
        csize,
        blocks,
        remain	: longint;
begin
	package_extract := FALSE;
        if (pkg = nil) or (filename = '') then exit;
        if pkg^.mode = PFM_CREATE then exit;

        idx := package_lookup(pkg, filename);
        if idx = 0 then exit;

        destdir := path_norm(destdir);
        assign(fo, destdir+'\'+filename);
        {$I-}
        rewrite(fo, 1);
        {$I+}
        if ioresult <> 0 then exit;

{$ifdef __DEBUG__}
        writeln('Extracting file ', filename, ' to ', destdir, '...');
{$endif}

        blocks := pkg^.list^[idx].size div PKGFILE_BUFSIZE;
        remain := pkg^.list^[idx].size mod PKGFILE_BUFSIZE;
        csize  := 0;
        seek(pkg^.handle, pkg^.list^[idx].offset);
        for i := 1 to blocks do
        begin
        	blockread(pkg^.handle, buffer, PKGFILE_BUFSIZE, nr);
                blockwrite(fo, buffer, nr, nw);
                if (nr <> PKGFILE_BUFSIZE) or (nw <> nr) then
                begin
{$ifdef __DEBUG__}
			writeln('Error writing block #', i, ' at ', i * PKGFILE_BUFSIZE, ' bytes');
{$endif}
                        close(fo);
                        erase(fo);
                        exit;
                end;
                inc(csize, nr);
                package_callback_disk(csize, pkg^.list^[idx].size);
        end;
        if remain <> 0 then
        begin
        	blockread(pkg^.handle, buffer, remain, nr);
                blockwrite(fo, buffer, nr, nw);
                if (nr <> remain) or (nw <> nr) then
                begin
{$ifdef __DEBUG__}
			writeln('Error writing remaining ', remain, ' bytes');
{$endif}
                        close(fo);
                        erase(fo);
                        exit;
                end;
                inc(csize, nr);
                package_callback_disk(csize, pkg^.list^[idx].size);
        end;
        close(fo);

        package_extract := TRUE;
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
	package_checksum := pkg^.hdr.crc32;
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
