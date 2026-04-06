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
#define NSINT     320                             /* - syntax rules table;  */
#define NCEL       20                             /* - goals stack;         */
#define NDST      500                             /* - achievements stack;  */
#define NVXOD      54                             /* - entries table;       */
#define NSTROKA   200                             /* - compressed text line;*/
#define NNETRM     18                             /* - non-terminals list;  */
#define MAXLTXT   100                             /* - output text;         */
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
 {/*.   58     .*/    59 ,    57 , "D  " ,  247 },
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

 {/*.  168     .*/   169 ,   165 , "IPE" ,  229 },
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


 {/*.  187     .*/   188 ,    49 , "C  " ,  201 },
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
 {/*.  200     .*/     0 ,   199 , "*  " ,    0 },
  /*                                            DEC FIXED path (alt from 187) */
 {/*.  201     .*/   202 ,    49 , "D  " ,    0 },
 {/*.  202     .*/   203 ,   201 , "E  " ,    0 },
 {/*.  203     .*/   204 ,   202 , "C  " ,    0 },
 {/*.  204     .*/   205 ,   203 , "   " ,    0 },
 {/*.  205     .*/   206 ,   204 , "F  " ,    0 },
 {/*.  206     .*/   207 ,   205 , "I  " ,    0 },
 {/*.  207     .*/   208 ,   206 , "X  " ,    0 },
 {/*.  208     .*/   209 ,   207 , "E  " ,    0 },
 {/*.  209     .*/   210 ,   208 , "D  " ,  263 },
 {/*.  210     .*/   211 ,   209 , "(  " ,    0 },
 {/*.  211     .*/   212 ,   210 , "RZR" ,    0 },
 {/*.  212     .*/   213 ,   211 , ")  " ,    0 },
 {/*.  213     .*/   214 ,   212 , ";  " ,  216 },
 {/*.  214     .*/   215 ,   213 , "ODC" ,    0 },
 {/*.  215     .*/     0 ,   214 , "*  " ,    0 },
 {/*.  216     .*/   217 ,   212 , "I  " ,    0 },
 {/*.  217     .*/   218 ,   216 , "N  " ,    0 },
 {/*.  218     .*/   219 ,   217 , "I  " ,    0 },
 {/*.  219     .*/   220 ,   218 , "T  " ,    0 },
 {/*.  220     .*/   221 ,   219 , "(  " ,    0 },
 {/*.  221     .*/   222 ,   220 , "LID" ,    0 },
 {/*.  222     .*/   223 ,   221 , ")  " ,    0 },
 {/*.  223     .*/   224 ,   222 , ";  " ,    0 },
 {/*.  224     .*/   225 ,   223 , "ODC" ,    0 },
 {/*.  225     .*/     0 ,   224 , "*  " ,    0 },
  /*                                            = as ZNL (logical comparison)*/
 {/*.  226     .*/   227 ,     0 , "=  " ,    0 },
 {/*.  227     .*/   228 ,   226 , "ZNL" ,    0 },
 {/*.  228     .*/     0 ,   227 , "*  " ,    0 },
  /*                                            RZR as AVI operand           */
 {/*.  229     .*/   230 ,   165 , "RZR" ,    0 },
 {/*.  230     .*/   231 ,   229 , "AVI" ,    0 },
 {/*.  231     .*/     0 ,   230 , "*  " ,    0 },
  /*                                            BIN FIXED without (RZR): ;    */
 {/*.  247     .*/   248 ,    58 , ";  " ,  250 },
 {/*.  248     .*/   249 ,   247 , "ODC" ,    0 },
 {/*.  249     .*/     0 ,   248 , "*  " ,    0 },
  /*                                            BIN FIXED ... INIT (LIT)     */
 {/*.  250     .*/   251 ,    58 , "I  " ,    0 },
 {/*.  251     .*/   252 ,   250 , "N  " ,    0 },
 {/*.  252     .*/   253 ,   251 , "I  " ,    0 },
 {/*.  253     .*/   254 ,   252 , "T  " ,    0 },
 {/*.  254     .*/   255 ,   253 , "(  " ,    0 },
 {/*.  255     .*/   256 ,   254 , "LIT" ,    0 },
 {/*.  256     .*/   257 ,   255 , ")  " ,    0 },
 {/*.  257     .*/   258 ,   256 , ";  " ,    0 },
 {/*.  258     .*/   259 ,   257 , "ODC" ,    0 },
 {/*.  259     .*/     0 ,   258 , "*  " ,    0 },
  /*                                            DEC FIXED without (RZR): ;   */
 {/*.  263     .*/   264 ,   209 , ";  " ,  266 },
 {/*.  264     .*/   265 ,   263 , "ODC" ,    0 },
 {/*.  265     .*/     0 ,   264 , "*  " ,    0 },
  /*                                            DEC FIXED ... INIT (LID)     */
 {/*.  266     .*/   267 ,   209 , "I  " ,    0 },
 {/*.  267     .*/   268 ,   266 , "N  " ,    0 },
 {/*.  268     .*/   269 ,   267 , "I  " ,    0 },
 {/*.  269     .*/   270 ,   268 , "T  " ,    0 },
 {/*.  270     .*/   271 ,   269 , "(  " ,    0 },
 {/*.  271     .*/   272 ,   270 , "LID" ,    0 },
 {/*.  272     .*/   273 ,   271 , ")  " ,    0 },
 {/*.  273     .*/   276 ,   272 , ";  " ,    0 },
 {/*.  276     .*/   277 ,   273 , "ODC" ,    0 },
 {/*.  277     .*/     0 ,   276 , "*  " ,    0 },
  /*                                            LID decimal literal (MAN)    */
 {/*.  279     .*/   280 ,     0 , "LID" ,    0 },
 {/*.  280     .*/   281 ,   279 , "MAN" ,    0 },
 {/*.  281     .*/     0 ,   280 , "*  " ,    0 }
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
  {/*.  17     .*/   "LID" , 279 , 'N' },
  {/*.  18     .*/   "ZNL" , 227 , 'N' },
  {/*.  19     .*/   "A  " ,  33 , 'T' },
  {/*.  20     .*/   "B  " ,  36 , 'T' },
  {/*.  21     .*/   "C  " ,  39 , 'T' },
  {/*.  22     .*/   "D  " ,  42 , 'T' },
  {/*.  23     .*/   "E  " ,  74 , 'T' },
  {/*.  24     .*/   "M  " ,  84 , 'T' },
  {/*.  25     .*/   "P  " ,  87 , 'T' },
  {/*.  26     .*/   "X  " ,  90 , 'T' },
  {/*.  27     .*/   "0  " ,   1 , 'T' },
  {/*.  28     .*/   "1  " ,   4 , 'T' },
  {/*.  29     .*/   "2  " ,   9 , 'T' },
  {/*.  30     .*/   "3  " ,  12 , 'T' },
  {/*.  31     .*/   "4  " ,  15 , 'T' },
  {/*.  32     .*/   "5  " ,  18 , 'T' },
  {/*.  33     .*/   "6  " ,  21 , 'T' },
  {/*.  34     .*/   "7  " ,  24 , 'T' },
  {/*.  35     .*/   "8  " ,  27 , 'T' },
  {/*.  36     .*/   "9  " ,  30 , 'T' },
  {/*.  37     .*/   "+  " , 107 , 'T' },
  {/*.  38     .*/   "-  " , 110 , 'T' },
  {/*.  39     .*/   ":  " ,   0 , 'T' },
  {/*.  40     .*/   "I  " ,   0 , 'T' },
  {/*.  41     .*/   "R  " ,   0 , 'T' },
  {/*.  42     .*/   "N  " ,   0 , 'T' },
  {/*.  43     .*/   "O  " ,   0 , 'T' },
  {/*.  44     .*/   "T  " ,   0 , 'T' },
  {/*.  45     .*/   "S  " ,   0 , 'T' },
  {/*.  46     .*/   "(  " ,   0 , 'T' },
  {/*.  47     .*/   ")  " ,   0 , 'T' },
  {/*.  48     .*/   "   " ,   0 , 'T' },
  {/*.  49     .*/   ";  " ,   0 , 'T' },
  {/*.  50     .*/   "L  " ,   0 , 'T' },
  {/*.  51     .*/   "F  " ,   0 , 'T' },
  {/*.  52     .*/   "=  " , 226 , 'T' },
  {/*.  53     .*/   "H  " ,   0 , 'T' },
  {/*.  54     .*/   "*  " , 198 , 'T' }
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
  {/*AVI*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*BUK*/ 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*CIF*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0  },
  {/*IDE*/ 0 , 0 , 0 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*IPE*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*IPR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*LIT*/ 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*MAN*/ 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*ODC*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0  },
  {/*OEN*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*OPA*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*OPR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0  },
  {/*PRO*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*RZR*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0  },
  {/*TEL*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 1  },
  {/*ZNK*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*LID*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 1 , 0  },
  {/*ZNL*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  A*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  B*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  C*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  D*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  E*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  M*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  P*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  X*/ 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  0*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  1*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  2*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  3*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  4*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  5*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0  },
  {/*  6*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0  },
  {/*  7*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE;IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  8*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  9*/ 0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  +*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  },
  {/*  -*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  },
  {/*  :*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  I*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  R*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  N*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  O*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  T*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  S*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  (*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  )*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  ;*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*   */ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  L*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN;OPA:OPR:PR0:RZR:TEL:ZNK|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
  {/*  F*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  =*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 1  },
  {/*  H*/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  },
  {/*  **/ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 0 , 0  }
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

char RESULT_REG[9] = "@R2";
char PENDING_LABEL[9] = "";

struct {
    char name[9];
    char type;
    int  size;
    char value[10];
} EXTRA[20];
int NEXTRA = 0;

#define COMM_LIM 25
void SETCOMM ( const char *s )
{
  int i;
  /* COMM field sometimes contains "overflown" operand tail characters
     (because operand can be longer than the fixed overlay field).
     Preserve the leading non-space bytes (the tail), then write the comment
     after them.

     If we preserved at least 1 char from the tail, keep one extra separator
     space so the listing doesn't show "(1)Mul" glued together. */
  int prefix = 0;
  while ( prefix < 52 && ASS_CARD._BUFCARD.COMM[prefix] != ' ' ) prefix++;

  int start = prefix;
  if ( prefix > 0 && prefix + 1 < 52 ) start = prefix + 1;

  /* Clear only from start onward (keep preserved tail + separator space). */
  for ( i = start; i < 52; i++ ) ASS_CARD._BUFCARD.COMM[i] = ' ';

  if ( !s ) return;
  for ( i = 0; i < COMM_LIM && s[i] != '\0' && start + i < 52; i++ )
    ASS_CARD._BUFCARD.COMM[start + i] = s[i];
}

static char *DIGIT_WORDS[] = {
  "ZERO","ONE","TWO","THREE","FOUR","FIVE","SIX","SEVEN","EIGHT","NINE"
};

void const_label ( char *buf, const char *val )
{
  long v = atol(val);
  /* Avoid relying on a table-of-pointers: some parts of the compiler
     manipulate fixed-width buffers and this can indirectly corrupt
     global pointer tables. Switch-case is safer for label creation. */
  switch ( v )
   {
    case 0: sprintf ( buf, "@ZERO" ); break;
    case 1: sprintf ( buf, "@ONE" ); break;
    case 2: sprintf ( buf, "@TWO" ); break;
    case 3: sprintf ( buf, "@THREE" ); break;
    case 4: sprintf ( buf, "@FOUR" ); break;
    case 5: sprintf ( buf, "@FIVE" ); break;
    case 6: sprintf ( buf, "@SIX" ); break;
    case 7: sprintf ( buf, "@SEVEN" ); break;
    case 8: sprintf ( buf, "@EIGHT" ); break;
    case 9: sprintf ( buf, "@NINE" ); break;
    default: sprintf ( buf, "@C%s", val ); break;
   }
}

/*..........................................................................*/

long int VALUE ( char* s )
 {
  long int S;
  int i, is_bin = 0;

  for ( i = 0; s[i] != '\0'; i++ )
    if ( s[i] == 'B' ) { is_bin = 1; break; }

  if ( is_bin )
   {
    i = 0; S = 0;
    while ( *(s + i) != 'B' )
     { S <<= 1; if ( *(s + i) == '1' ) S++; i++; }
    return (S);
   }
  else
    return atol(s);
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
  FORM ();

  for ( i = 0; i < ISYM; i++ )
   {
    if (  !strcmp ( SYM [i].NAME, FORMT [1] ) &&
	  strlen ( SYM [i].NAME ) == strlen ( FORMT [1] ) )
     return 6;
   }

  strcpy ( SYM [ISYM].NAME, FORMT [1] );

  if ( !strcmp ( FORMT [2], "BIN" ) && !strcmp ( FORMT [3], "FIXED" ) )
   {
    SYM [ISYM].TYPE = 'B';
    if ( FORMT [4][0] >= '0' && FORMT [4][0] <= '9' )
      strcpy ( SYM [ISYM].RAZR, FORMT [4] );
    else
      strcpy ( SYM [ISYM].RAZR, "31" );
    goto ODC11;
   }
  else if ( !strcmp ( FORMT [2], "DEC" ) && !strcmp ( FORMT [3], "FIXED" ) )
   {
    SYM [ISYM].TYPE = 'D';
    if ( FORMT [4][0] >= '0' && FORMT [4][0] <= '9' )
      strcpy ( SYM [ISYM].RAZR, FORMT [4] );
    else
      strcpy ( SYM [ISYM].RAZR, "3" );
    goto ODC11;
   }
  else
   {
    SYM [ISYM].TYPE = 'U';
    return 2;
   }

ODC11:
  if ( !strcmp ( FORMT [4], "INIT" )  )
   strcpy ( SYM [ISYM++].INIT, FORMT [5] );
  else if ( !strcmp ( FORMT [5], "INIT" )  )
   strcpy ( SYM [ISYM++].INIT, FORMT [6] );
  else
   { SYM [ISYM].INIT[0] = '\0'; ISYM++; }

   return 0;
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
  int i;
  char op_sign;
  char opbuf[40];

  FORM ();

  if ( IFORMT == 1 )
   {
    for ( i = 0; i < ISYM; i++ )
     {
      if ( !strcmp ( SYM [i].NAME, FORMT [0] ) &&
           strlen ( SYM [i].NAME ) == strlen ( FORMT [0] ) )
       {
        if ( SYM [i].TYPE == 'B' )
         {
          memcpy ( ASS_CARD._BUFCARD.OPERAC, "LH", 2 );
          strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R2," );
          strcat ( ASS_CARD._BUFCARD.OPERAND, FORMT [0] );
          ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
          SETCOMM ( "Load A ->@R2" );
          ZKARD ();
          strcpy ( RESULT_REG, "@R2" );
          return 0;
         }
        else if ( SYM [i].TYPE == 'D' )
         {
          memcpy ( ASS_CARD._BUFCARD.OPERAC, "ZAP", 3 );
          sprintf ( opbuf, "@BUF(8),%s(%s)", FORMT[0], SYM[i].RAZR );
          memcpy ( ASS_CARD.BUFCARD + 15, opbuf, strlen(opbuf) );
          SETCOMM ( "Copy pdec B->BUF" );
          ZKARD ();
          return 0;
         }
        else
         return 3;
       }
     }
    return 4;
   }
  else
   {
    op_sign = STROKA [ DST [I2].DST4 - strlen( FORMT [IFORMT-1] ) ];

    if ( op_sign == '*' )
     {
      int li;
      char const_val[10];
      strcpy ( const_val, FORMT[IFORMT-1] );

      { char clbl[9];
        const_label ( clbl, const_val );
        sprintf ( opbuf, "@BUF(8),%s(1)", clbl );
      }
      memcpy ( ASS_CARD._BUFCARD.OPERAC, "MP", 2 );
      memcpy ( ASS_CARD.BUFCARD + 15, opbuf, strlen(opbuf) );
      SETCOMM ( "Mul BUF by 5 ->BUF" );
      ZKARD ();

      memcpy ( ASS_CARD._BUFCARD.OPERAC, "CVB", 3 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R1,@BUF" );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "pdec BUF ->int @R1" );
      ZKARD ();

      strcpy ( RESULT_REG, "@R1" );

      for ( li = 0; li < NEXTRA; li++ )
        if ( !strcmp(EXTRA[li].value, const_val) ) break;
      if ( li == NEXTRA )
       {
        const_label ( EXTRA[NEXTRA].name, const_val );
        EXTRA[NEXTRA].type = 'P';
        EXTRA[NEXTRA].size = 1;
        strcpy ( EXTRA[NEXTRA].value, const_val );
        NEXTRA++;
       }

      return 0;
     }

    else if ( op_sign == '=' )
     {
      memcpy ( ASS_CARD._BUFCARD.OPERAC, "CH", 2 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R2," );
      strcat ( ASS_CARD._BUFCARD.OPERAND, FORMT[IFORMT-1] );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "Compare @R2 with C" );
      ZKARD ();

      memcpy ( ASS_CARD._BUFCARD.OPERAC, "BC", 2 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "8,@TRUE" );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "Branch if equal" );
      ZKARD ();

      memcpy ( ASS_CARD._BUFCARD.OPERAC, "LA", 2 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R3,0" );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "Set @R3=0 (F)" );
      ZKARD ();

      memcpy ( ASS_CARD._BUFCARD.OPERAC, "B", 1 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "@SAVE" );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "Jump to save" );
      ZKARD ();

      memcpy ( ASS_CARD._BUFCARD.METKA, "@TRUE", 5 );
      ASS_CARD._BUFCARD.METKA[5] = ' ';
      memcpy ( ASS_CARD._BUFCARD.OPERAC, "LA", 2 );
      strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R3,1" );
      ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
      SETCOMM ( "Set @R3=1 (T)" );
      ZKARD ();

      strcpy ( RESULT_REG, "@R3" );
      strcpy ( PENDING_LABEL, "@SAVE" );

      return 0;
     }

    else
     {
      for ( i = 0; i < ISYM; i++ )
       {
        if ( !strcmp ( SYM [i].NAME, FORMT [IFORMT-1] ) &&
             strlen ( SYM [i].NAME ) == strlen ( FORMT [IFORMT-1] ) )
         {
          if ( SYM [i].TYPE == 'B' )
           {
            if ( op_sign == '+' )
              memcpy ( ASS_CARD._BUFCARD.OPERAC, "AH", 2 );
            else if ( op_sign == '-' )
              memcpy ( ASS_CARD._BUFCARD.OPERAC, "SH", 2 );
            else
              return 5;

            strcpy ( ASS_CARD._BUFCARD.OPERAND, "@R2," );
            strcat ( ASS_CARD._BUFCARD.OPERAND, FORMT [IFORMT-1] );
            ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
            ZKARD ();
            strcpy ( RESULT_REG, "@R2" );
            return 0;
           }
          else
            return 3;
         }
       }
      return 4;
     }
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
  int i = 0;
  int ei;
  FORM ();

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "BCR", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND,"15,@RVIX", 8 );
  ASS_CARD._BUFCARD.OPERAND[8] = ' ';
  SETCOMM ( "Return to caller" );
  ZKARD ();

  for ( i = 0; i < ISYM; i++ )
   {
    if ( !isalpha ( SYM [i].NAME [0] ) ) continue;
    if ( SYM [i].TYPE != 'B' && SYM [i].TYPE != 'D' ) continue;

      memset ( ASS_CARD._BUFCARD.METKA, ' ', 8 );
      memcpy ( ASS_CARD._BUFCARD.METKA, SYM [i].NAME,
               strlen(SYM [i].NAME) );

      if ( SYM [i].TYPE == 'B' )
       {
        if ( SYM[i].INIT[0] == '\0' )
         {
          memcpy ( ASS_CARD._BUFCARD.OPERAC, "DS", 2 );
          strcpy ( ASS_CARD._BUFCARD.OPERAND, "H" );
          ASS_CARD._BUFCARD.OPERAND[1] = ' ';
          SETCOMM ( "Reserve halfword" );
         }
        else
         {
          memcpy ( ASS_CARD._BUFCARD.OPERAC, "DC", 2 );
          strcpy ( ASS_CARD._BUFCARD.OPERAND, "H\'" );
          sprintf ( RAB, "%ld", VALUE(SYM[i].INIT) );
          strcat ( ASS_CARD._BUFCARD.OPERAND, RAB );
          ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = '\'';
          SETCOMM ( "Store halfword init" );
         }
        ZKARD ();
       }
      else if ( SYM [i].TYPE == 'D' )
       {
        char plop[20];
        memcpy ( ASS_CARD._BUFCARD.OPERAC, "DC", 2 );
        sprintf ( plop, "PL%s\'%ld\'", SYM[i].RAZR, VALUE(SYM[i].INIT) );
        strcpy ( ASS_CARD._BUFCARD.OPERAND, plop );
        ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
        SETCOMM ( "Store pdec init" );
        ZKARD ();
       }
   }

  for ( ei = 0; ei < NEXTRA; ei++ )
   {
    char plop[20];
    strcpy ( ASS_CARD._BUFCARD.METKA, EXTRA[ei].name );
    ASS_CARD._BUFCARD.METKA[strlen(EXTRA[ei].name)] = ' ';
    memcpy ( ASS_CARD._BUFCARD.OPERAC, "DC", 2 );
    sprintf ( plop, "PL%d\'%s\'", EXTRA[ei].size, EXTRA[ei].value );
    strcpy ( ASS_CARD._BUFCARD.OPERAND, plop );
    ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
    SETCOMM ( "Store packed const" );
    ZKARD ();
   }

  if ( NEXTRA > 0 )
   {
    memcpy ( ASS_CARD._BUFCARD.OPERAC, "DS", 2 );
    strcpy ( ASS_CARD._BUFCARD.OPERAND, "0F" );
    ASS_CARD._BUFCARD.OPERAND[2] = ' ';
    SETCOMM ( "Align next item" );
    ZKARD ();

    memcpy ( ASS_CARD._BUFCARD.METKA, "@BUF", 4 );
    ASS_CARD._BUFCARD.METKA[4] = ' ';
    memcpy ( ASS_CARD._BUFCARD.OPERAC, "DC", 2 );
    strcpy ( ASS_CARD._BUFCARD.OPERAND, "PL8\'0\'" );
    ASS_CARD._BUFCARD.OPERAND[6] = ' ';
    SETCOMM ( "Clear packed BUF" );
    ZKARD ();
   }

  memcpy ( ASS_CARD._BUFCARD.METKA, "@RBASE", 6 );
  ASS_CARD._BUFCARD.METKA[6] = ' ';
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "5", 1 );
  SETCOMM ( "Set @RBASE=5" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.METKA, "@R1", 3 );
  ASS_CARD._BUFCARD.METKA[3] = ' ';
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "6", 1 );
  SETCOMM ( "Set @R1=6" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.METKA, "@R2", 3 );
  ASS_CARD._BUFCARD.METKA[3] = ' ';
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "2", 1 );
  SETCOMM ( "Set @R2=2" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.METKA, "@R3", 3 );
  ASS_CARD._BUFCARD.METKA[3] = ' ';
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "3", 1 );
  SETCOMM ( "Set @R3=3" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.METKA, "@RVIX", 5 );
  ASS_CARD._BUFCARD.METKA[5] = ' ';
  memcpy ( ASS_CARD._BUFCARD.OPERAC, "EQU", 3 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND, "14", 2 );
  SETCOMM ( "Set @RVIX=14" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "END", 3 );
  SETCOMM ( "Program ends" );
  ZKARD ();

  return 0;
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

  FORM ();

  for ( i = 0; i < ISYM; i++ )
   {
    if ( !strcmp ( SYM [i].NAME, FORMT [0] )  &&
         strlen ( SYM [i].NAME ) == strlen ( FORMT [0] ) )
     {
      if ( SYM [i].TYPE == 'B' )
       {
        if ( PENDING_LABEL[0] != '\0' )
         {
          strcpy ( ASS_CARD._BUFCARD.METKA, PENDING_LABEL );
          ASS_CARD._BUFCARD.METKA[strlen(PENDING_LABEL)] = ' ';
          PENDING_LABEL[0] = '\0';
         }

        memcpy ( ASS_CARD._BUFCARD.OPERAC, "STH", 3 );
        strcpy ( ASS_CARD._BUFCARD.OPERAND, RESULT_REG );
        strcat ( ASS_CARD._BUFCARD.OPERAND, "," );
        strcat ( ASS_CARD._BUFCARD.OPERAND, FORMT [0] );
        ASS_CARD._BUFCARD.OPERAND[strlen(ASS_CARD._BUFCARD.OPERAND)] = ' ';
        if ( !strcmp ( RESULT_REG, "@R3" ) )
          SETCOMM ( "Store bool ->C" );
        else
          SETCOMM ( "Store int ->C" );
        ZKARD ();
        return 0;
       }
      else
        return 3;
     }
   }
  return 4;
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
  SETCOMM ( "Init rel addr=0" );
  ZKARD ();                                       /* store Assembler card*/
						  /*                   */

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "BALR", 4 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND,
				  "@RBASE,0", 8 );
  ASS_CARD._BUFCARD.OPERAND[8] = ' ';
  SETCOMM ( "Set @RBASE=cur PC" );
  ZKARD ();

  memcpy ( ASS_CARD._BUFCARD.OPERAC, "USING", 5 );
  memcpy ( ASS_CARD._BUFCARD.OPERAND,
				   "*,@RBASE", 8 );
  ASS_CARD._BUFCARD.OPERAND[8] = ' ';
  SETCOMM ( "Use @RBASE for addr" );
  ZKARD ();

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

int LID1 ()
 {
  return 0;
 }

int LID2 ()
 {
  return 0;
 }

int ZNL1 ()
 {
  return 0;
 }

int ZNL2 ()
 {
  return 0;
 }

/*..........................................................................*/

static int sem_nop1 ( void ) { return 0; }
static int sem_nop2 ( void ) { return 0; }

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

  int (* FUN [NVXOD][2]) ( void ) =               /*with following fields:  */
   {                                              /*- DST.DST1 - op. code;*/
     {    AVI1, AVI2   },
     {    BUK1, BUK2   },
     {    CIF1, CIF2   },
     {    IDE1, IDE2   },
     {    IPE1, IPE2   },
     {    IPR1, IPR2   },
     {    LIT1, LIT2   },
     {    MAN1, MAN2   },
     {    ODC1, ODC2   },
     {    OEN1, OEN2   },
     {    OPA1, OPA2   },
     {    OPR1, OPR2   },
     {    PRO1, PRO2   },
     {    RZR1, RZR2   },
     {    TEL1, TEL2   },
     {    ZNK1, ZNK2   },
     {    LID1, LID2   },
     {    ZNL1, ZNL2   },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 },
     { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }, { sem_nop1, sem_nop2 }
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
