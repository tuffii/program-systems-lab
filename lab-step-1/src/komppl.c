/*
***** B E G I N N I N G   of high-level language compiler file
*/

/*
***** B l o c k  of macro definitions declarations
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
						  /* l i m i t i n g       */
						  /* sizes:                */
#define MAXNISXTXT 50                             /* - source text;         */
#define NSINT     201                             /* - syntax rules table;  */
#define NCEL       20                             /* - goals stack;         */
#define NDST      500                             /* - achievements stack;  */
#define NVXOD      53                             /* - entries table;       */
#define NSTROKA   200                             /* - compressed text line;*/
#define NNETRM     16                             /* - non-terminals list;  */
#define MAXLTXT    50                             /* - output text;         */
#define MAXFORMT   30                             /* - formatted inter-     */
                                                  /* preted fragment        */
                                                  /* of source text;        */
#define NSYM      100                             /* - names and labels tabl*/

/*
***** C o m p i l e r   database
*/

/*
***** B l o c k  of source text array declaration
*/

int  NISXTXT;                                     /* array length           */
char ISXTXT [MAXNISXTXT][80];                     /* array body             */

/*
***** B l o c k  of working variables declaration
*/

int I1,I2,I3,I4;                                  /* loop counters          */

char PREDSYM = ' ';                               /*last processed          */
						  /*character in compressed */
						  /*source text             */

char STROKA [ NSTROKA ];                          /*storage lction for comp.*/
						  /*ressed source text      */

int I,J,K,L;                                      /*current indices corresp.*/
						  /*pondingly in:           */
						  /* - compressed text;     */
						  /* - grammar rules table; */
						  /* - goals stack          */
						  /* achieved;              */
						  /* - achievements stack   */
						  /* achieved;              */

union                                             /*template for generation */
 {                                                /*of output file record   */
  char    BUFCARD [80];                           /*in IBM 370 ASSEMBLER    */
  struct
   {
    char METKA   [8];
    char PROB1;
    char OPERAC  [5];
    char PROB2;
    char OPERAND [12];
    char PROB3;
    char COMM    [52];
   } _BUFCARD;
 } ASS_CARD ;

char ASSTXT [ MAXLTXT ][80];                      /*array for storage       */
						  /*of output text in       */
						  /*IBM 370 ASSEMBLER       */

int  IASSTXT;                                     /*output array index      */

char FORMT [MAXFORMT] [9];                        /*array for formatted     */
                                                  /*representation (as seq. */
                                                  /*of 9-position strings-  */
                                                  /*lexemes) of interpreted */
                                                  /*source text fragment    */
                                                  /*                        */
                                                  /*                        */

int  IFORMT;                                      /*formatted array index   */
						                                      /*array                   */
/*
***** B l o c k  of database tables declaration
*/

/*
***** T a b l e used as achievements stack (LIFO)
*/

struct
 {
  char DST1 [ 4 ];
  int  DST2;
  int  DST3;
  int  DST4;
  int  DST5;
 } DST [ NDST ];

/*
***** T a b l e used as goals stack (LIFO)
*/

struct
 {
  char CEL1 [ 4 ];
  int  CEL2;
  int  CEL3;
 } CEL [ NCEL ];

/*
***** T a b l e of syntax rules written in recognition form,
***** grouped in "bushes" and presented as two-way
***** list with alternative branches
*/

struct
 {
  int  POSL;
  int  PRED;
  char DER[4];
  int  ALT;
 } SINT [ NSINT ] =
/*   __________ _________ _______ _______ ______
    |  NN      :    next : pred  :  der  : alt  |
    |__________:_________:_______:_______:______|                          */
{
 {/*.    0     .*/    -1 ,    -1 , "***" ,   -1 },
  /*                                               input from symbol - 0     */
 {/*.    1     .*/     2 ,     0 , "0  " ,    0 },
 {/*.    2     .*/     3 ,     1 , "CIF" ,    0 },
 {/*.    3     .*/     0 ,     2 , "*  " ,    0 },
  /*                                               input from symbol - 1     */
 {/*.    4     .*/     5 ,     0 , "1  " ,    0 },
 {/*.    5     .*/     6 ,     4 , "CIF" ,    7 },
 {/*.    6     .*/     0 ,     5 , "*  " ,    0 },

 {/*.    7     .*/     8 ,     4 , "MAN" ,    0 },
 {/*.    8     .*/     0 ,     7 , "*  " ,    0 },
  /*                                               input from symbol - 2     */
 {/*.    9     .*/    10 ,     0 , "2  " ,    0 },
 {/*.   10     .*/    11 ,     9 , "CIF" ,    0 },
 {/*.   11     .*/     0 ,    10 , "*  " ,    0 },
  /*                                               input from symbol - 3     */
 {/*.   12     .*/    13 ,     0 , "3  " ,    0 },
 {/*.   13     .*/    14 ,    12 , "CIF" ,    0 },
 {/*.   14     .*/     0 ,    13 , "*  " ,    0 },
  /*                                               input from symbol - 4     */
 {/*.   15     .*/    16 ,     0 , "4  " ,    0 },
 {/*.   16     .*/    17 ,    15 , "CIF" ,    0 },
 {/*.   17     .*/     0 ,    16 , "*  " ,    0 },
  /*                                               input from symbol - 5     */
 {/*.   18     .*/    19 ,     0 , "5  " ,    0 },
 {/*.   19     .*/    20 ,    18 , "CIF" ,    0 },
 {/*.   20     .*/     0 ,    19 , "*  " ,    0 },
  /*                                               input from symbol - 6     */
 {/*.   21     .*/    22 ,     0 , "6  " ,    0 },
 {/*.   22     .*/    23 ,    21 , "CIF" ,    0 },
 {/*.   23     .*/     0 ,    22 , "*  " ,    0 },
  /*                                               input from symbol - 7     */
 {/*.   24     .*/    25 ,     0 , "7  " ,    0 },
 {/*.   25     .*/    26 ,    24 , "CIF" ,    0 },
 {/*.   26     .*/     0 ,    25 , "*  " ,    0 },
  /*                                               input from symbol - 8     */
 {/*.   27     .*/    28 ,     0 , "8  " ,    0 },
 {/*.   28     .*/    29 ,    27 , "CIF" ,    0 },
 {/*.   29     .*/     0 ,    28 , "*  " ,    0 },
  /*                                               input from symbol - 9     */
 {/*.   30     .*/    31 ,     0 , "9  " ,    0 },
 {/*.   31     .*/    32 ,    30 , "CIF" ,    0 },
 {/*.   32     .*/     0 ,    31 , "*  " ,    0 },
  /*                                               input from symbol - A     */
 {/*.   33     .*/    34 ,     0 , "A  " ,    0 },
 {/*.   34     .*/    35 ,    33 , "BUK" ,    0 },
 {/*.   35     .*/     0 ,    34 , "*  " ,    0 },
  /*                                               input from symbol - B     */
 {/*.   36     .*/    37 ,     0 , "B  " ,    0 },
 {/*.   37     .*/    38 ,    36 , "BUK" ,    0 },
 {/*.   38     .*/     0 ,    37 , "*  " ,    0 },
  /*                                               input from symbol - C     */
 {/*.   39     .*/    40 ,     0 , "C  " ,    0 },
 {/*.   40     .*/    41 ,    39 , "BUK" ,    0 },
 {/*.   41     .*/     0 ,    40 , "*  " ,    0 },
  /*                                               input from symbol - D     */
 {/*.   42     .*/    43 ,     0 , "D  " ,    0 },
 {/*.   43     .*/    44 ,    42 , "BUK" ,   45 },
 {/*.   44     .*/     0 ,    43 , "*  " ,    0 },

 {/*.   45     .*/    46 ,    42 , "C  " ,    0 },
 {/*.   46     .*/    47 ,    45 , "L  " ,    0 },
 {/*.   47     .*/    48 ,    46 , "   " ,    0 },
 {/*.   48     .*/    49 ,    47 , "IDE" ,    0 },
 {/*.   49     .*/    50 ,    48 , "   " ,    0 },
 {/*.   50     .*/    51 ,    49 , "B  " ,  187 },
 {/*.   51     .*/    52 ,    50 , "I  " ,    0 },
 {/*.   52     .*/    53 ,    51 , "N  " ,    0 },
 {/*.   53     .*/    54 ,    52 , "   " ,    0 },
 {/*.   54     .*/    55 ,    53 , "F  " ,    0 },
 {/*.   55     .*/    56 ,    54 , "I  " ,    0 },
 {/*.   56     .*/    57 ,    55 , "X  " ,    0 },
 {/*.   57     .*/    58 ,    56 , "E  " ,    0 },
 {/*.   58     .*/    59 ,    57 , "D  " ,    0 },
 {/*.   59     .*/    60 ,    58 , "(  " ,    0 },
 {/*.   60     .*/    61 ,    59 , "RZR" ,    0 },
 {/*.   61     .*/    62 ,    60 , ")  " ,    0 },
 {/*.   62     .*/    63 ,    61 , ";  " ,   65 },
 {/*.   63     .*/    64 ,    62 , "ODC" ,    0 },
 {/*.   64     .*/    65 ,    63 , "*  " ,    0 },

 {/*.   65     .*/    66 ,    61 , "I  " ,    0 },
 {/*.   66     .*/    67 ,    65 , "N  " ,    0 },
 {/*.   67     .*/    68 ,    66 , "I  " ,    0 },
 {/*.   68     .*/    69 ,    67 , "T  " ,    0 },
 {/*.   69     .*/    70 ,    68 , "(  " ,    0 },
 {/*.   70     .*/    71 ,    69 , "LIT" ,    0 },
 {/*.   71     .*/    72 ,    70 , ")  " ,    0 },
 {/*.   72     .*/    73 ,    71 , ";  " ,    0 },
 {/*.   73     .*/   186 ,    72 , "ODC" ,    0 },
  /*                                               input from symbol - E     */
 {/*.   74     .*/    75 ,     0 , "E  " ,    0 },
 {/*.   75     .*/    76 ,    74 , "N  " ,   82 },
 {/*.   76     .*/    77 ,    75 , "D  " ,    0 },
 {/*.   77     .*/    78 ,    76 , "   " ,    0 },
 {/*.   78     .*/    79 ,    77 , "IPR" ,    0 },
 {/*.   79     .*/    80 ,    78 , ";  " ,    0 },
 {/*.   80     .*/    81 ,    79 , "OEN" ,    0 },
 {/*.   81     .*/     0 ,    80 , "*  " ,    0 },

 {/*.   82     .*/    83 ,    74 , "BUK" ,    0 },
 {/*.   83     .*/     0 ,    82 , "*  " ,    0 },
  /*                                               input from symbol - M     */
 {/*.   84     .*/    85 ,     0 , "M  " ,    0 },
 {/*.   85     .*/    86 ,    84 , "BUK" ,    0 },
 {/*.   86     .*/     0 ,    85 , "*  " ,    0 },
  /*                                               input from symbol - P     */
 {/*.   87     .*/    88 ,     0 , "P  " ,    0 },
 {/*.   88     .*/    89 ,    87 , "BUK" ,    0 },
 {/*.   89     .*/     0 ,    88 , "*  " ,    0 },
  /*                                               input from symbol - X     */
 {/*.   90     .*/    91 ,     0 , "X  " ,    0 },
 {/*.   91     .*/    92 ,    90 , "BUK" ,    0 },
 {/*.   92     .*/     0 ,    91 , "*  " ,    0 },
  /*                                               input from symbol - BUK   */
 {/*.   93     .*/    94 ,     0 , "BUK" ,    0 },
 {/*.   94     .*/    95 ,    93 , "IDE" ,    0 },
 {/*    95     .*/     0 ,    94 , "*  " ,    0 },
  /*                                               input from symbol - IDE   */
 {/*.   96     .*/    97 ,     0 , "IDE" ,    0 },
 {/*.   97     .*/    98 ,    96 , "BUK" ,  100 },
 {/*.   98     .*/    99 ,    97 , "IDE" ,    0 },
 {/*.   99     .*/     0 ,    98 , "*  " ,    0 },

 {/*.  100     .*/   101 ,    96 , "CIF" ,  103 },
 {/*.  101     .*/   102 ,   100 , "IDE" ,    0 },
 {/*.  102     .*/     0 ,   101 , "*  " ,    0 },

 {/*.  103     .*/   104 ,    96 , "IPE" ,  105 },
 {/*.  104     .*/     0 ,   103 , "*  " ,    0 },

 {/*.  105     .*/   106 ,    96 , "IPR" ,    0 },
 {/*.  106     .*/     0 ,   105 , "*  " ,    0 },
  /*                                               input from symbol - +     */
 {/*.  107     .*/   108 ,     0 , "+  " ,    0 },
 {/*.  108     .*/   109 ,   107 , "ZNK" ,    0 },
 {/*.  109     .*/     0 ,   108 , "*  " ,    0 },
  /*                                               input from symbol - -     */
 {/*.  110     .*/   111 ,     0 , "-  " ,    0 },
 {/*.  111     .*/   112 ,   110 , "ZNK" ,    0 },
 {/*.  112     .*/     0 ,   111 , "*  " ,    0 },
  /*                                               input from symbol - IPR   */
 {/*.  113     .*/   114 ,     0 , "IPR" ,    0 },
 {/*.  114     .*/   115 ,   113 , ":  " ,    0 },
 {/*.  115     .*/   116 ,   114 , "P  " ,    0 },
 {/*.  116     .*/   117 ,   115 , "R  " ,    0 },
 {/*.  117     .*/   118 ,   116 , "O  " ,    0 },
 {/*.  118     .*/   119 ,   117 , "C  " ,    0 },
 {/*.  119     .*/   120 ,   118 , "   " ,    0 },
 {/*.  120     .*/   121 ,   119 , "O  " ,    0 },
 {/*.  121     .*/   122 ,   120 , "P  " ,    0 },
 {/*.  122     .*/   123 ,   121 , "T  " ,    0 },
 {/*.  123     .*/   124 ,   122 , "I  " ,    0 },
 {/*.  124     .*/   125 ,   123 , "O  " ,    0 },
 {/*.  125     .*/   126 ,   124 , "N  " ,    0 },
 {/*.  126     .*/   127 ,   125 , "S  " ,    0 },
 {/*.  127     .*/   128 ,   126 , "(  " ,    0 },
 {/*.  128     .*/   129 ,   127 , "M  " ,    0 },
 {/*.  129     .*/   130 ,   128 , "A  " ,    0 },
 {/*.  130     .*/   131 ,   129 , "I  " ,    0 },
 {/*.  131     .*/   132 ,   130 , "N  " ,    0 },
 {/*.  132     .*/   133 ,   131 , ")  " ,    0 },
 {/*.  133     .*/   134 ,   132 , ";  " ,    0 },
 {/*.  134     .*/   135 ,   133 , "OPR" ,    0 },
 {/*.  135     .*/     0 ,   134 , "*  " ,    0 },
 /*                                                input from symbol - CIF   */
 {/*.  136     .*/   137 ,     0 , "CIF" ,    0 },
 {/*.  137     .*/   138 ,   136 , "RZR" ,    0 },
 {/*.  138     .*/     0 ,     0 , "*  " ,    0 },
  /*                                               input from symbol - RZR   */
 {/*.  139     .*/   140 ,     0 , "RZR" ,    0 },
 {/*.  140     .*/   141 ,   139 , "CIF" ,    0 },
 {/*.  141     .*/   142 ,   140 , "RZR" ,    0 },
 {/*.  142     .*/     0 ,   141 , "*  " ,    0 },
  /*                                               input from symbol - MAN   */
 {/*.  143     .*/   144 ,     0 , "MAN" ,    0 },
 {/*.  144     .*/   145 ,   143 , "B  " ,  147 },
 {/*.  145     .*/   146 ,   144 , "LIT" ,    0 },
 {/*.  146     .*/     0 ,   145 , "*  " ,    0 },

 {/*.  147     .*/   148 ,   143 , "0  " ,  150 },
 {/*.  148     .*/   149 ,   147 , "MAN" ,    0 },
 {/*.  149     .*/     0 ,   148 , "*  " ,    0 },

 {/*.  150     .*/   151 ,   143 , "1  " ,    0 },
 {/*.  151     .*/   152 ,   150 , "MAN" ,    0 },
 {/*.  152     .*/     0 ,   151 , "*  " ,    0 },
  /*                                               input from symbol - IPE   */
 {/*.  153     .*/   154 ,     0 , "IPE" ,    0 },
 {/*.  154     .*/   155 ,   153 , "=  " ,  159 },
 {/*.  155     .*/   156 ,   154 , "AVI" ,    0 },
 {/*.  156     .*/   157 ,   155 , ";  " ,    0 },
 {/*.  157     .*/   158 ,   156 , "OPA" ,    0 },
 {/*.  158     .*/     0 ,   157 , "*  " ,    0 },

 {/*.  159     .*/   160 ,   153 , "AVI" ,    0 },
 {/*.  160     .*/     0 ,   159 , "*  " ,    0 },
  /*                                               input from symbol - LIT   */
 {/*.  161     .*/   162 ,     0 , "LIT" ,    0 },
 {/*.  162     .*/   163 ,   161 , "AVI" ,    0 },
 {/*.  163     .*/     0 ,   162 , "*  " ,    0 },
  /*.                                              input from symbol - AVI   */
 {/*.  164     .*/   165 ,     0 , "AVI" ,    0 },
 {/*.  165     .*/   166 ,   164 , "ZNK" ,    0 },
 {/*.  166     .*/   167 ,   165 , "LIT" ,  168 },
 {/*.  167     .*/   197 ,   166 , "AVI" ,    0 },

 {/*.  168     .*/   169 ,   165 , "IPE" ,    0 },
 {/*.  169     .*/   170 ,   168 , "AVI" ,    0 },
 {/*.  170     .*/     0 ,   169 , "*  " ,    0 },
  /*                                               input from symbol - OPR   */
 {/*.  171     .*/   172 ,     0 , "OPR" ,    0 },
 {/*.  172     .*/   173 ,   171 , "TEL" ,    0 },
 {/*.  173     .*/   174 ,   172 , "OEN" ,    0 },
 {/*.  174     .*/   175 ,   173 , "PRO" ,    0 },
 {/*.  175     .*/     0 ,   174 , "*  " ,    0 },
  /*.                                              input from symbol - ODC   */
 {/*.  176     .*/   177 ,     0 , "ODC" ,    0 },
 {/*.  177     .*/   178 ,   176 , "TEL" ,    0 },
 {/*.  178     .*/     0 ,   177 , "*  " ,    0 },
  /*.                                              input from symbol - TEL   */
 {/*.  179     .*/   180 ,     0 , "TEL" ,    0 },
 {/*.  180     .*/   181 ,   179 , "ODC" ,  183 },
 {/*.  181     .*/   182 ,   180 , "TEL" ,    0 },
 {/*.  182     .*/     0 ,   181 , "*  " ,    0 },

 {/*.  183     .*/   184 ,   179 , "OPA" ,    0 },
 {/*.  184     .*/   185 ,   183 , "TEL" ,    0 },
 {/*.  185     .*/     0 ,   184 , "*  " ,    0 },

 {/*.  186     .*/     0 ,    73 , "*  " ,    0 },


 {/*.  187     .*/   188 ,    49 , "C  " ,    0 },
 {/*.  188     .*/   189 ,   187 , "H  " ,    0 },
 {/*.  189     .*/   190 ,   188 , "A  " ,    0 },
 {/*.  190     .*/   191 ,   189 , "R  " ,    0 },
 {/*.  191     .*/   192 ,   190 , "(  " ,    0 },
 {/*.  192     .*/   193 ,   191 , "RZR" ,    0 },
 {/*.  193     .*/   194 ,   192 , ")  " ,    0 },
 {/*.  194     .*/   195 ,   193 , ";  " ,    0 },
 {/*.  195     .*/   196 ,   194 , "ODC" ,    0 },
 {/*.  196     .*/     0 ,   195 , "*  " ,    0 },

 {/*.  197     .*/     0 ,   166 , "*  " ,    0 },

 {/*.  198     .*/   199 ,     0 , "*  " ,    0 },
 {/*.  199     .*/   200 ,   198 , "ZNK" ,    0 },
 {/*.  200     .*/     0 ,   199 , "*  " ,    0 }
};

/*
***** T a b l e of entries to "bushes" (roots) of grammar rules,
***** containing type (terminality or non-terminality) of root
***** symbols
*/

struct
 {
  char SYM [4];
  int  VX;
  char TYP;
 } VXOD [ NVXOD ] =
/*   __________ ___________ _____ ______
    |  NN      |   symbol|entry| type |
    |__________|___________|_____|______|                                  */

{
  {/*.   1     .*/   "AVI" , 164 , 'N' },
  {/*.   2     .*/   "BUK" ,  93 , 'N' },
  {/*.   3     .*/   "CIF" , 136 , 'N' },
  {/*.   4     .*/   "IDE" ,  96 , 'N' },
  {/*.   5     .*/   "IPE" , 153 , 'N' },
  {/*.   6     .*/   "IPR" , 113 , 'N' },
  {/*.   7     .*/   "LIT" , 161 , 'N' },
  {/*.   8     .*/   "MAN" , 143 , 'N' },
  {/*.   9     .*/   "ODC" , 176 , 'N' },
  {/*.  10     .*/   "OEN" ,   0 , 'N' },
  {/*.  11     .*/   "OPA" ,   0 , 'N' },
  {/*.  12     .*/   "OPR" , 171 , 'N' },
  {/*.  13     .*/   "PRO" ,   0 , 'N' },
  {/*.  14     .*/   "RZR" , 139 , 'N' },
  {/*.  15     .*/   "TEL" , 179 , 'N' },
  {/*.  16     .*/   "ZNK" ,   0 , 'N' },
  {/*.  17     .*/   "A  " ,  33 , 'T' },
  {/*.  18     .*/   "B  " ,  36 , 'T' },
  {/*.  19     .*/   "C  " ,  39 , 'T' },
  {/*.  20     .*/   "D  " ,  42 , 'T' },
  {/*.  21     .*/   "E  " ,  74 , 'T' },
  {/*.  22     .*/   "M  " ,  84 , 'T' },
  {/*.  23     .*/   "P  " ,  87 , 'T' },
  {/*.  24     .*/   "X  " ,  90 , 'T' },
  {/*.  25     .*/   "0  " ,   1 , 'T' },
  {/*.  26     .*/   "1  " ,   4 , 'T' },
  {/*.  27     .*/   "2  " ,   9 , 'T' },
  {/*.  28     .*/   "3  " ,  12 , 'T' },
  {/*.  29     .*/   "4  " ,  15 , 'T' },
  {/*.  30     .*/   "5  " ,  18 , 'T' },
  {/*.  31     .*/   "6  " ,  21 , 'T' },
  {/*.  32     .*/   "7  " ,  24 , 'T' },
  {/*.  33     .*/   "8  " ,  27 , 'T' },
  {/*.  34     .*/   "9  " ,  30 , 'T' },
  {/*.  35     .*/   "+  " , 107 , 'T' },
  {/*.  36     .*/   "-  " , 110 , 'T' },
  {/*.  37     .*/   ":  " ,   0 , 'T' },
  {/*.  38     .*/   "I  " ,   0 , 'T' },
  {/*.  39     .*/   "R  " ,   0 , 'T' },
  {/*.  40     .*/   "N  " ,   0 , 'T' },
  {/*.  41     .*/   "O  " ,   0 , 'T' },
  {/*.  42     .*/   "T  " ,   0 , 'T' },
  {/*.  43     .*/   "S  " ,   0 , 'T' },
  {/*.  44     .*/   "(  " ,   0 , 'T' },
  {/*.  45     .*/   ")  " ,   0 , 'T' },
  {/*.  46     .*/   "   " ,   0 , 'T' },
  {/*.  47     .*/   ";  " ,   0 , 'T' },
  {/*.  48     .*/   "L  " ,   0 , 'T' },
  {/*.  49     .*/   "F  " ,   0 , 'T' },
  {/*.  50     .*/   "=  " ,   0 , 'T' },
  {/*.  51     .*/   "H  " ,   0 , 'T' },
  {/*.  52     .*/   "*  " , 198 , 'T' }
};

/*
***** T a b l e of adjacency matrix - basis for successor matrix
***** construction
*/

char TPR [ NVXOD ] [ NNETRM ] =
 {
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*AVI*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*BUK*/ 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*CIF*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  },
  {/*IDE*/ 0 , 0 , 0 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*IPE*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0  },
  {/*IPR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0  },
  {/*LIT*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*MAN*/ 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*ODC*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0  },
  {/*OEN*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*OPA*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*OPR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0  },
  {/*PRO*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*RZR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  },
  {/*TEL*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0  },
  {/*ZNK*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  A*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  B*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  C*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  D*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  E*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  M*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  P*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  X*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  0*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  1*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  2*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  3*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  4*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  5*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  },
  {/*  6*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0  },
  {/*  7*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE;IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  8*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  9*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  +*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1  },
  {/*  -*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1  },
  {/*  :*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  I*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  R*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  N*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  O*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  T*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  S*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  (*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  )*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  ;*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*   */ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  L*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN;OPA:OPR:PR0:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  F*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  =*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  H*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  **/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1  }
/*|_______________________________________________________________________| */
  };
/*..........................................................................*/

/*
******* B E G I N N I N G  of source text processing
*/

/*..........................................................................*/

void compress_ISXTXT()                            /* Compression program    */
						  /* of source text by      */
						  /* removing "extra"       */
						  /* spaces, performing     */
						  /* role of primitive lex- */
						  /* ical analyzer          */
 {
  I3 = 0;
  for ( I1 = 0 ; I1 < NISXTXT ; I1++ )
   {
    for ( I2 = 0 ; I2 < 80 ; I2++ )
     if ( ISXTXT [ I1 ][ I2 ] != '\x0' )
      {
       if ( ISXTXT [ I1 ][ I2 ] == ' ' &&
	   ( PREDSYM == ' ' || PREDSYM == ';' ||
	     PREDSYM == ')' || PREDSYM == ':' ||
	     PREDSYM == '('
	   )
	  )
	{
	 PREDSYM = ISXTXT [ I1 ][ I2 ];
	 goto L2;
	}

       if
	 (
	  ( ISXTXT [ I1 ][ I2 ] == '+' ||
	    ISXTXT [ I1 ][ I2 ] == '-' ||
	    ISXTXT [ I1 ][ I2 ] == '=' ||
	    ISXTXT [ I1 ][ I2 ] == '(' ||
	    ISXTXT [ I1 ][ I2 ] == ')' ||
	    ISXTXT [ I1 ][ I2 ] == '*'
	  )
	   &&
	  PREDSYM == ' '
	 )
	{
	 I3-- ;
	 goto L1;
	}


       if ( ISXTXT [ I1 ][ I2 ] == ' ' &&
	   ( PREDSYM == '+' || PREDSYM == '-' ||
	     PREDSYM == '=' || PREDSYM == '*'
	   )
	  )
	{
	 goto L2;
	}

L1:
       PREDSYM = ISXTXT [ I1 ][ I2 ];
       STROKA [ I3 ] = PREDSYM;
       I3++ ;

L2:    continue;
      }
     else
      break;
   }
  STROKA [I3] = '\x0';
 }

/*..........................................................................*/

void build_TPR ()                                 /* Successor matrix table   */
						  /* from adjacency matrix  */
						  /* using Warshall alg.  */
						  /*                      */
 {
  for ( I1 = 0; I1 < NNETRM; I1++ )
   {
    for ( I2 = 0; I2 < NVXOD; I2++ )
     {
      if ( TPR [ I2 ][ I1 ] & ( I1 != I2 ) )
       {
	for ( I3 = 0; I3 < NNETRM; I3++ )
	 TPR [ I2 ][ I3 ] |= TPR [ I1 ][ I3 ];
       }
     }
   }
 }

/*..........................................................................*/

void mcel ( char* T1, int T2, int T3 )            /* program for filling    */
 {                                                /* cell of goals stack    */
						  /* placed                 */
  strcpy ( CEL [ K ].CEL1, T1 );
  CEL [ K ].CEL2 = T2;
  CEL [ K ].CEL3 = T3;
  K++;
 }

/*..........................................................................*/

void mdst ( char* T1, int T2, int T3, int T4, int T5 )
 {                                                /* program for filling    */
  strcpy ( DST [ L ].DST1, T1 );                  /* cell of achievements   */
  DST [ L ].DST2 = T2;                            /* stack                  */
  DST [ L ].DST3 = T3;
  DST [ L ].DST4 = T4;
  DST [ L ].DST5 = T5;
  L++;
 }


/*..........................................................................*/
						  /* p r o g r a m          */
int numb ( char* T1, int T2 )                     /* calculation of ordinal */
						  /* row number in table    */
						  /* VXOD corresponding to  */
						  /* function param. line   */
 {
  int k;

  for ( I1 = 0; I1 < NVXOD; I1++ )
   {
    for ( k = 0; k < T2; k++ )
     {
      if ( (*(T1+k) != VXOD [ I1 ].SYM [k] ) )
       goto numb1;
     }
  if ( (VXOD [ I1 ].SYM [k] == '\x0') ||
       (VXOD [ I1 ].SYM [k] == ' '  )
     )
    return ( I1 );
numb1:
    continue;
   }
  return -1;
 }

/*..........................................................................*/
						  /*   p r o g r a m        */
int sint_ANAL ()                                  /*   syntax tree constr.  */
						                                      /*parsing, performing role*/
 {                                                /*of syntax analyzer      */
						                                      /*                        */
  I4 = 0;

L1:

  K = 0;
  L = 0;
  I = 0;
  J = 1;
  mcel ( "PRO" , I , 999 );

  if (!TPR [numb ( &STROKA [I], 1 )][numb ( "PRO", 3 )])
   return 1;

L2:

  J = VXOD [ numb ( &STROKA [ I ], 1 ) ].VX ;

L3:

  J = SINT [ J ].POSL;

L31:

  I++;

  if ( I > I4 )


   I4 = I;

  if (VXOD [ numb ( SINT [ J ].DER, 3 ) ].TYP == 'T')
   {

    if ( STROKA [ I ] == SINT [ J ].DER [ 0 ] )
     goto L3;
    else
     goto L8;

   }

L4:

  if ( SINT [ SINT [ J ].POSL ].DER [ 0 ] == '*' )
   {
    I--;

    if ( !strcmp (SINT [J].DER, CEL [K-1].CEL1 ) )
     {
      mdst ( CEL[K-1].CEL1,CEL[K-1].CEL2,CEL[K-1].CEL3,I,J );

      if ( !strcmp( CEL[K-1].CEL1 , "PRO" ) )
       return 0;

L5:

      if (TPR [numb (CEL[K-1].CEL1, 3)] [numb (CEL[K-1].CEL1, 3)])
       {
	J = VXOD [ numb ( CEL[K-1].CEL1, 3 ) ].VX;
	goto L3;
       }

L6:

      J = CEL[K-1].CEL3;
      K--;
      goto L3;
     }

    if (!TPR [numb (SINT[J].DER, 3)] [numb (CEL[K-1].CEL1, 3)])
     goto L9;

    mdst ( SINT[J].DER, CEL[K-1].CEL2,0,I,J );
    J = VXOD [numb (SINT[J].DER, 3)].VX;
    goto L3;
   }

  if (!TPR [numb (&STROKA [I], 1)] [numb (SINT[J].DER, 3)])
   goto L8;

  mcel ( SINT[J].DER,I,J );
  goto L2;

L8:

  I--;

L9:

  if (SINT[J].ALT != 0)
   {
    J = SINT[J].ALT;
    goto L31;
   }

  J = SINT[J].PRED;

  if
   (
    ( VXOD [numb (SINT[J].DER, 3)].TYP == 'N' )
    &&
    ( SINT[J].PRED > 0 )
   )
   {
    mcel (DST[L-1].DST1, DST[L-1].DST2, DST[L-1].DST3);

L10:

    J = DST[L-1].DST5;
    L--;
    goto L9;
   }

  if
   (
    ( VXOD [numb (SINT[J].DER, 3)].TYP == 'N' )
    &&
    ( SINT[J].PRED == 0 )
   )
   {
    if (!strcmp ( CEL[K-1].CEL1, DST[L-1].DST1 ) )
     goto L6;
    else
     goto L10;
   }

  if ( SINT[J].PRED > 0 )
   goto L8;

  J = CEL[K-1].CEL3;
  K--;

  if ( J == 999 )
   return 2;
  else
   goto L8;

 }

/*..........................................................................*/

struct                                            /* table of labels and    */
 {                                                /* variables, filled at   */
  char NAME [8];                                  /* 1st pass semantic calc.*/
  char TYPE;                                      /* and used at 2nd pass   */
  char RAZR [5];                                  /* semantic calculation   */
  char INIT [50];                                 /*                        */
 } SYM [ NSYM ];                                  /*                        */

int ISYM = 0;                                     /* current index of names */
						                                      /* table                  */

char NFIL [30]="\x0";                             /* storage of translated  */
						                                      /* program name           */

/*..........................................................................*/

long int VALUE ( char* s )                        /* p r o g r a m          */
 {                                                /* for binary constant    */
  long int S;                                     /* translation from ASCII */
  int i;                                          /* to internal long int   */
						                                      /* representation         */
  i = 0;
  S = 0;
  while ( *(s + i) != 'B' )

   {

    S <<= 1;
    if ( *(s + i) == '1' )
     S++;
    i++;
   }

  return (S);
 }



/*..........................................................................*/
void FORM ()                                      /* p r o g r a m          */
 {                                                /* for fragment represen- */
                                                  /*tation as array of 9-   */
                                                  /*char. lexemes of comp.  */
                                                  /*text                    */
  int i,j;

  for ( IFORMT = 0; IFORMT < MAXFORMT; IFORMT++ )
   memcpy ( FORMT [IFORMT], "\x0\x0\x0\x0\x0\x0\x0\x0\x0", 9 );

  IFORMT = 0;
  j = DST [I2].DST2;

FORM1:

  for ( i = j; i <= DST [I2].DST4+1; i++ )
   {
     if ( STROKA [i] == ':' || STROKA [i] == ' ' ||
	  STROKA [i] == '(' || STROKA [i] == ')' ||
	  STROKA [i] == ';' || STROKA [i] == '+' ||
	  STROKA [i] == '-' || STROKA [i] == '=' ||
	  STROKA [i] == '*'
	)
	{
	 FORMT [IFORMT] [i-j] = '\x0';
	 IFORMT ++;
	 j = i+1;
	 goto FORM1;
	}
     else
	 FORMT [IFORMT][i-j] = STROKA [i];

   }

  return;
 }

/*..........................................................................*/
						  /* p r o g r a m          */
void ZKARD ()                                     /* for writing next gen.  */
 {                                                /* record to output file  */
                                                  /*array                   */
                                                  /* ASSTXT                 */
  char i;
  memcpy ( ASSTXT [ IASSTXT++ ],
			   ASS_CARD.BUFCARD, 80 );

  for ( i = 0; i < 79; i++ )
   ASS_CARD.BUFCARD [i] = ' ';
  return;
 }

/*..........................................................................*/
						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal AVI at */
						  /* 1st pass. Here AVI is  */
						  /* "arithm. expression"   */
int AVI1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal BUK at */
						  /* 1st pass. Here AVI is  */
						  /* BUK -   "letter"        */
int BUK1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal CIF at */
						  /* 1st pass. Here AVI is  */
						  /* CIF -   "digit"        */
int CIF1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IDE at */
						  /* 1st pass. Here AVI is  */
						  /* IDE -   "identifier"   */
int IDE1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IPE at */
						  /* 1st pass. Here AVI is  */
						  /* IPE - "variable name"  */
int IPE1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IPR at */
						  /* 1st pass. Here AVI is  */
						  /* IPR -   "program name" */
int IPR1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal LIT at */
						  /* 1st pass. Here AVI is  */
						  /* LIT -   "literal"      */
int LIT1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal MAN at */
						  /* 1st pass. Here AVI is  */
						  /* MAN -   "mantissa"     */
int MAN1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal ODC at */
						  /* 1st pass. Here AVI is  */
						  /* ODC - "PL1 op. DCL"    */
int ODC1 ()
 {
  int i;
  FORM ();                                        /* formatting of PL1 oper-*/
						  /* ator DCL             */

  for ( i = 0; i < ISYM; i++ )                    /* if we detect repeated  */
   {                                              /* declaration of         */
    if (  !strcmp ( SYM [i].NAME, FORMT [1] ) &&  /* second term of operator*/
	  strlen ( SYM [i].NAME ) ==                    /* DCL, then              */
			     strlen ( FORMT [1] )
       )
     return 6;                                    /* program completion     */
						                                      /* on error               */
   }

  strcpy ( SYM [ISYM].NAME, FORMT [1] );          /* if no repeated         */
  strcpy ( SYM [ISYM].RAZR, FORMT [4] );          /* identifier declaration */
                                                  /* we store it with       */
                                                  /* its precision in       */
                                                  /* table SYM              */

  if ( !strcmp ( FORMT [2], "BIN" ) &&            /* if identifier speci-   */
		  !strcmp ( FORMT [3], "FIXED" ) )            /* fied as bin fixed,     */
   {
    SYM [ISYM].TYPE = 'B';                        /* then set type          */
						                                      /* of identifier = 'B' and*/
    goto ODC11;                                   /* go to continue proces- */
						                                      /* sing, and              */
   }
  else                                            /* otherwise              */
   {
    SYM [ISYM].TYPE = 'U';                        /* set identifier type    */
						                                      /* = 'U' and              */
    return 2;                                     /* complete program       */
						                                      /* on error               */
   }

ODC11:                                            /* if identifier          */
						                                      /* has initial init-      */
  if ( !strcmp ( FORMT [5], "INIT" )  )           /* ialization, then store */
   strcpy ( SYM [ISYM++].INIT, FORMT [6] );       /* in SYM table this ini- */
						                                      /* tial value, and        */
  else                                            /* otherwise              */
   strcpy ( SYM [ISYM++].INIT, "0B" );            /* initialize identifier  */
						                                      /* with zero              */

   return 0;                                      /* successful completion  */
						                                      /* of program             */
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal OEN at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* OEN - "PL1 op. END"    */
int OEN1 ()
 {
  char i = 0;
  FORM ();                                        /* formatting of PL1 oper-*/
						                                      /* of operator END        */

  for ( i = 0; i < ISYM; i++ )                    /* if second term of oper-*/
						                                      /* ator END is in table   */
   {                                              /* SYM and its type = "P",*/
    if ( !strcmp ( SYM [i].NAME, FORMT [1] ) &&
		       (SYM [i].TYPE == 'P') &&
		       strlen (SYM [i].NAME) ==
			strlen ( FORMT [1] ) )
     return 0;                                    /* successful completion  */
						                                      /* of program             */
   }

  return 1;                                       /* otherwise program comp-*/
						                                      /* gram on error          */
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal OPA at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* OPA - "assign op.      */
                                                  /* arithmetic             */
int OPA1 ()
 {
  return 0;
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal OPR at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* OPR - "PL1 op. PROC"   */
int OPR1 ()
 {
  FORM ();                                        /* format PL1 operator    */
						                                      /* PL1 PROC               */

  strcpy ( SYM [ISYM].NAME, FORMT [0] );          /* copy PL1-program name  */
						                                      /* to SYM table,          */

  SYM [ISYM].TYPE   = 'P';                        /* set type of this       */
						                                      /* name = 'P'             */
  SYM [ISYM++].RAZR [0] = '\x0';                  /* set precision          */
						                                      /* equal to 0             */

  return 0;                                       /* successful completion  */
						                                      /* of program             */
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal PRO at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* PRO - "program"        */

int PRO1 ()
 {
  return 0;
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal RZR at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* RZR - "precision"      */

int RZR1 ()
 {
  return 0;
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal TEL at */
                                                  /* 1st pass. Here AVI is */
                                                  /* TEL - "program body"   */

int TEL1 ()
 {
  return 0;
 }

/*..........................................................................*/

                                                  /* p r o g r a m          */
                                                  /* semantic calculation   */
                                                  /* of non-terminal ZNK at */
                                                  /* 1st pass. Here AVI is  */
                                                  /* ZNK - "operation sign" */

int ZNK1 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal AVI at */
						  /* 2nd pass. Here OPA is  */
						  /* "arithm. expression"   */


int AVI2 ()
 {
  char i;
  FORM ();                                        /*format right part of    */
                                                  /*arithmetic PL1 assign-  */
                                                  /*ment operator           */

  if ( IFORMT == 1 )                              /* if right part is one-  */
     {                                            /* term, then:            */
    for ( i = 0; i < ISYM; i++ )                  /* search this term in    */
     {                                            /* names table and        */
      if ( !strcmp ( SYM [i].NAME, FORMT [0] )  &&/* if found, then:        */
	   strlen ( SYM [i].NAME ) ==
			      strlen ( FORMT [0] )
	 )
       {
	  if ( SYM [i].TYPE == 'B' )              /* if type=bin fixed      */
	   {

	    if ( strcmp ( SYM [i].RAZR, "15" )    /* and precision <= 15    */
					     <= 0 )
	     memcpy ( ASS_CARD._BUFCARD.OPERAC,   /* form assembler op.     */
					"LH", 2 );                        /* code LH,               */
	    else
	     memcpy ( ASS_CARD._BUFCARD.OPERAC,   /* and for precision >15  */
					 "L", 1 );                        /* form assembler op.     */
						                                /* code L                 */

	    strcpy ( ASS_CARD._BUFCARD.OPERAND,   /*       form             */
					"RRAB," );                        /*       1st and          */
	    strcat ( ASS_CARD._BUFCARD.OPERAND,   /* 2nd operands of assem- */
				       FORMT [0]);                  /* bler operation         */

	    ASS_CARD._BUFCARD.OPERAND [ strlen    /* insert separator       */
	     ( ASS_CARD._BUFCARD.OPERAND ) ] = ' ';

	    memcpy ( ASS_CARD._BUFCARD.COMM,      /* and line comment       */
	     "Load variable to register", 29 );

	    ZKARD ();                             /* store assembler op.    */
						                                /* and                    */
	    return 0;                             /* complete program       */
	   }
	  else
	   return 3;                              /* if term type not bin   */
                                            /* fixed, exit on error   */
                                            /*                        */
       }
     }
    return 4;                                     /* if term-identifier     */
                                                  /* undefined, exit        */
                                                  /* on error               */
   }
  else                                            /* if right part of arith-*/
                                                  /* metic expression       */
                                                  /* is two-term, then:     */
   {
    for ( i = 0; i < ISYM; i++ )                  /* if right term of arith-*/
     {                                            /* metic expression       */
      if ( !strcmp ( SYM [i].NAME,                /*defined in SYM table,   */
			    FORMT [IFORMT-1] )  &&
	   strlen ( SYM [i].NAME ) ==
		       strlen ( FORMT [IFORMT-1] )
	 )
       {
	  if ( SYM [i].TYPE == 'B' )              /* if right operand type  */
	   {                                      /* bin fixed, then:       */

	    if ( STROKA [ DST [I2].DST4 -         /* if operand sign "+",   */
	     strlen( FORMT [IFORMT-1] ) ] == '+' )
	     {
	      if ( strcmp ( SYM [i].RAZR, "15" )  /* if right operand prec. */
					    <= 0 )                        /* <= 15, then:           */
	       memcpy ( ASS_CARD._BUFCARD.OPERAC,
					"AH", 2 );                        /* form assembler op.*/
	      else                                /* code "AH", and*/
	       memcpy ( ASS_CARD._BUFCARD.OPERAC,
					 "A", 1 );                        /* otherwise - "A"            */
	     }

	    else

	     {
	      if ( STROKA [ DST [I2].DST4 -       /* if operation sign          */
		 strlen ( FORMT [IFORMT-1] ) ] ==       /* of arithmetic              */
					     '-' )                        /* expression "-", then:      */

	       {
		if ( strcmp ( SYM [i].RAZR, "15" )      /* if arithmetic              */
					    <= 0 )                        /* expr. precision <= 15      */
		 memcpy( ASS_CARD._BUFCARD.OPERAC,      /* form assembler op.         */
					"SH", 2 );                        /* code "SH",                 */
		else
		 memcpy( ASS_CARD._BUFCARD.OPERAC,      /* otherwise - "S"            */
					 "S", 1 );
	       }

	      else

	       return 5;                          /* if operation sign not       */
						                                /* "+" or "-", then            */
                                            /* program completion on       */
                                            /* error                       */
	     }
						                                /* form:                       */
	    strcpy ( ASS_CARD._BUFCARD.OPERAND,   /* - 1st operand of assem-     */
					"RRAB," );                        /* bler operation;             */
	    strcat ( ASS_CARD._BUFCARD.OPERAND,   /* - 2nd operand of assem-     */
			       FORMT [IFORMT-1] );            /* bler operation;             */
	    ASS_CARD._BUFCARD.OPERAND [ strlen
		  ( ASS_CARD._BUFCARD.OPERAND )] =      /* - separator space;          */
					      ' ';
	    memcpy ( ASS_CARD._BUFCARD.COMM,
	   "Form intermediate value",             /* - line comment              */
					     36 );
	    ZKARD ();                             /* store assembler             */
						                                /* operation                   */

	    return 0;                             /* successful completion       */
						                                /* of program                  */
	   }
	  else
	   return 3;                              /* if right operand type       */
                                            /* of arithmetic               */
                                            /* expression not bin fixed,   */
                                            /* then program                */
                                            /* tion on error               */
       }
     }
    return 4;                               /* if right operand       */
                                            /* of arithmetic          */
                                            /*expression not defined  */
                                            /* in SYM, then complete  */
                                            /* program on error       */
   }

 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal BUK at */
						  /* 2nd pass. Here OPA is  */
						  /* BUK -   "letter"        */

int BUK2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal CIF at */
						  /* 2nd pass. Here OPA is  */
						  /* CIF -   "digit"        */

int CIF2 ()
 {
  return 0;
 }

/*..........................................................................*/


						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IDE at */
						  /* 2nd pass. Here OPA is  */
						  /* IDE -   "identifier"   */

int IDE2 ()
 {
  return 0;
 }


/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IPE at */
						  /* 2nd pass. Here OPA is  */
						  /* IPE - "variable name"  */

int IPE2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal IPR at */
						  /* 2nd pass. Here OPA is  */
						  /* IPR -   "program name" */

int IPR2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal LIT at */
						  /* 2nd pass. Here OPA is  */
						  /* LIT -   "literal"      */

int LIT2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal MAN at */
						  /* 2nd pass. Here OPA is  */
						  /* MAN -   "mantissa"     */

int MAN2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal ODC at */
						  /* 2nd pass. Here OPA is  */
						  /* ODC - "PL1 op. DCL"    */

int ODC2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal OEN at */
						  /* 2nd pass. Here OPA is  */
						  /* OEN - "PL1 op. END"    */

						  /* program forms          */
						  /* epilogue of assembler  */
						  /* equivalent of PL1-prog-*/
						  /* ram                    */
int OEN2 ()
 {
  char RAB [20];
  char i = 0;
  FORM ();                                        /* format PL1-operator    */
						  /* END                    */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "BCR", 3 );  /* form unconditional   */
						  /*return control code     */
						  /* to calling program     */

  memcpy ( ASS_CARD._BUFCARD.OPERAND,"15,14", 5 );/* command operands and   */

  memcpy ( ASS_CARD._BUFCARD.COMM,                /* line comment field     */
		       "Exit from program", 18 );

  ZKARD ();                                       /* store operation        */
						  /* of Assembler           */

						  /* next is formation block*/
						  /* of declarative         */
						  /* DC pseudo-ops for      */
						  /* each identifier that   */
						  /* got into SYM table     */
  for ( i = 0; i < ISYM; i++ )
   {                                              /* if SYM table row       */
    if ( isalpha ( SYM [i].NAME [0] ) )           /* contains identifier,   */
						  /* i.e.starts with letter,*/
     {                                            /* then:                  */
      if ( SYM [i].TYPE == 'B' )                  /* if operator type bin   */
						  /* fixed, then:           */
       {
	strcpy ( ASS_CARD._BUFCARD.METKA,         /* write identifier to    */
				  SYM [i].NAME ); /* label field of pseudo- */
						  /* op DC                  */
	ASS_CARD._BUFCARD.METKA [ strlen
	     ( ASS_CARD._BUFCARD.METKA ) ] = ' '; /* write field separator*/

	memcpy ( ASS_CARD._BUFCARD.OPERAC,        /* write pseudo-op code   */
				       "DC", 2 ); /* DC                     */

	if ( strcmp ( SYM [i].RAZR, "15" ) <= 0 ) /* form pseudo-op operands*/
						  /* pseudo-op DC           */
	 strcpy ( ASS_CARD._BUFCARD.OPERAND,      /* for halfword case      */
					 "H\'" );
	else                                      /* or                     */

	 strcpy ( ASS_CARD._BUFCARD.OPERAND,      /* for word case          */					 "F\'" );

//Dos command
//	strcat ( ASS_CARD._BUFCARD.OPERAND,       /* form numeric           */
//		 ltoa ( VALUE (SYM [i].INIT),     /* part of pseudo-op      */
//				 &RAB [0], 10) ); /* operand,               */
//let's do that in Unix!
	strcat(ASS_CARD._BUFCARD.OPERAND, gcvt(VALUE(SYM[i].INIT), 10, &RAB[0]));
	ASS_CARD._BUFCARD.OPERAND [ strlen        /* closing apostrophe     */
	 ( ASS_CARD._BUFCARD.OPERAND ) ] = '\'';  /*          and             */

	memcpy ( ASS_CARD._BUFCARD.COMM,          /* line comment field     */
		 "Variable definition", 22 );

	ZKARD ();                                 /* store operation        */
						  /*    of Assembler        */
       }
     }
   }
						  /* next is declarative    */
						  /* assembler EQU-ops      */
						  /* defining base and      */
						  /* working registers      */
						  /* of general purpose     */
						  /*                        */

  memcpy ( ASS_CARD._BUFCARD.METKA, "RBASE", 5 ); /* form EQU-pseudo-op   */
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU",3 );   /* for working register   */
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "15", 2 );  /* number 15              */
						  /* of general purpose     */
						  /*                        */
  ZKARD ();                                       /* store it               */

  memcpy ( ASS_CARD._BUFCARD.METKA, "RRAB", 4 );  /* form EQU-pseudo-op   */
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU",3 );   /* for working register   */
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "5", 1 );   /* number 5               */
						  /* of general purpose     */
						  /*                        */
  ZKARD ();                                       /* store it               */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "END", 3 );  /* form assembler         */
						  /* pseudo-op END code,    */
						  /*                        */
  i = 0;

  while ( FORMT [1][i] != '\x0' )                 /* its operand            */
   ASS_CARD._BUFCARD.OPERAND [i] = FORMT [1][i++];/*         and            */

  memcpy ( ASS_CARD._BUFCARD.COMM,                /* line comment           */
			  "End of program", 15 );

  ZKARD ();                                       /* store pseudo-op        */
						  /*                        */

  return 0;                                       /* program completion       */
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal OPA at */
						  /* 2nd pass. Here OPA is*/
						  /* OPA - "assign op.    */
						  /* arithmetic           */

int OPA2 ()
 {
  int i;

  FORM ();                                        /*format PL1 operator     */
						  /*of arithmetic assign.   */

  for ( i = 0; i < ISYM; i++ )
   {                                              /* if right part identif. */
						  /* defined earlier via    */
    if ( !strcmp ( SYM [i].NAME, FORMT [0] )  &&  /* DCL operator, then:    */
	 strlen ( SYM [i].NAME ) ==               /*                        */
			     strlen ( FORMT [0] )
       )
       {
	  if ( SYM [i].TYPE == 'B' )              /* if this identifier     */
	   {                                      /* has type bin fixed,    */

	    if ( strcmp ( SYM [i].RAZR, "15" )    /* if bin fixed (15),     */
					    <= 0 )
	     memcpy ( ASS_CARD._BUFCARD.OPERAC,   /* form command           */
				       "STH", 3 );/* for halfword write     */

	    else                                  /* otherwise:             */
	     memcpy ( ASS_CARD._BUFCARD.OPERAC,   /* command for word write */
					"ST", 2 );

	    strcpy ( ASS_CARD._BUFCARD.OPERAND,   /*       complete         */
					"RRAB," );/*          operands      */

	    strcat ( ASS_CARD._BUFCARD.OPERAND,   /*           of command   */
				      FORMT [0]) ;

	    ASS_CARD._BUFCARD.OPERAND [ strlen    /*              and       */
	    ( ASS_CARD._BUFCARD.OPERAND ) ] = ' ';

	    memcpy ( ASS_CARD._BUFCARD.COMM,      /* line comment           */
	    "Form arithmetic expression value",
					     37 );
	    ZKARD ();                             /* store operation        */
						  /* of Assembler and       */
	    return 0;                             /* complete program       */
	   }

	  else                                    /* if identifier does not */
						  /* have type bin fixed,   */
	   return 3;                              /* completion with diagn. */
						  /* of error               */
       }
   }
  return 4;                                       /* if identifier not */
						  /* defined earlier via */
						  /* PL1 DCL operator, then*/
						  /* completion with diagn.*/
						  /* error                 */

 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal OPR at*/
						  /* 2nd pass. Here OPA is*/
						  /* OPR - "PL1 op. PROC" */

						  /* program forms          */
						  /* prologue of assembler*/
						  /* equivalent of source  */
						  /* PL1 program          */
int OPR2 ()
 {
  char i = 0;
  FORM ();                                        /* format PL1 operator    */
						  /* PL1 - "start of proced-*/
						  /* ure block"            */
  while ( FORMT [0][i] != '\x0' )
   ASS_CARD._BUFCARD.METKA [i++] = FORMT [0][i];  /* use 0 term as label*/
						  /* in START-pseudo-op of*/
						  /* Assembler  */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "START", 5 );/* complete code and oper- */
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "0", 1 );   /* ands in START-pseudo-op*/
  memcpy ( ASS_CARD._BUFCARD.COMM,                /* of Assembler    */
		      "Start of program", 16 );
  ZKARD ();                                       /* store Assembler card*/
						  /*                   */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "BALR", 4 ); /* form BALR-operation*/
  memcpy ( ASS_CARD._BUFCARD.OPERAND,             /* Assembler             */
				  "RBASE,0", 7 );
  memcpy ( ASS_CARD._BUFCARD.COMM,
		  "Load base register", 22 );
  ZKARD ();                                       /* and store it        */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "USING", 5 );/* form USING-pseudo-op*/
  memcpy ( ASS_CARD._BUFCARD.OPERAND,             /* of Assembler    */
				   "*,RBASE", 7 );
  memcpy ( ASS_CARD._BUFCARD.COMM,
		  "Set register as base", 23 );
  ZKARD ();                                       /* and store it        */

  return 0;                                       /* complete subroutine */
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal PRO at*/
						  /* 2nd pass. Here OPA is*/
						  /* PRO - "program"        */

int PRO2 ()                                       /*program forms output*/
 {                                                /*file                    */

  FILE *fp;                                       /*set of working                   */
						  /*variables                 */
						  /*              */

  strcat ( NFIL , "ass" );                        /*form output file name */
						  /*file              */

  if ( (fp = fopen ( NFIL , "wb" )) == NULL )     /*on unsuccessful open */
   return (7);                                    /* error message    */

  else                                            /*otherwise:                  */
   fwrite (ASSTXT, 80 , IASSTXT , fp);            /* form object file body*/
  fclose ( fp );                                  /*close object file*/
  return ( 0 );                                   /*complete subroutine */
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal RZR at*/
						  /* 2nd pass. Here OPA is*/
						  /* RZR - "precision"      */

int RZR2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal TEL at*/
						  /* 2nd pass. Here OPA is*/
						  /* TEL - "program body"   */

int TEL2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /* p r o g r a m          */
						  /* semantic calculation   */
						  /* of non-terminal ZNK at*/
						  /* 2nd pass. Here OPA is*/
						  /* ZNK - "operation sign" */

int ZNK2 ()
 {
  return 0;
 }

/*..........................................................................*/

						  /*  p r o g r a m     */
						  /* of abstract machine */
						  /* control - semantic */
						  /* calculator interpreting */
						  /* abstract program formed  */
						  /* by syntax analyzer in*/
						  /* achievements stack.*/
						  /* The algorithm essence */
						  /* is sequential        */

						  /* interpretation of*/
						  /*achievements stack rows*/
int gen_COD ()                                    /*from bottom to top.*/
 {                                                /*Each row is treated as*/
  int NOSH;                                       /*abstract machine command */

  int (* FUN [NNETRM][2]) () =                    /*with following fields:  */
   {                                              /*- DST.DST1 - op. code;*/
     {/*    1  */    AVI1, AVI2 },                /*- DST.DST2 - left bnd.  */
     {/*    2  */    BUK1, BUK2 },                /*of interpreted source      */
     {/*    3  */    CIF1, CIF2 },
     {/*    4  */    IDE1, IDE2 },		  /*text fragment;*/
     {/*    5  */    IPE1, IPE2 },      	  /*- DST.DST4 - right bnd.                    */
     {/*    6  */    IPR1, IPR2 },
     {/*    7  */    LIT1, LIT2 },            	  /*- DST.DST2 - left bnd.*/
     {/*    8  */    MAN1, MAN2 },		  /*of interpreted source  */
     {/*    9  */    ODC1, ODC2 },		  /*text fragment. */
     {/*   10  */    OEN1, OEN2 },
     {/*   11  */    OPA1, OPA2 },		  /**/
     {/*   12  */    OPR1, OPR2 },		  /*of interpreted source  */
     {/*   13  */    PRO1, PRO2 },		  /*text fragment. */
     {/*   14  */    RZR1, RZR2 },
     {/*   15  */    TEL1, TEL2 },
     {/*   16  */    ZNK1, ZNK2 }
   };

  for ( I2 = 0; I2 < L; I2++ )                    /* 1st pass organization    */
   if ( ( NOSH = FUN [                            /* of semantic calculation */
		      numb ( DST [I2].DST1, 3 )   /* calculation             */
		     ][0] ()
	) != 0
      )
    return (NOSH);                                /* program exit     */
						  /* on error               */

  for ( I2 = 0; I2 < L; I2++ )                    /* 2nd pass organization    */
   if ( ( NOSH = FUN [                            /* of semantic calculation */
		      numb ( DST [I2].DST1, 3 )   /* calculation             */
		     ][1] ()
	) != 0
      )
    return (NOSH);                                /* program exit     */
						  /* on error               */

  return 0;                                       /* successful completion  */
						  /* of program             */
 }

/*..........................................................................*/

						  /*  p r o g r a m,    */
						  /* organizing sequential*/
						  /* processing of source:  */
						  /* - lexical analyzer;        */
						  /* - syntax analyzer; */
						  /* - semantic calculator */
						  /* */
int main (int argc, char **argv )
 {                                                /* working variables:    */
  FILE *fp;                                       /* - file pointer;   */
  char *ptr=argv[1];                              /* - pointer to 1st  */
						  /*command line parameter */

  strcpy ( NFIL, ptr );                           /*get translated program*/
						  /*name from command line  */
						  /*to working field   */

						  /*check command line  */
						  /*correctness        */
  if ( argc != 2 )

   {                                              /* on command line error*/
    printf ("%s\n", "Error in command line"); /* output diagnosis and   */
    return;                                       /* complete translation   */
   }

						  /* check source file type*/
  if
   (
    strcmp ( &NFIL [ strlen ( NFIL )-3 ], "pli" ) /* if type not "pli", then: */
   )

   {
    printf ( "%s\n",                              /* output diagnosis and   */
     "Invalid file type with source text" );
    return;                                       /* complete translation   */
   }


  else                                            /* if file type "pli", then*/

   {                                              /*try to open file and */
    if ( (fp = fopen ( NFIL , "rb" )) == NULL )   /*on unsuccessful open */
						  /* error message and  */
     {
      printf ( "%s\n",
       "Source text file not found" );
      return;                                     /* complete translation  */
     }

    else                                          /* otherwise:                 */
						  /* write file to array    */
						  /*  ISXTXT                */
     {
      for ( NISXTXT = 0; NISXTXT <= MAXNISXTXT; NISXTXT++ )

       {
	if ( !fread ( ISXTXT [NISXTXT], 80, 1, fp ) )
	 {
	  if ( feof ( fp ) )                      /* at file end go to  */
	   goto main1;                            /* main1 label           */

	  else                                    /* on read error        */
	   {                                      /* output diagnosis     */
	    printf ( "%s\n",
	     "Error reading source text file" );
	    return;                               /* and complete translation */
	   }
	 }
       }

      printf ( "%s\n",                            /*on array ISXTXT overflow*/
       "Source text read buffer overflow" ); /* output diagnosis   */
      return;                                     /* and complete translation */
     }

   }

main1:                                            /* after reading completion   */
						  /* of source file form    */
   fclose ( fp );                                 /* output Assembler file*/
   NFIL [ strlen ( NFIL )-3 ] = '\x0';            /* Assembler file */

  memset ( ASS_CARD.BUFCARD, ' ', 80 );           /* clear output Assembler   */
						  /* file line buffer */
						  /*             */

  compress_ISXTXT ();                             /* lexical analysis     */
						  /* of source text       */

  build_TPR ();                                   /* build successor matrix     */
						  /* table                  */

  if ( (sint_ANAL ()) )                           /* syntax analysis  */
   {                                              /* of source text       */
     STROKA [I4 +20] = '\x0';
     printf                                       /* if syntax errors found,    */
      (                                           /* then:       */
       "%s%s%s%s\n",
       "syntax error in source text -> ",      /* output diagnosis and   */
       "\"...",&STROKA [I4], "...\""
      );
     printf
      (
       "%s\n", "translation interrupted"
      );
     return;                                      /* complete translation   */
   }
  else                                            /* otherwise do           */
   {
    switch ( gen_COD () )                         /* semantic calculation*/
     {
      case  0:                                    /*if completion code = 0,*/
						  /* then:                    */

       printf ( "%s\n",                           /* - diagnostic message;*/
	"translation completed successfully" );
       return;                                    /* - complete translation */


      case  1:                                    /*if completion code = 1,*/
						  /* then:                    */
       printf ( "%s\n",                           /* - diagnostic message;*/
	"procedure name mismatch in prologue-epilogue" );
       break;                                     /* - go to general  *//* - diagnostic message;*/
						  /*diagnosis             */

      case  2:                                    /*if completion code = 2,*/
						  /* then:                    */
       STROKA [ DST [I2].DST2 +20 ] = '\x0';      /* - diagnostic message;*/
       printf ( "%s%s\n%s%s%s\n",
	"invalid identifier type: ",
	 &FORMT [1], " in source text -> \"...",
	  &STROKA [ DST [I2].DST2 ], "...\"" );
       break;                                     /* - go to general  */
						  /*diagnosis             */

      case  3:                                    /*if completion code = 3,*/
						  /* then:                    */
       STROKA [ DST [I2].DST2 + 20 ] = '\x0';     /* - diagnostic message;*/
       printf ( "%s%s\n%s%s%s\n",
	"invalid identifier type: ",
	  &FORMT [IFORMT-1], " in source text -> \"...",
	   &STROKA [ DST [I2].DST2 ], "...\"" );
       break;                                     /* - go to general   */
						  /*diagnosis             */

      case  4:                                    /*if completion code = 4,*/
						  /* then:                    */
       STROKA [ DST [I2].DST2 + 20 ] = '\x0';     /* - diagnostic message;*/
       printf ( "%s%s\n%s%s%s\n",
	"undefined identifier: ",
	 &FORMT [IFORMT-1], " in source text -> \"...",
	  &STROKA [ DST [I2].DST2 ], "...\"" );
       break;                                     /* - go to general  */
						  /*diagnosis             */

      case  5:                                    /*if completion code = 5,*/
						  /* then:                    */
       STROKA [ DST [I2].DST2 + 20 ] = '\x0';     /* - diagnostic message;*/
       printf ( "%s%c\n%s%s%s\n",
	"invalid operation: ",
	 STROKA [ DST [I2].DST4 - strlen ( FORMT [IFORMT-1] ) ],
	 " in source text -> \"...", &STROKA [ DST [I2].DST2 ], "...\"");
       break;                                     /* - go to general  */
						  /*diagnosis             */

      case  6:                                    /*if completion code = 6 */
						  /* then:                    */
       STROKA [ DST [I2].DST2 + 20 ] = '\x0';     /* - diagnostic message;*/
       printf ( "%s%s\n%s%s%s\n",
	"repeated identifier declaration: ",
	 &FORMT [1], " in source text -> \"...",
	  &STROKA [ DST [I2].DST2 ], "...\"" );
       break;                                     /* - go to general  */
						  /*diagnosis             */

     }

   }

  printf ( "%s\n", "translation interrupted" );       /* general diagnosis */
	
  return 0;
 }
/*..........................................................................*/
