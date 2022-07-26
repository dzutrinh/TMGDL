{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   2048.PAS                                                                 }
{   --------                                                                 }
{   2048 Game for DOS                                                        }
{   Version 1.2                                                              }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2019-2021 by Infinity Group                               }
{                                                                            }
{   Compilers                                                                }
{   ---------                                                                }
{   . Turbo/Borland Pascal 7.0                                               }
{   . Free Pascal 1.0                                                        }
{   . TMT Pascal 3.5+                                                        }
{                                                                            }
{   Tools used                                                               }
{   ----------                                                               }
{   . TheDraw                                                                }
{                                                                            }
{ ////////////////////////////////////////////////////////////////////////// }

{$i ../../src/preps.inc}

(* {$define __DEBUG__} *)

uses	libtmgd, libtext, libctls, libutil;

{$i backgd.inc}			{ main screen data            }
{$i globals.inc}		{ global constants, variables }
{$i engine.inc}			{ game engine                 }
{$i game.inc}			{ game handlers               }

begin
	gameInit(1, 2, '0702');	{ init game }
	if gameDemo then	{ playing a so-called demo }
		gamePlay;	{ game loop }
	gameDone;		{ shutdown game }
end.
