############################################################
# This file is imported from a different project.
# DO NOT make modifications in this repo.
# File a patch instead and assign it to Ryan Davis
############################################################

# -*- racc -*-

class RubyParser

token kCLASS kMODULE kDEF kUNDEF kBEGIN kRESCUE kENSURE kEND kIF kUNLESS
      kTHEN kELSIF kELSE kCASE kWHEN kWHILE kUNTIL kFOR kBREAK kNEXT
      kREDO kRETRY kIN kDO kDO_COND kDO_BLOCK kRETURN kYIELD kSUPER
      kSELF kNIL kTRUE kFALSE kAND kOR kNOT kIF_MOD kUNLESS_MOD kWHILE_MOD
      kUNTIL_MOD kRESCUE_MOD kALIAS kDEFINED klBEGIN klEND k__LINE__
      k__FILE__ tIDENTIFIER tFID tGVAR tIVAR tCONSTANT tCVAR tNTH_REF
      tBACK_REF tSTRING_CONTENT tINTEGER tFLOAT tREGEXP_END tUPLUS
      tUMINUS tUMINUS_NUM tPOW tCMP tEQ tEQQ tNEQ tGEQ tLEQ tANDOP
      tOROP tMATCH tNMATCH tDOT tDOT2 tDOT3 tAREF tASET tLSHFT tRSHFT
      tCOLON2 tCOLON3 tOP_ASGN tASSOC tLPAREN tLPAREN2 tRPAREN tLPAREN_ARG
      tLBRACK tRBRACK tLBRACE tLBRACE_ARG tSTAR tSTAR2 tAMPER tAMPER2
      tTILDE tPERCENT tDIVIDE tPLUS tMINUS tLT tGT tPIPE tBANG tCARET
      tLCURLY tRCURLY tBACK_REF2 tSYMBEG tSTRING_BEG tXSTRING_BEG tREGEXP_BEG
      tWORDS_BEG tAWORDS_BEG tSTRING_DBEG tSTRING_DVAR tSTRING_END tSTRING
      tSYMBOL tNL tEH tCOLON tCOMMA tSPACE tSEMI tLAST_TOKEN

prechigh
  right    tBANG tTILDE tUPLUS
  right    tPOW
  right    tUMINUS_NUM tUMINUS
  left     tSTAR2 tDIVIDE tPERCENT
  left     tPLUS tMINUS
  left     tLSHFT tRSHFT
  left     tAMPER2
  left     tPIPE tCARET
  left     tGT tGEQ tLT tLEQ
  nonassoc tCMP tEQ tEQQ tNEQ tMATCH tNMATCH
  left     tANDOP
  left     tOROP
  nonassoc tDOT2 tDOT3
  right    tEH tCOLON
  left     kRESCUE_MOD
  right    tEQL tOP_ASGN
  nonassoc kDEFINED
  right    kNOT
  left     kOR kAND
  nonassoc kIF_MOD kUNLESS_MOD kWHILE_MOD kUNTIL_MOD
  nonassoc tLBRACE_ARG
  nonassoc tLOWEST
preclow

rule

         program:   {
                      self.lexer.lex_state = :expr_beg
                    }
                    compstmt
                    {
                      result = val[1]
                    }

        bodystmt: compstmt opt_rescue opt_else opt_ensure
                    {
                      result = new_body val
                    }

        compstmt: stmts opt_terms
                    {
                      result = new_compstmt val
                    }

           stmts: none
                | stmt
                | stmts terms stmt
                    {
                      result = self.block_append val[0], val[2]
                    }
                | error stmt
                    {
                      result = val[1]
                    }

            stmt: kALIAS fitem
                    {
                      lexer.lex_state = :expr_fname
                      result = self.lexer.lineno
                    }
                    fitem
                    {
                      result = s(:alias, val[1], val[3]).line(val[2])
                    }
                | kALIAS tGVAR tGVAR
                    {
                      result = s(:valias, val[1].to_sym, val[2].to_sym)
                    }
                | kALIAS tGVAR tBACK_REF
                    {
                      result = s(:valias, val[1].to_sym, :"$#{val[2]}")
                    }
                | kALIAS tGVAR tNTH_REF
                    {
                      yyerror "can't make alias for the number variables"
                    }
                | kUNDEF undef_list
                    {
                      result = val[1]
                    }
                | stmt kIF_MOD expr_value
                    {
                      result = new_if val[2], val[0], nil
                    }
                | stmt kUNLESS_MOD expr_value
                    {
                      result = new_if val[2], nil, val[0]
                    }
                | stmt kWHILE_MOD expr_value
                    {
                      result = new_while val[0], val[2], true
                    }
                | stmt kUNTIL_MOD expr_value
                    {
                      result = new_until val[0], val[2], true
                    }
                | stmt kRESCUE_MOD stmt
                    {
                      result = s(:rescue, val[0], s(:resbody, s(:array), val[2]))
                    }
                | klBEGIN
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "BEGIN in method"
                      end
                      self.env.extend
                    }
                    tLCURLY compstmt tRCURLY
                    {
                      result = new_iter s(:preexe), nil, val[3] # TODO: add test?
                      result = nil # TODO: since it isn't supposed to go in the AST
                    }
                | klEND tLCURLY compstmt tRCURLY
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "END in method; use at_exit"
                      end
                      result = new_iter s(:postexe), nil, val[2]
                    }
                | lhs tEQL command_call
                    {
                      result = self.node_assign val[0], val[2]
                    }
                | mlhs tEQL command_call
                    {
                      result = new_masgn val[0], val[2], :wrap
                    }
                | var_lhs tOP_ASGN command_call
                    {
                      result = new_op_asgn val
                    }
                | primary_value "[" aref_args tRBRACK tOP_ASGN command_call
                    {
                      result = s(:op_asgn1, val[0], val[2], val[4].to_sym, val[5])
                    }
                | primary_value tDOT tIDENTIFIER tOP_ASGN command_call
                    {
                      result = s(:op_asgn, val[0], val[4], val[2], val[3])
                    }
                | primary_value tDOT tCONSTANT tOP_ASGN command_call
                    {
                      result = s(:op_asgn, val[0], val[4], val[2], val[3])
                    }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN command_call
                    {
                      result = s(:op_asgn, val[0], val[4], val[2], val[3])
                    }
                | backref tOP_ASGN command_call
                    {
                      self.backref_assign_error val[0]
                    }
                | lhs tEQL mrhs
                    {
                      result = self.node_assign val[0], s(:svalue, val[2])
                    }
                | mlhs tEQL arg_value
                    {
                      result = new_masgn val[0], val[2], :wrap
                    }
                | mlhs tEQL mrhs
                    {
                      result = new_masgn val[0], val[2]
                    }
                | expr

            expr: command_call
                | expr kAND expr
                    {
                      result = logop(:and, val[0], val[2])
                    }
                | expr kOR expr
                    {
                      result = logop(:or, val[0], val[2])
                    }
                | kNOT expr
                    {
                      result = s(:not, val[1])
                    }
                | tBANG command_call
                    {
                      result = s(:not, val[1])
                    }
                | arg

      expr_value: expr
                    {
                      result = value_expr(val[0])
                    }

    command_call: command
                | block_command
                | kRETURN call_args
                    {
                      result = s(:return, ret_args(val[1]))
                    }
                | kBREAK call_args
                    {
                      result = s(:break, ret_args(val[1]))
                    }
                | kNEXT call_args
                    {
                      result = s(:next, ret_args(val[1]))
                    }

   block_command: block_call
                | block_call tDOT operation2 command_args
                    {
                      result = new_call val[0], val[2], val[3]
                    }
                | block_call tCOLON2 operation2 command_args
                    {
                      result = new_call val[0], val[2], val[3]
                    }

 cmd_brace_block: tLBRACE_ARG
                    {
                      self.env.extend(:dynamic)
                      result = self.lexer.lineno
                    }
                    opt_block_var
                    {
                      result = self.env.dynamic.keys
                    }
                    compstmt tRCURLY
                    {
                      result = new_iter nil, val[2], val[4]
                      self.env.unextend
                    }

         command: operation command_args =tLOWEST
                    {
                      result = new_call nil, val[0].to_sym, val[1]
                    }
                | operation command_args cmd_brace_block
                    {
                      result = new_call nil, val[0].to_sym, val[1]
                      if val[2] then
                        if result[0] == :block_pass then
                          raise "both block arg and actual block given"
                        end
                        result, operation = val[2], result
                        result.insert 1, operation
                      end
                    }
                | primary_value tDOT operation2 command_args =tLOWEST
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                    }
                | primary_value tDOT operation2 command_args cmd_brace_block
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                    }
                | primary_value tCOLON2 operation2 command_args =tLOWEST
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                    }
                | primary_value tCOLON2 operation2 command_args cmd_brace_block
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                      if val[4] then
                        if result[0] == :block_pass then # REFACTOR
                          raise "both block arg and actual block given"
                        end
                        val[2] << result
                        result = val[2]
                      end
                    }
                | kSUPER command_args
                    {
                      result = new_super val[1]
                    }
                | kYIELD command_args
                    {
                      result = new_yield val[1]
                    }

            mlhs: mlhs_basic
                | tLPAREN mlhs_entry tRPAREN
                    {
                      result = val[1]
                    }

      mlhs_entry: mlhs_basic
                | tLPAREN mlhs_entry tRPAREN
                    {
                      result = s(:masgn, s(:array, val[1]))
                    }

      mlhs_basic: mlhs_head
                    {
                      result = s(:masgn, val[0])
                    }
                | mlhs_head mlhs_item
                    {
                      result = s(:masgn, val[0] << val[1].compact)
                    }
                | mlhs_head tSTAR mlhs_node
                    {
                      result = s(:masgn, val[0] << s(:splat, val[2]))
                    }
                | mlhs_head tSTAR
                    {
                      result = s(:masgn, val[0] << s(:splat))
                    }
                | tSTAR mlhs_node
                    {
                      result = s(:masgn, s(:array, s(:splat, val[1])))
                    }
                | tSTAR
                    {
                      result = s(:masgn, s(:array, s(:splat)))
                    }

       mlhs_item: mlhs_node
                | tLPAREN mlhs_entry tRPAREN
                    {
                      result = val[1]
                    }

       mlhs_head: mlhs_item tCOMMA
                    {
                      result = s(:array, val[0])
                    }
                | mlhs_head mlhs_item tCOMMA
                    {
                      result = val[0] << val[1].compact
                    }

       mlhs_node: variable
                    {
                      result = self.assignable val[0]
                    }
                | primary_value "[" aref_args tRBRACK
                    {
                      result = self.aryset val[0], val[2]
                    }
                | primary_value tDOT tIDENTIFIER
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=", s(:arglist))
                    }
                | primary_value tCOLON2 tIDENTIFIER
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=", s(:arglist))
                    }
                | primary_value tDOT tCONSTANT
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=", s(:arglist))
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "dynamic constant assignment"
                      end

                      result = s(:const, s(:colon2, val[0], val[2].to_sym), nil)
                    }
                | tCOLON3 tCONSTANT
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "dynamic constant assignment"
                      end

                      result = s(:const, nil, s(:colon3, val[1].to_sym))
                    }
                | backref
                    {
                      self.backref_assign_error val[0]
                    }

             lhs: variable
                    {
                      result = self.assignable val[0]
                    }
                | primary_value "[" aref_args tRBRACK
                    {
                      result = self.aryset val[0], val[2]
                    }
                | primary_value tDOT tIDENTIFIER
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=")
                    }
                | primary_value tCOLON2 tIDENTIFIER
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=")
                    }
                | primary_value tDOT tCONSTANT
                    {
                      result = s(:attrasgn, val[0], :"#{val[2]}=")
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "dynamic constant assignment"
                      end

                      result = s(:const, s(:colon2, val[0], val[2].to_sym))
                    }
                | tCOLON3 tCONSTANT
                    {
                      if (self.in_def || self.in_single > 0) then
                        yyerror "dynamic constant assignment"
                      end

                      result = s(:const, s(:colon3, val[1].to_sym))
                    }
                | backref
                    {
                      self.backref_assign_error val[0]
                    }

           cname: tIDENTIFIER
                    {
                      yyerror "class/module name must be CONSTANT"
                    }
                | tCONSTANT

           cpath: tCOLON3 cname
                    {
                      result = s(:colon3, val[1].to_sym)
                    }
                | cname
                    {
                      result = val[0].to_sym
                    }
                | primary_value tCOLON2 cname
                    {
                      result = s(:colon2, val[0], val[2].to_sym)
                    }

           fname: tIDENTIFIER | tCONSTANT | tFID
                | op
                    {
                      lexer.lex_state = :expr_end
                      result = val[0]
                    }

                | reswords
                    {
                      lexer.lex_state = :expr_end
                      result = val[0]
                    }

                    # TODO: cruby has fsym and dsym
           fitem: fname  { result = s(:lit, val[0].to_sym) }
                | symbol { result = s(:lit, val[0]) }

      undef_list: fitem
                    {
                      result = new_undef val[0]
                    }
                |
                    undef_list tCOMMA
                    {
                      lexer.lex_state = :expr_fname
                    }
                    fitem
                    {
                      result = new_undef val[0], val[3]
                    }

              op: tPIPE    | tCARET     | tAMPER2 | tCMP   | tEQ     | tEQQ
                | tMATCH   | tGT        | tGEQ    | tLT    | tLEQ    | tLSHFT
                | tRSHFT   | tPLUS      | tMINUS  | tSTAR2 | tSTAR   | tDIVIDE
                | tPERCENT | tPOW       | tTILDE  | tUPLUS | tUMINUS | tAREF
                | tASET    | tBACK_REF2

        reswords: k__LINE__ | k__FILE__   | klBEGIN | klEND  | kALIAS  | kAND
                | kBEGIN    | kBREAK      | kCASE   | kCLASS | kDEF    | kDEFINED
                | kDO       | kELSE       | kELSIF  | kEND   | kENSURE | kFALSE
                | kFOR      | kIN         | kMODULE | kNEXT  | kNIL    | kNOT
                | kOR       | kREDO       | kRESCUE | kRETRY | kRETURN | kSELF
                | kSUPER    | kTHEN       | kTRUE   | kUNDEF | kWHEN   | kYIELD
                | kIF_MOD   | kUNLESS_MOD | kWHILE_MOD | kUNTIL_MOD | kRESCUE_MOD

             arg: lhs tEQL arg
                    {
                      result = self.node_assign val[0], val[2]
                    }
                | lhs tEQL arg kRESCUE_MOD arg
                    {
                      result = self.node_assign val[0], s(:rescue, val[2], s(:resbody, s(:array), val[4]))
                      # result.line = val[0].line
                    }
                | var_lhs tOP_ASGN arg
                    {
                      result = new_op_asgn val
                    }
                | primary_value "[" aref_args tRBRACK tOP_ASGN arg
                    {
                      result = s(:op_asgn1, val[0], val[2], val[4].to_sym, val[5])
                      val[2][0] = :arglist
                    }
                | primary_value tDOT tIDENTIFIER tOP_ASGN arg
                    {
                      result = s(:op_asgn2, val[0], :"#{val[2]}=", val[3].to_sym, val[4])
                    }
                | primary_value tDOT tCONSTANT tOP_ASGN arg
                    {
                      result = s(:op_asgn2, val[0], :"#{val[2]}=", val[3].to_sym, val[4])
                    }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN arg
                    {
                      result = s(:op_asgn, val[0], val[4], val[2], val[3])
                    }
                | primary_value tCOLON2 tCONSTANT tOP_ASGN arg

                    {
                      yyerror "constant re-assignment"
                    }
                | tCOLON3 tCONSTANT tOP_ASGN arg
                    {
                      yyerror "constant re-assignment"
                    }
                | backref tOP_ASGN arg
                    {
                      self.backref_assign_error val[0]
                    }
                | arg tDOT2 arg
                    {
                      v1, v2 = val[0], val[2]
                      if v1.node_type == :lit and v2.node_type == :lit and Fixnum === v1.last and Fixnum === v2.last then
                        result = s(:lit, (v1.last)..(v2.last))
                      else
                        result = s(:dot2, v1, v2)
                      end
                    }
                | arg tDOT3 arg
                    {
                      v1, v2 = val[0], val[2]
                      if v1.node_type == :lit and v2.node_type == :lit and Fixnum === v1.last and Fixnum === v2.last then
                        result = s(:lit, (v1.last)...(v2.last))
                      else
                        result = s(:dot3, v1, v2)
                      end
                    }
                | arg tPLUS arg
                    {
                      result = new_call val[0], :+, s(:arglist, val[2])
                    }
                | arg tMINUS arg
                    {
                      result = new_call val[0], :-, s(:arglist, val[2])
                    }
                | arg tSTAR2 arg
                    {
                      result = new_call val[0], :*, s(:arglist, val[2])
                    }
                | arg tDIVIDE arg
                    {
                      result = new_call val[0], :"/", s(:arglist, val[2])
                    }
                | arg tPERCENT arg
                    {
                      result = new_call val[0], :%, s(:arglist, val[2])
                    }
                | arg tPOW arg
                    {
                      result = new_call val[0], :**, s(:arglist, val[2])
                    }
                | tUMINUS_NUM tINTEGER tPOW arg
                    {
                      result = new_call(new_call(s(:lit, val[1]), :"**", s(:arglist, val[3])), :"-@", s(:arglist))
                    }
                | tUMINUS_NUM tFLOAT tPOW arg
                    {
                      result = new_call(new_call(s(:lit, val[1]), :"**", s(:arglist, val[3])), :"-@", s(:arglist))
                    }
                | tUPLUS arg
                    {
                      if val[1][0] == :lit then
                        result = val[1]
                      else
                        result = new_call val[1], :"+@", s(:arglist)
                      end
                    }
                | tUMINUS arg
                    {
                      result = new_call val[1], :"-@", s(:arglist)
                    }
                | arg tPIPE arg
                    {
                      result = new_call val[0], :"|", s(:arglist, val[2])
                    }
                | arg tCARET arg
                    {
                      result = new_call val[0], :"^", s(:arglist, val[2])
                    }
                | arg tAMPER2 arg
                    {
                      result = new_call val[0], :"&", s(:arglist, val[2])
                    }
                | arg tCMP arg
                    {
                      result = new_call val[0], :"<=>", s(:arglist, val[2])
                    }
                | arg tGT arg
                    {
                      result = new_call val[0], :">", s(:arglist, val[2])
                    }
                | arg tGEQ arg
                    {
                      result = new_call val[0], :">=", s(:arglist, val[2])
                    }
                | arg tLT arg
                    {
                      result = new_call val[0], :"<", s(:arglist, val[2])
                    }
                | arg tLEQ arg
                    {
                      result = new_call val[0], :"<=", s(:arglist, val[2])
                    }
                | arg tEQ arg
                    {
                      result = new_call val[0], :"==", s(:arglist, val[2])
                    }
                | arg tEQQ arg
                    {
                      result = new_call val[0], :"===", s(:arglist, val[2])
                    }
                | arg tNEQ arg
                    {
                      val[0] = value_expr val[0] # TODO: port call_op and clean these
                      val[2] = value_expr val[2]
                      result = s(:not, new_call(val[0], :"==", s(:arglist, val[2])))
                    }
                | arg tMATCH arg
                    {
                      result = self.get_match_node val[0], val[2]
                    }
                | arg tNMATCH arg
                    {
                      result = s(:not, self.get_match_node(val[0], val[2]))
                    }
                | tBANG arg
                    {
                      result = s(:not, val[1])
                    }
                | tTILDE arg
                    {
                      val[2] = value_expr val[2]
                      result = new_call val[1], :"~", s(:arglist)
                    }
                | arg tLSHFT arg
                    {
                      val[0] = value_expr val[0]
                      val[2] = value_expr val[2]
                      result = new_call val[0], :"\<\<", s(:arglist, val[2])
                    }
                | arg tRSHFT arg
                    {
                      val[0] = value_expr val[0]
                      val[2] = value_expr val[2]
                      result = new_call val[0], :">>", s(:arglist, val[2])
                    }
                | arg tANDOP arg
                    {
                      result = logop(:and, val[0], val[2])
                    }
                | arg tOROP arg
                    {
                      result = logop(:or, val[0], val[2])
                    }
                | kDEFINED opt_nl arg
                    {
                      result = s(:defined, val[2])
                    }
                | arg tEH arg tCOLON arg
                    {
                      result = s(:if, val[0], val[2], val[4])
                    }
                | primary

       arg_value: arg
                    {
                      result = value_expr(val[0])
                    }

       aref_args: none
                | command opt_nl
                    {
                      warning 'parenthesize argument(s) for future version'
                      result = s(:array, val[0])
                    }
                | args trailer
                    {
                      result = val[0]
                    }
                | args tCOMMA tSTAR arg opt_nl
                    {
                      result = self.arg_concat val[0], val[3]
                    }
                | assocs trailer
                    {
                      result = s(:array, s(:hash, *val[0].values))
                    }
                | tSTAR arg opt_nl
                    {
                      result = s(:array, s(:splat, val[1]))
                    }

      paren_args: tLPAREN2 none tRPAREN
                    {
                      result = val[1]
                    }
                | tLPAREN2 call_args opt_nl tRPAREN
                    {
                      result = val[1]
                    }
                | tLPAREN2 block_call opt_nl tRPAREN
                    {
                      warning "parenthesize argument(s) for future version"
                      result = s(:array, val[1])
                    }
                | tLPAREN2 args tCOMMA block_call opt_nl tRPAREN
                    {
                      warning "parenthesize argument(s) for future version"
                      result = val[1].add val[3]
                    }

  opt_paren_args: none
                | paren_args

       call_args: command
                    {
                      warning "parenthesize argument(s) for future version"
                      result = s(:array, val[0])
                    }
                | args opt_block_arg
                    {
                      result = self.arg_blk_pass val[0], val[1]
                    }
                | args tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat val[0], val[3]
                      result = self.arg_blk_pass result, val[4]
                    }
                | assocs opt_block_arg
                    {
                      result = s(:array, s(:hash, *val[0].values))
                      result = self.arg_blk_pass result, val[1]
                    }
                | assocs tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat s(:array, s(:hash, *val[0].values)), val[3]
                      result = self.arg_blk_pass result, val[4]
                    }
                | args tCOMMA assocs opt_block_arg
                    {
                      result = val[0] << s(:hash, *val[2].values)
                      result = self.arg_blk_pass result, val[3]
                    }
                | args tCOMMA assocs tCOMMA tSTAR arg opt_block_arg
                    {
                      val[0] << s(:hash, *val[2].values)
                      result = self.arg_concat val[0], val[5]
                      result = self.arg_blk_pass result, val[6]
                    }
                | tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_blk_pass s(:splat, val[1]), val[2]
                    }
                | block_arg

      call_args2: arg_value tCOMMA args opt_block_arg
                    {
                      args = self.list_prepend val[0], val[2]
                      result = self.arg_blk_pass args, val[3]
                    }
                | arg_value tCOMMA block_arg
                    {
                      result = self.arg_blk_pass val[0], val[2]
                    }
                | arg_value tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat s(:array, val[0]), val[3]
                      result = self.arg_blk_pass result, val[4]
                    }
                | arg_value tCOMMA args tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat s(:array, val[0], s(:hash, *val[2].values)), val[5]
                      result = self.arg_blk_pass result, val[6]
                    }
                | assocs opt_block_arg
                    {
                      result = s(:array, s(:hash, *val[0].values))
                      result = self.arg_blk_pass result, val[1]
                    }
                | assocs tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = s(:array, s(:hash, *val[0].values), val[3])
                      result = self.arg_blk_pass result, val[4]
                    }
                | arg_value tCOMMA assocs opt_block_arg
                    {
                      result = s(:array, val[0], s(:hash, *val[2].values))
                      result = self.arg_blk_pass result, val[3]
                    }
                | arg_value tCOMMA args tCOMMA assocs opt_block_arg
                    {
                      result = s(:array, val[0]).add_all(val[2]).add(s(:hash, *val[4].values))
                      result = self.arg_blk_pass result, val[5]
                    }
                | arg_value tCOMMA assocs tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat s(:array, val[0]).add(s(:hash, *val[2].values)), val[5]
                      result = self.arg_blk_pass result, val[6]
                    }
                | arg_value tCOMMA args tCOMMA assocs tCOMMA tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_concat s(:array, val[0]).add_all(val[2]).add(s(:hash, *val[4].values)), val[7]
                      result = self.arg_blk_pass result, val[8]
                    }
                | tSTAR arg_value opt_block_arg
                    {
                      result = self.arg_blk_pass s(:splat, val[1]), val[2]
                    }
                | block_arg

    command_args:   {
                      result = lexer.cmdarg.stack.dup
                      lexer.cmdarg.push true
                    }
                    open_args
                    {
                      lexer.cmdarg.stack.replace val[0]
                      result = val[1]
                    }

       open_args: call_args
                | tLPAREN_ARG
                    {
                      lexer.lex_state = :expr_endarg
                    }
                    tRPAREN
                    {
                      warning "don't put space before argument parentheses"
                      result = nil
                    }
                | tLPAREN_ARG call_args2
                    {
                      lexer.lex_state = :expr_endarg
                    }
                    tRPAREN
                    {
                      warning "don't put space before argument parentheses"
                      result = val[1]
                    }

       block_arg: tAMPER arg_value
                    {
                      result = s(:block_pass, val[1])
                    }

   opt_block_arg: tCOMMA block_arg
                    {
                      result = val[1]
                    }
                | none_block_pass

            args: arg_value
                    {
                      result = s(:array, val[0])
                    }
                | args tCOMMA arg_value
                    {
                      result = self.list_append val[0], val[2]
                    }

            mrhs: args tCOMMA arg_value
                    {
                      result = val[0] << val[2]
                    }
                | args tCOMMA tSTAR arg_value
                    {
                      result = self.arg_concat val[0], val[3]
                    }
                | tSTAR arg_value
                    {
                      result = s(:splat, val[1])
                    }

         primary: literal
                | strings
                | xstring
                | regexp
                | words
                | awords
                | var_ref
                | backref
                | tFID
                    {
                      result = new_call nil, val[0].to_sym
                    }
                | kBEGIN
                    {
                      result = self.lexer.lineno
                    }
                    bodystmt kEND
                    {
                      unless val[2] then
                        result = s(:nil)
                      else
                        result = s(:begin, val[2])
                      end

                      result.line = val[1]
                    }
                | tLPAREN_ARG expr
                    {
                      lexer.lex_state = :expr_endarg
                    }
                    opt_nl tRPAREN
                    {
                      warning "(...) interpreted as grouped expression"
                      result = val[1]
                    }
                | tLPAREN compstmt tRPAREN
                    {
                      result = val[1] || s(:nil)
                      result.paren = true
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                      result = s(:colon2, val[0], val[2].to_sym)
                    }
                | tCOLON3 tCONSTANT
                    {
                      result = s(:colon3, val[1].to_sym)
                    }
                | primary_value "[" aref_args tRBRACK
                    {
                      result = new_aref val
                    }
                | tLBRACK aref_args tRBRACK
                    {
                      result = val[1] || s(:array)
                    }
                | tLBRACE assoc_list tRCURLY
                    {
                      result = s(:hash, *val[1].values)
                    }
                | kRETURN
                    {
                      result = s(:return)
                    }
                | kYIELD tLPAREN2 call_args tRPAREN
                    {
                      result = new_yield val[2]
                    }
                | kYIELD tLPAREN2 tRPAREN
                    {
                      result = new_yield
                    }
                | kYIELD
                    {
                      result = new_yield
                    }
                | kDEFINED opt_nl tLPAREN2 expr tRPAREN
                    {
                      result = s(:defined, val[3])
                    }
                | operation brace_block
                    {
                      oper, iter = val[0], val[1]
                      call = new_call(nil, oper.to_sym)
                      iter.insert 1, call
                      result = iter
                      call.line ||= iter.line
                    }
                | method_call
                | method_call brace_block
                    {
                      call, iter = val[0], val[1]
                      iter.insert 1, call
                      result = iter
                    }
                | kIF expr_value then compstmt if_tail kEND
                    {
                      result = new_if val[1], val[3], val[4]
                    }
                | kUNLESS expr_value then compstmt opt_else kEND
                    {
                      result = new_if val[1], val[4], val[3]
                    }
                | kWHILE
                    {
                      lexer.cond.push true
                    }
                    expr_value do
                    {
                      lexer.cond.pop
                    }
                    compstmt kEND
                    {
                      result = new_while val[5], val[2], true
                    }
                | kUNTIL
                    {
                      lexer.cond.push true
                    }
                    expr_value do
                    {
                      lexer.cond.pop
                    }
                    compstmt kEND
                    {
                      result = new_until val[5], val[2], true
                    }
                | kCASE expr_value opt_terms case_body kEND
                    {
                      result = new_case val[1], val[3]
                    }
                | kCASE            opt_terms case_body kEND
                    {
                      result = new_case nil, val[2]
                    }
                | kCASE opt_terms kELSE compstmt kEND # TODO: need a test
                    {
                      result = new_case nil, val[3]
                    }
                | kFOR block_var kIN
                    {
                      lexer.cond.push true
                    }
                    expr_value do
                    {
                      lexer.cond.pop
                    }
                    compstmt kEND
                    {
                      result = new_for val[4], val[1], val[7]
                    }
                | kCLASS
                    {
                      result = self.lexer.lineno
                    }
                    cpath superclass
                    {
                      self.comments.push self.lexer.comments
                      if (self.in_def || self.in_single > 0) then
                        yyerror "class definition in method body"
                      end
                      self.env.extend
                    }
                    bodystmt kEND
                    {
                      result = new_class val
                      self.env.unextend
                    }
                | kCLASS tLSHFT
                    {
                      result = self.lexer.lineno
                    }
                    expr
                    {
                      result = self.in_def
                      self.in_def = false
                    }
                    term
                    {
                      result = self.in_single
                      self.in_single = 0
                      self.env.extend
                    }
                    bodystmt kEND
                    {
                      result = new_sclass val
                      self.env.unextend
                    }
                | kMODULE
                    {
                      result = self.lexer.lineno
                    }
                    cpath
                    {
                      self.comments.push self.lexer.comments
                      yyerror "module definition in method body" if
                        self.in_def or self.in_single > 0

                      self.env.extend
                    }
                    bodystmt kEND
                    {
                      result = new_module val
                      self.env.unextend
                    }
                | kDEF fname
                    {
                      self.comments.push self.lexer.comments
                      self.in_def = true
                      self.env.extend
                      result = self.lexer.lineno
                    }
                    f_arglist bodystmt kEND
                    {
                      result = new_defn val
                      self.env.unextend
                      self.in_def = false
                    }
                | kDEF singleton dot_or_colon
                    {
                      self.comments.push self.lexer.comments
                      lexer.lex_state = :expr_fname
                    }
                    fname
                    {
                      self.in_single += 1
                      self.env.extend
                      lexer.lex_state = :expr_end # force for args
                    }
                    f_arglist bodystmt kEND
                    {
                      result = new_defs val

                      self.env.unextend
                      self.in_single -= 1
                    }
                | kBREAK
                    {
                      result = s(:break)
                    }
                | kNEXT
                    {
                      result = s(:next)
                    }
                | kREDO
                    {
                      result = s(:redo)
                    }
                | kRETRY
                    {
                      result = s(:retry)
                    }

   primary_value: primary
                    {
                      result = value_expr(val[0])
                    }

            then: term
                | tCOLON
                | kTHEN
                | term kTHEN

              do: term
                | tCOLON
                | kDO_COND

         if_tail: opt_else
                | kELSIF expr_value then compstmt if_tail
                    {
                      result = s(:if, val[1], val[3], val[4])
                    }

        opt_else: none
                | kELSE compstmt
                    {
                      result = val[1]
                    }

       block_var: lhs
                | mlhs
                    {
                      val[0].delete_at 1 if val[0][1].nil? # HACK
                    }

   opt_block_var: none
                | tPIPE tPIPE
                    {
                      result = 0
                    }
                | tOROP
                    {
                      result = 0
                    }
                | tPIPE block_var tPIPE
                    {
                      result = val[1]
                    }

        do_block: kDO_BLOCK
                    {
                      self.env.extend :dynamic
                    }
                    opt_block_var
                    {
                      result = self.env.dynamic.keys
                    }
                    compstmt kEND
                    {
                      vars   = val[2]
                      body   = val[4]
                      result = new_iter nil, vars, body

                      self.env.unextend
                    }

      block_call: command do_block
                    {
                      raise SyntaxError, "Both block arg and actual block given." if
                        val[0] && val[0][0] == :blockpass

                      result = val[1]
                      result.insert 1, val[0]
                    }
                | block_call tDOT operation2 opt_paren_args
                    {
                      result = new_call val[0], val[2], val[3]
                    }
                | block_call tCOLON2 operation2 opt_paren_args
                    {
                      result = new_call val[0], val[2], val[3]
                    }

     method_call: operation
                    {
                      result = self.lexer.lineno
                    }
                    paren_args
                    {
                      result = new_call nil, val[0].to_sym, val[2]
                    }
                | primary_value tDOT operation2 opt_paren_args
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                    }
                | primary_value tCOLON2 operation2 paren_args
                    {
                      result = new_call val[0], val[2].to_sym, val[3]
                    }
                | primary_value tCOLON2 operation3
                    {
                      result = new_call val[0], val[2].to_sym
                    }
                | kSUPER paren_args
                    {
                      result = new_super val[1]
                    }
                | kSUPER
                    {
                      result = s(:zsuper)
                    }

     brace_block: tLCURLY
                    {
                      self.env.extend :dynamic
                      result = self.lexer.lineno
                    }
                    opt_block_var
                    {
                      result = self.env.dynamic.keys
                    }
                    compstmt tRCURLY
                    {
                      # REFACTOR
                      args   = val[2]
                      body   = val[4]
                      result = new_iter nil, args, body
                      self.env.unextend
                      result.line = val[1]
                    }
                | kDO
                    {
                      self.env.extend :dynamic
                      result = self.lexer.lineno
                    }
                 opt_block_var
                    {
                      result = self.env.dynamic.keys
                    }
                    compstmt kEND
                    {
                      args = val[2]
                      body = val[4]
                      result = new_iter nil, args, body
                      self.env.unextend
                      result.line = val[1]
                    }

       case_body: kWHEN
                    {
                      result = self.lexer.lineno
                    }
                    when_args then compstmt cases
                    {
                      result = s(:when, val[2], val[4])
                      result.line = val[1]
                      result << val[5] if val[5]
                    }

       when_args: args
                | args tCOMMA tSTAR arg_value
                    {
                      result = self.list_append val[0], s(:when, val[3], nil)
                    }
                | tSTAR arg_value
                    {
                      result = s(:array, s(:when, val[1], nil))
                    }

           cases: opt_else | case_body

      opt_rescue: kRESCUE exc_list exc_var then compstmt opt_rescue
                    {
                      klasses, var, body, rest = val[1], val[2], val[4], val[5]

                      klasses ||= s(:array)
                      klasses << node_assign(var, s(:gvar, :"$!")) if var

                      result = s(:resbody, klasses, body)
                      result << rest if rest # UGH, rewritten above
                    }
                |
                    {
                      result = nil
                    }

        exc_list: arg_value
                    {
                      result = s(:array, val[0])
                    }
                | mrhs
                | none

         exc_var: tASSOC lhs
                    {
                      result = val[1]
                    }
                | none

      opt_ensure: kENSURE compstmt
                    {
                      if (val[1] != nil) then
                        result = val[1]
                      else
                        result = s(:nil)
                      end
                    }
                | none

         literal: numeric { result = s(:lit, val[0]) }
                | symbol  { result = s(:lit, val[0]) }
                | dsym

         strings: string
                    {
                      val[0] = s(:dstr, val[0].value) if val[0][0] == :evstr
                      result = val[0]
                    }

          string: string1
                | string string1
                    {
                      result = self.literal_concat val[0], val[1]
                    }

         string1: tSTRING_BEG string_contents tSTRING_END
                    {
                      result = val[1]
                    }
                | tSTRING
                    {
                      result = s(:str, val[0])
                    }

         xstring: tXSTRING_BEG xstring_contents tSTRING_END
                    {
                      result = new_xstring val[1]
                    }

          regexp: tREGEXP_BEG xstring_contents tREGEXP_END
                    {
                      result = new_regexp val
                    }

           words: tWORDS_BEG tSPACE tSTRING_END
                    {
                      result = s(:array)
                    }
                | tWORDS_BEG word_list tSTRING_END
                    {
                      result = val[1]
                    }

       word_list: none
                    {
                      result = s(:array)
                    }
                | word_list word tSPACE
                    {
                      word = val[1][0] == :evstr ? s(:dstr, "", val[1]) : val[1]
                      result = val[0] << word
                    }

            word: string_content
                | word string_content
                    {
                      result = self.literal_concat val[0], val[1]
                    }

          awords: tAWORDS_BEG tSPACE tSTRING_END
                    {
                      result = s(:array)
                    }
                | tAWORDS_BEG qword_list tSTRING_END
                    {
                      result = val[1]
                    }

      qword_list: none
                    {
                      result = s(:array)
                    }
                | qword_list tSTRING_CONTENT tSPACE
                    {
                      result = val[0] << s(:str, val[1])
                    }

 string_contents: none
                    {
                      result = s(:str, "")
                    }
                | string_contents string_content
                    {
                      result = literal_concat(val[0], val[1])
                    }

xstring_contents: none
                    {
                      result = nil
                    }
                | xstring_contents string_content
                    {
                      result = literal_concat(val[0], val[1])
                    }

  string_content: tSTRING_CONTENT
                    {
                      result = s(:str, val[0])
                    }
                | tSTRING_DVAR
                    {
                      result = lexer.lex_strterm
                      lexer.lex_strterm = nil
                      lexer.lex_state = :expr_beg
                    }
                    string_dvar
                    {
                      lexer.lex_strterm = val[1]
                      result = s(:evstr, val[2])
                    }
                | tSTRING_DBEG
                    {
                      result = lexer.lex_strterm
                      lexer.lex_strterm = nil
                      lexer.lex_state = :expr_beg
                      lexer.cond.push false
                      lexer.cmdarg.push false
                    }
                    compstmt tRCURLY
                    {
                      lexer.lex_strterm = val[1]
                      lexer.cond.lexpop
                      lexer.cmdarg.lexpop

                      case val[2]
                      when Sexp then
                        case val[2][0]
                        when :str, :dstr, :evstr then
                          result = val[2]
                        else
                          result = s(:evstr, val[2])
                        end
                      when nil then
                        result = s(:evstr)
                      else
                        raise "unknown rescue body: #{val[2].inspect}"
                      end
                    }

     string_dvar: tGVAR { result = s(:gvar, val[0].to_sym) }
                | tIVAR { result = s(:ivar, val[0].to_sym) }
                | tCVAR { result = s(:cvar, val[0].to_sym) }
                | backref


          symbol: tSYMBEG sym
                    {
                      lexer.lex_state = :expr_end
                      result = val[1].to_sym
                    }
                | tSYMBOL
                    {
                      result = val[0].to_sym
                    }

             sym: fname | tIVAR | tGVAR | tCVAR

            dsym: tSYMBEG xstring_contents tSTRING_END
                    {
                      lexer.lex_state = :expr_end
                      result = val[1]

                      yyerror "empty symbol literal" if
                        result.nil? or result.empty?

                      case result[0]
                      when :dstr then
                        result[0] = :dsym
                      when :str then
                        result = s(:lit, result.last.intern)
                      else
                        result = s(:dsym, "", result)
                      end
                    }

         numeric: tINTEGER
                | tFLOAT
                | tUMINUS_NUM tINTEGER =tLOWEST
                    {
                      result = -val[1] # TODO: pt_testcase
                    }
                | tUMINUS_NUM tFLOAT   =tLOWEST
                    {
                      result = -val[1] # TODO: pt_testcase
                    }

        variable: tIDENTIFIER
                | tIVAR
                | tGVAR
                | tCONSTANT
                | tCVAR
                | kNIL      { result = :nil      }
                | kSELF     { result = :self     }
                | kTRUE     { result = :true     }
                | kFALSE    { result = :false    }
                | k__FILE__ { result = :__FILE__ }
                | k__LINE__ { result = :__LINE__ }

         var_ref: variable
                    {
                      result = self.gettable val[0]
                    }

         var_lhs: variable
                    {
                      result = self.assignable val[0]
                    }

         backref: tNTH_REF  { result = s(:nth_ref,  val[0]) }
                | tBACK_REF { result = s(:back_ref, val[0]) }

      superclass: term
                    {
                      result = nil
                    }
                | tLT
                    {
                      lexer.lex_state = :expr_beg
                    }
                    expr_value term
                    {
                      result = val[2]
                    }
                | error term
                    {
                      yyerrok
                      result = nil
                    }

       f_arglist: tLPAREN2 f_args opt_nl tRPAREN
                    {
                      result = val[1]
                      lexer.lex_state = :expr_beg
                    }
                | f_args term
                    {
                      result = val[0]
                    }

          f_args: f_arg tCOMMA f_optarg tCOMMA f_rest_arg opt_f_block_arg
                    {
                      result = args val[0], val[2], val[4], val[5]
                    }
                | f_arg tCOMMA f_optarg                opt_f_block_arg
                    {
                      result = args val[0], val[2],    nil, val[3]
                    }
                | f_arg tCOMMA              f_rest_arg opt_f_block_arg
                    {
                      result = args val[0],    nil, val[2], val[3]
                    }
                | f_arg                             opt_f_block_arg
                    {
                      result = args val[0],    nil,    nil, val[1]
                    }
                |           f_optarg tCOMMA f_rest_arg opt_f_block_arg
                    {
                      result = args    nil, val[0], val[2], val[3]
                    }
                |           f_optarg                opt_f_block_arg
                    {
                      result = args    nil, val[0],    nil, val[1]
                    }
                |                        f_rest_arg opt_f_block_arg
                    {
                      result = args    nil,    nil, val[0], val[1]
                    }
                |                                       f_block_arg
                    {
                      result = args    nil,    nil,    nil, val[0]
                    }
                |
                    {
                      result = args    nil,    nil,    nil,    nil
                    }

      f_norm_arg: tCONSTANT
                    {
                      yyerror "formal argument cannot be a constant: #{val[0]}"
                    }
                | tIVAR
                    {
                      yyerror "formal argument cannot be an instance variable"
                    }
                | tCVAR
                    {
                      yyerror "formal argument cannot be a class variable"
                    }
                | tIDENTIFIER
                    {
                      identifier = val[0].to_sym
                      self.env[identifier] = :lvar

                      result = val[0]
                    }

           f_arg: f_norm_arg
                    {
                      result = s(:args)
                      result << val[0].to_sym
                    }
                | f_arg tCOMMA f_norm_arg
                    {
                      val[0] << val[2].to_sym
                      result = val[0]
                    }

           f_opt: tIDENTIFIER tEQL arg_value
                    {
                      result = self.assignable val[0], val[2]
                      # TODO: detect duplicate names
                    }

        f_optarg: f_opt
                    {
                      result = s(:block, val[0])
                    }
                | f_optarg tCOMMA f_opt
                    {
                      result = self.append_to_block val[0], val[2]
                    }

    restarg_mark: tSTAR2 | tSTAR

      f_rest_arg: restarg_mark tIDENTIFIER
                    {
                      # TODO: differs from parse.y - needs tests
                      name = val[1].to_sym
                      self.assignable name
                      result = :"*#{name}"
                    }
                | restarg_mark
                    {
                      name = :"*"
                      self.env[name] = :lvar
                      result = name
                    }

     blkarg_mark: tAMPER2 | tAMPER

     f_block_arg: blkarg_mark tIDENTIFIER
                    {
                      identifier = val[1].to_sym

                      self.env[identifier] = :lvar
                      result = s(:block_arg, identifier.to_sym)
                    }

 opt_f_block_arg: tCOMMA f_block_arg
                    {
                      result = val[1]
                    }
                |
                    {
                      result = nil
                    }

       singleton: var_ref
                | tLPAREN2
                    {
                      lexer.lex_state = :expr_beg
                    }
                    expr opt_nl tRPAREN
                    {
                      result = val[2]
                      yyerror "Can't define single method for literals." if
                        result[0] == :lit
                    }

      assoc_list: none # [!nil]
                    {
                      result = s(:array)
                    }
                | assocs trailer # [!nil]
                    {
                      result = val[0]
                    }
                | args trailer
                    {
                      size = val[0].size
                      if (size % 2 != 1) then # != 1 because of leading :array
                        yyerror "Odd number (#{size}) list for Hash. #{val[0].inspect}"
                      end
                      result = val[0]
                    }

          assocs: assoc
                | assocs tCOMMA assoc
                    {
                      list = val[0].dup
                      more = val[2][1..-1]
                      list.push(*more) unless more.empty?
                      result = list
                    }

           assoc: arg_value tASSOC arg_value
                    {
                      result = s(:array, val[0], val[2])
                    }

       operation: tIDENTIFIER | tCONSTANT | tFID
      operation2: tIDENTIFIER | tCONSTANT | tFID | op
      operation3: tIDENTIFIER | tFID | op
    dot_or_colon: tDOT | tCOLON2
       opt_terms:  | terms
          opt_nl:  | tNL
         trailer:  | tNL | tCOMMA

            term: tSEMI { yyerrok }
                | tNL

           terms: term
                | terms tSEMI { yyerrok }

            none: { result = nil }

 none_block_pass: { result = nil }

end

---- inner

require "ruby_lexer"

# Local Variables: **
# racc-token-length-max:14 **
# End: **
