#!/bin/env python

import os
import sys
import string
from xml.dom import minidom

#
# opgen.py -- generates tables and constants for decoding
#
# - itab.c
# - itab.h
#

#
# special mnemonic types for internal purposes.
#
spl_mnm_types = [   'd3vil',      \
                    'na',         \
                    'grp_reg',    \
                    'grp_rm',     \
                    'grp_vendor', \
                    'grp_x87',    \
                    'grp_mode',   \
                    'grp_osize',  \
                    'grp_asize',  \
                    'grp_mod',    \
                    'none'        \
                ]

#
# opcode-vendor dictionary
#                
vend_dict = { 
    'AMD'       : '00', 
    'INTEL'     : '01' 
}


#
# opcode-mode dictionary
#

mode_dict   = { 
    '16'        : '00', 
    '32'        : '01', 
    '64'        : '02' 
}

#
# opcode-operand dictionary
#
operand_dict = {
    "Ap"       : [    "OP_A"        , "SZ_P"     ],
    "E"        : [    "OP_E"        , "SZ_NA"    ],
    "Eb"       : [    "OP_E"        , "SZ_B"     ],
    "Ew"       : [    "OP_E"        , "SZ_W"     ],
    "Ev"       : [    "OP_E"        , "SZ_V"     ],
    "Ed"       : [    "OP_E"        , "SZ_D"     ],
    "Ez"       : [    "OP_E"        , "SZ_Z"     ],
    "Ex"       : [    "OP_E"        , "SZ_MDQ"   ],
    "Ep"       : [    "OP_E"        , "SZ_P"     ],
    "G"        : [    "OP_G"        , "SZ_NA"    ],
    "Gb"       : [    "OP_G"        , "SZ_B"     ],
    "Gw"       : [    "OP_G"        , "SZ_W"     ],
    "Gv"       : [    "OP_G"        , "SZ_V"     ],
    "Gvw"      : [    "OP_G"        , "SZ_MDQ"   ],
    "Gd"       : [    "OP_G"        , "SZ_D"     ],
    "Gx"       : [    "OP_G"        , "SZ_MDQ"   ],
    "Gz"       : [    "OP_G"        , "SZ_Z"     ],
    "M"        : [    "OP_M"        , "SZ_NA"    ],
    "Mb"       : [    "OP_M"        , "SZ_B"     ],
    "Mw"       : [    "OP_M"        , "SZ_W"     ],
    "Ms"       : [    "OP_M"        , "SZ_W"     ],
    "Md"       : [    "OP_M"        , "SZ_D"     ],
    "Mq"       : [    "OP_M"        , "SZ_Q"     ],
    "Mt"       : [    "OP_M"        , "SZ_T"     ],
    "I1"       : [    "OP_I1"       , "SZ_NA"    ],
    "I3"       : [    "OP_I3"       , "SZ_NA"    ],
    "Ib"       : [    "OP_I"        , "SZ_B"     ],
    "Isb"      : [    "OP_I"        , "SZ_SB"    ],
    "Iw"       : [    "OP_I"        , "SZ_W"     ],
    "Iv"       : [    "OP_I"        , "SZ_V"     ],
    "Iz"       : [    "OP_I"        , "SZ_Z"     ],
    "Jv"       : [    "OP_J"        , "SZ_V"     ],
    "Jz"       : [    "OP_J"        , "SZ_Z"     ],
    "Jb"       : [    "OP_J"        , "SZ_B"     ],
    "R"        : [    "OP_R"        , "SZ_RDQ"   ], 
    "C"        : [    "OP_C"        , "SZ_NA"    ],
    "D"        : [    "OP_D"        , "SZ_NA"    ],
    "S"        : [    "OP_S"        , "SZ_NA"    ],
    "Ob"       : [    "OP_O"        , "SZ_B"     ],
    "Ow"       : [    "OP_O"        , "SZ_W"     ],
    "Ov"       : [    "OP_O"        , "SZ_V"     ],
    "V"        : [    "OP_V"        , "SZ_NA"    ],
    "W"        : [    "OP_W"        , "SZ_NA"    ],
    "P"        : [    "OP_P"        , "SZ_NA"    ],
    "Q"        : [    "OP_Q"        , "SZ_NA"    ],
    "VR"       : [    "OP_VR"       , "SZ_NA"    ],
    "PR"       : [    "OP_PR"       , "SZ_NA"    ],
    "AL"       : [    "OP_AL"       , "SZ_NA"    ],
    "CL"       : [    "OP_CL"       , "SZ_NA"    ],
    "DL"       : [    "OP_DL"       , "SZ_NA"    ],
    "BL"       : [    "OP_BL"       , "SZ_NA"    ],
    "AH"       : [    "OP_AH"       , "SZ_NA"    ],
    "CH"       : [    "OP_CH"       , "SZ_NA"    ],
    "DH"       : [    "OP_DH"       , "SZ_NA"    ],
    "BH"       : [    "OP_BH"       , "SZ_NA"    ],
    "AX"       : [    "OP_AX"       , "SZ_NA"    ],
    "CX"       : [    "OP_CX"       , "SZ_NA"    ],
    "DX"       : [    "OP_DX"       , "SZ_NA"    ],
    "BX"       : [    "OP_BX"       , "SZ_NA"    ],
    "SI"       : [    "OP_SI"       , "SZ_NA"    ],
    "DI"       : [    "OP_DI"       , "SZ_NA"    ],
    "SP"       : [    "OP_SP"       , "SZ_NA"    ],
    "BP"       : [    "OP_BP"       , "SZ_NA"    ],
    "eAX"      : [    "OP_eAX"      , "SZ_NA"    ],
    "eCX"      : [    "OP_eCX"      , "SZ_NA"    ],
    "eDX"      : [    "OP_eDX"      , "SZ_NA"    ],
    "eBX"      : [    "OP_eBX"      , "SZ_NA"    ],
    "eSI"      : [    "OP_eSI"      , "SZ_NA"    ],
    "eDI"      : [    "OP_eDI"      , "SZ_NA"    ],
    "eSP"      : [    "OP_eSP"      , "SZ_NA"    ],
    "eBP"      : [    "OP_eBP"      , "SZ_NA"    ],
    "rAX"      : [    "OP_rAX"      , "SZ_NA"    ],
    "rCX"      : [    "OP_rCX"      , "SZ_NA"    ],
    "rBX"      : [    "OP_rBX"      , "SZ_NA"    ],
    "rDX"      : [    "OP_rDX"      , "SZ_NA"    ],
    "rSI"      : [    "OP_rSI"      , "SZ_NA"    ],
    "rDI"      : [    "OP_rDI"      , "SZ_NA"    ],
    "rSP"      : [    "OP_rSP"      , "SZ_NA"    ],
    "rBP"      : [    "OP_rBP"      , "SZ_NA"    ],
    "ES"       : [    "OP_ES"       , "SZ_NA"    ],
    "CS"       : [    "OP_CS"       , "SZ_NA"    ],
    "DS"       : [    "OP_DS"       , "SZ_NA"    ],
    "SS"       : [    "OP_SS"       , "SZ_NA"    ],
    "GS"       : [    "OP_GS"       , "SZ_NA"    ],
    "FS"       : [    "OP_FS"       , "SZ_NA"    ],
    "ST0"      : [    "OP_ST0"      , "SZ_NA"    ],
    "ST1"      : [    "OP_ST1"      , "SZ_NA"    ],
    "ST2"      : [    "OP_ST2"      , "SZ_NA"    ],
    "ST3"      : [    "OP_ST3"      , "SZ_NA"    ],
    "ST4"      : [    "OP_ST4"      , "SZ_NA"    ],
    "ST5"      : [    "OP_ST5"      , "SZ_NA"    ],
    "ST6"      : [    "OP_ST6"      , "SZ_NA"    ],
    "ST7"      : [    "OP_ST7"      , "SZ_NA"    ],
    "NONE"     : [    "OP_NONE"     , "SZ_NA"    ],
    "ALr8b"    : [    "OP_ALr8b"    , "SZ_NA"    ],
    "CLr9b"    : [    "OP_CLr9b"    , "SZ_NA"    ],
    "DLr10b"   : [    "OP_DLr10b"   , "SZ_NA"    ],
    "BLr11b"   : [    "OP_BLr11b"   , "SZ_NA"    ],
    "AHr12b"   : [    "OP_AHr12b"   , "SZ_NA"    ],
    "CHr13b"   : [    "OP_CHr13b"   , "SZ_NA"    ],
    "DHr14b"   : [    "OP_DHr14b"   , "SZ_NA"    ],
    "BHr15b"   : [    "OP_BHr15b"   , "SZ_NA"    ],
    "rAXr8"    : [    "OP_rAXr8"    , "SZ_NA"    ],
    "rCXr9"    : [    "OP_rCXr9"    , "SZ_NA"    ],
    "rDXr10"   : [    "OP_rDXr10"   , "SZ_NA"    ],
    "rBXr11"   : [    "OP_rBXr11"   , "SZ_NA"    ],
    "rSPr12"   : [    "OP_rSPr12"   , "SZ_NA"    ],
    "rBPr13"   : [    "OP_rBPr13"   , "SZ_NA"    ],
    "rSIr14"   : [    "OP_rSIr14"   , "SZ_NA"    ],
    "rDIr15"   : [    "OP_rDIr15"   , "SZ_NA"    ],
    "jWP"      : [    "OP_J"        , "SZ_WP"    ],
    "jDP"      : [    "OP_J"        , "SZ_DP"    ],

}

#
# opcode prefix dictionary
# 
pfx_dict = { 
    "aso"      : "P_aso",   
    "oso"      : "P_oso",   
    "rexw"     : "P_rexw", 
    "rexb"     : "P_rexb",  
    "rexx"     : "P_rexx",  
    "rexr"     : "P_rexr",
    "inv64"    : "P_inv64", 
    "def64"    : "P_def64", 
    "depM"     : "P_depM",
    "cast1"    : "P_c1",    
    "cast2"    : "P_c2",    
    "cast3"    : "P_c3"   
}


#
# globals
#
opr_constants = []
siz_constants = []
tables        = {}
table_sizes   = {}
mnm_list      = []
default_opr   = 'O_NONE, O_NONE, O_NONE'


#
# collect the operand/size constants
# 
for o in operand_dict.keys():
    if not (operand_dict[o][0] in opr_constants):
        opr_constants.append(operand_dict[o][0])
    if not (operand_dict[o][1] in siz_constants):
        siz_constants.append(operand_dict[o][1])

xmlDoc = minidom.parse( "../docs/x86optable.xml" )
tlNode = xmlDoc.firstChild

#
# look for top-level optable node
#
while tlNode and tlNode.localName != "x86optable": tlNode = tlNode.nextSibling

#
# creates a table entry
#
def centry(i, defmap):
    if defmap["type"][0:3] == "grp":
        opr    = default_opr
        mnm    = 'UD_I' + defmap["type"].lower()
        pfx    = defmap["name"].upper()
    elif defmap["type"] == "leaf":
        mnm    = "UD_I" + defmap["name"]
        opr    = defmap["opr"]
        pfx    = defmap["pfx"]
        if len(mnm) == 0: mnm = "UD_Ina"
        if len(opr) == 0: opr = default_opr
        if len(pfx) == 0: pfx = "P_none"
    else:
        opr    = default_opr
        pfx    = "P_none"
        mnm    = "UD_Iinvalid"

    return "  /* %s */  { %-16s %-26s %s },\n" % (i, mnm + ',', opr + ',', pfx)

#
# makes a new table and adds it to the global
# list of tables
#
def mktab(name, size):
    if not (name in tables.keys()):
        tables[name] = {}
        table_sizes[name] = size

for node in tlNode.childNodes:

    opcodes = []
    iclass  = ''
    vendor  = ''

    # we are only interested in <instruction>
    if node.localName != 'instruction':
        continue

     # we need the mnemonic attribute
    if not ('mnemonic' in node.attributes.keys()):
        print "error: no mnemonic given in <instruction>."
        sys.exit(-1) 

    # check if this instruction was already defined.
    # else add it to the global list of mnemonics
    mnemonic = node.attributes['mnemonic'].value
    if mnemonic in mnm_list:
        print "error: multiple declarations of mnemonic='%s'" % mnemonic;
        sys.exit(-1)
    else:
        mnm_list.append(mnemonic)

    #
    # collect instruction 
    #   - vendor
    #   - class
    #
    for n in node.childNodes:
        if n.localName == 'vendor':
            vendor = (n.firstChild.data).strip();
        elif n.localName == 'class':
            iclass = n.firstChild.data;

    #
    # for each opcode definition
    #
    for n in node.childNodes:
        if n.localName != 'opcode':
            continue;
        
        opcode = n.firstChild.data.strip();
        parts  = opcode.split(";"); 
        flags  = []
        opr    = []
        pfx    = []
        opr    = []
        pfx_c  = []

        # get cast attribute, if given
        if 'cast' in n.attributes.keys():
            pfx_c.append( "P_c" + n.attributes['cast'].value )

	# get implicit addressing attribute, if given
	if 'imp_addr' in n.attributes.keys():
            if int( n.attributes['imp_addr'].value ):
                pfx_c.append( "P_ImpAddr" )

        # get mode attribute, if given
        if 'mode' in n.attributes.keys():
            v = (n.attributes['mode'].value).strip()
            modef = v.split();
            for m in modef:
                if not (m in pfx_dict):
                    print "warning: unrecognized mode attribute '%s'" % m
                else:
                     pfx_c.append(pfx_dict[m])

        #
        # split opcode definition into
        #   1. prefixes (pfx)
        #   2. opcode bytes (opc)
        #   3. operands
        #
        if len(parts) == 1:
            opc = parts[0].split()
        elif len(parts) == 2:
            opc = parts[0].split()
            opr = parts[1].split()
            for o in opc:
                if o in pfx_dict:
                    pfx = parts[0].split()
                    opc = parts[1].split()
                    break
        elif len(parts) == 3:
            pfx = parts[0].split()
            opc = parts[1].split()
            opr = parts[2].split()
        else:
            print "error: invalid opcode definition of %s\n" % mnemonic
            sys.exit(-1)
        # Convert opcodes to upper case
        for i in range(len(opc)):
            opc[i] = opc[i].upper()

        #
        # check for special cases of instruction translation
        # and ignore them
        # 
        if mnemonic == 'pause' or \
           ( mnemonic == 'nop' and opc[0] == '90' ) or \
           mnemonic == 'invalid' or \
            mnemonic == 'db' :
            continue

        #
        # Convert prefix
        #
        for p in pfx:
            if not ( p in pfx_dict.keys() ):
                print "error: invalid prefix specification: %s \n" % pfx
            pfx_c.append( pfx_dict[p] )
        if len(pfx) == 0:
            pfx_c.append( "P_none" )
        pfx = "|".join( pfx_c )

        #
        # Convert operands
        #
        opr_c = [ "O_NONE", "O_NONE", "O_NONE" ]
        for i in range(len(opr)): 
            if not (opr[i] in operand_dict.keys()):
                print "error: invalid operand declaration: %s\n" % opr[i]
            opr_c[i] = "O_" + opr[i]
        opr = "%-8s %-8s %s" % (opr_c[0] + ",", opr_c[1] + ",", opr_c[2])

        table_sse   = ''
        table_name  = 'itab__1byte'
        table_size  = 256
        table_index = ''

        for op in opc:
            if op[0:3] == 'SSE':
                table_sse = op
            elif op == '0F' and len(table_sse): 
                table_name = "itab__pfx_%s__0f" % table_sse
                table_size = 256
                table_sse  = ''
            elif op == '0F':
                table_name = "itab__0f"
                table_size = 256
            elif op[0:5] == '/X87=':
                tables[table_name][table_index] = { \
                    'type' : 'grp_x87',  \
                    'name' : "%s__op_%s__x87" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(op[5:7], 16)
                table_size = 64
            elif op[0:4] == '/RM=':
                tables[table_name][table_index] = { \
                    'type' : 'grp_rm',  \
                    'name' : "%s__op_%s__rm" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(op[4:6])
                table_size  = 8
            elif op[0:5] == '/MOD=':
                tables[table_name][table_index] = { \
                    'type' : 'grp_mod',  \
                    'name' : "%s__op_%s__mod" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']
                if len(op) == 8:
                    v = op[5:8]
                else:
                    v = op[5:7]
                mod_dict    = { '!11' : 0, '11' : 1 }
                table_index = "%02X" % int(mod_dict[v])
                table_size  = 2
            elif op[0:2] == '/O':
                tables[table_name][table_index] = { \
                    'type' : 'grp_osize',  \
                    'name' : "%s__op_%s__osize" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(mode_dict[op[2:4]])
                table_size  = 3
            elif op[0:2] == '/A':
                tables[table_name][table_index] = { \
                    'type' : 'grp_asize',  \
                    'name' : "%s__op_%s__asize" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(mode_dict[op[2:4]])
                table_size  = 3
            elif op[0:2] == '/M':
                tables[table_name][table_index] = { \
                    'type' : 'grp_mode',  \
                    'name' : "%s__op_%s__mode" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(mode_dict[op[2:4]])
                table_size  = 3
            elif op[0:6] == '/3DNOW':
                table_name  = "itab__3dnow"
                table_size  = 256
            elif op[0:1] == '/':
                tables[table_name][table_index] = { \
                    'type' : 'grp_reg',  \
                    'name' : "%s__op_%s__reg" % (table_name, table_index) \
                }                  
                table_name  = tables[table_name][table_index]['name']    
                table_index = "%02X" % int(op[1:2])
                table_size  = 8
            else:
                table_index = op

            mktab(table_name, table_size)

        if len(vendor):
            tables[table_name][table_index] = { \
                'type' : 'grp_vendor',  \
                'name' : "%s__op_%s__vendor" % (table_name, table_index) \
            }                  
            table_name  = tables[table_name][table_index]['name']    
            table_index = vend_dict[vendor]
            table_size = 2
            mktab(table_name, table_size)

        tables[table_name][table_index] = { \
            'type'  : 'leaf',   \
            'name'  : mnemonic, \
            'pfx'   : pfx,      \
            'opr'   : opr,      \
            'flags' : flags     \
        }

# ---------------------------------------------------------------------
# Generate itab.h
# ---------------------------------------------------------------------

f = open("itab.h", "w")

f.write('''
/* itab.h -- auto generated by opgen.py, do not edit. */

#ifndef UD_ITAB_H
#define UD_ITAB_H

''')

#
# Generate enumeration of size constants
#
siz_constants.sort()
f.write('''
''')

f.write("\nenum ud_itab_vendor_index {\n" )
f.write("  ITAB__VENDOR_INDX__AMD,\n" )
f.write("  ITAB__VENDOR_INDX__INTEL,\n" )
f.write("};\n\n")


f.write("\nenum ud_itab_mode_index {\n" )
f.write("  ITAB__MODE_INDX__16,\n" )
f.write("  ITAB__MODE_INDX__32,\n" )
f.write("  ITAB__MODE_INDX__64\n" )
f.write("};\n\n")


f.write("\nenum ud_itab_mod_index {\n" )
f.write("  ITAB__MOD_INDX__NOT_11,\n" )
f.write("  ITAB__MOD_INDX__11\n" )
f.write("};\n\n")

#
# Generate enumeration of the tables
#
table_names = tables.keys()
table_names.sort();

f.write( "\nenum ud_itab_index {\n" )
for name in table_names:
    f.write("  %s,\n" % name.upper() );
f.write( "};\n\n" ) 

#
# Generate mnemonics list
#
f.write("\nenum ud_mnemonic_code {\n")
for m in mnm_list:
    f.write("  UD_I%s,\n" % m)
for m in spl_mnm_types:
    f.write("  UD_I%s,\n" % m)
f.write("};\n\n")

#
# Generate struct defs
#
f.write( \
'''

extern const char* ud_mnemonics_str[];;
extern struct ud_itab_entry* ud_itab_list[];

''' )


f.write("#endif\n")

f.close()

# ---------------------------------------------------------------------
# Generate itab.c
# ---------------------------------------------------------------------

f = open("itab.c", "w")

f.write('''
/* itab.c -- auto generated by opgen.py, do not edit. */

#include "types.h"
#include "decode.h"
#include "itab.h"

''')

#
# generate mnemonic list
#
f.write("const char * ud_mnemonics_str[] = {\n")
for m in mnm_list:
    f.write("  \"%s\",\n" % m )
f.write("};\n\n")

#
# generate instruction tables
#

f.write("\n")
for t in table_names:
    f.write("\nstatic struct ud_itab_entry " + t.lower() + "[%d] = {\n" % table_sizes[t]);
    for i in range(int(table_sizes[t])):
        index = "%02X" % i
        if index in tables[t]:
            f.write(centry(index, tables[t][index]))
        else:
            f.write(centry(index,{"type":"invalid"}))
    f.write("};\n");

#
# write the instruction table list
#
f.write( "\n/* the order of this table matches enum ud_itab_index */")
f.write( "\nstruct ud_itab_entry * ud_itab_list[] = {\n" )
for name in table_names:
    f.write( "  %s,\n" % name.lower() )
f.write( "};\n" );

f.close();

# vim:expandtab
# vim:sw=4
# vim:ts=4
