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

  add_tests("alias",
            "Compiler" => bytecode do |g|
              in_class :X do |d|
                d.push_context
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
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :b, 0, true
              g.make_array 1
              g.push :self
              g.send :c, 0, true
              g.cast_array
              g.send :+, 1
              g.set_local 0
            end)

  add_tests("argscat_svalue",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push :self
              g.send :b, 0, true
              g.push :self
              g.send :c, 0, true
              g.make_array 2
              g.push :self
              g.send :d, 0, true

              g.cast_array
              g.send :+, 1
              g.cast_array
              g.dup
              g.send :size, 0
              g.push 1
              g.swap
              g.send :<, 1 # TODO: or empty?

              g.git t

              g.push 0
              g.send :at, 1

              t.set!

              g.set_local 0
            end)

  add_tests("argspush",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true
              g.push :self
              g.send :b, 0, true
              g.cast_array
              g.push :self
              g.send :c, 0, true
              g.swap
              g.push :nil
              g.send_with_splat :[]=, 1, false, true
            end)

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
            "Compiler" => bytecode do |g|
              g.push 0
              g.set_local 0
              g.pop
              g.push 42
              g.push_local 0
              g.send :method=, 1, false
            end)

  add_tests("attrasgn_index_equals",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true
              g.push 42
              g.push 24
              g.send :[]=, 2, false
            end)

  add_tests("attrasgn_index_equals_space",
            "Compiler" => bytecode do |g|
              g.make_array 0
              g.set_local 0
              g.pop
              g.push_local 0
              g.push 42
              g.push 24
              g.send :[]=, 2, false
            end)

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

  add_tests("break",
            "Compiler" => bytecode do |g|
              break_value = :nil # TODO: refactor later

              top   = g.new_label
              cond  = g.new_label
              rtry  = g.new_label
              brk   = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.gif cond

              g.push break_value
              g.goto brk
              g.goto rtry # TODO: only used when there is a retry statement

              cond.set!
              g.push :nil

              rtry.set!
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
              rtry  = g.new_label
              brk   = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.gif cond

              g.push break_value
              g.goto brk
              g.goto rtry # TODO: only used when there is a retry statement

              cond.set!
              g.push :nil

              rtry.set!
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

  add_tests("call_arglist_space",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push 1
              g.push 2
              g.push 3
              g.send :a, 3, true
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
            "Compiler" => bytecode do |g|
              g.push_context
              g.push_literal :X
              g.push 42
              g.send :__const_set__, 2
            end)

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
              g.push_context
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

#   a.each { |x|
#     b.each { |y|
#       x = (x + 1)
#     } if true
#   }

#   add_tests("dasgn_0",
#             "Compiler" => bytecode do |g|
#               g.push :self
#               g.send :a, 0, true
#               g.in_block_send :each, 1, 0, false do |d|
#                 f = d.new_label

#                 d.push :true
#                 d.gif f

#                 d.push :self
#                 d.send :b, 0, true
#                 d.in_block_send :each, 1, 0, false do |d2|
#                   d2.push_local_depth 1, 0
#                   d2.push 1
#                   d2.meta_send_op_plus
#                   d2.set_local_depth 1, 0
#                 end

#                 f.set!
#               end
#             end)

  add_tests("dasgn_0",
            "Compiler" => :skip)

#   a.each { |x|
#     b.each { |y|
#       c = (c + 1)
#     } if true
#   }

#   add_tests("dasgn_1",
#             "Compiler" => bytecode do |g|
#               g.push :self
#               g.send :a, 0, true
#               g.in_block_send :each, 1, 0, false do |d|
#                 f = d.new_label

#                 d.push :true
#                 d.gif f

#                 d.push :self
#                 d.send :b, 0, true
#                 d.in_block_send :each, 1, 0, false do |d2|
#                   d2.push_local_depth 0, 1
#                   d2.push 1
#                   d2.meta_send_op_plus
#                   d2.set_local_depth 0, 1
#                 end

#                 f.set!
#               end
#             end)

  add_tests("dasgn_1",
            "Compiler" => :skip)

# a.each do |x|
#   if true then
#     c = 0
#     b.each { |y| c = (c + 1) }
#   end
# end

#   add_tests("dasgn_2",
#             "Compiler" => bytecode do |g|
#               g.push :self
#               g.send :a, 0, true
#               g.in_block_send :each, 1, 0, false do |d|
#                 f = d.new_label

#                 d.push :true
#                 d.gif f

#                 d.push 0
#                 d.set_local_depth 0, 1
#                 d.pop

#                 d.push :self
#                 d.send :b, 0, true
#                 d.in_block_send :each, 1, 0, false do |d2|
#                   d2.push_local_depth 1, 1
#                   d2.push 1
#                   d2.meta_send_op_plus
#                   d2.set_local_depth 1, 1
#                 end

#                 f.set!
#               end
#             end)

  add_tests("dasgn_2",
            "Compiler" => :skip)

  add_tests("dasgn_curr",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :data, 0, true
              g.in_block_send :each, 2, 0, false do |d|
                d.push 1
                d.set_local_depth 0, 2
                d.pop

                d.push_local_depth 0, 2
                d.set_local_depth 0, 3
                d.pop

                d.push_local_depth 0, 0
                d.set_local_depth 0, 2
                d.set_local_depth 0, 3
              end
            end)

# a do
#   v = nil
#   assert_block(full_message) do
#     begin
#       yield
#     rescue Exception => v
#       break
#     end
#   end
# end

# TODO: should be pretty... but isn't
#   add_tests("dasgn_icky",
#             "Compiler" => bytecode do |g|
#               g.push :self
#               g.in_block_send :a do |d|
#                 d.push :nil
#                 d.set_local_depth 0, 0
#                 d.pop
#
#                 d.push :self
#                 d.push :self
#                 d.send :full_message, 0, true
#
#                 d.in_block_send :assert_block, 0, 1 do |d2|
#                   d2.in_rescue :Exception do |good_side|
#                     if good_side then
#                       d2.push_block
#                       d2.meta_send_call 0
#                     else
#                       d2.push_exception
#                       d2.set_local_depth 1, 0
#                       d2.push :nil
#                       d2.push_local 0
#                       d2.swap
#                       d2.send :break_value=, 1
#                       d2.pop
#                       d2.push_local 0
#                       d2.raise_exc
#                     end
#                   end
#                 end
#               end
#             end)

  add_tests("dasgn_icky",
            "Compiler" => :skip)

  add_tests("dasgn_mixed",
            "Compiler" => bytecode do |g|
              g.push 0
              g.set_local 0
              g.pop

              g.push :self
              g.send :ns, 0, true

              in_block_send :each, 1, 0, false, 1 do |d|
                d.push_local 0
                d.push_local_depth 0, 0
                d.meta_send_op_plus
                d.set_local 0
              end
            end)

  add_tests("defined",
            "Compiler" => bytecode do |g|
              t = g.new_label
              f = g.new_label

              g.push_const :Globals
              g.push_literal :$x
              g.send :key?, 1
              g.git t

              g.push :nil
              g.goto f

              t.set!
              g.push_literal "global-variable"

              f.set!
            end)

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
                d.in_rescue :StandardError do |good_side|
                  if good_side then
                    d.push :self
                    d.send :uuid, 0, false
                    d.push_local 0
                    d.send :uuid, 0, false
                    d.meta_send_op_equal
                  else
                    d.push :false
                  end
                end
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
                # TODO we emit a ret instruction even though the last statement
                # is itself a return, so we get to return instructions, one
                # after another. We could instead detect that an only output
                # the one.
                d.ret
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

  add_tests("dot2",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Range
              g.push :self
              g.send :a, 0, true
              g.push :self
              g.send :b, 0, true
              g.send :new, 2
            end)

  add_tests("dot3",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Range
              g.push :self
              g.send :a, 0, true
              g.push :self
              g.send :b, 0, true
              g.push :true
              g.send :new, 3
            end)

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

  # begin
  #   (1 + 1)
  # rescue SyntaxError => e1
  #   2
  # rescue Exception => e2
  #   3
  # else
  #   4
  # ensure
  #   5
  # end

  add_tests("ensure",
            "Compiler" => bytecode do |g|
              # TODO: refactor in_rescue to work with this... I think I have the
              # pattern down now

              top      = g.new_label
              bottom   = g.new_label
              label_1  = g.new_label
              label_5  = g.new_label
              label_10 = g.new_label
              label_14 = g.new_label
              label_19 = g.new_label
              label_24 = g.new_label
              label_26 = g.new_label
              label_28 = g.new_label
              label_30 = g.new_label

              top.set!

              g.push_modifiers

              label_1.set!
              label_1.set!

              g.push 1
              g.push 1
              g.meta_send_op_plus

              g.goto label_26

              label_5.set!

              g.push_const :SyntaxError
              g.push_exception
              g.send :===, 1
              g.git label_10
              g.goto label_14

              label_10.set!

              g.push_exception
              g.set_local 0
              g.push 2
              g.goto label_28

              label_14.set!

              g.push_const :Exception
              g.push_exception
              g.send :===, 1
              g.git label_19
              g.goto label_24

              label_19.set!

              g.push_exception
              g.set_local 1
              g.push 3
              g.clear_exception
              g.goto label_28

              label_24.set!

              g.push_exception
              g.raise_exc

              label_26.set!

              g.pop
              g.push 4

              label_28.set!

              g.pop_modifiers
              g.goto bottom

              label_30.set!

              g.push 5
              g.pop
              g.push_exception
              g.raise_exc

              bottom.set!

              g.push 5
              g.pop
            end)

  add_tests("false",
            "Compiler" => bytecode do |g|
              g.push :false
            end)

  add_tests("fcall_arglist",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push 42
              g.send :m, 1, true
            end)

  add_tests("fcall_arglist_hash",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push_cpath_top
              g.find_const :Hash

              g.push_unique_literal :a
              g.push 1
              g.push_unique_literal :b
              g.push 2

              g.send :[], 4

              g.send :m, 1, true
            end)

  add_tests("fcall_arglist_norm_hash",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push 42
              g.push_cpath_top
              g.find_const :Hash

              g.push_unique_literal :a
              g.push 1
              g.push_unique_literal :b
              g.push 2

              g.send :[], 4

              g.send :m, 2, true
            end)

  add_tests("fcall_arglist_norm_hash_splat",
            "Compiler" => bytecode do |g|
              g.push :self
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

              g.send_with_splat :m, 2, true, false
            end)

  add_tests("fcall_block",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push_unique_literal :b

              g.in_block_send :a, 0, 1 do |d|
                d.push_unique_literal :c
              end
            end)

  add_tests("fcall_index_space",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push 42
              g.make_array 1
              g.send :a, 1, true
            end)

  add_tests("fcall_keyword",
            "Compiler" => bytecode do |g|
              t = g.new_label
              f = g.new_label

              g.push_block
              g.gif f
              g.push 42
              g.goto t
              f.set!
              g.push :nil
              t.set!
            end)

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
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :"$x" # REFACTOR g.get_global("$x")
              g.push 42
              g.send :[]=, 2
            end)

  add_tests("global",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :"$stderr"
              g.send :[], 1
            end)

  add_tests("gvar",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :"$x" # REFACTOR g.get_global("$x")
              g.send :[], 1
            end)

  add_tests("gvar_underscore",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :"$_"
              g.send :[], 1
            end)

  add_tests("gvar_underscore_blah",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :"$__blah"
              g.send :[], 1
            end)

  add_tests("hash",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Hash
              g.push 1
              g.push 2
              g.push 3
              g.push 4
              g.send :[], 4
            end)

  add_tests("hash_rescue",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Hash
              g.push 1

              g.in_rescue :StandardError do |good_side|
                if good_side then
                  g.push 2
                else
                  g.push 3
                end
              end

              g.send :[], 2
            end)

  add_tests("iasgn",
            "Compiler" => bytecode do |g|
              g.push 4
              g.set_ivar :@a
            end)

  add_tests("if_block_condition",
            "Compiler" => bytecode do |g|
              f      = g.new_label
              bottom = g.new_label

              g.push 5
              g.set_local 0
              g.pop

              g.push_local 0
              g.push 1
              g.meta_send_op_plus
              g.gif f

              g.push :nil
              g.goto bottom

              f.set!
              g.push :nil

              bottom.set!
            end)

  add_tests("if_lasgn_short",
            "Compiler" => bytecode do |g|
              f      = g.new_label
              bottom = g.new_label

              g.push :self
              g.send :obj, 0, true
              g.send :x, 0, false
              g.set_local 0

              g.gif f

              g.push_local 0
              g.send :do_it, 0, false
              g.goto bottom

              f.set!
              g.push :nil

              bottom.set!
            end)

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

  add_tests("iteration_call_arglist_space",
            "Compiler" => :skip)

  add_tests("iteration_dasgn_curr_dasgn_madness",
            "Compiler" => :skip)

  add_tests("iteration_double_var",
            "Compiler" => :skip)

  add_tests("iteration_masgn",
            "Compiler" => :skip)

  add_tests("lasgn_array",
            "Compiler" => bytecode do |g|
              g.push_literal "foo"
              g.string_dup
              g.push_literal "bar"
              g.string_dup
              g.make_array 2
              g.set_local 0
            end)

  add_tests("lasgn_call",
            "Compiler" => bytecode do |g|
              g.push 2
              g.push 3
              g.meta_send_op_plus
              g.set_local 0
            end)

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
            "Compiler" => bytecode do |g|
              g.push 42
              g.set_local 0
              g.pop

              in_method :a do |d|
                d.push :self

                d.in_block_send :c, 0 do |d2|
                  d2.in_rescue :RuntimeError do |good_side|
                    if good_side then
                      d2.push :self
                      d2.send :do_stuff, 0, true
                    else
                      d2.push_exception
                      d2.set_local_depth 0, 0
                      d2.push :self
                      d2.push_local_depth 0, 0
                      d2.send :puts, 1, true
                    end
                  end
                end
              end
            end)

  add_tests("masgn",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :c, 0, true
              g.push :self
              g.send :d, 0, true
              g.rotate 2

              g.set_local 0
              g.pop

              g.set_local 1
              g.pop

              g.push :true
            end)

  add_tests("masgn_argscat",
            "Compiler" => bytecode do |g|
              g.push 1
              g.push 2
              g.make_array 2
              g.push 3
              g.push 4
              g.make_array 2
              g.cast_array
              g.send :+, 1
              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.cast_array
              g.set_local 2
              g.pop

              g.push :true
            end)

  add_tests("masgn_attrasgn",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :d, 0, true
              g.push :self
              g.send :e, 0, true
              g.rotate 2

              g.set_local 0
              g.pop

              g.push :self
              g.send :b, 0, true
              g.send :c=, 1, false

              g.pop

              g.push :true
            end)

  add_tests("masgn_attrasgn_idx",
            "Compiler" => bytecode do |g|
              g.make_array 0
              g.push 1
              g.push 2
              g.rotate 3

              g.set_local 0
              g.pop

              g.set_local 1
              g.pop

              g.set_local 2
              g.pop

              g.push :true
              g.pop

              g.push_local 0
              g.push_local 2
              g.send :[], 1, false
              g.push_local 0
              g.push_local 1
              g.send :[], 1, false
              g.rotate 2

              g.push_local 0
              g.push_local 1
              g.send :[]=, 2, false
              g.pop

              g.push_local 0
              g.push_local 2
              g.send :[]=, 2, false
              g.pop

              g.push :true
            end)

  add_tests("masgn_iasgn",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :c, 0, true
              g.push :self
              g.send :d, 0, true
              g.rotate 2

              g.set_local 0
              g.pop

              g.set_ivar :@b
              g.pop

              g.push :true
            end)

  add_tests("masgn_masgn",
            "Compiler" => bytecode do |g|
              g.push 1
              g.push 2
              g.push 3
              g.make_array 2
              g.make_array 2
              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.cast_tuple
              g.shift_tuple
              g.set_local 1
              g.pop

              g.shift_tuple
              g.set_local 2
              g.pop

              g.pop
              g.push :true # FIX: necessary?!?
              g.pop
              g.pop

              g.push :true
            end)

  add_tests("masgn_splat",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :d, 0, true
              g.push :self
              g.send :e, 0, true
              g.push :self
              g.send :f, 0, true
              g.push :self
              g.send :g, 0, true
              g.make_array 2 # TODO: 2?!?

              g.set_local 2 # TODO: backwards
              g.pop

              g.set_local 1
              g.pop

              g.set_local 0
              g.pop

              g.push :true
            end)

  add_tests("masgn_splat_no_name_to_ary",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :c, 0, true
              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.cast_array
              g.cast_array
              g.pop

              g.push :true
            end)

  add_tests("masgn_splat_no_name_trailing",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :c, 0, true
              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.pop # TODO: why?

              g.push :true
            end)

  add_tests("masgn_splat_to_ary",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :d, 0, true

              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.cast_array
              g.set_local 2
              g.pop

              g.push :true
            end)

  add_tests("masgn_splat_to_ary2",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :d, 0, true
              g.push_literal "f"
              g.string_dup
              g.send :e, 1, false

              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.cast_array
              g.set_local 2
              g.pop

              g.push :true
            end)

  add_tests("match",
            "Compiler" => bytecode do |g|
              g.push_literal :$_ # REFACTOR - we use this block a lot
              g.push_cpath_top
              g.find_const :Globals # FIX: find the other Globals, order flipped
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
            "Compiler" => bytecode do |g|
              top    = g.new_label
              bottom = g.new_label
              dunno1 = g.new_label
              f      = g.new_label

              g.push_modifiers
              top.set!

              g.push :false
              g.gif f
              g.goto top
              g.goto dunno1

              f.set!

              g.push :nil

              dunno1.set!
              g.pop
              g.goto top

              bottom.set!

              g.pop_modifiers
            end)

  add_tests("next_arg",
            "Compiler" => testcases['next']['Compiler']) # FIX: not the same

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
            "Compiler" => bytecode do |g|
              g.push_context
              g.push 1
              g.send :nth_ref, 1
            end)

  add_tests("op_asgn1",
            "Compiler" => bytecode do |g|
              l_or = g.new_label
              l_and = g.new_label
              l_idx = g.new_label
              l_rhs = g.new_label

              g.make_array 0
              g.set_local 0
              g.pop

              g.push_local 0
              g.dup
              g.push 1
              g.send :[], 1

              g.dup
              g.git l_or
              g.pop

              g.push 1
              g.push 10
              g.send :[]=, 2

              g.goto l_and

              l_or.set!

              g.swap
              g.pop

              l_and.set!

              g.pop
              g.push_local 0
              g.dup
              g.push 2
              g.send :[], 1
              g.dup
              g.gif l_idx

              g.pop
              g.push 2
              g.push 11
              g.send :[]=, 2
              g.goto l_rhs

              l_idx.set!

              g.swap
              g.pop

              l_rhs.set!

              g.pop
              g.push_local 0
              g.dup
              g.push 3
              g.send :[], 1

              g.push 12
              g.send :+, 1

              g.push 3
              g.swap
              g.send :[]=, 2
            end)

  add_tests("op_asgn1_ivar",
            "Compiler" => bytecode do |g|
              l_or = g.new_label
              l_and = g.new_label
              l_idx = g.new_label
              l_rhs = g.new_label

              g.make_array 0
              g.set_ivar :@b
              g.pop

              g.push_ivar :@b
              g.dup
              g.push 1
              g.send :[], 1

              g.dup
              g.git l_or
              g.pop

              g.push 1
              g.push 10
              g.send :[]=, 2

              g.goto l_and

              l_or.set!

              g.swap
              g.pop

              l_and.set!

              g.pop
              g.push_ivar :@b
              g.dup
              g.push 2
              g.send :[], 1
              g.dup
              g.gif l_idx

              g.pop
              g.push 2
              g.push 11
              g.send :[]=, 2
              g.goto l_rhs

              l_idx.set!

              g.swap
              g.pop

              l_rhs.set!

              g.pop
              g.push_ivar :@b
              g.dup
              g.push 3
              g.send :[], 1

              g.push 12
              g.send :+, 1

              g.push 3
              g.swap
              g.send :[]=, 2
            end)

  add_tests("op_asgn2",
            "Compiler" => bytecode do |g|
              l_or = g.new_label
              l_and = g.new_label
              l_plus = g.new_label
              l_or2 = g.new_label
              l_rhs = g.new_label
              bottom = g.new_label

              g.push_const :Struct
              g.push_unique_literal :var
              g.send :new, 1, false
              g.set_local 0
              g.pop

              g.push_local 0
              g.push :nil
              g.send :new, 1, false
              g.set_local 1
              g.pop

              g.push_local 1
              g.dup
              g.send :var, 0
              g.dup
              g.git l_or

              g.pop
              g.push 20
              g.send :var=, 1
              g.goto l_and

              l_or.set!

              g.swap
              g.pop

              l_and.set!

              g.pop
              g.push_local 1
              g.dup
              g.send :var, 0
              g.dup
              g.gif l_plus
              g.pop
              g.push 21
              g.send :var=, 1
              g.goto l_or2

              l_plus.set!

              g.swap
              g.pop

              l_or2.set!

              g.pop
              g.push_local 1
              g.dup
              g.send :var, 0
              g.push 22
              g.send :+, 1
              g.send :var=, 1
              g.pop

              g.push_local 1
              g.send :d, 0, false
              g.send :e, 0, false
              g.dup
              g.send :f, 0
              g.dup

              g.git l_rhs

              g.pop
              g.push 42
              g.send :f=, 1
              g.goto bottom

              l_rhs.set!

              g.swap
              g.pop

              bottom.set!
            end)

  add_tests("op_asgn2_self",
            "Compiler" => bytecode do |g|
              t = g.new_label
              f = g.new_label

              g.push :self
              g.dup
              g.send :Bag, 0
              g.dup
              g.git t
              g.pop
              g.push_const :Bag
              g.send :new, 0, false
              g.send :"Bag=", 1
              g.goto f

              t.set!

              g.swap
              g.pop

              f.set!
            end)

  add_tests("op_asgn_and",
            "Compiler" => bytecode do |g|
              g.push 0
              g.set_local 0
              g.pop

              g.push_local 0
              g.dup

              f = g.new_label
              g.gif f
              g.pop
              g.push 2
              g.set_local 0

              f.set!
            end)

  add_tests("op_asgn_and_ivar2",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push_ivar :@fetcher
              g.dup
              g.gif t
              g.pop

              g.push :self
              g.push_const :Gem
              g.send :configuration, 0, false

              g.push_unique_literal :http_proxy
              g.send :[], 1, false

              g.send :new, 1, true

              g.set_ivar :@fetcher

              t.set!
            end)

  add_tests("op_asgn_or",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push 0
              g.set_local 0
              g.pop             # FIX: lame
              g.push_local 0
              g.dup
              g.git t
              g.pop

              g.push 1

              g.set_local 0

              t.set!
            end)

  add_tests("op_asgn_or_block",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push_local 0
              g.dup
              g.git t
              g.pop

              in_rescue :StandardError do |good_side|
                if good_side then
                  g.push :self
                  g.send :b, 0, true
                else
                  g.push :self
                  g.send :c, 0, true
                end
              end

              g.set_local 0

              t.set!
            end)

  add_tests("op_asgn_or_ivar",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push_ivar :@v
              g.dup
              g.git t
              g.pop

              g.push_cpath_top
              g.find_const :Hash
              g.send :[], 0

              g.set_ivar :@v

              t.set!
            end)

  add_tests("op_asgn_or_ivar2",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push_ivar :@fetcher
              g.dup
              g.git t
              g.pop

              g.push :self
              g.push_const :Gem
              g.send :configuration, 0, false

              g.push_unique_literal :http_proxy
              g.send :[], 1, false

              g.send :new, 1, true

              g.set_ivar :@fetcher

              t.set!
            end)

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
            "Compiler" => bytecode do |g|
              j1 = g.new_label
              j2 = g.new_label

              g.push :self
              g.send :a, 0, true
              g.dup
              g.git j1
              g.pop

              g.push :self
              g.send :b, 0, true
              j1.set!
              g.dup
              g.git j2
              g.pop

              g.push :self
              g.send :c, 0, true
              g.dup
              g.gif j2
              g.pop

              g.push :self
              g.send :d, 0, true

              j2.set!
              j2.set!
            end)

  add_tests("or_big2",
            "Compiler" => testcases['or_big']['Compiler'])

  add_tests("parse_floats_as_args",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                opt_arg_1 = d.new_label
                opt_arg_2 = d.new_label

                d.passed_arg 0
                d.git opt_arg_1
                d.push 0.0
                d.set_local 0
                d.pop

                opt_arg_1.set!

                d.passed_arg 1
                d.git opt_arg_2
                d.push 0.0
                d.set_local 1
                d.pop

                opt_arg_2.set!

                d.push_local 0
                d.push_local 1
                d.meta_send_op_plus
              end
            end)

  add_tests("postexe",
            "Compiler" => bytecode do |g|
              g.push :self
              in_block_send :at_exit, 0 do |d|
                d.push 1
              end
            end)

  add_tests("proc_args_0",
            "Compiler" => bytecode do |g|
              g.push :self
              in_block_send :proc, 0 do |d|
                d.push :self
                d.send :x, 0, true
                d.push 1
                d.meta_send_op_plus
              end
            end)

  add_tests("proc_args_1",
            "Compiler" => bytecode do |g|
              g.push :self
              in_block_send :proc, 1 do |d|
                d.push_local_depth 0, 0
                d.push 1
                d.meta_send_op_plus
              end
            end)

  add_tests("proc_args_2",
            "Compiler" => bytecode do |g|
              g.push :self
              in_block_send :proc, 2 do |d|
                d.push_local_depth 0, 0
                d.push_local_depth 0, 1
                d.meta_send_op_plus
              end
            end)

  add_tests("proc_args_no", # TODO shouldn't 0 bitch if there are args?
            "Compiler" => testcases['proc_args_0']['Compiler'])

  add_tests("redo",
            "Compiler" => bytecode do |g|
              top = g.new_label
              f = g.new_label
              t = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!

              g.push :false
              g.gif f
              g.goto top
              g.goto t

              f.set!

              g.push :nil

              t.set!

              g.pop
              g.goto top

              bottom.set!

              g.pop_modifiers
            end)

  add_tests("rescue",
            "Compiler" => :skip)

  add_tests("rescue_block_body",
            "Compiler" => :skip)

  add_tests("rescue_block_nada",
            "Compiler" => :skip)

  add_tests("rescue_exceptions",
            "Compiler" => :skip)

  add_tests("retry",
            "Compiler" => bytecode do |g| # TODO: maybe have a real example?
              g.push :self
              g.push_const :LocalJumpError
              g.push_literal "retry used in invalid context"
              g.send :raise, 2, true
            end)

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
            "Compiler" => bytecode do |g|
                g.push :self
                g.dup
                g.send :__verify_metaclass__, 0 # TODO: maybe refactor...
                g.pop
                g.open_metaclass
                g.dup

                g.push_literal_desc do |d2|
                  d2.push 42
                  d2.ret
                end

                g.swap
                g.attach_method :__metaclass_init__
                g.pop
                g.send :__metaclass_init__, 0
            end)

  add_tests("sclass_trailing_class",
            "Compiler" => bytecode do |g|
              in_class :A do |d|
                d.push :self
                d.dup
                d.send :__verify_metaclass__, 0 # TODO: maybe refactor...
                d.pop
                d.open_metaclass
                d.dup

                d.push_literal_desc do |d2|
                  d2.push :self
                  d2.send :a, 0, true
                  d2.ret
                end

                d.swap
                d.attach_method :__metaclass_init__
                d.pop
                d.send :__metaclass_init__, 0
                d.pop
                d.push :nil
                d.open_class :B
              end
            end)

  add_tests("splat",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push :self
                d.push_local 0
                d.cast_array
                d.push :nil
                d.send_with_splat :a, 0, true, false
              end
            end)

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

#   a.b do |c, d|
#     unless e.f(c) then
#       g = false
#       d.h { |x, i| g = true }
#     end
#   end


#   add_tests("structure_extra_block_for_dvar_scoping",
#             "Compiler" => bytecode do |g|
#               g.push :self
#               g.send :a, 0, true

#               g.in_block_send :b, 2, 0, false do |d|
#                 f = d.new_label
#                 wtf = d.new_label

#                 d.push :self
#                 d.send :e, 0, true
#                 d.push_local_depth 0, 0
#                 d.send :f, 1, false
#                 d.git f

#                 d.push :false
#                 d.set_local_depth 0, 2
#                 d.pop

#                 d.push_local_depth 0, 1

#                 d.in_block_send :h, 2, 0, false, wtf do |d2|
#                   d2.push :true
#                   d2.set_local_depth 1, 2
#                 end

#                 f.set!
#               end
#             end)

  # FIX OMG this is HORRIBLE but the block-in-a-block scenario is too
  # intertwined to use the bytecode helper methods. It SHOULD look
  # something lke the above but nooooooo... that'd be too easy!
  add_tests("structure_extra_block_for_dvar_scoping",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true

             g.in_block_send :b, 2, 0, false do |d|
                f      = d.new_label
                bottom = d.new_label

                d.push :self
                d.send :e, 0, true
                d.push_local_depth 0, 0
                d.send :f, 1, false
                d.git bottom

                d.push :false
                d.set_local_depth 0, 2
                d.pop

                d.push_local_depth 0, 1

                ############################################################
                # d.in_block_send :h, 2, 0, false, :wtf do |d2|
                d2             = d
                msg            = :h
                block_count    = 2
                call_count     = 0
                block_send_vis = false
                wtf            = :wtf
                top            = d2.new_label
                dunno1         = d2.new_label
                dunno2         = d2.new_label
                dunno3         = d2.new_label
                dunno4         = d2.new_label
                uncaught       = d2.new_label

                d2.create_block_desc do |d3|
                  inner_top = d3.new_label

                  case block_count
                  when 0 then
                  when 1 then
                    d3.cast_for_single_block_arg
                    d3.set_local_depth 0, 0
                  else
                    d3.cast_for_multi_block_arg
                    (0...block_count).each do |n|
                      d3.shift_tuple
                      d3.set_local_depth 0, n
                      d3.pop
                    end
                  end

                  d3.pop

                  d3.push_modifiers
                  inner_top.set!

                  d3.push :true
                  d3.set_local_depth 1, 2

                  d3.pop_modifiers
                  d3.ret
                end

                top.set!

                d2.push_cpath_top
                d2.find_const :LongReturnException
                d2.send :allocate, 0
                d2.set_local 0
                d2.pop

                d2.send_with_block msg, call_count, block_send_vis
                d2.goto dunno3

                dunno1.set!

                d2.push_exception
                d2.dup
                d2.push_local 0
                d2.equal
                d2.gif uncaught
                d2.clear_exception
                d2.dup
                d2.send :is_return, 0
                d2.gif dunno2

                uncaught.set!
                d2.raise_exc

                dunno2.set!

                d2.send :value, 0

                dunno3.set!

                d2.goto f

                bottom.set!

                d2.push :nil

                # end
                ############################################################

                f.set!
             end
            end)

  add_tests("structure_remove_begin_1",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true

              in_rescue :StandardError do |good_side|
                if good_side then
                  g.push :self
                  g.send :b, 0, true
                else
                  g.push :self
                  g.send :c, 0, true
                end
              end

              g.send :<<, 1, false
            end)

  add_tests("structure_remove_begin_2",
            "Compiler" => bytecode do |g|
              bottom = g.new_label
              f      = g.new_label

              g.push :self
              g.send :c, 0, true
              g.gif f

              in_rescue :StandardError do |good_side|
                if good_side then
                  g.push :self
                  g.send :b, 0, true
                else
                  g.push :nil
                end
              end
              g.goto bottom

              f.set!
              g.push :nil

              bottom.set!
              g.set_local 0
              g.pop
              g.push_local 0
            end)

  add_tests("structure_unused_literal_wwtt",
            "Compiler" => bytecode do |g|
              g.open_module :Graffle
            end)

  add_tests("super",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push 4
                d.push :nil
                d.push :nil
                d.send_super :x, 1
              end
            end)

  add_tests("super_block_pass",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push :self
              g.send :a, 0, true

              g.push :nil

              g.push :self
              g.send :b, 0, true

              g.dup
              g.is_nil
              g.git t

              g.push_cpath_top
              g.find_const :Proc
              g.swap

              g.send :__from_block__, 1

              t.set!

              g.send_super nil, 1 # TODO: nil?
            end)

  add_tests("super_block_splat",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :a, 0, true

              g.push :self
              g.send :b, 0, true

              g.cast_array

              g.push :nil
              g.send_super nil, 1, true # TODO: nil?
            end)

  add_tests("super_multi",
            "Compiler" => bytecode do |g|
              in_method :x do |d|
                d.push 4
                d.push 2
                d.push 1
                d.push :nil
                d.push :nil
                d.send_super :x, 3
              end
            end)

  add_tests("svalue",
            "Compiler" => bytecode do |g|
              t = g.new_label

              g.push :self
              g.send :b, 0, true
              g.cast_array
              g.cast_array
              g.dup

              g.send :size, 0
              g.push 1
              g.swap
              g.send :<, 1
              g.git t

              g.push 0
              g.send :at, 1
              t.set!
              g.set_local 0
            end)

  add_tests("to_ary",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :c, 0, true
              g.cast_tuple

              g.shift_tuple
              g.set_local 0
              g.pop

              g.shift_tuple
              g.set_local 1
              g.pop

              g.pop
              g.push :true
            end)

  add_tests("true",
            "Compiler" => bytecode do |g|
              g.push :true
            end)

  add_tests("undef",
            "Compiler" => bytecode do |g|
              undef_bytecode :x
            end)

  add_tests("undef_2",
            "Compiler" => bytecode do |g|
              undef_bytecode :x, :y
            end)

  add_tests("undef_3",
            "Compiler" => bytecode do |g|
              undef_bytecode :x, :y, :z
            end)

  add_tests("undef_block_1",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :f1, 0, true
              g.pop

              undef_bytecode :x
            end)

  add_tests("undef_block_2",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :f1, 0, true
              g.pop

              undef_bytecode :x, :y
            end)

  add_tests("undef_block_3",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :f1, 0, true
              g.pop

              undef_bytecode :x, :y, :z
            end)

  add_tests("undef_block_3_post",
            "Compiler" => bytecode do |g|
              undef_bytecode :x, :y, :z
              g.pop

              g.push :self
              g.send :f2, 0, true
            end)

  add_tests("undef_block_wtf",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :f1, 0, true
              g.pop

              undef_bytecode :x, :y, :z
              g.pop

              g.push :self
              g.send :f2, 0, true
            end)

  add_tests("until_post",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!

              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              dunno1.set!

              g.push :false
              g.git bottom

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("until_post_not",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!

              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              dunno1.set!

              g.push :true
              g.gif bottom

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("until_pre",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push :false
              g.git bottom

              dunno1.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("until_pre_mod",
            "Compiler" => testcases["until_pre"]["Compiler"])

  add_tests("until_pre_not",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.gif bottom

              dunno1.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("until_pre_not_mod",
            "Compiler" => testcases["until_pre_not"]["Compiler"])

  add_tests("valias",
            "Compiler" => bytecode do |g|
              g.push_cpath_top
              g.find_const :Globals
              g.push_literal :$x
              g.push_literal :$y
              g.send :add_alias, 2
            end)

  add_tests("vcall",
            "Compiler" => bytecode do |g|
              g.push :self
              g.send :method, 0, true
            end)

  add_tests("while_post",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              dunno1.set!
              g.push :false
              g.gif bottom

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_post2",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push 1
              g.push 2
              g.meta_send_op_plus
              g.pop

              g.push 3
              g.push 4
              g.meta_send_op_plus
              g.pop

              dunno1.set!
              g.push :false
              g.gif bottom

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_post_not",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              dunno1.set!
              g.push :true
              g.git bottom

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_pre",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push :false
              g.gif bottom

              dunno1.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_pre_mod",
            "Compiler" => testcases["while_pre"]["Compiler"])

  add_tests("while_pre_nil",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push :false
              g.gif bottom

              dunno1.set!
              g.push :nil
              g.pop

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_pre_not",
            "Compiler" => bytecode do |g|
              top    = g.new_label
              dunno1 = g.new_label
              dunno2 = g.new_label
              bottom = g.new_label

              g.push_modifiers

              top.set!
              g.push :true
              g.git bottom

              dunno1.set!
              g.push 1
              g.push 1
              g.meta_send_op_plus
              g.pop

              g.goto top

              bottom.set!
              g.push :nil

              dunno2.set!

              g.pop_modifiers
            end)

  add_tests("while_pre_not_mod",
            "Compiler" => testcases["while_pre_not"]["Compiler"])

  add_tests("xstr",
            "Compiler" => bytecode do |g|
              g.push :self
              g.push_literal "touch 5"
              g.string_dup
              g.send :"`", 1, true
            end)

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
      node    = comp.convert_sexp s(:snippit, sexp)
      actual   = TestGenerator.new
      node.bytecode actual

      actual.should == expected
    end
  end
end
