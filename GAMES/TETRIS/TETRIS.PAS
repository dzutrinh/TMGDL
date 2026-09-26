{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   TETRIS.PAS                                                               }
{   ----------                                                               }
{   Tetris Game for DOS                                                      }
{   Version 1.1                                                              }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2020-2026 by Infinity Group                               }
{                                                                            }
{   Compilers                                                                }
{   ---------                                                                }
{   . Turbo/Borland Pascal 7.0                                               }
{   . TMT Pascal 3.5+                                                        }
{                                                                            }
{   Tools used                                                               }
{   ----------                                                               }
{   . TheDraw                                                                }
{                                                                            }
{   DDK16 | DDK32                                                            }
{									     }
{ ////////////////////////////////////////////////////////////////////////// }

{$i ../../src/preps.inc}

{$define RANDOM_ROTATION}	{ randomized piece rotation }

uses	libtmgd, libtext, libctls, libutil, libsnd;

{$i backgd.inc}		        { main screen (ANSI-Art)    }
{$i globals.inc}		{ engine global definitions }
{$i engine.inc}			{ game engine               }
{$i game.inc}			{ the game itself           }

begin
	gameInit(1, 1, '0925');	{ init the game             }
	gameMenu;		{ main loop                 }
	gameDone;		{ clean-up and exit         }
end.