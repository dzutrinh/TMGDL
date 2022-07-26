{$i preps.inc}

{ Base Text-mode Game Development Library }

unit libtmgd;

interface

uses	libtext, libctls, libpack, libutil;

const
{$ifdef __TMT__}
	COMPILER	: string[3] = 'TMT';
	ENVIRONMENT	: string[6] = '32-bit';
{$else}
{$ifdef FPC}
        COMPILER	: string[3] = 'FPC';
	ENVIRONMENT	: string[6] = '32-bit';
{$else}
        COMPILER	: string[3] = 'BPC';
	ENVIRONMENT	: string[6] = '16-bit';
{$endif}
{$endif}

	GAME_VERSION	: word = $0000;		{ Game version number }
	GAME_BUILD	: string[4] = '';	{ Build string }
	GAME_COPYRIGHTS	: string = '';		{ Copyrights string }

implementation

end.
