/*compile using -lncurses
for example, gcc absgraph.c -o absgraph -lncurses
*/
#include <stdio.h>                                /*incl.lib.func. std.i/o  */
#include <string.h>                               /*incl.lib.func. str.char.*/
#include <stdlib.h>                               /*incl.lib.func. conv.d.  */
#include <ctype.h>                                /*incl.lib.func. conv.c.  */
#include <curses.h>

#define  NSPIS  5                                 /*size of load prog.list  */
#define  NOBJ   50                                /*size of array obj.cards */
#define  DOBLZ  1024                              /*length of load area     */
#define  NOP 6                                    /*number of processed     */
						  /* commands               */

/*
***** Convert byte to printable char, return '.' for non-printable
*/
char byte_to_print(unsigned char byte)
{
  if (byte >= 32 && byte < 127)
    return (char)byte;
  return '.';
}


char NFIL [30] = "\x0";

int  IOBJC   = 0;                                 /*index of free row OBJCARD*/
char OBJCARD [NOBJ][80];                          /*array stor. object cards*/

int  ISPIS   = 0;                                 /*index of free row SPISOK*/
char SPISOK  [NSPIS][80];                         /*array stor. program list*/

WINDOW *wblue, *wgreen, *wred, *wcyan, *wmargenta;

struct STR_BUF_TXT                                /*struct of TXT card buff */
 {
  unsigned char POLE1      ;                      /*place for code 0x02     */
  unsigned char POLE2  [ 3];                      /*field of object card type*/
  unsigned char POLE3      ;                      /*space                   */
  unsigned char ADOP   [ 3];                      /*relative addr of oper.  */
  unsigned char POLE5  [ 2];                      /*spaces                  */
  unsigned char DLNOP  [ 2];                      /*length of operation     */
  unsigned char POLE7  [ 2];                      /*spaces                  */
  unsigned char POLE71 [ 2];                      /*internal ident.of prog. */
  unsigned char OPER   [56];                      /*body of operation       */
  unsigned char POLE9  [ 8];                      /*identification field    */
 };


union                                             /*define union            */
 {
  struct STR_BUF_TXT STR_TXT;                     /*buffer structure        */
  unsigned char BUF_TXT [80];                     /*TXT card buffer         */
 } TXT;


unsigned char INST [6];                           /*array,cont. processed   */
						  /*command                 */


/*..........................................................................*/
						  /*p r o t o t y p e  for  */
int FRR();                                        /*subr.proc. oper.RR-form.*/

/*..........................................................................*/

						  /*p r o t o t y p e  for  */
int FRX();                                        /*subr.proc. oper.RX-form.*/
/*..........................................................................*/


int X1 = 1;                                       /* init.coord.            */
int Y1 = 15;                                      /* on screen              */
int R1,                                           /*number 1-st reg-oper-   */
						                                      /*rand in formatsRR and RX*/
    R2,                                           /*number 2-nd reg-oper-   */
						                                      /*rand in format RX       */
    D,                                            /*displacement in formatRX*/
    X,                                            /*number of index reg.    */
						                                      /*in format RX            */
    B;                                            /*number of base reg.     */
						                                      /*in format RX            */
unsigned long I,                                  /*cnterof addr.of curr.cmd*/
	      BAS_ADDR,                                 /*adr.of strt of load area*/
	      I1,ADDR,ARG,VS;                           /*auxiliary variables     */
unsigned long VR[16],                             /*array,cont.reg.values   */
	      LIGHTPTR;                                 /*addr.of start of hghligt*/


int x,y,i,j,k,kk;                                 /*working variables       */

int CUR_IND;                                      /*index of load array,    */
						                                      /*corr.to curr.cmd of prog*/
int BAS_IND;                                      /*index of load array,    */
						                                      /*corr.to 1st cmd of prog.*/

  union U1                                        /*constant part of tmplate*/
   {                                              /*for reg.highlight on    */
    struct                                        /*console screen          */
     {
      char NOMREG  [ 3];
      char ZNEQUEL [1];
     } REGS_ASC [16];

    char BUFR [16][4];
   }R_ASC;

  union u2                                        /*template for calculation*/
   {                                              /*of absolute elements    */
    struct
     {
      unsigned int SMESH;
      unsigned int SEGM;
     } VAL_P;
   unsigned char *P_OBLZ ;
   } POINT;

  unsigned char OBLZ [DOBLZ] ;                    /*load area of traced     */
						                                      /*program                 */
/*
***** TABLE of machine operations
*/

 struct TMOP                                      /*strctof row mach.op.tble*/
  {
   unsigned char MNCOP [5];                       /*mnemonic code of op.    */
   unsigned char CODOP    ;                       /*machine code of op.     */
   unsigned char DLOP     ;                       /*length of op.in bytes   */
   int (*BXPROG)()        ;                       /*pointer to subr.proc.   */
  } T_MOP [NOP]  =                                /*decl.of mach.op.table   */
    {
{{'B' , 'A' , 'L' , 'R' , ' '} , '\x05', 2 , FRR},/*initialization          */
{{'B' , 'C' , 'R' , ' ' , ' '} , '\x07', 2 , FRR},/*of rows                 */
{{'S' , 'T' , ' ' , ' ' , ' '} , '\x50', 4 , FRX},/*of table                */
{{'L' , ' ' , ' ' , ' ' , ' '} , '\x58', 4 , FRX},/*of machine              */
{{'A' , ' ' , ' ' , ' ' , ' '} , '\x5A', 4 , FRX},/*of operations           */
{{'S' , ' ' , ' ' , ' ' , ' '} , '\x5B', 4 , FRX},/*                        */
    };
//..........................................................................
//p r o g r a m for implementing semantics of command BALR
int P_BALR(void)
{
  if (R2 != 0)
    I = VR[R2];
  if (R1 != 0)
    VR[R1] = I;
    
  return 0;
}
//..........................................................................
//p r o g r a m for implementing semantics of command BCR with mask 15
int P_BCR(void)
{
  int ret;
  
  ret = 1;
  if (R1 == 15)
  {
    ret = 0;
    if ((VR[R2] != 0) && (R2 != 0))
      I = VR[R2];
    else
    {
      if (R2 != 0)
      {
        waddstr(wcyan, "jump to address = 0 or end prog.tracing after key press");
	wrefresh(wcyan);
	ret = 1;
      }
    }
  }  

  return ret;
}
/*..........................................................................*/

int P_ST()                                        /*  p r o g r a m         */
						                                      /*for implmenting smantics*/
 {                                                /*of command ST           */
  int sm,i;                                       /*working                 */
  char bytes[4];                                  /*variables               */

  ADDR = VR[B] + VR[X] + D;                       /*calc.of abs.addr.and    */
  sm = (int) (ADDR -I);                           /*displacement            */

  bytes[0] = (VR[R1] -                            /*conversion of content   */
		VR[R1]% 0x1000000L)/0x1000000L;               /*of RON,used as          */
  bytes[1] = ((VR[R1] -                           /*1st operand,            */
		VR[R1]%0x10000L)/0x10000L)%0x100;             /*to format accepted in   */
  bytes[2] = ((VR[R1] % 0x10000L) -               /*ES EVM                  */
		((VR[R1]%0x10000L)%0x100))/0x100;             /*                        */
  bytes[3] = (VR[R1] % 0x10000L) % 0x100;         /*                        */

  for (i=0; i<4; i++)                             /*writing of converted    */
   OBLZ[BAS_IND + CUR_IND + sm + i] = bytes[i];   /*value to addr.of 2nd    */
						                                      /*operand                 */
  return 0;                                       /*successful prog.compl.  */
 }

/*..........................................................................*/

int P_L()                                         /*  p r o g r a m         */
						  /*for implementing semantics*/
 {                                                /*of command L            */
   int sm;                                        /*working variable        */

   ADDR = VR[B] + VR[X] + D;                      /*calc.of abs.addr.and    */
   sm = (int) ( ADDR - I );                       /*displacement            */
   VR[R1] =                                       /*conversion of content   */
    OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L +   /*of 2nd operand to format*/
    OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L + /*accepted in IBM PC,and  */
    OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 +    /*writing to RON,used as  */
    OBLZ[BAS_IND + CUR_IND + sm + 3];             /*1st operand             */

   return 0;                                      /*successful prog.compl.  */
 }

/*..........................................................................*/

int P_A()                                         /*  p r o g r a m         */
						                                      /*for implmenting semntics*/
 {                                                /*of command A            */
  int sm;                                         /*working variable        */

  ADDR = VR[B] + VR[X] + D;                       /*calc.of abs.addr.and    */
  sm = ( int ) ( ADDR -I );                       /*displacement            */
  ARG = OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L+/*formation of content    */
   OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L +  /*of 2nd operand in       */
   OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 +     /*ES EVM conventions      */
   OBLZ[BAS_IND + CUR_IND + sm + 3];              /*                        */
						  /*and                     */
  VR[R1] = VR[R1] + ARG;                          /*addtion with 1st operand*/

  return 0;                                       /*successful prog.compl.  */
 }

/*..........................................................................*/

int P_S()                                         /* p r o g r a m          */
						  /* for implementing semantics*/
 {                                                /* of command S           */
  int sm;                                         /*working variable        */

  ADDR = VR[B] + VR[X] + D;                       /*calc.of working         */
  sm = ( int ) ( ADDR - I );                      /*addr.and displacement   */

  ARG = OBLZ[BAS_IND + CUR_IND + sm] * 0x1000000L+/*formation of content    */
     OBLZ[BAS_IND + CUR_IND + sm + 1] * 0x10000L +/*of 2nd operand in       */
     OBLZ[BAS_IND + CUR_IND + sm + 2] * 0x100 +   /*ES EVM conventions      */
     OBLZ[BAS_IND + CUR_IND + sm + 3];            /*                        */
						  /* and                    */
  VR[R1] = VR[R1] - ARG;                          /*subtr.from 1st operand  */

  return 0;                                       /*successful prog.compl.  */
 }


//..........................................................................
int FRR(void)
{
  int i, j;
  
  for (i = 0; i < NOP; i++)
  {
    if (INST[0] == T_MOP[i].CODOP)
    {
      waddstr(wgreen, "      ");
      for (j = 0; j < 5; j++)
        waddch(wgreen, T_MOP[i].MNCOP[j]);
      waddstr(wgreen, " "); 
      
      j = (INST[1] - (INST[1] % 0x10)) / 0x10;
      R1 = j;
      wprintw(wgreen, "%1d, ", j);
      j = INST[1] % 0x10;
      R2 = j;
      wprintw(wgreen, "%1d\n", j);
      break;
    }
  }

  return 0; 
}
//...........................................................................
int FRX(void)
{
  int i, j;
  
  for (i = 0; i < NOP; i++)
  {
    if (INST[0] == T_MOP[i].CODOP)
    {
      waddstr(wgreen, "  ");
      for (j = 0; j < 5; j++)
        waddch(wgreen, T_MOP[i].MNCOP[j]);
      waddstr(wgreen, " ");
      
      j = INST[1] >> 4;
      R1 = j;
      wprintw(wgreen, "%.1d, ", j);
      
      j = INST[2] % 16;
      j = j * 256 + INST[3];
      D = j;
      wprintw(wgreen, "X'%.3X'(", j);
      
      j = INST[1] % 16;
      X = j;
      wprintw(wgreen, "%1d, ", j);
      
      j = INST[2] >> 4;
      B = j;
      wprintw(wgreen, "%1d)", j);
      
      ADDR = VR[B] + VR[X] + D;
      wprintw(wgreen,"        %.06lX       \n", ADDR);
      if (ADDR % 4 != 0)
        return (7);
      break;
    }
  }

  return 0;
} 

//...........................................................................   
//---------------------------------------------------------------------------
int wind(void)
{
  int j1, k, temp;
  unsigned long line_addr;
  int line_kk;

  x = 0;
  y = 16;

  LIGHTPTR = I1;

  for (j1 = 0; j1 < 8; j1++)
  {
    line_addr = I1 + (j1 * 16);
    line_kk = (int)(I1 - BAS_ADDR) + BAS_IND + (j1 * 16);
    
    wmove(wred, j1, 0);
    wprintw(wred, "%.06lX: ", line_addr);
    
    for (j = 0; j < 4; j++)
    {
      for (k = 0; k < 4; k++)
	wprintw(wred, "%.02X", OBLZ[line_kk + j * 4 + k]);
      waddstr(wred, " ");
    }

    /* CHAR column - printable ASCII or '.' */
    waddstr(wred, "| CHAR: ");
    for (j = 0; j < 16; j++)
    {
      waddch(wred, byte_to_print(OBLZ[line_kk + j]));
    }

    waddstr(wred, " */");
  }
  wrefresh(wred);			//вывод на экран
  wclear(wred);				//очистка содержимого окна дампа
  
  return 0;
}
//---------------------------------------------------------------------------
//program for command-by-command interpretation(debugging)
// of loaded program
int sys(void) 			
{
  int res, temp;
  int ch;
  int gr_pos_x, gr_pos_y;
  int ii = 0, jj = 0;
  int gr_y;
  char wstr[80];
  int zizi = 0, tempI;
  
  
  I = BAS_ADDR;			/*set current address     */
  				          /*equal to initial        */
//bottom field  
  wmargenta = newwin(1, 80, 24, 0);
  wbkgd(wmargenta, COLOR_PAIR(COLOR_MAGENTA));
  waddstr(wmargenta, "\"PgUp\",\"PgDn\",\"Up\",\"Down\"->view dump; \"Enter\"->execute next cmd");

//status line
  wcyan = newwin(1, 80, 23, 0);
  wbkgd(wcyan, COLOR_PAIR(COLOR_CYAN));

//load area dump
  wred = newwin(8, 72, 15, 0);
  wbkgd(wred, COLOR_PAIR(COLOR_RED));

//registers field
  wblue = newwin(16, 12, 0, 72);
  wbkgd(wblue, COLOR_PAIR(COLOR_BLUE));

//text
  gr_pos_x = 0;
  gr_pos_y = 14;
  gr_y = 11;
  wgreen = newwin(gr_y, 67, gr_pos_y, gr_pos_x);	/*create new window       */
  wbkgd(wgreen, COLOR_PAIR(COLOR_GREEN));	        /*select color pair       */


  keypad(wmargenta, TRUE);				                /*enable kbd code convrsin*/

BEGIN:  

//compare all valid cmd codes with current and on
// match,remember row number of op.table
  for (i = 0; i < NOP; i++)
  {
    if (OBLZ[BAS_IND + CUR_IND] == T_MOP[i].CODOP)
    {
      k = i;
      wprintw(wgreen, "%.06lX: ", I);
//draw window,output text
      for (j = 0; j < 6; j++)                     /*                        */
      {                                           /*                        */
        if (j < T_MOP[i].DLOP)                    /*                        */
        {                                         /* output hexadecimal     */
	     wprintw(wgreen, "%.02X", OBLZ[BAS_IND + CUR_IND + j]);
						                                      /* remember it in         */
        INST[j] =                                 /* variable INST,         */
  		     OBLZ [BAS_IND + CUR_IND + j];          /*                        */
        }                                         /*                        */
        else INST [j] = '\x00';                   /*                        */
      }
      if ((res = T_MOP[i].BXPROG()) != 0)         /* go to subr.for selctig */
	return (res);   			                          /* assembler equivalent   */
						                                      /* of current command,    */
						                                      /*                        */
	goto l0;                                        /* proceed to further     */
    }
  }
  return (6);
  
l0:
//scroll window up
  wrefresh(wgreen);
  ii++;
  if (gr_pos_y > 14 - gr_y + 1)
    mvwin(wgreen, gr_pos_y--, gr_pos_x);
//when reaching certain position,movement stops,and
//window scrolling is performed
  else
  {
      for (jj = 0; jj < gr_y - 1; jj++)
      {
        temp = mvwinnstr(wgreen, jj + 1, 0, wstr, 67);
        mvwaddnstr(wgreen, jj, 0, wstr, 67);
        wrefresh(wgreen);
      }
  }
  wrefresh(wgreen);

  I += T_MOP[k].DLOP;                           /*correct addr.counter    */
  CUR_IND = ( int ) ( I - BAS_ADDR );           /*set current index       */
						                                    /*in OBLZ array           */
  I1 = I;                                       /*set highlight area      */
						                                    /*start address           */
      
  for ( i = 0; i < 16; i++)
  {
    if (i < 10)
      waddstr(wblue, "R0");
    else 
      waddstr(wblue, "R");
    wprintw(wblue, "%d:", i);
    wprintw(wblue, "%.08lX", VR[i]);
  }      
  wrefresh(wblue);			                          /*output to screen        */
  wclear(wblue);			                            /*clear registers window  */
  wind();

  waddstr(wcyan, "ready to execute next command at address ");
  wprintw(wcyan, "%.06lX", I - T_MOP[k].DLOP);
  waddstr(wcyan, "\n");
  wrefresh(wcyan);
  wclear(wcyan);

WAIT:
  
  CUR_IND = (int)(I - BAS_ADDR);

  ch = wgetch(wmargenta);

  switch (ch)
  {
    case 10:
    {
      goto SKIP;
    }
    
    case  KEY_UP:
    {
      I1 = LIGHTPTR - 16;
      CUR_IND = (int)(I1 - BAS_ADDR);
      wind();
      goto WAIT;
    }
    
    case  KEY_DOWN:
    {
      I1 = LIGHTPTR + 16;
      CUR_IND = (int)(I1 - BAS_ADDR);
      wind();
      goto WAIT;      
    }
    
    case  KEY_PPAGE:
    {
      I1 = LIGHTPTR - 128;
      CUR_IND = (int)(I1 - BAS_ADDR);
      wind();
      goto WAIT;
    }
    
    case  KEY_NPAGE:
    {
      I1 = LIGHTPTR + 128 ;
      CUR_IND = (int)(I1 - BAS_ADDR);
      wind();
      goto WAIT;
    }
  }
goto WAIT;

SKIP:
   switch (T_MOP[k].CODOP)                        /*according to cmd code,  */
   {                                              /*selected by addr.counter*/
						                                      /*select subr.for inter-  */
    case '\x05' : P_BALR();                       /*preting semantics       */
		   break;                                     /*of current command      */
    case '\x07' : { i = P_BCR();
		    getch();
		    if (i == 1)
		     return 8;
		  }
		   break;
    case '\x50' : P_ST();
		   break;
    case '\x58' : P_L();
		   break;
    case '\x5A' : P_A();
		   break;
    case '\x5B' : P_S();
   }
   
   goto BEGIN;	
   
   delwin(wblue);			  
   delwin(wred);
   delwin(wgreen);
   delwin(wmargenta);

  return 0;
}
//...........................................................................
//..........................curses Initialization..............................
int InitCurses(void)
{
  initscr();					    /*initialize curses library */
  curs_set(0);
  noecho();					      /*hide input                */
  cbreak();					      /*read one character        */
                          /*at a time,dont wait for \n*/
  keypad(stdscr, TRUE);		/*enable kbard code convrsin*/
  start_color();

  
  if (has_colors())
  {
    init_pair(COLOR_BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_RED, COLOR_WHITE, COLOR_RED);
    init_pair(COLOR_CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_MAGENTA, COLOR_WHITE, COLOR_MAGENTA);
  }

  
  return 0;
}
//...........................................................................

int main( int argc, char **argv )                 /* p r o g r a m          */
						                                      /*of absolute loader      */
						                                      /*of object files         */
{
  int  I,K,N,J0,res;                              /*working                 */
  unsigned long J;                                /*variables               */
  FILE *fp;                                       /*program                 */
  char *ptr;

//main programm


  if ( argc != 2 )
  {
    printf ( "%s\n", "Error in command line" );
    return -1;
  }
 
  ptr = argv[1];
  strcpy ( NFIL, ptr );

  if ( strcmp ( &NFIL [ strlen ( NFIL )-3 ], "mod" ) )
  {
    goto ERR9;
    return -1;
  }
  
  if ((fp = fopen(NFIL,"rt")) == NULL)
    goto ERR1;                                    /*error message           */
  else
  {
    while ( !feof( fp ) )                         /*read all file cards     */
     {                                            /*with list               */
      fgets ( SPISOK [ISPIS++] , 80 , fp );       /*to SPISOK array         */
      if ( ISPIS == NSPIS )                       /*if this array is over-  */
       {                                          /*filled,then:            */
	fclose ( fp );                                  /*close file with list    */
	goto ERR4;                                      /*and output error message*/
       }
     }
    fclose ( fp );                                /*close SPISOK file       */

    if ( ISPIS == 0 )                             /*if list is empty        */
						                                      /*then:                   */
     goto ERR2;                                   /* error message          */
    else                                          /*otherwise:              */
     goto CONT1;                                  /* continue processing    */
   }

CONT1:

  for ( I = 0; I < ISPIS; I++ )                   /*iterating all assembled */
   {
                                                  /*object files,           */
    if ((fp = fopen(SPISOK[I], "rb" )) ==  NULL)
      goto ERR3;                                  /*                        */
    else                                          /* otherwise:             */
     {                                            /*                        */
      while ( !feof( fp) )                        /*  read file to end,     */
       {                                          /*  place records in      */
	fread ( OBJCARD [IOBJC++] , 80 , 1 , fp );      /*  OBJCARD array and,if  */
	if ( IOBJC == NOBJ )                            /*  not all records read, */
	 {                                              /*  then:                 */
	  fclose ( fp );                                /*   output error message */
	  goto ERR5;                                    /*                        */
	 }
       }                                          /*                        */
      fclose ( fp );                              /*  close next file       */

      goto CONT2;                                 /* and continue processing*/
     }
   }
       
CONT2:

  POINT.P_OBLZ = OBLZ;                            /*calculate absolute      */
  J = POINT.VAL_P.SEGM ;                          /*addr.of load area       */
  J = J << 4;                                     /*OBLZ in variable J      */
  J += POINT.VAL_P.SMESH;

  if ( ( J0 = (int) J%8 ) == 0 )                  /*align received          */
   {
    BAS_ADDR = J;                                 /*value to boundary       */
    BAS_IND  = 0;
   }
  else                                            /*double word and store   */
   {
    BAS_ADDR = ( ( J >> 3 ) + 1 ) << 3;           /*it in BAS_ADDR variable */
    BAS_IND = 8 - J0;                             /*and corr. OBLZ arr.index*/
   }						  /*in BAS_IND              */

  for ( I = 0; I < IOBJC; I++ )                   /*iterating all read      */
   {                                              /*object file cards,      */
    if ( !memcmp ( &OBJCARD [I][1] , "TXT" , 3 ) )/*select belonging to type*/
     {                                            /*TXT and calc:           */
      memcpy ( TXT.BUF_TXT , OBJCARD [I] , 80 );  /*                        */
      J = TXT.STR_TXT.ADOP [0];                   /* in variable J initial  */
      J = (J << 8) + TXT.STR_TXT.ADOP [1];        /*  load index in arr-    */
      J = (J << 8) + TXT.STR_TXT.ADOP [2];        /*  ay OBLZ               */
      J += BAS_IND;                               /*and                     */
						  /*                        */
      K = TXT.STR_TXT.DLNOP [0];                  /* in variable K length   */
      K = (K << 8) + TXT.STR_TXT.DLNOP [1];       /* of loaded data         */

      for ( N=0; N < K; N++ )                     /*load data from current  */
       OBLZ [ (int) J++ ] = TXT.STR_TXT.OPER [N]; /*object card             */
     }
   }
   



  InitCurses();

  res = sys(); 
  
  switch (res)
  {
    case 6: 
    {
      endwin();
      goto ERR6;
    }
    case 7:
    {
      endwin();
      goto ERR7;
    }
    case 8:
    {
      endwin();
      goto ERR8;
    }
  }
  
  endwin();
  END:
  printf ("\n%s\n", "processing complete");

  return 0;
//D I A G N O S T I C  M E S S A G E S  B L O C K
ERR1:
  printf ("%s%s\n", "error opening file with list of assembled ", "modules");
  goto END;

ERR2:
  printf ("%s\n", "empty file with list of assembled modules");
  goto END;

ERR3:
  printf ("%s: %s\n" ,
   "error opening file" , SPISOK [I] );
  goto END;

ERR4:
  printf ("%s\n" ,
   "overflow of assembled modules list" );
  goto END;

ERR5:
  printf ("%s\n" ,
   "overflow of object cards storage buffer");
  goto END;

ERR6:
  printf ("%s\n" ,
   "invalid command code" );
  goto END;

ERR7:
  printf("interrupt - addressing error\n");
  goto END;

ERR8:
  goto END;

ERR9:
  printf ( "%s\n", "Invalid file type with source text" );
  goto END;
}
