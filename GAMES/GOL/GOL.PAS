{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   GOL.PAS                                                                  }
{   -------                                                                  }
{   Conway's Game Of Life for DOS                                            }
{   Version 1.1                                                              }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2020-2021 by Infinity Group                               }
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
{									     }
{ ////////////////////////////////////////////////////////////////////////// }

{$i ../../src/preps.inc}

(* {$define __DEBUG__} *)
uses	libtmgd, libtext, libctls, libutil, libsnd;

{$i backgd.inc}
{$i globals.inc}
{$i patterns.inc}
{$i engine.inc}
{$i game.inc}

begin
	gameInit(1, 1, '0925');
        gameMenu;
	gameDone;
end.
