{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   T3.PAS                                                                   }
{   ------                                                                   }
{   Tic-Tac-Toe Game for DOS                                                 }
{   Version 1.1                                                              }
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

{$i ../../src/preps.inc}

uses	libtmgd, libutil, libtext, libctls;

{$i backgd.inc}
{$i globals.inc}
{$i engine.inc}
{$i game.inc}

begin
	gameInit(1, 1, '0714');
	gameMenu;
        gameShutdown;
end.
