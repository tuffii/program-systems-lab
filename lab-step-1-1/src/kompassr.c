#define DL_ASSTEXT 16
#define DL_OBJTEXT 50                             /*length of obj. text     */
#define NSYM 10                                   /*size of symbol table    */
#define NPOP 6                                    /*size of pseudo-op table */
#define NOP  6                                    /*size of op. table       */
#include <string.h>                               /*incl. string subr.      */
#include <stdlib.h>                               /*incl. data conv. subr.  */
#include <stdio.h>                                /*incl. std I/O subr.     */
#include <ctype.h>                                /*incl. char class. subr. */

/*
******* B L O C K  of static working variables declarations
*/

char NFIL [30] = "\x0";

unsigned char PRNMET = 'N';                       /*label detect. indicator */
int I3;                                           /*loop counter            */

/*
***** B L O C K  of 1st pass subroutine prototypes declarations
*/

						  /* p r o t o t y p e  for */
int FDC();                                        /*subr.proc. DC pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FDS();                                        /*subr.proc. DS pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FEND();                                       /*subr.proc. END pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FEQU();                                       /*subr.proc. EQU pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FSTART();                                     /*subr.proc. START pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FUSING();                                     /*subr.proc. USING pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FRR();                                        /*subr.proc. op. RR-format*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int FRX();                                        /*subr.proc. op. RX-format*/
/*..........................................................................*/

/*
***** B L O C K  of 2nd pass subroutine prototypes declarations
*/

						  /* p r o t o t y p e  for */
int SDC();                                        /*subr.proc. DC pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SDS();                                        /*subr.proc. DS pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SEND();                                       /*subr.proc. END pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SEQU();                                       /*subr.proc. EQU pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SSTART();                                     /*subr.proc. START pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SUSING();                                     /*subr.proc. USING pseudo-op.*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SRR();                                        /*subr.proc. op. RR-format*/
/*..........................................................................*/
						  /* p r o t o t y p e  for */
int SRX();                                        /*subr.proc. op. RX-format*/
/*..........................................................................*/

/*
******* B L O C K  of compiler database tables declarations
*/

/*
******* DECLARATION of source text line (card) structure
*/

 struct ASSKARTA                                  /*struct. of ASSEMBLER card*/
  {
   unsigned  char METKA    [ 8];                  /*label field             */
   unsigned  char PROBEL1  [ 1];                  /*separator space         */
   unsigned  char OPERAC   [ 5];                  /*operation field         */
   unsigned  char PROBEL2  [ 1];                  /*separator space         */
   unsigned  char OPERAND  [12];                  /*operand field           */
   unsigned  char PROBEL3  [ 1];                  /*separator space         */
   unsigned  char COMM     [52];                  /*comment field           */
  };

/*
******* OVERLAY of source text card structure onto input buffer
*/

 union                                            /*define union            */
  {
   unsigned char BUFCARD [80];                    /*source text card buffer */
   struct ASSKARTA STRUCT_BUFCARD;                /*overlay template on buff*/
  } TEK_ISX_KARTA;

/*
***** RELATIVE ADDRESS COUNTER (offset relative to base)
*/

 int CHADR;                                       /*counter                 */

/*
***** SYMBOL TABLE          
*/

 int ITSYM = -1;                                  /*index of free sym. table*/
 struct TSYM                                      /*symbol table row struct */
  {
   unsigned char IMSYM [8];                       /*symbol name             */
   int           ZNSYM;                           /*symbol value            */
   int           DLSYM;                           /*symbol length           */
   char          PRPER;                           /*relocation flag         */
  };

 struct TSYM T_SYM [NSYM];                        /*symbol table definition */

/*
***** MACHINE OPERATIONS TABLE
*/

 struct TMOP                                      /*machine op. table struct*/
  {
   unsigned char MNCOP [5];                       /*operation mnemonic      */
   unsigned char CODOP    ;                       /*machine op. code        */
   unsigned char DLOP     ;                       /*length of op. in bytes  */
   int (*BXPROG)()        ;                       /*pointer to proc. subr.  */
  } T_MOP [NOP]  =                                /*machine op. table decl. */
    {
     {{'B','A','L','R',' '} , '\x05' , 2 , FRR} , /*initialization          */
     {{'B','C','R',' ',' '} , '\x07' , 2 , FRR} , /*of                      */
     {{'S','T',' ',' ',' '} , '\x50' , 4 , FRX} , /*table                   */
     {{'L',' ',' ',' ',' '} , '\x58' , 4 , FRX} , /*machine                 */
     {{'A',' ',' ',' ',' '} , '\x5A' , 4 , FRX} , /*operations              */
     {{'S',' ',' ',' ',' '} , '\x5B' , 4 , FRX} , /*                        */
    };

/*
***** PSEUDO-OPERATIONS TABLE
*/

 struct TPOP                                      /*pseudo-op. table struct */
  {
   unsigned char MNCPOP[5];                       /*pseudo-op. mnemonic     */
   int (*BXPROG) ()       ;                       /*pointer to proc. subr.  */
  } T_POP [NPOP] =                                /*pseudo-op. table decl.  */
    {
     {{'D','C',' ',' ',' '} , FDC   },            /*initialization          */
     {{'D','S',' ',' ',' '} , FDS   },            /*of                      */
     {{'E','N','D',' ',' '} , FEND  },            /*table                   */
     {{'E','Q','U',' ',' '} , FEQU  },            /*of pseudo-operations    */
     {{'S','T','A','R','T'} , FSTART},            /*                        */
     {{'U','S','I','N','G'} , FUSING}             /*                        */
    };

/*
***** BASE REGISTERS TABLE    
*/

 struct TBASR                                     /*base reg. table struct  */
  {
   int SMESH;                                     /*                        */
   char PRDOST;                                   /*                        */
  } T_BASR[15] =                                  /*                        */
    {
     {0x00,'N'},                                  /*initialization          */
     {0x00,'N'},                                  /*of                      */
     {0x00,'N'},                                  /*table                   */
     {0x00,'N'},                                  /*of base                 */
     {0x00,'N'},                                  /*registers               */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'},                                  /*                        */
     {0x00,'N'}                                   /*                        */
    };

/*
***** B L O C K   of object text array declaration
*/

  unsigned char OBJTEXT [DL_OBJTEXT][80];         /*object cards array      */
  int ITCARD = 0;                                 /*current card pointer    */

  struct OPRR                                     /*RR fmt. op. bufer struct*/
   {
    unsigned char OP;                             /*operation code          */
    unsigned char R1R2;                           /*R1 - 1st operand        */
						  /*R2 - 2nd operand        */
   };

  union                                           /*define union            */
   {
    unsigned char BUF_OP_RR [2];                  /*define buffer           */
    struct OPRR OP_RR;                            /*structure it            */
   } RR;

  struct OPRX                                     /*RX fmt. op. buffer struct*/
   {
    unsigned char OP;                             /*operation code          */
    unsigned char R1X2;                           /*R1 - 1st operand        */
    short B2D2;                                   /*X2 - 2nd operand        */
//    int B2D2;                                   /*X2 - 2nd operand        */
						  /*B2 - base reg. of 2nd op*/
						  /*D2 - disp. rel. base    */
   };

  union                                           /*define union            */
   {
    unsigned char BUF_OP_RX [4];                  /*define buffer           */
    struct OPRX OP_RX;                            /*structure it            */
   } RX;

  struct STR_BUF_ESD                              /*ESD card buffer struct  */
   {
    unsigned char POLE1      ;                    /*place for code 0x02     */
    unsigned char POLE2  [ 3];                    /*object card type field  */
    unsigned char POLE3  [ 6];                    /*spaces                  */
    unsigned char POLE31 [ 2];                    /*data length on card     */
    unsigned char POLE32 [ 2];                    /*spaces                  */
    unsigned char POLE4  [ 2];                    /*internal prog. name id. */
    unsigned char IMPR   [ 8];                    /*program name            */
    unsigned char POLE6      ;                    /*ESD name type code      */
    unsigned char ADPRG  [ 3];                    /*relative prog. address  */
    unsigned char POLE8      ;                    /*spaces                  */
    unsigned char DLPRG  [ 3];                    /*program length          */
    unsigned char POLE10 [40];                    /*spaces                  */
    unsigned char POLE11 [ 8];                    /*identification field    */
   };

 struct STR_BUF_TXT                               /*TXT card buffer struct  */
   {
    unsigned char POLE1      ;                    /*place for code 0x02     */
    unsigned char POLE2  [ 3];                    /*object card type field  */
    unsigned char POLE3      ;                    /*space                   */
    unsigned char ADOP   [ 3];                    /*relative op. address    */
    unsigned char POLE5  [ 2];                    /*spaces                  */
    unsigned char DLNOP  [ 2];                    /*operation length        */
    unsigned char POLE7  [ 2];                    /*spaces                  */
    unsigned char POLE71 [ 2];                    /*internal prog. ident.   */
    unsigned char OPER   [56];                    /*operation body          */
    unsigned char POLE9  [ 8];                    /*identification field    */
   };

struct STR_BUF_END                                /*END card buffer struct  */
   {
    unsigned char POLE1      ;                    /*place for code 0x02     */
    unsigned char POLE2  [ 3];                    /*object card type field  */
    unsigned char POLE3  [68];                    /*spaces                  */
    unsigned char POLE9  [ 8];                    /*identification field    */
   };

  union                                           /*define union            */
   {
    struct STR_BUF_ESD STR_ESD;                   /*buffer structure        */
    unsigned char BUF_ESD [80];                   /*ESD card buffer         */
   } ESD;


  union                                           /*define union            */
   {
    struct STR_BUF_TXT STR_TXT;                   /*buffer structure        */
    unsigned char BUF_TXT [80];                   /*TXT card buffer         */
   } TXT;

  union                                           /*define union            */
   {
    struct STR_BUF_END STR_END;                   /*buffer structure        */
    unsigned char BUF_END [80];                   /*ESD card buffer         */
   } END;


/*
******* B L O C K  of subroutines used in 1st pass declarations
*/

int FDC()                                         /*subr.proc. DC pseudo-op.*/
 {
  if ( PRNMET == 'Y' )                            /*if DC pseudo-op marked, */
   {                                              /*then:                   */
    if                                            /* if DC pseudo-op        */
     (                                            /* defines constant       */
      TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F'/* of type F, then do     */
     )                                            /* following:             */
     {
      T_SYM[ITSYM].DLSYM = 4;                     /*  set symbol length = 4,*/
      T_SYM[ITSYM].PRPER = 'R';                   /*  and reloc. flag='R'   */
      if ( CHADR % 4 )                            /* and, if CHADR not point*/
       {                                          /* to word boundary, then:*/
	CHADR = (CHADR /4 + 1) * 4;                     /*   set CHADR to word bnd*/
	T_SYM[ITSYM].ZNSYM = CHADR;                     /*   and store in sym. tbl*/
       }
      PRNMET = 'N';                               /*  reset PRNMET to 'N'   */
     }
    else
     return (1);                                  /* otherwise error exit   */
   }
  else                                            /*if pseudo-op not marked */
   if ( CHADR % 4 )                               /*and CHADR not mult.of 4:*/
    CHADR = (CHADR /4 + 1) * 4;                   /* set CHADR to word bnd. */

  CHADR = CHADR + 4;                              /*increase CHADR by 4     */
  return (0);                                     /*sucessfuly complete subr*/
 }
/*..........................................................................*/
int FDS()                                         /*subr.proc. DS pseudo-op.*/
 {
  if ( PRNMET == 'Y' )                            /*if DC pseudo-op marked, */
   {                                              /*then:                   */
    if                                            /* if DC pseudo-op        */
     (                                            /* defines constant       */
      TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F'/* of type F, then do     */
     )                                            /* following:             */
     {
      T_SYM[ITSYM].DLSYM = 4;                     /*  set symbol length = 4,*/
      T_SYM[ITSYM].PRPER = 'R';                   /*  and reloc. flag='R'   */
      if ( CHADR % 4 )                            /* and, if CHADR not point*/
       {                                          /* to word boundary, then:*/
	CHADR = (CHADR /4 + 1) * 4;                     /*  set CHADR to word bnd.*/
	T_SYM[ITSYM].ZNSYM = CHADR;                     /*  and store in sym. tbl.*/
       }
      PRNMET = 'N';                               /*  reset PRNMET to 'N'   */
     }
    else
     return (1);                                  /* otherwise error exit   */
   }
  else                                            /*if pseudo-op not marked */
   if ( CHADR % 4 )                               /*and CHADR not mult of 4:*/
    CHADR = (CHADR /4 + 1) * 4;                   /* set CHADR to word bnd. */

  CHADR = CHADR + 4;                              /*increase CHADR by 4     */
  return (0);                                     /*successfully complt subr*/
 }
/*..........................................................................*/
int FEND()                                        /*subr.proc. END pseudo-op*/
 {
  return (100);                                   /*exit with 1st pass end  */
						  /*flag                    */
 }
/*..........................................................................*/
int FEQU()                                        /*subr.proc EQU pseudo-op.*/
 {
  if                                              /*if in operand           */
   (                                              /*of DC pseudo-op         */
    TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0] =='*' /*symbol '*' is used,     */
   )                                              /*then                    */
    {                                             /* store in symbol table: */
     T_SYM[ITSYM].ZNSYM = CHADR;                  /*  CHADR to ZNSYM field, */
     T_SYM[ITSYM].DLSYM = 1;                      /*  1 to DLSYM field,     */
     T_SYM[ITSYM].PRPER = 'R';                    /*  'R' to PRPER field    */
    }
  else                                            /*otherwise store in sym. */
    {                                             /* value of DC pseudo-op  */
     T_SYM[ITSYM].ZNSYM = atoi (                  /* operand to ZNSYM field,*/
      (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND
			       );
     T_SYM[ITSYM].DLSYM = 1;                      /* 1 to DLSYM field,      */
     T_SYM[ITSYM].PRPER = 'A';                    /* 'A' to PRPER field     */
    }
  PRNMET = 'N';                                   /*reset PRNMET to 'N'     */
  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int FSTART()                                      /*subrproc START pseudo-op*/
 {                                                /*set CHADR equal to      */
  CHADR =                                         /*operand value           */
       atoi(
       (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND/*of START pseudo-op and, */
	   );
  if ( CHADR % 8 )                                /*if this value is not    */
   {                                              /*multiple of 8, make it  */
    CHADR = ( CHADR + ( 8 - CHADR % 8 ) );        /*multiple of 8           */
   }                                              /*store in symbol table:  */
  T_SYM[ITSYM].ZNSYM = CHADR;                     /* CHADR to ZNSYM field,  */
  T_SYM[ITSYM].DLSYM = 1;                         /* 1 to DLSYM field,      */
  T_SYM[ITSYM].PRPER = 'R';                       /* 'R' to PRPER field     */
  PRNMET = 'N';                                   /*reset PRNMET to 'N'     */
  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int FUSING()                                      /*subrproc USING pseudo-op*/
 {
  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int FRR()                                         /*subr.proc. op. RR-format*/
 {
  CHADR = CHADR + 2;                              /*incre addr. counter by 2*/
  if ( PRNMET == 'Y' )                            /*if label detect earlier,*/
   {                                              /*then in symbol table:   */
    T_SYM[ITSYM].DLSYM = 2;                       /*store machine op. length*/
    T_SYM[ITSYM].PRPER = 'R';                     /*and set relocation flag */
   }
  return(0);                                      /*exit subroutine         */
 }
/*..........................................................................*/
int FRX()                                         /*subr.proc. op. RX-format*/
 {
  CHADR = CHADR + 4;                              /*increa addr counter by 4*/
  if ( PRNMET == 'Y' )                            /*if label detect earlier,*/
   {                                              /*then in symbol table:   */
    T_SYM[ITSYM].DLSYM = 4;                       /*store machine op. length*/
    T_SYM[ITSYM].PRPER = 'R';                     /*and set relocation flag */
   }
  return(0);                                      /*exit subroutine         */
 }
/*..........................................................................*/

/*
******* B L O C K  of subroutines used in 2nd pass declarations
*/

void STXT( int ARG )                              /*subroutin form. TXT card*/
 {
  char *PTR;                                      /*working pointer variable*/

  PTR = (char *)&CHADR;                           /*form ADOP field         */
  TXT.STR_TXT.ADOP[2]  = *PTR;                    /*of TXT card in          */
  TXT.STR_TXT.ADOP[1]  = *(PTR+1);                /*binary integer format   */
  TXT.STR_TXT.ADOP[0]  = '\x00';                  /*in ES EVM conventions   */

  if ( ARG == 2 )                                 /*form OPER field         */
   {
    memset ( TXT.STR_TXT.OPER , 64 , 4 );
    memcpy ( TXT.STR_TXT.OPER,RR.BUF_OP_RR , 2 ); /* for RR format          */
    TXT.STR_TXT.DLNOP [1] = 2;
   }
  else
   {
    memcpy ( TXT.STR_TXT.OPER , RX.BUF_OP_RX , 4);/* for RX format          */
    TXT.STR_TXT.DLNOP [1] = 4;
   }
  memcpy (TXT.STR_TXT.POLE9,ESD.STR_ESD.POLE11,8);/*form identifcation field*/

  memcpy ( OBJTEXT[ITCARD] , TXT.BUF_TXT , 80 );  /*write object card       */
  ITCARD += 1;                                    /*correct free card index */
  CHADR = CHADR + ARG;                            /*correct address counter */
  return;
 }

int SDC()                                         /*subr.proc. DC pseudo-op.*/
 {
  char *RAB;                                      /*working variable        */

  RX.OP_RX.OP   = 0;                              /*zero two high           */
  RX.OP_RX.R1X2 = 0;                              /*bytes of RX.OP_RX       */
  if
    (                                             /* if operand starts      */
     !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND,/* with combination       */
	      "F'", 2)                                  /* F',                    */
    )                                             /* then                   */
   {
    RAB=strtok                                    /*in var. with pointer RAB*/
	 (                                              /*select first lexeme     */
    (char*)TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND+2,/*of current card operand */
	  "'"                                           /*of ASSEMBLER source text*/
	 );

    RX.OP_RX.B2D2 = atoi ( RAB );                 /*ASCII to int conversion */
    RAB = (char *) &RX.OP_RX.B2D2;                /*conversion to convetions*/
    swab ( RAB , RAB , 2 );                       /* of ES EVM              */
   }
  else                                            /*otherwise               */
   return (1);                                    /*error message           */

  STXT (4);                                       /*form TXT card           */


  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int SDS()                                         /*subr.proc. DS pseudo-op.*/
 {

  RX.OP_RX.OP   = 0;                              /*zero two high           */
  RX.OP_RX.R1X2 = 0;                              /*bytes of RX.OP_RX       */
  if
    (                                             /* if operand starts      */
     TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND[0]=='F' /* with combination F'    */
    )                                             /* then:                  */
   RX.OP_RX.B2D2 = 0;                             /*zero RX.OP_RX.B2D2      */
  else                                            /*otherwise               */
   return (1);                                    /*error message           */

  STXT (4);                                       /*form TXT card           */

  return (0);                                     /*sucessfuly complete subr*/
 }
/*..........................................................................*/
int SEND()                                        /*subr.proc. END pseudo-op*/
 {
						  /*formation               */
  memcpy (                                        /*identification field    */
	  END.STR_END.POLE9 ,                           /* END card               */
	  ESD.STR_ESD.POLE11,                           /*                        */
	  8                                             /*                        */
	 );                                             /*                        */
  memcpy (                                        /*write object card       */
	  OBJTEXT[ITCARD],                              /* to                     */
	  END.BUF_END,                                  /* array                  */
	  80                                            /* object                 */
	 );                                             /* cards                  */
  ITCARD += 1;                                    /*correct free card index */
  return (100);                                   /*exit with 2nd pass end  */
						                                      /*flag                    */
 }
/*..........................................................................*/
int SEQU()                                        /*subr.proc. EQU pseudo-op*/
 {
  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int SSTART()                                      /*subrproc START pseudo-op*/
 {
  char *PTR;                                      /*set                     */
  char *METKA;                                    /*working                 */
  char *METKA1;                                   /*variables               */
  int J;                                          /*subroutine              */
  int RAB;                                        /*                        */

  METKA1 = strtok                                 /*in var. with ptr METKA1 */
	   (                                            /*select first lexeme     */
    (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.METKA,   /*of current card operand */
	    " "                                         /*of ASSEMBLER source text*/
	   );
  for ( J=0; J<=ITSYM; J++ )                      /* all source text labels */
   {                                              /* in table T_SYM compare */
						                                      /* with value *METKA1     */
    METKA = strtok (
		    (char*) T_SYM[J].IMSYM , " "
		   );
    if( !strcmp ( METKA , METKA1 ) )              /* and on match:          */
     {                                            /*  take addr.counter diff*/
      RAB = CHADR - T_SYM[J].ZNSYM;               /*  of this label, forming*/
      PTR = (char *)&RAB;                         /*  program length in     */
      swab ( PTR , PTR , 2 );                     /*  ES EVM conventions,   */
      ESD.STR_ESD.DLPRG [0] = 0;                  /*  write it to ESD card  */
      ESD.STR_ESD.DLPRG [1] = *PTR;               /*  byte by byte          */
      ESD.STR_ESD.DLPRG [2] = *(PTR + 1);         /*                        */
      CHADR = T_SYM[J].ZNSYM;                     /*set CHADR equal to      */
                                                  /*operand of START pseudo-*/
                                                  /*of source text          */
      PTR = (char *)&CHADR;                       /*form ADPRG field        */
      ESD.STR_ESD.ADPRG[2]  = *PTR;               /*of ESD card in          */
      ESD.STR_ESD.ADPRG[1]  = *(PTR+1);           /*binary integer format   */
      ESD.STR_ESD.ADPRG[0]  = '\x00';             /*in ES EVM conventions   */
      memcpy (                                    /*formation               */
	       ESD.STR_ESD.IMPR ,                       /* program name           */
	       METKA ,                                  /*  and                   */
	       strlen ( METKA )                         /*                        */
	     );                                         /*                        */
      memcpy (                                    /*identification field    */
	       ESD.STR_ESD.POLE11 ,                     /* ESD card               */
	       METKA ,                                  /*                        */
	       strlen ( METKA )                         /*                        */
	     );                                         /*                        */
      memcpy (                                    /*write object card       */
	       OBJTEXT[ITCARD] ,                        /* to                     */
	       ESD.BUF_ESD ,                            /* array                  */
	       80                                       /* object                 */
	     );                                         /* cards                  */
      ITCARD += 1;                                /*correct free card index */
      return (0);                                 /*successful subr. compl. */
     }
   }
  return (2);                                     /*erroneous prog. compl.  */
 }
/*..........................................................................*/
int SUSING()                                      /*subrproc USING pseudo-op*/
 {
						                                      /*set                     */
  char *METKA;                                    /*working                 */
  char *METKA1;                                   /*variables               */
  char *METKA2;                                   /*                        */
  int J;                                          /*                        */
  int NBASRG;                                     /*                        */
  METKA1 = strtok                                 /*in var. with ptr METKA1 */
	   (                                            /*select first lexeme     */
    (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*of current card operand */
	    ","                                         /*of ASSEMBLER source text*/
	   );
  METKA2 = strtok                                 /*in var. with ptr METKA2 */
	   (                                            /*select second lexeme    */
	    NULL,                                       /*of current card operand */
	    " "                                         /*of ASSEMBLER source text*/
	   );
  if ( isalpha ( (int) *METKA2 ) )                /*if lexeme starts        */
   {                                              /*with letter, then:      */

    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA2     */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA2 ) )            /* and on match:          */
       {                                          /*  store value           */
	 if ( (NBASRG = T_SYM[J].ZNSYM) <= 0x0f )       /*  in NBASRG and if next */
	  goto SUSING1;                                 /*  NBASRG <= 0x0f go to  */
						                                      /*  set base register     */
	 else                                           /* otherwise:             */
	  return (6);                                   /*  error message         */

       }
     }
    return (2);                                   /*complete subr. on error */
   }
  else                                            /*otherwise if 2nd operand*/
   {                                              /*starts with digit, then:*/
    NBASRG = atoi ( METKA2 );                     /* store it in NBASRG     */
    if ( (NBASRG = T_SYM[J].ZNSYM) <= 0x0f )      /* and,if it <= 0x0f,then:*/
     goto SUSING1;                                /* go to set base reg.    */
    else                                          /*otherwise:              */
     return (6);                                  /* error message          */
   }

 SUSING1:                                         /*set base register       */

  T_BASR [NBASRG - 1].PRDOST = 'Y';               /* set active flag        */
  if ( *METKA1 == '*' )                           /* if 1st operand == '*', */
   {
    T_BASR[NBASRG-1].SMESH = CHADR;               /* select base value=CHADR*/
   }
  else                                            /*otherwise:              */
   {
    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA1     */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA1 ) )            /* and on match:          */
       {                                          /*  take value of this    */
	T_BASR[NBASRG-1].SMESH = T_SYM[J].ZNSYM;        /*  of this label as base */
       }
     }
    return (2);                                   /*prog. completion on err */
   }
  return (0);                                     /*successful subr. compl. */
 }
/*..........................................................................*/
int SRR()                                         /*subr.proc. op. RR-format*/
 {
  char *METKA;                                    /*set                     */
  char *METKA1;                                   /*working                 */
  char *METKA2;                                   /*variables               */
  unsigned char R1R2;                             /*                        */
  int J;                                          /*                        */
  RR.OP_RR.OP = T_MOP[I3].CODOP;                  /*form operation code     */

  METKA1 = strtok                                 /*in var. with ptr METKA1 */
	   (                                            /*select first lexeme     */
    (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*of current card operand */
	    ","                                         /*of ASSEMBLER source text*/
	   );

  METKA2 = strtok                                 /*in var. with ptr METKA2 */
	   (                                            /*select second lexeme    */
	    NULL,                                       /*of current card operand */
	    " "                                         /*of ASSEMBLER source text*/
	   );

  if ( isalpha ( (int) *METKA1 ) )                /*if lexeme starts        */
   {                                              /*with letter, then:      */
    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA1     */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA1 ) )            /* and on match:          */
       {                                          /*  take value of this    */
	 R1R2 = T_SYM[J].ZNSYM << 4;                    /*  label as 1st          */
	 goto SRR1;
       }                                          /*  operand of machine cmd*/
     }
    return(2);                                    /*message "undecl. ident" */
   }
  else                                            /*otherwise, take as      */
   {                                              /*1t operand of machin cmd*/
     R1R2 = atoi ( METKA1 ) << 4;                 /*value of selected lexeme*/
   }


 SRR1:


  if ( isalpha ( (int) *METKA2 ) )                /*if lexeme starts        */
   {                                              /*with letter, then:      */
    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA2     */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA2 ) )            /* and on match:          */
       {                                          /*  take value of this    */
	 R1R2 = R1R2 + T_SYM[J].ZNSYM;                  /*  label as 2nd          */
	 goto SRR2;                                     /*                        */
       }                                          /*  operand of machine cmd*/
     }                                            /*                        */
    return(2);                                    /*message "undecl. ident" */
   }
  else                                            /*otherwise, take as      */
   {                                              /*2d operand of machin cmd*/
     R1R2 = R1R2 + atoi ( METKA2 );               /*value of selected lexeme*/
   }

 SRR2:

  RR.OP_RR.R1R2 = R1R2;                           /*form machine command    */
						                                      /*operands                */

  STXT(2);
  return(0);                                      /*exit subroutine         */
 }

/*..........................................................................*/
int SRX()                                         /*subr.proc. op. RX-format*/
 {
  char *METKA;                                    /*set                     */
  char *METKA1;                                   /*working                 */
  char *METKA2;                                   /*variables               */
  char *PTR;                                      /*                        */
  int  DELTA;                                     /*                        */
  int  ZNSYM;                                     /*                        */
  int  NBASRG;                                    /*                        */
  int J;                                          /*                        */
  int I;                                          /*                        */
  unsigned char R1X2;                             /*                        */
  int B2D2;                                       /*                        */
  RX.OP_RX.OP = T_MOP[I3].CODOP;                  /*form operation code     */
  METKA1 = strtok                                 /*in var. with ptr METKA1 */
	   (                                            /*select first lexeme     */
    (char*) TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAND, /*of current card operand */
	    ","                                         /*of ASSEMBLER source text*/
	   );

  METKA2 = strtok                                 /*in var. with ptr METKA2 */
	   (                                            /*select second lexeme    */
	    NULL,                                       /*of current card operand */
	    " "                                         /*of ASSEMBLER source text*/
	   );

  if ( isalpha ( (int) *METKA1 ) )                /*if lexeme starts        */
   {                                              /*with letter, then:      */
    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA      */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA1 ) )            /* and on match:          */

       {                                          /*  take value of this    */
	 R1X2 = T_SYM[J].ZNSYM << 4;                    /*  label as 1st          */
	 goto SRX1;
       }                                          /*  operand of machine cmd*/
     }
    return(2);                                    /*message "undecl. ident" */
   }
  else                                            /*otherwise, take as      */
   {                                              /*1t operand of machin cmd*/
     R1X2 = atoi ( METKA1 ) << 4;                 /*value of selected lexeme*/
   }


 SRX1:


  if ( isalpha ( (int) *METKA2 ) )                /*if lexeme starts        */
   {                                              /*with letter, then:      */
    for ( J=0; J<=ITSYM; J++ )                    /* all source text labels */
     {                                            /* in table T_SYM compare */
						                                      /* with value *METKA      */
      METKA = strtok (
		      (char*) T_SYM[J].IMSYM , " "
		     );
      if( !strcmp ( METKA , METKA2 ) )            /* and on match:          */
       {                                          /*  set initial value:    */
	NBASRG = 0;                                     /*   base register number */
	DELTA  = 0xfff - 1;                             /*   and its value, also  */
	ZNSYM  = T_SYM[J].ZNSYM;                        /*   displacement of 2nd  */
	for ( I=0; I<15; I++ )                          /*further in loop from all*/
	 {                                              /*registers select as base*/
	  if (                                          /*the one that has:       */
	       T_BASR[I].PRDOST == 'Y'                  /* active flag,           */
	      &&                                        /*  and                   */
	       ZNSYM - T_BASR[I].SMESH >= 0             /* value, smaller in      */
	      &&                                        /* magnitude,but closest  */
	       ZNSYM - T_BASR[I].SMESH < DELTA          /* to displacement of 2nd */
	     )                                          /* operand                */
	   {
	    NBASRG = I + 1;
	    DELTA  = ZNSYM - T_BASR[I].SMESH;
	   }
	 }
	if ( NBASRG == 0 || DELTA > 0xfff )       /*if base reg not select, */
	 return(5);                               /* complete subr. on error*/
	else                                      /*otherwise               */
	 {                                        /*  form machine          */
	  B2D2 = NBASRG << 12;                    /*  representation of 2nd */
	  B2D2 = B2D2 + DELTA;                    /* operand as B2D2        */
	  PTR = (char *)&B2D2;                    /*  and in ES EVM conv.   */
	  swab ( PTR , PTR , 2 );                 /*  write to command body */
	  RX.OP_RX.B2D2 = B2D2;
	 }
	goto SRX2;                                      /*go to form 1st          */
       }                                          /*  operand of machine cmd*/
     }
    return(2);                                    /*message "undecl. ident" */
   }
  else                                            /*otherwise, take as      */
   {                                              /*2d operand of machin cmd*/
    return(4);                                    /*value of selected lexeme*/
   }

 SRX2:

  RX.OP_RX.R1X2 = R1X2;                           /*rewrite 1st operand     */

  STXT(4);                                        /*form TXT card           */
  return(0);                                      /*exit subroutine         */
 }
/*..........................................................................*/
int SOBJFILE()                                    /*subr.form.object        */
 {                                                /*file                    */
  FILE *fp;                                       /*set of working          */
  int RAB2;                                       /*variables               */
						  /*form path and name      */
  strcat ( NFIL , "tex" );                        /*of object file          */
  if ( (fp = fopen ( NFIL , "wb" )) == NULL )     /*on unsuccessful open    */
   return (-7);                                   /* error message          */
  else                                            /*otherwise:              */
   RAB2 =fwrite (OBJTEXT, 80 , ITCARD , fp);      /* form object file body  */
  fclose ( fp );                                  /*close object file       */
  return ( RAB2 );                                /*complete subroutine     */

 }
/*..........................................................................*/
void INITUNION ()
 {

/*
***** i n i t i a l i z a t i o n   of ESD-type record buffer fields
*****                             for output object file
*/

  ESD.STR_ESD.POLE1      =  0x02;
  memcpy ( ESD.STR_ESD.POLE2, "ESD", 3 );
  memset ( ESD.STR_ESD.POLE3,  0x40, 6 );
  ESD.STR_ESD.POLE31 [0] = 0x00;
  ESD.STR_ESD.POLE31 [1] = 0x10;
  memset ( ESD.STR_ESD.POLE32, 0x40, 2 );
  ESD.STR_ESD.POLE4  [0] = 0x00;
  ESD.STR_ESD.POLE4  [1] = 0x01;
  memset ( ESD.STR_ESD.IMPR,   0x40, 8 );
  ESD.STR_ESD.POLE6      = 0x00;
  memset ( ESD.STR_ESD.ADPRG,  0x00, 3 );
  ESD.STR_ESD.POLE8      = 0x40;
  memset ( ESD.STR_ESD.DLPRG,  0x00, 3 );
  memset ( ESD.STR_ESD.POLE10, 0x40,40 );
  memset ( ESD.STR_ESD.POLE11, 0x40, 8 );

/*
***** i n i t i a l i z a t i o n   of TXT-type record buffer fields
*****                             for output object file
*/

  TXT.STR_TXT.POLE1      =  0x02;
  memcpy ( TXT.STR_TXT.POLE2, "TXT", 3 );
  TXT.STR_TXT.POLE3      =  0x40;
  memset ( TXT.STR_TXT.ADOP,   0x00, 3 );
  memset ( TXT.STR_TXT.POLE5,  0x40, 2 );
  memset ( TXT.STR_TXT.DLNOP,  0X00, 2 );
  memset ( TXT.STR_TXT.POLE7,  0x40, 2 );
  TXT.STR_TXT.POLE71 [0] = 0x00;
  TXT.STR_TXT.POLE71 [1] = 0x01;
  memset ( TXT.STR_TXT.OPER,   0x40,56 );
  memset ( TXT.STR_TXT.POLE9,  0x40, 8 );

/*
***** i n i t i a l i z a t i o n   of END-type record buffer fields
*****                             for output object file
*/

  END.STR_END.POLE1      =  0x02;
  memcpy ( END.STR_END.POLE2, "END", 3 );
  memset ( END.STR_END.POLE3,  0x40,68 );
  memset ( END.STR_END.POLE9,  0x40, 8 );

 }

/*..........................................................................*/

int main( int argc, char **argv )                /*main program            */
 {
   FILE *fp;
   char *ptr = argv [1];
   unsigned char ASSTEXT [DL_ASSTEXT][80];

/*
******* B L O C K  of working variables declarations
*/

  int I1 , I2 , RAB;                              /* loop variables         */

  INITUNION ();                                   /* initial filling        */
                                                  /* of formation buffers   */
                                                  /* of output object       */
                                                  /* file records           */

/*
*******       B E G I N N I N G   O F   1 S T   P A S S      *****
*/
/*
***** B L O C K  of ASSTEXT array initialization, replacing init. in declaration
*****          (introduced as response to BORLANDC++ 2.0 requirements)
*/

  strcpy ( NFIL, ptr );

  if ( argc != 2 )

   {
    printf ( "%s\n", "Error in command line   " );
    return;
   }


  if ( strcmp ( &NFIL [ strlen ( NFIL )-3 ], "ass" ) )

   {
    printf ( "%s\n", "Invalid file type with source text" );
    return;
   }


  else

   {
    if ( (fp = fopen ( NFIL , "rb" )) == NULL )   /*on unsuccessful open    */
						                                      /* error message          */
     {
      printf ( "%s\n", "Source text file not found" );
      return;
     }

    else

     {
      for ( I1 = 0; I1 <= DL_ASSTEXT; I1++ )

       {
	if ( !fread ( ASSTEXT [I1], 80, 1, fp ) )
	 {
	  if ( feof ( fp ) )
	   goto main1;
	  else
	   {
	    printf ( "%s\n", "Error reading source text file" );
	    return;
	   }
	 }
       }

      printf ( "%s\n", "Source text read buffer overflow" );
      return;
     }

   }

main1:

   fclose ( fp );
   NFIL [ strlen ( NFIL )-3 ] = '\x0';

/*
***** E N D   of initialization block
*/

  for ( I1=0; I1 < DL_ASSTEXT; I1++ )             /*for cards from 1 to last*/
   {                                              /*                        */
    memcpy ( TEK_ISX_KARTA.BUFCARD , ASSTEXT[I1], /*read next card to buffer*/
					     80 );/*                        */
    if (TEK_ISX_KARTA.STRUCT_BUFCARD.METKA [0] == /*jump on absence         */
					     ' ') /*label                   */
     goto CONT1;                                  /*to CONT1,               */
						  /*otherwise:              */
    ITSYM += 1;                                   /* go to next TSYM row    */
    PRNMET = 'Y';                                 /* set label presence flag*/
    memcpy ( T_SYM[ITSYM].IMSYM ,                 /* store symbol name      */
	TEK_ISX_KARTA.STRUCT_BUFCARD.METKA , 8 ); /* and                    */
    T_SYM[ITSYM].ZNSYM = CHADR;                   /* its value(rel.addr.)   */

/*
***** B L O C K  of current operation search among pseudo-operations
*/

   CONT1:

    for ( I2=0; I2 < NPOP; I2++ )                 /*forall rows pseudoop tbl*/
     {                                            /*execute following:      */
      if(                                         /* if                     */
      !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* pseudo-operation       */
	      T_POP[I2].MNCPOP , 5)                     /* recognized,            */
	)                                               /* then:                  */
						                                      /*                        */
       { switch ( T_POP[I2].BXPROG () )           /* go to processing subr. */
	 {
	  case 0:
	   goto CONT2;                                  /* and complete cycle     */
	  case 1:
	   goto ERR1;
	  case 100:
	   goto CONT3;
	 }
       }                                          /*                        */
     }                                            /*                        */

    for ( I3=0; I3 < NOP ; I3++ )                 /*forall row machineop tbl*/
     {                                            /*execute following:      */
      if(                                         /* if                     */
      !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* machine operation      */
	      T_MOP[I3].MNCOP , 5)                      /* recognized,            */
	)                                               /* then:                  */
						                                      /*                        */
       {
	 T_MOP[I3].BXPROG ();                           /* go to processing subr. */
	 PRNMET = 'N';                                  /* remove lbl presence flg*/
	 goto CONT2;                                    /* and complete cycle     */
       }
     }

    goto ERR3;                                    /*msg mnemonic not rcgnizd*/


 CONT2:
    continue;                                     /*end of processing cycle */
   }                                              /*of source text cards    */


/*
*******       B E G I N N I N G   O F   2 N D   P A S S      *****
*/

CONT3:

 T_MOP[0].BXPROG = SRR;                           /*set pointers            */
 T_MOP[1].BXPROG = SRR;                           /*to processing subr.     */
 T_MOP[2].BXPROG = SRX;                           /*of ASSEMBLER commands at*/
 T_MOP[3].BXPROG = SRX;                           /*second pass             */
 T_MOP[4].BXPROG = SRX;
 T_MOP[5].BXPROG = SRX;

 T_POP[0].BXPROG = SDC;                           /*set pointers            */
 T_POP[1].BXPROG = SDS;                           /*to processing subr.     */
 T_POP[2].BXPROG = SEND;                          /*of ASSEMBLER pseudo-cmnd*/
 T_POP[3].BXPROG = SEQU;                          /*at second pass          */
 T_POP[4].BXPROG = SSTART;
 T_POP[5].BXPROG = SUSING;

  for ( I1=0; I1 < DL_ASSTEXT; I1++ )             /*for cards from 1 to last*/
   {     					                                /*                        */
    memcpy ( TEK_ISX_KARTA.BUFCARD , ASSTEXT [I1],/*read next card to buffer*/
					     80 );                              /*                        */
/*
***** B L O C K  of current operation search among pseudo-operations
*/

    for ( I2=0; I2 < NPOP; I2++ )                 /*foral rows pseudo-op tbl*/
     {                                            /*execute following:      */
      if(                                         /* if                     */
      !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* pseudo-operation       */
	      T_POP[I2].MNCPOP , 5)                     /* recognized,            */
	)                                               /* then                   */
						                                      /*                        */
       { switch ( T_POP[I2].BXPROG () )           /* go to processing subr. */
	 {
	  case 0:
	   goto CONT4;                                  /* and complete cycle     */
	  case 100:                                     /*go to formation         */
	   goto CONT5;                                  /*object file             */
	 }
       }                                          /*                        */
     }                                            /*                        */

    for ( I3=0; I3 < NOP ; I3++ )                 /*foral rows machineop tbl*/
     {                                            /*execute following:      */
      if(                                         /* if                     */
      !memcmp(TEK_ISX_KARTA.STRUCT_BUFCARD.OPERAC,/* machine operation      */
	      T_MOP[I3].MNCOP , 5)                      /* recognized             */
	)                                               /* then                   */
						                                      /*                        */
       {
	 switch ( T_MOP[I3].BXPROG () )                 /* go to processing subr. */
	  {
	   case 0:
	    goto CONT4;                           /* and complete cycle     */

	   case 2:                                /*output diagnostic       */
	    goto ERR2;                            /*message                 */

	   case 4:                                /*output diagnostic       */
	    goto ERR4;                            /*message                 */

	   case 5:                                /*output diagnostic       */
	    goto ERR5;                            /*message                 */

	   case 6:                                /*output diagnostic       */
	    goto ERR6;                            /*message                 */

	   case 7:                                /*output diagnostic       */
	    goto ERR6;                            /*message                 */

	  }
	}

     }

 CONT4:
   continue;                                      /*end of processing cycle */
   }                                              /*of source text cards    */

CONT5:

  if ( ITCARD == (RAB = SOBJFILE ()) )            /*if to object file       */
						                                      /*all cards output, then: */
   printf                                         /* message of successful  */
    (                                             /* completion             */
     "%s\n",
     "successful translation completion"
    );
  else                                            /*otherwise:              */
   {
    if ( RAB == -7 )
     goto ERR7;
    else
     printf                                       /* message of unsuccessful*/
      (                                           /* formation of object    */
       "%s\n",                                    /* file                   */
       "error forming object file"
      );
   }
    return;                                       /*complete main program   */


ERR1:
  printf ("%s\n","data format error");            /*output diagnostic messag*/
  goto CONT6;

ERR2:
  printf ("%s\n","undeclared identifier   ");     /*output diagnostic message*/
  goto CONT6;

ERR3:
  printf ("%s\n","operation code error");         /*output diagnostic message*/
  goto CONT6;

ERR4:
  printf ("%s\n","second operand error");         /*output diagnostic message*/
  goto CONT6;

ERR5:
  printf ("%s\n","base addressing error");        /*output diagnostic message*/
  goto CONT6;

ERR6:
  printf ("%s\n","invalid register number ");     /*output diagnostic message*/
  goto CONT6;

ERR7:
  printf ("%s\n","object file open error  ");     /*output diagnostic message*/
  goto CONT6;


CONT6:
  printf ("%s%d\n","error in card N ",I1+1);     /*output diagnostic message*/

 return 0;
 }                                               /*end of main program      */

