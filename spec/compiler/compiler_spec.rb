require File.dirname(__FILE__) + '/spec_helper'
require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib'
require 'pt_testcase'

class CompilerTestCase < ParseTreeTestCase
  def self.bytecode &block
    @tg = TestGenerator.new
    @tg.instance_eval &block
    @tg
  end

  # just to be used when unstubbing a skip - TODO: remove
  bogo_bytecode = bytecode do end
  bogo_klass_bc = bytecode do in_class :X do end end

  add_tests("alias",
            "Compiler" => bytecode do |g|
              in_class :X do |d|
                d.push :self
                d.push_literal :y
                d.push_literal :x
                d.send :alias_method, 2, true
              end
            end)

  add_tests("alias_ugh",
            "Compiler" => testcases["alias"]["Compiler"])

  add_tests("and",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true
              g.dup

              lhs_true = g.new_label
              g.gif lhs_true

              g.pop
              g.push :self
              g.send :b, 0, true

              lhs_true.set!
            end)

  add_tests("argscat_inside",
            "Compiler" => :skip)

  add_tests("argscat_svalue",
            "Compiler" => :skip)

  add_tests("argspush",
            "Compiler" => :skip)

  add_tests("array",
            "Compiler" => :skip)

  add_tests("array_pct_W",
            "Compiler" => :skip)

  add_tests("array_pct_W_dstr",
            "Compiler" => :skip)

  add_tests("attrasgn",
            "Compiler" => :skip)

  add_tests("attrasgn_index_equals",
            "Compiler" => :skip)

  add_tests("attrasgn_index_equals_space",
            "Compiler" => :skip)

  add_tests("attrset",
            "Compiler" => :skip)

  add_tests("back_ref",
            "Compiler" => :skip)

  add_tests("begin",
            "Compiler" => :skip)

  add_tests("begin_def",
            "Compiler" => :skip)

  add_tests("begin_rescue_ensure",
            "Compiler" => :skip)

  add_tests("begin_rescue_twice",
            "Compiler" => :skip)

  add_tests("begin_rescue_twice_mri_verbose_flag",
            "Compiler" => :skip)

  add_tests("block_attrasgn",
            "Compiler" => :skip)

  add_tests("block_lasgn",
            "Compiler" => :skip)

  add_tests("block_mystery_block",
            "Compiler" => :skip)

  add_tests("block_pass_args_and_splat",
            "Compiler" => :skip)

  add_tests("block_pass_call_0",
            "Compiler" => :skip)

  add_tests("block_pass_call_1",
            "Compiler" => :skip)

  add_tests("block_pass_call_n",
            "Compiler" => :skip)

  add_tests("block_pass_fcall_0",
            "Compiler" => :skip)

  add_tests("block_pass_fcall_1",
            "Compiler" => :skip)

  add_tests("block_pass_fcall_n",
            "Compiler" => :skip)

  add_tests("block_pass_omgwtf",
            "Compiler" => :skip)

  add_tests("block_pass_splat",
            "Compiler" => :skip)

  add_tests("block_pass_super",
            "Compiler" => :skip)

  add_tests("block_pass_thingy",
            "Compiler" => :skip)

  add_tests("block_stmt_after",
            "Compiler" => :skip)

  add_tests("block_stmt_after_mri_verbose_flag",
            "Compiler" => :skip)

  add_tests("block_stmt_before",
            "Compiler" => :skip)

  add_tests("block_stmt_before_mri_verbose_flag",
            "Compiler" => :skip)

  add_tests("block_stmt_both",
            "Compiler" => :skip)

  add_tests("block_stmt_both_mri_verbose_flag",
            "Compiler" => :skip)

  add_tests("bmethod",
            "Compiler" => :skip)

  add_tests("bmethod_noargs",
            "Compiler" => :skip)

  add_tests("bmethod_splat",
            "Compiler" => :skip)

  add_tests("break",
            "Compiler" => :skip)

  add_tests("break_arg",
            "Compiler" => :skip)

  add_tests("call",
            "Compiler" => :skip)

  add_tests("call_arglist",
            "Compiler" => :skip)

  add_tests("call_arglist_hash",
            "Compiler" => :skip)

  add_tests("call_arglist_norm_hash",
            "Compiler" => :skip)

  add_tests("call_arglist_norm_hash_splat",
            "Compiler" => :skip)

  add_tests("call_command",
            "Compiler" => :skip)

  add_tests("call_expr",
            "Compiler" => :skip)

  add_tests("call_index",
            "Compiler" => :skip)

  add_tests("call_index_no_args",
            "Compiler" => :skip)

  add_tests("call_index_space",
            "Compiler" => :skip)

  add_tests("call_unary_neg",
            "Compiler" => :skip)

  add_tests("case",
            "Compiler" => :skip)

  add_tests("case_nested",
            "Compiler" => :skip)

  add_tests("case_nested_inner_no_expr",
            "Compiler" => :skip)

  add_tests("case_no_expr",
            "Compiler" => :skip)

  add_tests("case_splat",
            "Compiler" => :skip)

  add_tests("cdecl",
            "Compiler" => :skip)

  add_tests("class_plain",
            "Compiler" => :skip)

  add_tests("class_scoped",
            "Compiler" => :skip)

  add_tests("class_scoped3",
            "Compiler" => :skip)

  add_tests("class_super_array",
            "Compiler" => :skip)

  add_tests("class_super_expr",
            "Compiler" => :skip)

  add_tests("class_super_object",
            "Compiler" => :skip)

  add_tests("colon2",
            "Compiler" => :skip)

  add_tests("colon3",
            "Compiler" => :skip)

  add_tests("conditional1",
            "Compiler" => :skip)

  add_tests("conditional2",
            "Compiler" => :skip)

  add_tests("conditional3",
            "Compiler" => :skip)

  add_tests("conditional4",
            "Compiler" => :skip)

  add_tests("conditional5",
            "Compiler" => :skip)

  add_tests("conditional_post_if",
            "Compiler" => :skip)

  add_tests("conditional_post_if_not",
            "Compiler" => :skip)

  add_tests("conditional_post_unless",
            "Compiler" => :skip)

  add_tests("conditional_post_unless_not",
            "Compiler" => :skip)

  add_tests("conditional_pre_if",
            "Compiler" => :skip)

  add_tests("conditional_pre_if_not",
            "Compiler" => :skip)

  add_tests("conditional_pre_unless",
            "Compiler" => :skip)

  add_tests("conditional_pre_unless_not",
            "Compiler" => :skip)

  add_tests("const",
            "Compiler" => :skip)

  add_tests("constX",
            "Compiler" => :skip)

  add_tests("constY",
            "Compiler" => :skip)

  add_tests("constZ",
            "Compiler" => :skip)

  add_tests("cvar",
            "Compiler" => :skip)

  add_tests("cvasgn",
            "Compiler" => :skip)

  add_tests("cvasgn_cls_method",
            "Compiler" => :skip)

  add_tests("cvdecl",
            "Compiler" => :skip)

  add_tests("dasgn_0",
            "Compiler" => :skip)

  add_tests("dasgn_1",
            "Compiler" => :skip)

  add_tests("dasgn_2",
            "Compiler" => :skip)

  add_tests("dasgn_curr",
            "Compiler" => :skip)

  add_tests("dasgn_icky",
            "Compiler" => :skip)

  add_tests("dasgn_mixed",
            "Compiler" => :skip)

  add_tests("defined",
            "Compiler" => :skip)

  add_tests("defn_args_mand",
            "Compiler" => :skip)

  add_tests("defn_args_opt",
            "Compiler" => :skip)

  add_tests("defn_args_splat",
            "Compiler" => :skip)

  add_tests("defn_args_block",
            "Compiler" => :skip)

  add_tests("defn_args_mand_opt",
            "Compiler" => :skip)

  add_tests("defn_args_mand_splat",
            "Compiler" => :skip)

  add_tests("defn_args_mand_block",
            "Compiler" => :skip)

  add_tests("defn_args_opt_splat",
            "Compiler" => :skip)

  add_tests("defn_args_opt_block",
            "Compiler" => :skip)

  add_tests("defn_args_splat_block",
            "Compiler" => :skip)

  add_tests("defn_args_mand_opt_splat",
            "Compiler" => :skip)

  add_tests("defn_args_mand_opt_block",
            "Compiler" => :skip)

  add_tests("defn_args_mand_spl",
            "Compiler" => :skip)

  add_tests("defn_args_opt_splat_block",
            "Compiler" => :skip)

  add_tests("defn_args_mand_opt_splat_block",
            "Compiler" => :skip)

  add_tests("defn_args_mand_opt_splat_no_name",
            "Compiler" => :skip)

  add_tests("defn_args_opt_splat_no_name",
            "Compiler" => :skip)

  add_tests("defn_empty",
            "Compiler" => :skip)

  add_tests("defn_empty_args",
            "Compiler" => :skip)

  add_tests("defn_lvar_boundary",
            "Compiler" => :skip)

  add_tests("defn_optargs",
            "Compiler" => :skip)

  add_tests("defn_or",
            "Compiler" => :skip)

  add_tests("defn_rescue",
            "Compiler" => :skip)

  add_tests("defn_rescue_mri_verbose_flag",
            "Compiler" => :skip)

  add_tests("defn_something_eh",
            "Compiler" => :skip)

  add_tests("defn_splat_no_name",
            "Compiler" => :skip)

  add_tests("defn_zarray",
            "Compiler" => :skip)

  add_tests("defs",
            "Compiler" => :skip)

  add_tests("defs_empty",
            "Compiler" => :skip)

  add_tests("defs_empty_args",
            "Compiler" => :skip)

  add_tests("dmethod",
            "Compiler" => :skip)

  add_tests("dot2",
            "Compiler" => :skip)

  add_tests("dot3",
            "Compiler" => :skip)

  add_tests("dregx",
            "Compiler" => :skip)

  add_tests("dregx_interp",
            "Compiler" => :skip)

  add_tests("dregx_n",
            "Compiler" => :skip)

  add_tests("dregx_once",
            "Compiler" => :skip)

  add_tests("dregx_once_n_interp",
            "Compiler" => :skip)

  add_tests("dstr",
            "Compiler" => :skip)

  add_tests("dstr_2",
            "Compiler" => :skip)

  add_tests("dstr_3",
            "Compiler" => :skip)

  add_tests("dstr_concat",
            "Compiler" => :skip)

  add_tests("dstr_gross",
            "Compiler" => :skip)

  add_tests("dstr_heredoc_expand",
            "Compiler" => :skip)

  add_tests("dstr_heredoc_windoze_sucks",
            "Compiler" => :skip)

  add_tests("dstr_heredoc_yet_again",
            "Compiler" => :skip)

  add_tests("dstr_nest",
            "Compiler" => :skip)

  add_tests("dstr_str_lit_start",
            "Compiler" => :skip)

  add_tests("dstr_the_revenge",
            "Compiler" => :skip)

  add_tests("dsym",
            "Compiler" => :skip)

  add_tests("dxstr",
            "Compiler" => :skip)

  add_tests("ensure",
            "Compiler" => :skip)

  add_tests("false",
            "Compiler" => :skip)

  add_tests("fbody",
            "Compiler" => :skip)

  add_tests("fcall_arglist",
            "Compiler" => :skip)

  add_tests("fcall_arglist_hash",
            "Compiler" => :skip)

  add_tests("fcall_arglist_norm_hash",
            "Compiler" => :skip)

  add_tests("fcall_arglist_norm_hash_splat",
            "Compiler" => :skip)

  add_tests("fcall_block",
            "Compiler" => :skip)

  add_tests("fcall_index_space",
            "Compiler" => :skip)

  add_tests("fcall_keyword",
            "Compiler" => :skip)

  add_tests("flip2",
            "Compiler" => :skip)

  add_tests("flip2_method",
            "Compiler" => :skip)

  add_tests("flip3",
            "Compiler" => :skip)

  add_tests("for",
            "Compiler" => :skip)

  add_tests("for_no_body",
            "Compiler" => :skip)

  add_tests("gasgn",
            "Compiler" => :skip)

  add_tests("global",
            "Compiler" => :skip)

  add_tests("gvar",
            "Compiler" => :skip)

  add_tests("gvar_underscore",
            "Compiler" => :skip)

  add_tests("gvar_underscore_blah",
            "Compiler" => :skip)

  add_tests("hash",
            "Compiler" => :skip)

  add_tests("hash_rescue",
            "Compiler" => :skip)

  add_tests("iasgn",
            "Compiler" => :skip)

  add_tests("if_block_condition",
            "Compiler" => :skip)

  add_tests("if_lasgn_short",
            "Compiler" => :skip)

  add_tests("iteration1",
            "Compiler" => :skip)

  add_tests("iteration2",
            "Compiler" => :skip)

  add_tests("iteration3",
            "Compiler" => :skip)

  add_tests("iteration4",
            "Compiler" => :skip)

  add_tests("iteration5",
            "Compiler" => :skip)

  add_tests("iteration6",
            "Compiler" => :skip)

  add_tests("iteration7",
            "Compiler" => :skip)

  add_tests("iteration8",
            "Compiler" => :skip)

  add_tests("iteration9",
            "Compiler" => :skip)

  add_tests("iterationA", # TODO: rename all of these
            "Compiler" => :skip)

  add_tests("iteration_dasgn_curr_dasgn_madness",
            "Compiler" => :skip)

  add_tests("iteration_double_var",
            "Compiler" => :skip)

  add_tests("iteration_masgn",
            "Compiler" => :skip)

  add_tests("ivar",
            "Compiler" => :skip)

  add_tests("lasgn_array",
            "Compiler" => :skip)

  add_tests("lasgn_call",
            "Compiler" => :skip)

  add_tests("lit_bool_false",
            "Compiler" => bytecode do |g|
              g.push :false
            end)

  add_tests("lit_bool_true",
            "Compiler" => bytecode do |g|
              g.push :true
            end)

  add_tests("lit_float",
            "Compiler" => bytecode do |g|
              g.push 1.1
            end)

  add_tests("lit_long",
            "Compiler" => bytecode do |g|
              g.push 1
            end)

  add_tests("lit_long_negative",
            "Compiler" => bytecode do |g|
              g.push -1
            end)

  add_tests("lit_range2",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Range
              g.push 1
              g.push 10
              g.send :new, 2
            end)

  add_tests("lit_range3",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Range
              g.push 1
              g.push 10
              g.push :true
              g.send :new, 3
            end)

  add_tests("lit_regexp",
            "Compiler" => bytecode do |g|
              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 0
                g.send :new, 2
              end
            end)

  add_tests("lit_regexp_i_wwtt",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :str, 0, true

              g.memoize do
                g.push_const :Regexp
                g.push_literal ""
                g.push 1
                g.send :new, 2
              end

              g.send :split, 1, false
            end)

  add_tests("lit_regexp_n",
            "Compiler" => bytecode do |g|
              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 16
                g.send :new, 2
              end
            end)

  add_tests("lit_regexp_once", # TODO: same as lit_regexp. verify
            "Compiler" => bytecode do |g|
              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 0
                g.send :new, 2
              end
            end)

  add_tests("lit_sym",
            "Compiler" => bytecode do |g|
              g.push_unique_literal :x
            end)

  add_tests("lit_sym_splat",
            "Compiler" => bytecode do |g|
              g.push_unique_literal :"*args"
            end)

  add_tests("lvar_def_boundary",
            "Compiler" => :skip)

  add_tests("masgn",
            "Compiler" => :skip)

  add_tests("masgn_argscat",
            "Compiler" => :skip)

  add_tests("masgn_attrasgn",
            "Compiler" => :skip)

  add_tests("masgn_attrasgn_idx",
            "Compiler" => :skip)

  add_tests("masgn_iasgn",
            "Compiler" => :skip)

  add_tests("masgn_masgn",
            "Compiler" => :skip)

  add_tests("masgn_splat",
            "Compiler" => :skip)

  add_tests("masgn_splat_no_name_to_ary",
            "Compiler" => :skip)

  add_tests("masgn_splat_no_name_trailing",
            "Compiler" => :skip)

  add_tests("masgn_splat_to_ary",
            "Compiler" => :skip)

  add_tests("masgn_splat_to_ary2",
            "Compiler" => :skip)

  add_tests("match",
            "Compiler" => bytecode do |g|
              g.push_literal :$_ # REFACTOR - we use this block a lot
              g.push_cpath_top
              g.find_const :Globals
              g.send :[], 1

              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 0
                g.send :new, 2
              end

              f = g.new_label
              t = g.new_label

              g.send :=~, 1
              g.gif f
              g.push 1
              g.goto t
              f.set!
              g.push :nil
              t.set!
            end)

  add_tests("match2",
            "Compiler" => bytecode do |g|
              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 0
                g.send :new, 2
              end

              g.push_literal "blah"
              g.string_dup
              g.send :=~, 1
            end)

  add_tests("match3",
            "Compiler" => bytecode do |g|
              g.push_literal "blah"
              g.string_dup

              g.memoize do
                g.push_const :Regexp
                g.push_literal "x"
                g.push 0
                g.send :new, 2
              end

              g.send :=~, 1
            end)

  add_tests("module",
            "Compiler" => :skip)

  add_tests("module_scoped",
            "Compiler" => :skip)

  add_tests("module_scoped3",
            "Compiler" => :skip)

  add_tests("next",
            "Compiler" => :skip)

  add_tests("next_arg",
            "Compiler" => :skip)

  add_tests("not",
            "Compiler" => bytecode do |g|
              f = g.new_label
              t = g.new_label

              g.push :true
              g.git f

              g.push :true
              g.goto t

              f.set!
              g.push :false

              t.set!
            end)

  add_tests("nth_ref",
            "Compiler" => :skip)

  add_tests("op_asgn1",
            "Compiler" => :skip)

  add_tests("op_asgn1_ivar",
            "Compiler" => :skip)

  add_tests("op_asgn2",
            "Compiler" => :skip)

  add_tests("op_asgn2_self",
            "Compiler" => :skip)

  add_tests("op_asgn_and",
            "Compiler" => :skip)

  add_tests("op_asgn_and_ivar2",
            "Compiler" => :skip)

  add_tests("op_asgn_or",
            "Compiler" => :skip)

  add_tests("op_asgn_or_block",
            "Compiler" => :skip)

  add_tests("op_asgn_or_ivar",
            "Compiler" => :skip)

  add_tests("op_asgn_or_ivar2",
            "Compiler" => :skip)

  add_tests("or",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true
              g.dup

              lhs_true = g.new_label
              g.git lhs_true

              g.pop
              g.push :self
              g.send :b, 0, true

              lhs_true.set!
            end)

  add_tests("or_big",
            "Compiler" => :skip)

  add_tests("or_big2",
            "Compiler" => :skip)

  add_tests("parse_floats_as_args",
            "Compiler" => :skip)

  add_tests("postexe",
            "Compiler" => :skip)

  add_tests("proc_args_0",
            "Compiler" => :skip)

  add_tests("proc_args_1",
            "Compiler" => :skip)

  add_tests("proc_args_2",
            "Compiler" => :skip)

  add_tests("proc_args_no",
            "Compiler" => :skip)

  add_tests("redo",
            "Compiler" => :skip)

  add_tests("rescue",
            "Compiler" => :skip)

  add_tests("rescue_block_body",
            "Compiler" => :skip)

  add_tests("rescue_block_nada",
            "Compiler" => :skip)

  add_tests("rescue_exceptions",
            "Compiler" => :skip)

  add_tests("retry",
            "Compiler" => :skip)

  add_tests("return_0",
            "Compiler" => :skip)

  add_tests("return_1",
            "Compiler" => :skip)

  add_tests("return_n",
            "Compiler" => :skip)

  add_tests("sclass",
            "Compiler" => :skip)

  add_tests("sclass_trailing_class",
            "Compiler" => :skip)

  add_tests("splat",
            "Compiler" => :skip)

  add_tests("str",
            "Compiler" => :skip)

  add_tests("str_concat_newline", # FIX? make prettier? possible?
            "Compiler" => :skip)

  add_tests("str_concat_space",
            "Compiler" => :skip)

  add_tests("str_heredoc",
            "Compiler" => :skip)

  add_tests("str_heredoc_call",
            "Compiler" => :skip)

  add_tests("str_heredoc_double",
            "Compiler" => :skip)

  add_tests("str_heredoc_indent",
            "Compiler" => :skip)

  add_tests("str_interp_file",
            "Compiler" => :skip)

  add_tests("structure_extra_block_for_dvar_scoping",
            "Compiler" => :skip)

  add_tests("structure_remove_begin_1",
            "Compiler" => :skip)

  add_tests("structure_remove_begin_2",
            "Compiler" => :skip)

  add_tests("structure_unused_literal_wwtt",
            "Compiler" => :skip)

  add_tests("super",
            "Compiler" => :skip)

  add_tests("super_block_pass",
            "Compiler" => :skip)

  add_tests("super_block_splat",
            "Compiler" => :skip)

  add_tests("super_multi",
            "Compiler" => :skip)

  add_tests("svalue",
            "Compiler" => :skip)

  add_tests("to_ary",
            "Compiler" => :skip)

  add_tests("true",
            "Compiler" => :skip)

  add_tests("undef",
            "Compiler" => :skip)

  add_tests("undef_2",
            "Compiler" => :skip)

  add_tests("undef_3",
            "Compiler" => :skip)

  add_tests("undef_block_1",
            "Compiler" => :skip)

  add_tests("undef_block_2",
            "Compiler" => :skip)

  add_tests("undef_block_3",
            "Compiler" => :skip)

  add_tests("undef_block_3_post",
            "Compiler" => :skip)

  add_tests("undef_block_wtf",
            "Compiler" => :skip)

  add_tests("until_post",
            "Compiler" => :skip)

  add_tests("until_post_not",
            "Compiler" => :skip)

  add_tests("until_pre",
            "Compiler" => :skip)

  add_tests("until_pre_mod",
            "Compiler" => :skip)

  add_tests("until_pre_not",
            "Compiler" => :skip)

  add_tests("until_pre_not_mod",
            "Compiler" => :skip)

  add_tests("valias",
            "Compiler" => :skip)

  add_tests("vcall",
            "Compiler" => :skip)

  add_tests("while_post",
            "Compiler" => :skip)

  add_tests("while_post2",
            "Compiler" => :skip)

  add_tests("while_post_not",
            "Compiler" => :skip)

  add_tests("while_pre",
            "Compiler" => :skip)

  add_tests("while_pre_mod",
            "Compiler" => :skip)

  add_tests("while_pre_nil",
            "Compiler" => :skip)

  add_tests("while_pre_not",
            "Compiler" => :skip)

  add_tests("while_pre_not_mod",
            "Compiler" => :skip)

  add_tests("xstr",
            "Compiler" => :skip)

  add_tests("yield_0",
            "Compiler" => :skip)

  add_tests("yield_1",
            "Compiler" => :skip)

  add_tests("yield_n",
            "Compiler" => :skip)

  add_tests("zarray",
            "Compiler" => :skip)

  add_tests("zsuper",
            "Compiler" => :skip)
end

describe "Compiler::*Nodes" do
  ParseTreeTestCase.testcases.each do |node, hash|
    next if Array === hash['Ruby']
    next if hash['Compiler'] == :skip

    it "compiles :#{node}" do
      input    = hash['Ruby']
      expected = hash['Compiler']
      sexp     = input.to_sexp

      comp    = Compiler.new TestGenerator
      node    = comp.convert_sexp [:snippit, sexp]
      actual   = TestGenerator.new
      node.bytecode actual

      actual.should == expected
    end
  end
end
