#!/usr/bin/env python3
"""Apply structural patches to komppl.c for lab-step-2."""
import re

path = "src/komppl.c"
text = open(path, encoding="utf-8").read()

text = text.replace("#define NSINT     232", "#define NSINT     280")
text = text.replace("#define NVXOD      53", "#define NVXOD      55")
text = text.replace("#define NNETRM     16", "#define NNETRM     19")

# VXOD: insert LID, LVI, ZNL after ZNK
vxod_old = """  {/*.  16     .*/   "ZNK" ,   0 , 'N' },
  {/*.  17     .*/   "A  " ,  33 , 'T' },"""
vxod_new = """  {/*.  16     .*/   "ZNK" ,   0 , 'N' },
  {/*.  17     .*/   "LID" , 262 , 'N' },
  {/*.  18     .*/   "LVI" , 232 , 'N' },
  {/*.  19     .*/   "ZNL" , 226 , 'N' },
  {/*.  20     .*/   "A  " ,  33 , 'T' },"""
if vxod_old not in text:
    raise SystemExit("VXOD anchor not found")
text = text.replace(vxod_old, vxod_new)

# Renumber VXOD comments /*.  18 .. 52 */ -> +3 for terminals (20-55) — optional; skip for brevity

# Replace TPR block
tpr_adj = open("/tmp/tpr_adj.txt", encoding="utf-8").read()
old_tpr = re.search(
    r"char TPR \[ NVXOD \] \[ NNETRM \] =\s*\{[\s\S]*?\n  \};",
    text,
)
if not old_tpr:
    raise SystemExit("TPR block not found")
new_tpr = """char TPR [ NVXOD ] [ NNETRM ] =
 {
/*
   __________ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___ ___
  |       AVI:BUK:CIF:IDE:IPE:IPR:LIT:MAN:ODC:OEN:OPA:OPR:PRO:RZR:TEL:ZNK:LID:LVI:ZNL|
  |__________:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___:___| */
""" + tpr_adj + """
/*|_______________________________________________________________________| */
  };"""
text = text[: old_tpr.start()] + new_tpr + text[old_tpr.end() :]

# IPE = LVI
text = text.replace(
    '{/*.  155     .*/   156 ,   154 , "AVI" ,    0 },',
    '{/*.  155     .*/   156 ,   154 , "LVI" ,    0 },',
)

# = as ZNL
text = text.replace(
    '{/*.  227     .*/   228 ,   226 , "ZNK" ,    0 },',
    '{/*.  227     .*/   228 ,   226 , "ZNL" ,    0 },',
)

# DEC INIT: LIT -> LID
text = text.replace(
    '{/*.  221     .*/   222 ,   220 , "LIT" ,    0 },',
    '{/*.  221     .*/   222 ,   220 , "LID" ,    0 },',
)

# After FIXED (58 D): '(' alt ';' and INIT
text = text.replace(
    '{/*.   59     .*/    60 ,    58 , "(  " ,    0 },',
    '{/*.   59     .*/    60 ,    58 , "(  " ,  243 },',
)

# Insert new SINT rows before closing }; of SINT (before line 421)
sint_extra = r'''
 {/*.  232     .*/   233 ,     0 , "LVI" ,    0 },
 {/*.  233     .*/   234 ,   232 , "AVI" ,    0 },
 {/*.  234     .*/   235 ,   233 , "ZNL" ,  237 },
 {/*.  235     .*/   236 ,   234 , "AVI" ,    0 },
 {/*.  236     .*/     0 ,   235 , "*  " ,    0 },
 {/*.  237     .*/     0 ,   234 , "*  " ,    0 },
 /* BIN FIXED without (): ; or INIT after D */
 {/*.  243     .*/   244 ,    58 , ";  " ,  246 },
 {/*.  244     .*/   245 ,   243 , "ODC" ,    0 },
 {/*.  245     .*/     0 ,   244 , "*  " ,    0 },
 {/*.  246     .*/   247 ,    58 , "I  " ,    0 },
 {/*.  247     .*/   248 ,   246 , "N  " ,    0 },
 {/*.  248     .*/   249 ,   247 , "I  " ,    0 },
 {/*.  249     .*/   250 ,   248 , "T  " ,    0 },
 {/*.  250     .*/   251 ,   249 , "(  " ,    0 },
 {/*.  251     .*/   252 ,   250 , "LIT" ,  268 },
 {/*.  252     .*/   253 ,   251 , ")  " ,    0 },
 {/*.  253     .*/   254 ,   252 , ";  " ,    0 },
 {/*.  254     .*/   255 ,   253 , "ODC" ,    0 },
 {/*.  255     .*/     0 ,   254 , "*  " ,    0 },
 {/*.  268     .*/   252 ,   250 , "LID" ,    0 },
 /* LID ::= RZR (same shape as RZR digits) */
 {/*.  262     .*/   263 ,     0 , "LID" ,    0 },
 {/*.  263     .*/   264 ,   262 , "RZR" ,    0 },
 {/*.  264     .*/   265 ,   263 , "CIF" ,    0 },
 {/*.  265     .*/   266 ,   264 , "RZR" ,    0 },
 {/*.  266     .*/     0 ,   265 , "*  " ,    0 },
'''

text = text.replace(
    "{/*.  231     .*/     0 ,   230 , \"*  \" ,    0 }\n};",
    "{/*.  231     .*/     0 ,   230 , \"*  \" ,    0 }" + sint_extra + "\n};",
)

open(path, "w", encoding="utf-8").write(text)
print("patched", path)
