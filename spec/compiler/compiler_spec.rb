require File.dirname(__FILE__) + '/spec_helper'
require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib'
require 'pt_testcase'

class CompilerTestCase < ParseTreeTestCase
  def self.bytecode &block
    @tg = TestGenerator.new
    @tg.instance_eval(&block)
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
            "Compiler" => bytecode do |g|
              g.push 1
              g.push_unique_literal :b
              g.push_literal "c"
              g.string_dup
              g.make_array 3
            end)

  add_tests("array_pct_W",
            "Compiler" => bytecode do |g|
              g.push_literal "a"
              g.string_dup
              g.push_literal "b"
              g.string_dup
              g.push_literal "c"
              g.string_dup
              g.make_array 3
            end)

  add_tests("array_pct_W_dstr",
            "Compiler" => bytecode do |g|
              g.push_literal "a"
              g.string_dup

              g.push_ivar :@b
              g.send :to_s, 0, true
              g.push_literal ""
              g.string_dup
              g.string_append

              g.push_literal "c"
              g.string_dup
              g.make_array 3
            end)

  add_tests("attrasgn",
            "Compiler" => :skip)

  add_tests("attrasgn_index_equals",
            "Compiler" => :skip)

  add_tests("attrasgn_index_equals_space",
            "Compiler" => :skip)

  add_tests("attrset",
            "Compiler" => :skip)

  add_tests("back_ref",
            "Compiler" => bytecode do |g|
              g.push_context
              g.push_literal :"&"
              g.send :back_ref, 1

              g.push_context
              g.push_literal :"`"
              g.send :back_ref, 1

              g.push_context
              g.push_literal :"'"
              g.send :back_ref, 1

              g.push_context
              g.push_literal :"+"
              g.send :back_ref, 1

              g.make_array 4
            end)

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

  # "Ruby"         => "loop { break if true }",
  add_tests("break",
            "Compiler" => bytecode do |g|
              break_value = :nil

              top   = g.new_label
              cond  = g.new_label
              dunno = g.new_label
              brk   = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.gif cond

              g.push break_value
              g.goto brk
              g.goto dunno # TODO: unreachable

              cond.set!
              g.push :nil

              dunno.set!
              g.pop
              g.goto top

              brk.set!

              g.pop_modifiers
            end)

  # "Ruby"         => "loop { break 42 if true }",
  add_tests("break_arg",
            "Compiler" => bytecode do |g|
              break_value = 42

              top   = g.new_label
              cond  = g.new_label
              dunno = g.new_label
              brk   = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.gif cond

              g.push break_value
              g.goto brk
              g.goto dunno # TODO: unreachable

              cond.set!
              g.push :nil

              dunno.set!
              g.pop
              g.goto top

              brk.set!

              g.pop_modifiers
            end)

  add_tests("call",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :method, 0, false
            end)

  add_tests("call_arglist",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :o, 0, true
              g.push 42
              g.send :puts, 1, false
            end)

  add_tests("call_arglist_hash",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :o, 0, true
              g.push_cpath_top
              g.find_const :Hash
              g.push_unique_literal :a
              g.push 1
              g.push_unique_literal :b
              g.push 2
              g.send :[], 4
              g.send :m, 1, false
            end)

  add_tests("call_arglist_norm_hash",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :o, 0, true
              g.push 42
              g.push_cpath_top
              g.find_const :Hash
              g.push_unique_literal :a
              g.push 1
              g.push_unique_literal :b
              g.push 2
              g.send :[], 4
              g.send :m, 2, false
            end)

  add_tests("call_arglist_norm_hash_splat",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :o, 0, true
              g.push 42
              g.push_cpath_top
              g.find_const :Hash
              g.push_unique_literal :a
              g.push 1
              g.push_unique_literal :b
              g.push 2
              g.send :[], 4

              g.push :self
              g.send :c, 0, true
              g.cast_array
              g.push :nil
              g.send_with_splat :m, 2, false, false
            end)

  add_tests("call_command",
            "Compiler" => bytecode do |g|
              g.push 1
              g.push :self
              g.send :c, 0, true
              g.send :b, 1, false
            end)

  add_tests("call_expr",
            "Compiler" => bytecode do |g|
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.set_local 0
              g.send :zero?, 0, false
            end)

  add_tests("call_index",
            "Compiler" => bytecode do |g|
              g.make_array 0
              g.set_local 0
              g.pop
              g.push_local 0
              g.push 42
              g.send :[], 1, false
            end)

  add_tests("call_index_no_args",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true
              g.send :[], 0, false
            end)

  add_tests("call_index_space",
            "Compiler" => testcases["call_index"]["Compiler"])

  add_tests("call_unary_neg",
            "Compiler" => bytecode do |g|
              g.push 2
              g.push 31
              g.send :**, 1, false
              g.send :-@, 0, false
            end)

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
            "Compiler" => bytecode do |g|
              in_class :X do |d|
                d.push :self
                d.push 1
                d.push 1
                d.meta_send_op_plus
                d.send :puts, 1, true
                d.pop

                d.in_method :blah do |d2|
                  d2.push :self
                  d2.push_literal "hello"
                  d2.string_dup
                  d2.send :puts, 1, true
                end
              end
            end)

  add_tests("class_scoped",
            "Compiler" => bytecode do |g|
              in_class "X::Y" do |d|
                d.push :self
                d.send :c, 0, true
              end
            end)

  add_tests("class_scoped3",
            "Compiler" => bytecode do |g|
              in_class :Y do |d|
                d.push :self
                d.send :c, 0, true
              end
            end)

  add_tests("class_super_array",
            "Compiler" => bytecode do |g|
              g.push_const :Array
              g.open_class :X
            end)

  add_tests("class_super_expr",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :expr, 0, true
              g.open_class :X
            end)

  add_tests("class_super_object",
            "Compiler" => bytecode do |g|
              g.push_const :Object
              g.open_class :X
            end)

  add_tests("colon2",
            "Compiler" => bytecode do |g|
              g.push_const :X
              g.find_const :Y
            end)

  add_tests("colon3",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :X
            end)

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
            "Compiler" => bytecode do |g|
              g.push_const :X
            end)

  add_tests("constX",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push_literal :X
              g.push 1
              g.send :__const_set__, 2
            end)

  add_tests("constY",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.push_literal :X
              g.push 1
              g.send :__const_set__, 2
            end)

  add_tests("constZ",
            "Compiler" => bytecode do |g|
              g.push_const :X
              g.push_literal :Y
              g.push 1
              g.send :__const_set__, 2
            end)

  add_tests("cvar",
            "Compiler" => bytecode do |g|
              g.push_context
              g.push_literal :@@x
              g.send :class_variable_get, 1
            end)

  add_tests("cvasgn",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push_context
                d.push_literal :@@blah
                d.push 1
                d.send :class_variable_set, 2
              end
            end)

  # => "def self.quiet_mode=(boolean)\n  @@quiet_mode = boolean\nend",
  add_tests("cvasgn_cls_method",
            "Compiler" => bytecode do |g|
              g.push :self
              in_method :quiet_mode=, :singleton do |d|
                d.push_context
                d.push_literal :@@quiet_mode
                d.push_local 0
                d.send :class_variable_set, 2
              end
            end)

  add_tests("cvdecl",
            "Compiler" => bytecode do |g|
              in_class :X do |d|
                d.push :self
                d.push_literal :@@blah
                d.push 1
                d.send :class_variable_set, 2
              end
            end)

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

  add_tests("defn_args_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.block_arg 0
                d.push :nil
              end
            end)

  add_tests("defn_args_mand",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.block_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_opt",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_opt_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 1
                d.block_arg 2
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_opt_splat",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_opt_splat_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 1
                d.block_arg 3
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_opt_splat_no_name",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.optional_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_splat",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.push :nil
              end
            end)

  add_tests("defn_args_mand_splat_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.block_arg 2
                d.push :nil
              end
            end)

  add_tests("defn_args_opt",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 0
                d.push :nil
              end
            end)

  add_tests("defn_args_opt_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 0
                d.block_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_args_opt_splat",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 0
                d.push :nil
              end
            end)

  add_tests("defn_args_opt_splat_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.optional_arg 0
                d.block_arg 2
                d.push :nil
              end
            end)

  add_tests("defn_args_opt_splat_no_name",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.optional_arg 0
                d.push :nil
              end
            end)

  add_tests("defn_args_splat",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.push :nil
              end
            end)

  add_tests("defn_args_splat_block",
            "Compiler" => bytecode do |g|
              in_method :f do |d|
                d.block_arg 1
                d.push :nil
              end
            end)

  add_tests("defn_empty",
            "Compiler" => bytecode do |g|
              in_method :empty do |d|
                d.push :nil
              end
            end)

  add_tests("defn_empty_args",
            "Compiler" => bytecode do |g|
              in_method :empty do |d|
                d.push :nil
              end
            end)

  add_tests("defn_lvar_boundary",
            "Compiler" => bytecode do |g|
              # TODO: this one is _all_brokey_ where's the puts???
              g.push 42
              g.set_local 0
              g.pop

              in_method :instantiate_all do |d|
                d.push_const :Thread

                d.create_block_desc do |d2|
                  block_top  = d2.new_label
                  d2.pop
                  d2.push_modifiers
                  block_top.set!
                  d2.push_modifiers
                  d2.push :nil
                  d2.pop_modifiers
                  d2.pop_modifiers
                  d2.ret # TODO: maybe refactor this into create_block_desc
                end

                top         = d.new_label
                dunno1      = d.new_label
                long_return = d.new_label
                dunno2      = d.new_label
                bottom      = d.new_label

                top.set!

                d.push_cpath_top
                d.find_const :LongReturnException
                d.send :allocate, 0
                d.set_local 0
                d.pop

                d.send_with_block :new, 0, false

                d.goto bottom

                dunno1.set!

                d.push_exception
                d.dup
                d.push_local 0
                d.equal
                d.gif long_return
                d.clear_exception

                d.dup
                d.send :is_return, 0
                d.gif dunno2
                d.send :value, 0
                d.ret

                long_return.set!

                d.raise_exc

                dunno2.set!

                d.send :value, 0

                bottom.set!
              end
            end)

  add_tests("defn_optargs",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push :self
                d.push_local 0
                d.push_local 1
                d.send :p, 2, true
              end
            end)

  add_tests("defn_or",
            "Compiler" => bytecode do |g|
              in_method :"|" do |d|
                d.push :nil
              end
            end)

  add_tests("defn_rescue",
            "Compiler" => bytecode do |g|
              in_method :eql? do |d|
                top  = d.new_label
                dunno1  = d.new_label
                stderr = d.new_label
                other = d.new_label
                bottom = d.new_label

                d.push_modifiers

                top.set!
                top.set! # TODO: fix

                d.push :self
                d.send :uuid, 0, false
                d.push_local 0
                d.send :uuid, 0, false
                d.meta_send_op_equal
                d.goto bottom

                dunno1.set!

                d.push_const :StandardError
                d.push_exception
                d.send :===, 1
                d.git stderr
                d.goto other

                stderr.set!

                d.push :false
                d.clear_exception
                d.goto bottom # Is this the reason for the second set?

                other.set!

                d.push_exception
                d.raise_exc

                bottom.set!
                bottom.set! # TODO: fix

                d.pop_modifiers
              end
            end)

  add_tests("defn_rescue_mri_verbose_flag",
            "Compiler" => testcases["defn_rescue"]["Compiler"])

  add_tests("defn_something_eh",
            "Compiler" => bytecode do |g|
              in_method :something? do |d|
                d.push :nil
              end
            end)

  add_tests("defn_splat_no_name",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push :self
                d.push_local 0
                d.send :p, 1, true
              end
            end)

  add_tests("defn_zarray",
            "Compiler" => bytecode do |g|
              in_method :zarray do |d|
                d.make_array 0
                d.set_local 0
                d.pop
                d.push_local 0
                d.ret # TODO: fix?
              end
            end)

  add_tests("defs",
            "Compiler" => bytecode do |g|
              g.push :self
              in_method :x, true do |d|
                d.push_local 0
                d.push 1
                d.meta_send_op_plus
              end
            end)

  add_tests("defs_empty",
            "Compiler" => bytecode do |g|
              g.push :self
              in_method :empty, true do |d|
                d.push :nil
              end
            end)

  add_tests("defs_empty_args",
            "Compiler" => bytecode do |g|
              g.push :self
              in_method :empty, true do |d|
                d.push :nil
              end
            end)

  add_tests("dmethod",
            "Compiler" => :skip)

  add_tests("dot2",
            "Compiler" => :skip)

  add_tests("dot3",
            "Compiler" => :skip)

  add_tests("dregx",
            "Compiler" => bytecode do |g|
              g.push_const :Regexp

              g.push_literal "y"    # 1
              g.string_dup

              g.push 1              # 2
              g.push 1
              g.meta_send_op_plus
              g.send :to_s, 0, true

              g.push_literal "x"    # 3
              g.string_dup

              2.times do
                g.string_append
              end

              g.push 0
              g.send :new, 2
            end)

  add_tests("dregx_interp",
            "Compiler" => bytecode do |g|
              g.push_const :Regexp

              g.push_ivar :@rakefile
              g.send :to_s, 0, true

              g.push_literal ""
              g.string_dup

              g.string_append

              g.push 0
              g.send :new, 2
            end)

  add_tests("dregx_n",
            "Compiler" => bytecode do |g|
              g.push_const :Regexp

              g.push 1
              g.send :to_s, 0, true

              g.push_literal ""
              g.string_dup

              g.string_append

              g.push 16
              g.send :new, 2
            end)

  add_tests("dregx_once",
            "Compiler" => bytecode do |g|
              memoize do
                g.push_const :Regexp

                g.push_literal "y"    # 1
                g.string_dup

                g.push 1              # 2
                g.push 1
                g.meta_send_op_plus
                g.send :to_s, 0, true

                g.push_literal "x"    # 3
                g.string_dup

                2.times do
                  g.string_append
                end

                g.push 0
                g.send :new, 2
              end
            end)

  add_tests("dregx_once_n_interp",
            "Compiler" => bytecode do |g|
              memoize do
                g.push_const :Regexp

                g.push_const :SB      # 1
                g.send :to_s, 0, true

                g.push_const :IAC     # 2
                g.send :to_s, 0, true

                g.push_literal ""     # 3
                g.string_dup

                2.times do
                  g.string_append
                end

                g.push 16
                g.send :new, 2
              end
            end)

  add_tests("dstr",
            "Compiler" => bytecode do |g|
              g.push 1
              g.set_local 0
              g.pop

              g.push_literal "y"    # 1
              g.string_dup

              g.push_local 0        # 2
              g.send :to_s, 0, true

              g.push_literal "x"    # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_2",
            "Compiler" => bytecode do |g|
              g.push 1
              g.set_local 0
              g.pop

              g.push_literal "y"    # 1
              g.string_dup

              g.push_literal "%.2f" # 2
              g.string_dup
              g.push 3.14159
              g.send :%, 1, false
              g.send :to_s, 0, true

              g.push_literal "x"    # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_3",
            "Compiler" => bytecode do |g|
              g.push 2
              g.set_local 0
              g.pop
              g.push 1
              g.set_local 1
              g.pop

              g.push_literal "y"  # - # 1
              g.string_dup

              g.push_literal "f"  # 1
              g.string_dup

              g.push_local 0      # 2
              g.send :to_s, 0, true

              g.push_literal "%." # 3
              g.string_dup

              2.times do
                g.string_append
              end

              g.push 3.14159      # - # 2
              g.send :%, 1, false
              g.send :to_s, 0, true

              g.push_literal "x"  # - # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_concat",
            "Compiler" => bytecode do |g|
              g.push 66             # 1
              g.send :to_s, 0, true

              g.push_literal "55"   # 2
              g.string_dup

              g.push 44             # 3
              g.send :to_s, 0, true

              g.push_literal "cd"   # 4
              g.string_dup

              g.push_literal "aa"   # 5
              g.string_dup

              g.push 22             # 6
              g.send :to_s, 0, true

              g.push_literal ""     # 7
              g.string_dup

              6.times do
                g.string_append
              end
            end)

  add_tests("dstr_gross",
            "Compiler" => bytecode do |g|
              g.push_literal " d"           # 1
              g.string_dup

              g.push_context                # 2
              g.push_literal :@@cvar
              g.send :class_variable_get, 1
              g.send :to_s, 0, true

              g.push_literal " c "          # 3
              g.string_dup

              g.push_ivar :@ivar            # 4
              g.send :to_s, 0, true

              g.push_literal " b "          # 5
              g.string_dup

              g.push_cpath_top              # 6
              g.find_const :Globals
              g.push_literal :$global
              g.send :[], 1
              g.send :to_s, 0, true

              g.push_literal "a "           # 7
              g.string_dup

              6.times do
                g.string_append
              end
            end)

  add_tests("dstr_heredoc_expand",
            "Compiler" => bytecode do |g|
              g.push_literal "blah\n"   # 1
              g.string_dup

              g.push 1                  # 2
              g.push 1
              g.meta_send_op_plus
              g.send :to_s, 0, true

              g.push_literal "  blah\n" # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_heredoc_windoze_sucks",
            "Compiler" => bytecode do |g|
              g.push_literal "_valid_feed\n" # 1
              g.string_dup

              g.push :self                   # 2
              g.send :action, 0, true
              g.send :to_s, 0, true

              g.push_literal "def test_"     # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_heredoc_yet_again",
            "Compiler" => bytecode do |g|
              g.push_literal "\n"         # 1
              g.string_dup

              g.push_literal "(eval)"     # 2
              g.string_dup

              g.push_literal "' s2\n"     # 3
              g.string_dup

              g.push_const :RUBY_PLATFORM # 4
              g.send :to_s, 0, true

              g.push_literal "s1 '"       # 5
              g.string_dup

              4.times do
                g.string_append
              end
            end)

  add_tests("dstr_nest",
            "Compiler" => bytecode do |g|
              g.push_literal "] after"  # 1
              g.string_dup

              g.push :self              # 2
              g.send :nest, 0, true
              g.send :to_s, 0, true

              g.push_literal "before [" # 3
              g.string_dup

              2.times do
                g.string_append
              end
            end)

  add_tests("dstr_str_lit_start",
            "Compiler" => bytecode do |g|
              g.push_literal ")"           # 1
              g.string_dup

              g.push_exception             # 2
              g.send :class, 0, false
              g.send :to_s, 0, true

              g.push_literal " ("          # 3
              g.string_dup

              g.push_exception             # 4
              g.send :message, 0, false
              g.send :to_s, 0, true

              g.push_literal ": warning: " # 5
              g.string_dup

              g.push 1                     # 6
              g.send :to_s, 0, true

              g.push_literal "blah(eval):" # 7
              g.string_dup

              6.times do
                g.string_append
              end
            end)

  add_tests("dstr_the_revenge",
            "Compiler" => bytecode do |g|
              g.push_literal ")"        # 1
              g.string_dup

              g.push 1                  # 2
              g.send :to_s, 0, true

              g.push_literal ":"        # 3
              g.string_dup

              g.push_literal "(eval)"   # 4
              g.string_dup

              g.push_literal " ("       # 5
              g.string_dup

              g.push :self              # 6
              g.send :to, 0, true
              g.send :to_s, 0, true

              g.push_literal " middle " # 7
              g.string_dup

              g.push :self              # 8
              g.send :from, 0, true
              g.send :to_s, 0, true

              g.push_literal "before "  # 9
              g.string_dup

              8.times do
                g.string_append
              end
            end)

  add_tests("dsym",
            "Compiler" => bytecode do |g|
                g.push_literal "y"
                g.string_dup

                g.push 1
                g.push 1
                g.meta_send_op_plus
                g.send :to_s, 0, true

                g.push_literal "x"
                g.string_dup

                g.string_append
                g.string_append

                g.send :to_sym, 0, true
            end)

  add_tests("dxstr",
            "Compiler" => bytecode do |g|
              g.push 5
              g.set_local 0
              g.pop

              g.push :self
              g.push_local 0
              g.send :to_s, 0, true

              g.push_literal "touch "
              g.string_dup

              g.string_append

              g.send :"`", 1, true
            end)

  add_tests("ensure",
            "Compiler" => :skip)

  add_tests("false",
            "Compiler" => bytecode do |g|
              g.push :false
            end)

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
              g.push(-1)
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
            "Compiler" => bytecode do |g|
              in_module :X do |d|
                d.in_method :y do |d2|
                  d2.push :nil
                end
              end
            end)

  add_tests("module_scoped",
            "Compiler" => bytecode do |g|
              in_module "X::Y" do |d|
                d.push :self
                d.send :c, 0, true
              end
            end)

  add_tests("module_scoped3",
            "Compiler" => bytecode do |g|
              in_module :Y do |d|
                d.push :self
                d.send :c, 0, true
              end
            end)

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
            "Compiler" => bytecode do |g|
              g.push :nil
              g.ret
            end)

  add_tests("return_1",
            "Compiler" => bytecode do |g|
              g.push 1
              g.ret
            end)

  add_tests("return_n",
            "Compiler" => bytecode do |g|
              g.push 1
              g.push 2
              g.push 3
              g.make_array 3
              g.ret
            end)

  add_tests("sclass",
            "Compiler" => :skip)

  add_tests("sclass_trailing_class",
            "Compiler" => :skip)

  add_tests("splat",
            "Compiler" => :skip)

  add_tests("str",
            "Compiler" => bytecode do |g|
              g.push_literal "x"
              g.string_dup
            end)

  add_tests("str_concat_newline",
            "Compiler" => bytecode do |g|
              g.push_literal "before after"
              g.string_dup
            end)

  add_tests("str_concat_space",
            "Compiler" => testcases["str_concat_newline"]["Compiler"])

  add_tests("str_heredoc",
            "Compiler" => bytecode do |g|
              g.push_literal "  blah\nblah\n"
              g.string_dup
            end)

  add_tests("str_heredoc_call",
            "Compiler" => bytecode do |g|
              g.push_literal "  blah\nblah\n"
              g.string_dup
              g.send :strip, 0, false
            end)

#   "Ruby"         => "a += <<-H1 + b + <<-H2\n  first\nH1\n  second\nH2",
#   "ParseTree"    => s(:lasgn, :a,
#                       s(:call,
#                         s(:lvar, :a),
#                         :+,
#                         s(:arglist,
#                           s(:call,
#                             s(:call, s(:str, "  first\n"), :+,
#                               s(:arglist,
#                                 s(:call, nil, :b, s(:arglist)))),
#                             :+,
#                             s(:arglist, s(:str, "  second\n")))))),

  add_tests("str_heredoc_double",
            "Compiler" => bytecode do |g|
              g.push_local 0

              g.push_literal "  first\n"
              g.string_dup

              g.push :self
              g.send :b, 0, true
              g.meta_send_op_plus

              g.push_literal "  second\n"
              g.string_dup

              g.meta_send_op_plus
              g.meta_send_op_plus

              g.set_local 0
            end)

  add_tests("str_heredoc_indent",
            "Compiler" => bytecode do |g|
              g.push_literal "  blah\nblah\n\n"
              g.string_dup
            end)

  add_tests("str_interp_file",
            "Compiler" => bytecode do |g|
              g.push_literal "file = (eval)\n"
              g.string_dup
            end)

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
            "Compiler" => bytecode do |g|
              g.push_block
              g.meta_send_call 0
            end)

  add_tests("yield_1",
            "Compiler" => bytecode do |g|
              g.push_block
              g.push 42
              g.meta_send_call 1
            end)

  add_tests("yield_n",
            "Compiler" => bytecode do |g|
              g.push_block
              g.push 42
              g.push 24
              g.make_array 2
              g.meta_send_call 1
            end)

  add_tests("zarray",
            "Compiler" => bytecode do |g|
              g.make_array 0
              g.set_local 0
            end)

  add_tests("zsuper",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push :nil
                d.push :nil
                d.send_super :x, 0
              end
            end)
end

describe "Compiler::*Nodes" do
  ParseTreeTestCase.testcases.sort.each do |node, hash|
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
