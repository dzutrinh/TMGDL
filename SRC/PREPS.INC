{ //////////////////////// PREDEFINED PREPROCESSORS /////////////////////////}

{$ifdef __TMT__}	        { --- TMT Pascal }
	{$MMX+}	                { use MMX instructions }
	{$OPTFRM+}	        { frame optimization }
	{$OPTREG+}	        { registers optimization }
	{$AMD+}	                { enable AMD's 3DNow! instructions }
	{$TPO+}	                { enable typed inc/dec operations }
{$else}
	{$ifdef VER70}	        { --- Turbo/Borland Pascal }
		{$ifdef __DEBUG__}
			{$A+,B+,D+,E+,F+,G+,I+,L+,N+,O-,P+,Q+,R+,S+,T-,V+,X+,Y+}
		{$else}
			{$A+,B+,D-,E-,F+,G+,I-,L-,N+,O-,P+,Q-,R-,S-,T-,V+,X+,Y-}
		{$endif}
		{$ifdef __NOFPU__}
			{$E+}			{ FPU emulation             }
		{$endif}
		{$M 25600, 65536, 655360}	{ use all available memory  }
	{$else}
                {$ifdef FPC}    { --- Free Pascal }
                        {$ASMMODE INTEL}	{ Using Intel assembly mode }
                        {$MODE TP}		{ TP compatibility code     }
                        {$MMX+}			{ MMX support               }
			{$PACKRECORDS 1}	{ turn off record alignment }
                {$endif}
	{$endif}
{$endif}

{ ////////////////////////////////////////////////////////////////////////// }