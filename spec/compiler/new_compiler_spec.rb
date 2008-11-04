require File.dirname(__FILE__) + '/spec_helper'
require File.dirname(__FILE__) + '/../spec_helper'

alias :old_description :description
def description name = nil
  tg = TestGenerator.new
  yield tg
  s(:method_description, *Sexp.from_array(tg.stream))
end

class TestGenerator
  alias :old_slot :new_slot
  def new_slot
    @slot += 1
    @slot
  end
end

require File.dirname(__FILE__) + '/compiler_test'

class Symbol # TODO: nuke when we flip the compiler and can update the specs
  alias :old_eq2 :==
  def == o
    case o
    when TestGenerator::Label
      old_eq2 o.to_sym
    else
      old_eq2 o
    end
  end
end

class NewCompiler < SexpProcessor
  def initialize _ignored = nil
    super()
    self.auto_shift_type = true
    self.strict = false

    @slots = {}
    @current_slot = -1
    @jump = @literal = 0
    @jump_handled = nil
  end

  def process exp
    exp = super exp

    exp = strip_dummies exp

    if exp.first == :dummy then
      exp[0] = :test_generator
    else
      exp = s(:test_generator, exp)
    end if self.context.empty?

    exp
  end

  def process_and exp
    result = s(:dummy)

    fixme = [] # TODO: see angry note in process_or

    until exp.empty? do
      result << process(exp.shift)
      result << s(:dup)
      result << s(:gif)
      fixme  << result.last
      result << s(:pop)
    end
    3.times { result.pop }

    bottom = new_jump
    fixme.each { |s| s << bottom }
    result << s(:set_label, bottom)

    result
  end

  def process_arglist exp
    result = s(:dummy)
    until exp.empty? do
      result << process(exp.shift)
    end
    result
  end

  def process_args exp
    result = s(:dummy)

    @slots.clear
    block_arg = nil
    opt_args = exp.block(true)

    until exp.empty? do
      arg = exp.shift
      slot = new_slot

      case arg.to_s
      when /^\*(.*)/ then
        next if $1.empty?
        arg = $1.to_sym
      when /^\&(.+)/ then
        arg = $1.to_sym
        block_arg = slot
      end

      @slots[arg] = slot
    end

    if opt_args then
      opt_args.map! { |opt_arg|
        next opt_arg if Symbol === opt_arg
        name, val = opt_arg[1..2]
        s(:s_if, s(:passed_arg, name2slot(name)),
          s(:lasgn, name, val),
          s(:pop))
      }

      opt_args[0] = :dummy
      result[1, 0] = process(opt_args)[1..-1]
    end

    if block_arg then
      result << s(:push_block) << s(:dup)
      result << process(s(:s_if, s(:is_nil),
                          s(:push_const, :Proc),
                          s(:swap),
                          s(:send, :__from_block__, 1)))
      result << s(:set_local, slot) << s(:pop)
    end

    result
  end

  def process_argscat exp
    s(:dummy,
      process(exp.shift),
      process(exp.shift),
      s(:cast_array),
      s(:send, :+, 1))
  end

  def process_array exp
    result = s(:dummy)

    until exp.empty? do
      result << process(exp.shift)
    end

    result << s(:make_array, result.size - 1)

    result
  end

  def process_block exp
    return rewrite(s(:nil)) if exp.empty?

    result = s(:dummy)

    until exp.empty? do
      result << process(exp.shift)
      result << s(:pop)
    end

    result.pop if result.size > 2 # remove last pop

    result
  end

  def process_block_pass exp
    jump_proc = new_jump

    s(:dummy,
      process(exp.shift),
      s(:dup),
      s(:is_nil),
      s(:git, jump_proc),
      s(:push_cpath_top),
      s(:find_const, :Proc),
      s(:swap),
      s(:send, :__from_block__, 1),
      s(:set_label, jump_proc))
  end

  def process_call exp
    recv  = process(exp.shift)
    mesg  = exp.shift
    args  = exp.shift
    arity = args.size - 1

    block_pass = args.find { |s| Sexp === s && s[0] == :block_pass }
    splat      = args.find { |s| Sexp === s && s[0] == :splat }
    arity -= 1 if block_pass
    arity -= 1 if splat

    args[0] = :dummy
    args  = process(args)

    private_send = recv.nil?
    recv ||= s(:push, :self)
    recv = s(:push, :self) if private_send

    case mesg    # TODO: this sucks... we shouldn't do this analysis here
    when :+ then
      s(:dummy, recv, args, s(:meta_send_op_plus))
    when :- then
      s(:dummy, recv, args, s(:meta_send_op_minus))
    when :== then
      s(:dummy, recv, args, s(:meta_send_op_equal))
    else
      if splat then
        # FIX: I think this is tarded...
        # FIX: we don't need send, send_with_splat, send_with_block... merge
        if block_pass then
          s(:dummy, recv, args,
            s(:send_with_splat, mesg, arity, private_send, false))
        else
          s(:dummy, recv, args, s(:send_with_splat, mesg, arity, private_send))
        end
      elsif block_pass then
        s(:dummy, recv, args, s(:send_with_block, mesg, arity, private_send))
      else
        s(:dummy, recv, args, s(:send, mesg, arity, private_send))
      end
    end
  end

  def process_colon2 exp
    s(:dummy,
      process(exp.shift),
      s(:find_const, exp.shift))
  end

  def process_colon3 exp
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, exp.shift))
  end

  def process_const exp
    s(:push_const, exp.shift)
  end

  def defn_or_defs exp, type
    recv = process(exp.shift) if type == :defs
    name = exp.shift
    args = process(exp.shift)
    body = process(exp.shift)
    msg  = type == :defs ? :attach_method : :__add_method__

    body[0] = :method_description
    body[1, 0] = args[1..-1] # HACK: fucking fix dummy!
    body << s(:ret)

    s(:dummy,
      (recv if type == :defs),
      type == :defn ? s(:push_context) : s(:send, :metaclass, 0),
      s(:push_literal, name),
      s(:push_literal, body),
      s(:send, msg, 2)).compact
  end

  def process_defn exp
    defn_or_defs exp, :defn
  end

  def process_defs exp
    defn_or_defs exp, :defs
  end

  def process_dregx exp
    options = Fixnum === exp.last ? exp.pop : 0

    s(:dummy,
      s(:push_const, :Regexp),
      *process_dstr(exp)[1..-1]) << s(:push, options) << s(:send, :new, 2)
  end

  def process_dstr exp
    result = s(:dummy)

    size = exp.size

    until exp.empty? do
      part = exp.pop # going from the back
      case part
      when String
        result << s(:push_literal, part)
        result << s(:string_dup)
      else
        result << process(part)
      end
    end

    (size-1).times do
      result << s(:string_append)
    end

    result
  end

  def process_evstr exp
    s(:dummy, process(exp.shift), s(:send, :to_s, 0, true))
  end

  def process_gasgn exp
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, :Globals),
      s(:push_literal, exp.shift),
      process(exp.shift),
      s(:send, :[]=, 2))
  end

  def process_gvar exp
    case exp.last
    when :$! then
      exp.clear
      s(:push_exception)
    else
      s(:dummy,
        s(:push_cpath_top),
        s(:find_const, :Globals),
        s(:push_literal, exp.shift),
        s(:send, :[], 1))
    end
  end

  def process_if exp
    c = process(exp.shift)
    t = process(exp.shift)
    f = process(exp.shift)
    j = :gif

    t, f, j = f, t, :git if t.nil?

    t ||= s(:push, :nil)
    f ||= s(:push, :nil)

    j2 = s(j,          new_jump)
    j3 = s(:goto,      new_jump)
    s2 = s(:set_label, j2.last)
    s3 = s(:set_label, j3.last)

    s(:dummy, c, j2, t, j3, s2, f, s3).compact
  end

  def process_iter exp
    call = exp.shift
    args = exp.shift
    body = exp.shift

    # HACKS
    msg            = :dunno_message
    shift          = 0
    block_count    = 0,
    call_count     = 0,
    block_send_vis = true,
    shift          = 0,
    nested         = false,
    lvl            = 0

    inner_top = new_jump

    @jump = 0 # HACK

    top       = new_jump
    dunno1    = new_jump
    uncaught  = new_jump
    dunno2    = new_jump
    bottom    = new_jump

    return s(:dummy, :not_yet_iter_postexe) if call == s(:postexe) # HACK

    call = process(call)
    send = call.pop
    send[0] = :send_with_block

    s(:dummy,
      call,

      s(:create_block,
        s(:method_description,
          #       case block_count
          #       when Float then # yes... I'm a dick
          #         d.cast_for_single_block_arg
          #         d.set_local 0
          #       when -2 then
          #         d.cast_for_multi_block_arg
          #       when -1 then
          #         d.cast_array
          #         d.set_local_depth lvl, 0
          #       when 0 then
          #       when 1 then
          s(:cast_for_single_block_arg),
          s(:set_local_depth, lvl, 0),
          #       else
          #         d.cast_for_multi_block_arg
          #         (0...block_count).each do |n|
          #           d.shift_tuple
          #           d.set_local_depth lvl, n
          #           d.pop
          #         end
          #       end

          s(:pop),
          s(:push_modifiers),
          s(:set_label, inner_top),
          process(body),
          s(:pop_modifiers),
          s(:ret))),

      s(:set_label, top),

      s(:push_context),
      s(:set_local, 0 + shift),
      s(:pop),

      send,

      s(:goto, bottom),

      s(:set_label, dunno1),

      s(:push_exception),
      s(:dup),
      s(:push_cpath_top),
      s(:find_const, :LongReturnException),
      s(:swap),
      s(:kind_of),
      s(:gif, uncaught),

      s(:dup),
      s(:send, :context, 0),
      s(:push_context),
      s(:equal),
      s(:gif, uncaught),

      s(:clear_exception),
      s(:dup),
      s(:send, :is_return, 0),
      s(:gif, dunno2),

      #     unless nested then
      s(:send, :value, 0),
      s(:ret),
      #     end

      s(:set_label, uncaught),
      s(:raise_exc),
      s(:set_label, dunno2),
      s(:send, :value, 0),
      s(:set_label, bottom))
  end

  def process_lasgn exp
    lhs = exp.shift
    idx = name2slot lhs, false

    rhs = process(exp.shift)

    s(:dummy, rhs, s(:set_local, idx))
  end

  def process_lit exp # TODO: rewriter
    val = exp.shift

    case val
    when Float, Integer then
      s(:push, val)
    when Range then
      if val.exclude_end? then
        rewrite_dot3(s(:dot3, s(:push, val.begin), s(:push, val.end)))
      else
        rewrite_dot2(s(:dot2, s(:push, val.begin), s(:push, val.end)))
      end
    when Symbol then
      s(:push_unique_literal, val)
    when Regexp then
      cache do
        s(:dummy,
          s(:push_const, :Regexp),
          s(:push_literal, val.source),
          s(:push, val.options),
          s(:send, :new, 2))
      end
    else
      raise "not yet"
    end
  end

  def process_lvar exp
    lhs = exp.shift
    idx = name2slot lhs

    s(:push_local, idx)
  end

  def process_masgn exp
    lhs     = exp.shift
    rhs     = exp.shift
    lhs_n   = lhs && lhs.size - 1
    rhs_n   = rhs && rhs.size - 1
    lhs_ary = lhs && lhs.first == :array
    rhs_ary = rhs && rhs.first == :array

    rhs[0]  = :dummy if rhs_ary
    lhs[0]  = :dummy if lhs_ary

    result  = s(:dummy)

    result << process(rhs)

    if lhs.last.first == :splat then # minor layer violation
      delta = rhs_n - lhs_n + 1
      result << s(:make_array, delta) if delta > 0
    end

    subresult = []
    lhs.shift # type
    until lhs.empty? do
      subresult.unshift s(:pop)
      subresult.unshift(*process(lhs.shift))
    end

    subresult.delete(:dummy)
    result.push(*subresult.compact)

    result << s(:push, :true) # FIX: this is just wrong
    result
  end

  def process_match exp
    # HACK: this is a bug in the old compiler
    recv = process(s(:gvar, :$_))
    recv[1, 0] = [recv.delete_at -2]

    s(:dummy,
      recv,
      process(exp.shift),
      s(:send, :=~, 1))
  end

  def process_match2 exp
    s(:dummy,
      process(exp.shift),
      process(exp.shift),
      s(:send, :=~, 1))
  end

  def process_match3 exp
    s(:dummy,
      process(exp.pop),
      process(exp.shift),
      s(:send, :=~, 1))
  end

  def process_op_asgn1 exp
    lhs = process exp.shift
    idx = process exp.shift
    msg = exp.shift
    rhs = process exp.shift


    case msg
    when :"&&", :"||" then # shortcuts need jumps
      jump_set_type = msg == :"&&" ? :gif : :git
      jump_set   = new_jump
      jump_unset = new_jump
      s(:dummy,
        lhs,
        s(:dup),
        idx.deep_clone,
        s(:send, :[], 1),
        s(:dup),
        s(jump_set_type, jump_set),
        s(:pop),
        idx,
        rhs,
        s(:send, :[]=, 2),
        s(:goto, jump_unset),
        s(:set_label, jump_set),
        s(:swap),
        s(:pop),
        s(:set_label, jump_unset))
    else
      s(:dummy,
        lhs,
        s(:dup),
        idx.deep_clone,
        s(:send, :[], 1),
        rhs,
        s(:send, msg, 1),
        idx,
        s(:swap),
        s(:send, :[]=, 2))
    end
  end

  def process_op_asgn2 exp
    lhs = process exp.shift
    set = exp.shift
    get = set.to_s[0..-2].to_sym
    msg = exp.shift
    rhs = process exp.shift

    case msg
    when :"&&", :"||" then # shortcuts need jumps
      jump_set_type = msg == :"&&" ? :gif : :git
      jump_set   = new_jump
      jump_unset = new_jump
      s(:dummy,
        lhs,
        s(:dup),
        s(:send, get, 0),
        s(:dup),
        s(jump_set_type, jump_set),
        s(:pop),
        rhs,
        s(:send, set, 1),
        s(:goto, jump_unset),
        s(:set_label, jump_set),
        s(:swap),
        s(:pop),
        s(:set_label, jump_unset))
    else
      s(:dummy,
        lhs,
        s(:dup),
        s(:send, get, 0),
        rhs,
        s(:send, msg, 1),
        s(:send, set, 1))
    end
  end

  def process_op_asgn_and exp
    lhs = process exp.shift
    rhs = process exp.shift

    jump_set = new_jump

    s(:dummy,
      lhs,
      s(:dup),
      s(:gif, jump_set),
      s(:pop),
      rhs,
      s(:set_label, jump_set))
  end

  def process_op_asgn_or exp
    lhs = process exp.shift
    rhs = process exp.shift

    jump_set = new_jump

    s(:dummy,
      lhs,
      s(:dup),
      s(:git, jump_set),
      s(:pop),
      rhs,
      s(:set_label, jump_set))
  end

  def process_or exp
    result = s(:dummy)

    fixme = [] # TODO: see note below

    until exp.empty? do
      result << process(exp.shift)
      result << s(:dup)
      result << s(:git)
      fixme  << result.last
      result << s(:pop)
    end
    3.times { result.pop }

    # TODO: this is just to delay the creation of the labels so I can
    # TODO: pass the specs with the old way of generating labels (at
    # TODO: set_label... there is NO NEED FOR IT OTHERWISE, and is
    # TODO: cleaner without. So once the tests pass, remove this.
    bottom = new_jump
    fixme.each { |s| s << bottom }

    result << s(:set_label, bottom)

    result
  end

  def process_resbody exp
    exceptions = exp.shift
    body       = exp.shift

    jump_ex_body  = new_jump
    jump_uncaught = new_jump
    @jump_else    = new_jump # FIX: THIS IS BULLSHIT
    @jump_handled = new_jump # TODO: this is fucked once you stack,
                             # but for some stupid reason, it has to
                             # be generated here instead of above

    exceptions.shift # type

    assign = exceptions.pop if
      exceptions.last && exceptions.last.first == :lasgn

    exceptions << s(:StandardError) if exceptions.empty?

    result = s(:dummy)
    until exceptions.empty? do
      exception = exceptions.shift
      result << s(:push_const, exception.last)
      result << s(:push_exception)
      result << s(:send, :===, 1)
      result << s(:git, jump_ex_body)
    end
    result << s(:goto, jump_uncaught)
    result << s(:set_label, jump_ex_body)

    result << process(assign) if assign

    body = process(body) || s(:push, :nil)
    result << body

    result << s(:clear_exception)
    result << s(:goto, @jump_handled)
    result << s(:set_label, jump_uncaught)
    result << s(:push_exception)
    result << s(:raise_exc)

    result
  end

  def process_rescue exp
    jump_top    = new_jump
    jump_dunno1 = new_jump
    jump_dunno2 = new_jump

    # TODO: maybe make into block handler

    else_body = exp.pop unless exp.last.first == :resbody

    result = s(:dummy)
    result << s(:push_modifiers)
    result << s(:set_label, jump_top)
    result << s(:set_label, jump_dunno1) # FIX: seriously, no clue... can't find

    result << process(exp.shift)

    resbodies = []
    until exp.empty? do
      resbodies << process(exp.shift)
    end

    result << s(:goto, @jump_else)
    result << s(:set_label, jump_dunno2)

    result.push(*resbodies)

    result << s(:set_label, @jump_else) # TODO: don't emit if we don't have one!
    result << process(else_body) if else_body
    result << s(:set_label, @jump_handled)

    @jump_handled = nil # FIX: see bitchy note below

    result << s(:pop_modifiers)

    result
  end

  def process_return exp
    val = process(exp.shift) || s(:push, :nil)

    s(:dummy, val, s(:ret))
  end

  ##
  # Invented node type is an if w/o an else body, allowing us to use
  # this internally for several different forms.

  def process_s_if exp
    c = process(exp.shift)
    flip = exp.pop if exp.last == true
    t = process(s(:dummy, *exp))
    j = flip ? :gif : :git

    exp.clear # appease the sexp processor gods

    j2 = s(j, new_jump)
    s2 = s(:set_label, j2.last)

    s(:dummy, c, j2, t, s2)
  end

  def process_splat exp
    return s(:dummy) if exp.empty? # TODO: fix? shouldn't even get here
    var  = process(exp.shift)
    call = process(exp.shift)

    return var if call.nil? # TODO: also prolly shouldn't even get here

    call.last[0] = :send_with_splat
    call.last << false # FIX: no clue

    call[2, 0] = [var,
                  s(:cast_array),
                  s(:push, :nil)]

    call
  end

  def process_str exp
    s(:dummy,
      s(:push_literal, exp.shift),
      s(:string_dup))
  end

  def process_to_ary exp
    sexp = process exp.shift

    s(:dummy,
      sexp,
      s(:cast_tuple),
      s(:shift_tuple))
  end

  def process_undef exp
    name = exp.shift.last
    s(:dummy,
      s(:push, :self),
      s(:send, :metaclass, 0),
      s(:push_literal, name),
      s(:send, :undef_method, 1))
  end

  def process_until exp
    while_or_until exp, :git
  end

  def process_while exp
    while_or_until exp, :gif
  end

  def process_yield exp
    empty  = exp.empty?
    result = s(:dummy, s(:push_block))

    until exp.empty? do
      result << process(exp.shift)
    end

    result << s(:meta_send_call, empty ? 0 : 1)

    result
  end

  ############################################################
  # Rewrites

  def rewrite_alias exp
    exp[1][0] = exp[2][0] = :push_literal # TODO: this is horridly inconsistent

    s(:dummy,
      s(:push_context),
      exp[1],
      exp[2],
      s(:send, :alias_method, 2, true))
  end

  def rewrite_attrasgn exp
    exp[0] = :call
    exp
  end

  def rewrite_back_ref exp
    s(:dummy,
      s(:push_context),
      s(:push_literal, exp.last),
      s(:send, :back_ref, 1))
  end

  ##
  # Rewrites splat to outside:
  #   s(:call, recv, :msg, s(:splat, s(:lvar, :val))) =>
  #   s(:splat, s(:lvar, :val), s(:call, recv, :msg, s(:arglist)))))))

  def rewrite_call exp
    case exp[3][0]
    when :splat then
      call = exp
      splat = exp.pop
      call << s(:arglist)
      splat << call
    else
      exp
    end
  end

  def rewrite_cdecl exp
    cdecl_or_cvdecl exp, s(:push_context), :__const_set__
  end

  def rewrite_class exp
    class_or_module exp, :class
  end

  def rewrite_cvar exp
    s(:dummy,
      s(:push_context),
      s(:push_literal, exp[1]),
      s(:send, :class_variable_get, 1))
  end

  def rewrite_cvdecl exp
    cdecl_or_cvdecl exp, s(:push, :self), :class_variable_set
  end
  alias :rewrite_cvasgn :rewrite_cvdecl

  def rewrite_dot2 exp
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, :Range),
      exp[1],
      exp[2],
      s(:send, :new, 2))
  end

  def rewrite_dot3 exp
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, :Range),
      exp[1],
      exp[2],
      s(:push, :true),
      s(:send, :new, 3))
  end

  def rewrite_false exp
    s(:push, :false)
  end

  def rewrite_hash exp
    exp[0] = :dummy
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, :Hash),
      exp,
      s(:send, :[], exp.size - 1))
  end

  def rewrite_iasgn exp
    s(:dummy,
      exp[2],
      s(:set_ivar, exp[1]))
  end

  def rewrite_ivar exp
    s(:push_ivar, exp[1])
  end

  def rewrite_module exp
    class_or_module exp, :module
  end

  def rewrite_nil exp
    s(:push, :nil)
  end

  def rewrite_scope exp
    exp[0] = :dummy
    exp
  end

  def rewrite_self exp
    s(:push, :self)
  end

  def rewrite_true exp
    s(:push, :true)
  end

  def rewrite_valias exp
    s(:dummy,
      s(:push_cpath_top),
      s(:find_const, :Globals),
      s(:push_literal, exp[2]),
      s(:push_literal, exp[1]),
      s(:send, :add_alias, 2))
  end

  def rewrite_xstr exp
    s(:dummy,
      s(:push, :self),
      s(:push_literal, exp[1]),
      s(:string_dup),
      s(:send, :"`", 1, true))
  end

  ############################################################
  # Helpers

  def cache
    literal = new_literal

    s(:dummy,
      s(:add_literal, nil),
      s(:push_literal_at, literal),
      s(:dup),
      process(s(:s_if, s(:is_nil),
                s(:pop),
                yield,
                s(:set_literal, literal),
                true)))
  end

  def cdecl_or_cvdecl exp, recv, mesg
    lhs = exp[1]
    rhs = exp[2]

    case lhs
    when Symbol then
      lhs = s(:dummy, recv, s(:push_literal, lhs))
    when Sexp then
      case lhs.first
      when :colon2 then
        _, a, b = lhs
        lhs = s(:dummy, a, s(:push_literal, b))
      when :colon3 then # FIX: not sure, but I think this fails X::Y::Z::Q...
        lhs = s(:dummy, s(:push_cpath_top), s(:push_literal, lhs.last))
      else
        raise "wtf: #{lhs.inspect}"
      end
    else
      raise "wtf: #{lhs.inspect}"
    end

    s(:dummy, lhs, rhs, s(:send, mesg, 2))
  end

  def class_or_module exp, type
    _ = exp.shift # type
    name = exp.shift
    supr = exp.shift || s(:push, :nil) if type == :class
    body = exp.shift

    name = case name
           when Symbol then
             s(:"open_#{type}", name)
           when Sexp then
             case name.first
             when :colon2 then
               s, supr = supr, nil
               s(:dummy, name[1], s,
                 s(:"open_#{type}_under", name.last)).compact
             when :colon3 then
               s(:"open_#{type}", name.last)
             else
               raise "no? #{name.inspect}"
             end
           else
             raise "no? #{name.inspect}"
           end

    result = s(:dummy,
               supr,
               name).compact

    if body != s(:dummy) then # TODO: this seems icky
      result.push(s(:dup),
                  s(:push_literal,
                    s(:method_description,
                      s(:push_self), # FIX: ARGH!
                      s(:add_scope),
                      body,
                      s(:ret))),
                  s(:swap),
                  s(:attach_method, :"__#{type}_init__"),
                  s(:pop),
                  s(:send, :"__#{type}_init__", 0))
    end

    result
  end

  # TODO: move to name2index sexp processor phase
  def name2slot name, raise_if_missing = false # TODO: true
    idx = @slots[name]
    if raise_if_missing then
      raise "unknown var name #{name.inspect} in #{@slots.inspect}" unless idx
    else
      idx = @slots[name] = new_slot
    end unless idx
    idx
  end

  def new_jump
    @jump += 1
    :"label_#{@jump}"
  end

  def new_literal # FIX: this is gonna break. original based on @ip, not incr
    @literal += 1
    @literal
  end

  def new_slot
    @current_slot += 1
    @current_slot
  end

  def strip_dummies exp # TODO: build this in
    return if exp.nil?

    result = s(exp.shift)

    until exp.empty? do
      v = exp.shift

      if Sexp === v then
        if v.first == :dummy then
          result.push(*v[1..-1].map { |o| strip_dummies(o) })
        else
          result.push(Sexp.from_array(v.map { |o|
                                        Sexp === o ? strip_dummies(o) : o
                                      }))
        end
      else
        result << v
      end
    end
    result
  end

  def while_or_until exp, jump
    cond = exp.shift
    body = exp.shift
    pre  = exp.shift

    jump_top   = new_jump
    jump_dunno = new_jump
    jump_f     = new_jump
    jump_bot   = new_jump

    result = s(:dummy)
    result << s(:push_modifiers)
    result << s(:set_label, jump_top)

    if pre then
      result << process(cond)
      result << s(jump, jump_f)
      result << s(:set_label, jump_dunno)
    end

    result << (process(body) || s(:push, :nil)) # TODO: ewww
    result << s(:pop)

    unless pre then
      result << s(:set_label, jump_dunno)
      result << process(cond)
      result << s(jump, jump_f)
    end

    result << s(:goto, jump_top)
    result << s(:set_label, jump_f)
    result << s(:push, :nil)
    result << s(:set_label, jump_bot)
    result << s(:pop_modifiers)

    result
  end
end

describe "Compiler::*Nodes" do
  ParseTreeTestCase.testcases.sort.each do |node, hash|
    next if Array === hash['Ruby']
    next if hash['Compiler'] == :skip

    it "compiles :#{node}" do
      input    = hash['Ruby']
      expected = hash['Compiler']

      input.should_not == nil
      expected.should_not == nil

      sexp   = Sexp.from_array input.to_sexp("(eval)", 1, false)
      comp   = ::NewCompiler.new
      node   = comp.process sexp

      expected = s(:test_generator, *Sexp.from_array(expected.stream))

      expected.should == node
    end
  end
end
