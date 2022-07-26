{$ifndef __TETRIS_BACKGD__}
{$define __TETRIS_BACKGD__}

const
  BACKGD_IMG: ANSIDATA = (
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, '�', #25,
     '�', #25, '�',  #9, '�', #25, '�', #25, '�',  #9, '�',  #9, ' ',  #9,
     '�', #25, '�', #25, '�',  #9, '�',#121, '�',  #9, '�',  #9, '�',  #9,
     ' ',  #9, '�', #25, '�', #25, '�',  #9, '�', #25, '�', #25, '�',  #9,
     '�',  #9, ' ',  #9, '�', #25, '�', #25, '�',  #9, '�', #25, '�', #25,
     '�',  #9, '�',  #9, ' ',  #9, '�', #25, '�', #25, '�', #25, '�',  #9,
     ' ',  #9, '�', #25, '�', #25, '�',  #9, '�', #25, '�', #25, '�',  #9,
     '�',  #9, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, '�', #25, '�', #25, ' ', #15, '�', #25, '�',  #9,
     '�',  #9, '�',  #9, ' ',  #9, '�', #25, '�', #25, ' ', #15, '�', #25,
     '�',#121, '�',  #9, '�',  #9, ' ',  #9, '�', #25, '�', #25, ' ', #15,
     '�', #25, '�',  #9, '�',  #9, '�',  #9, ' ',  #9, '�', #25, '�', #25,
     ' ', #15, '�', #25, '�',  #9, '�',  #9, '�',  #9, ' ',  #9, '�',  #9,
     '�',  #9, '�',  #9, '�',  #9, ' ',  #9, '�', #25, '�', #25, ' ', #15,
     '�',  #9, '�',  #9, '�',  #9, '�',  #9, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, '�',  #9, '�',  #9,
     ' ',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, ' ',  #9, '�',  #9,
     '�',  #9, ' ',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, ' ',  #9,
     '�',  #9, '�',  #9, ' ',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9,
     ' ',  #9, '�',  #9, '�',  #9, ' ',  #9, '�',  #9, '�',  #9, '�',  #9,
     '�',  #9, ' ',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, ' ',  #9,
     '�',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, '�',#127, '�',#127, '�',#127,
     '�',  #7, ' ',  #7, '�',#127, '�',#127, ' ', #15, '�',#127, '�',#127,
     '�',#127, '�',  #7, ' ',  #7, ' ',  #7, ' ',  #7, ' ',  #7, '�',#127,
     '�',#127, '�',#127, '�',  #7, ' ',  #7, '�',#127, '�',#127, ' ', #15,
     '�',  #7, '�',#127, '�',#127, '�',#127, ' ', #15, '�',  #7, '�',#127,
     '�',#127, '�',#127, ' ', #15, '�',#127, '�',#127, ' ', #15, '�',  #7,
     '�',#127, '�',#127, '�',#127, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     '�',#127, '�',#127, '�',  #7, '�',  #7, ' ',  #7, '�',#127, '�',#127,
     ' ', #15, '�',#127, '�',#127, '�',  #7, '�',  #7, ' ',  #7, ' ',  #7,
     ' ',  #7, ' ',  #7, '�',#127, '�',#127, '�',  #7, '�',  #7, ' ',  #7,
     '�',#127, '�',#127, ' ', #15, '�',  #7, '�',  #7, '�',#127, '�',#127,
     ' ', #15, '�',  #7, '�',  #7, '�',#127, '�',#127, ' ', #15, '�',#127,
     '�',#127, ' ', #15, '�',  #7, '�',  #7, '�',#127, '�',#127, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, '�',  #1, '�',  #1, '�',  #1, '�',  #1,
     '�',  #1, '�',  #1, '�',  #9, '�',  #1, '�',  #9, '�',  #9, '�',  #1,
     '�',  #1, '�',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9, '�',  #9,
     '�',  #9, '�',  #9, ' ',  #9, '�',#127, '�',  #7, '�',  #7, '�',  #7,
     ' ',  #7, '�',#127, '�',  #7, '�',  #7, '�',#127, '�',  #7, '�',  #7,
     '�',  #7, ' ',  #7, '�',  #9, '�',  #9, ' ',  #9, '�',#127, '�',  #7,
     '�',  #7, '�',  #7, ' ',  #7, '�',  #7, '�',#127, ' ', #15, '�',  #9,
     '�',  #9, '�',  #9, '�',  #9, ' ',  #9, '�',  #7, '�',  #7, '�',  #7,
     '�',#127, ' ', #15, '�',  #7, '�',#127, ' ', #15, '�',  #7, '�',  #7,
     '�',  #7, '�',#127, ' ', #15, '�',  #9, '�',  #9, '�',  #9, '�',  #9,
     '�',  #9, '�',  #9, '�',  #1, '�',  #9, '�',  #9, '�',  #1, '�',  #1,
     '�',  #1, '�',  #9, '�',  #1, '�',  #1, '�',  #1, '�',  #1, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, '�',  #7,
     '�',  #7, '�',  #7, '�',  #7, ' ',  #7, '�',  #7, '�',  #7, '�',  #7,
     '�',  #7, '�',  #7, '�',  #7, '�',  #7, ' ',  #7, ' ',  #7, ' ',  #7,
     ' ',  #7, '�',  #7, '�',  #7, '�',  #7, '�',  #7, ' ',  #7, '�',  #7,
     '�',  #7, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     '�',  #7, '�',  #7, '�',  #7, '�',  #7, ' ',  #7, '�',  #7, '�',  #7,
     '�',  #7, '�',  #7, '�',  #7, '�',  #7, '�',  #7, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, '�',  #8, '�',  #8, '�',  #8, '�',  #8, ' ',  #8,
     '�',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8,
     ' ',  #8, ' ',  #8, ' ',  #8, ' ',  #8, '�',  #8, '�',  #8, '�',  #8,
     '�',  #8, ' ',  #8, '�',  #8, '�',  #8, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, '�',  #8, '�',  #8, '�',  #8, '�',  #8,
     ' ',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8, '�',  #8,
     '�',  #8, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15,
     ' ', #15, ' ', #15, ' ', #15, ' ', #15, ' ', #15);

{$endif}
