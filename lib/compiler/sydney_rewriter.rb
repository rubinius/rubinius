require 'compiler/lit_rewriter'

class Sexp
  def file
    @file || "<missing filename>"
  end

  def line
    @line || -1
  end

  def set_file(ary, file)
    @file = ary.instance_variable_get(:@file) || file
  end

  def set_line(ary, line)
    @line = ary.instance_variable_get(:@line) || line
  end
end

class Rubinius::SydneyRewriter
  def self.sexp_from_array(ary, file, line)
    ary = Array(ary)

    result = Sexp.new
    result.set_file ary, file
    result.set_line ary, line

    ary.each do |x|
      if Array === x
        exp = sexp_from_array x, file, line
        result << exp
      else
        result << x
      end
    end

    result
  end

  # Adapted from SexpProcessor
  def self.rewriters
    unless @rewriters
      @rewriters = {}
      public_instance_methods.each do |name|
        if /^rewrite_(.*)/ =~ name
          @rewriters[$1.to_sym] = name.to_sym
        end
      end
    end
    @rewriters
  end

  def process(exp, name="(eval)", line=1)
    return exp unless Array === exp
    exp = self.class.sexp_from_array exp, name, line
    rewrite exp, name, line
  end

  # Adapted from SexpProcessor
  def rewrite(exp, file, line)
    orig = exp
    type = exp.first

    exp.map! { |sub| Array === sub ? rewrite(sub, file, line) : sub }

    begin
      meth = self.class.rewriters[type]
      exp  = self.send(meth, exp) if meth

      unless exp.equal? orig
        exp.set_file orig.file, file
        exp.set_line orig.line, line
      end

      break unless Sexp === exp
      old_type, type = type, exp.first
    end until old_type == type

    exp
  end

  # Rewriters

  def rewrite_argscat(exp)
    exp[1] << s(:splat, exp.last)
  end

  def rewrite_argspush(exp)
    exp[0] = :arglist
    exp
  end

  def rewrite_attrasgn(exp)
    ary = exp.array rescue nil
    ary[0] = :arglist if ary

    exp << s(:arglist) unless Array === exp.last

    exp
  end

  def rewrite_begin(exp)
    exp.last
  end

  def rewrite_block_pass(exp)
    args = exp.last
    case args
    when Array
      args = exp.pop
      if args.first == :super
        return args << exp
      else
        args.last << exp
        return args
      end
    end

    exp
  end

  # Adapted from UnifiedRuby
  def rewrite_call(exp)
    args = exp.last
    case args
    when nil
      exp.pop
    when Array
      case args.first
      when :array
        args[0] = :arglist
      when :splat
        splat = exp.pop
        exp << s(:arglist, splat)
      end
      return exp
    end

    exp << s(:arglist)
    exp
  end

  # Adapted from UnifiedRuby
  def rewrite_defn(exp)
    args = exp.scope.block.args(true)
    exp.insert 2, args if args

    block_arg = exp.scope.block.block_arg(true) rescue nil
    if block_arg
      sym = :"&#{block_arg.last}"
      if Array === exp.args.last
        exp.args.insert(-2, sym)
      else
        exp.args << sym
      end
    end

    exp
  end

  # Adapted from UnifiedRuby
  def rewrite_defs(exp)
    receiver = exp.delete_at 1

    exp = s(exp.shift, exp.shift,
            s(:scope,
              s(:block, exp.scope.args))) if exp.scope.args

    result = rewrite_defn(exp)
    result.insert 1, receiver

    result
  end

  def rewrite_dstr(exp)
    str = exp[1]
    if String === str
      while Array === (s = exp[2]) and s.first == :str
        exp.delete_at 2
        str << s.last
      end
      exp[1] = str
    end

    exp[0] = :str if exp.size == 2 and String === exp.last

    exp
  end

  def rewrite_evstr(exp)
    return s(:str, exp.file) if exp.last == s(:file)
    exp
  end

  # Adapted from UnifiedRuby
  def rewrite_fcall(exp)
    exp[0] = :call
    exp.insert 1, nil
    exp.push nil if exp.size <= 3

    rewrite_call(exp)
  end

  # TODO: fix this clusterfuck in grammar_runtime.cpp
  def rewrite_masgn(exp)
    last = exp.last

    case exp.size
    when 2
      kind = last.first
      unless kind == :array or kind == :to_ary
        last = s(:splat, last) unless last.first == :splat
        exp[1] = s(:array, last)
      end
    when 3
      lhs, rhs = exp[1], exp[2]

      if lhs.first == :array
        case rhs.first
        when :to_ary, :array
          # do nothing
        when :splat
          lhs << exp.pop if rhs.size == 1
        else
          last = exp.pop
          last = s(:splat, last)
          lhs << last
        end
      elsif lhs.first == :splat
        exp[1] = s(:array, exp[1])
      else
        case rhs.first
        when :array
          exp[1] = s(:array, s(:splat, lhs))
          rhs[0] = :to_ary if rhs.size == 2
        when :splat
          exp[1] = s(:array, s(:splat, lhs))
        end
      end
    when 4
      last = exp.delete_at 2
      last = s(:splat, last) unless last.first == :splat
      exp[1] << last
    end

    exp
  end

  def rewrite_match(exp)
    s(:match, s(:regex, exp[1], exp[2]))
  end

  def rewrite_negate(exp)
    s(:lit, -exp.last.last)
  end

  def rewrite_op_asgn1(exp)
    args = exp[2]
    case args
    when Array
      if args.first == :array
        args[0] = :arglist
      end
    end

    exp
  end

  def exception_asgn?(exp)
    case exp.first
    when :lasgn, :iasgn, :gasgn, :attrasgn
      return true if exp.last == s(:gvar, :$!)
      return false
    end if Array === exp

    false
  end

  # Adapted from UnifiedRuby
  def rewrite_resbody(exp)
    result = s()

    code = result
    while exp and exp.first == :resbody do
      code << exp.shift
      list = exp.shift || s(:array)
      body = exp.empty? ? nil : exp.shift
      exp  = exp.empty? ? nil : exp.shift

      case body.first
      when nil
        # do nothing
      when :block
        if exception_asgn? body[1]
          list << body.delete_at(1)
        end

        body = body.last if body.size == 2
      else
        if exception_asgn? body
          list << body
          body = nil
        end
      end if body

      code << list << body
      if exp then
        code = s()
        result << code
      end
    end

    result
  end

  def flatten_resbody(exp, list)
    sexp = s()
    list << sexp
    exp.each do |x|
      if Array === x and x.first == :resbody
        flatten_resbody(x, list)
      else
        sexp << x
      end
    end

    list
  end

  # This rewrites
  #   s(:rescue, ... s(:resbody, ... s(:resbody, ... s(:resbody, ...))))
  # to
  #   s(:rescue, ... s(:resbody, ...), s(:resbody, ...), ...)
  #
  # Since sexp_processor calls this method more than once, the rewrite
  # must handle both cases.
  def rewrite_rescue(exp)
    sexp = s()
    exp.each do |x|
      if Array === x and x.first == :resbody
        flatten_resbody(x, s()).each { |z| sexp << z }
      else
        sexp << x
      end
    end

    sexp
  end

  def rewrite_super(exp)
    # this rewrite is because of how argscat is rewritten
    args = exp[1]
    case args
    when Array
      if args.first == :array
        args[0] = :super
        return args
      end
    end

    exp
  end

  # Adapted from UnifiedRuby
  def rewrite_vcall(exp)
    exp.push nil
    rewrite_fcall(exp)
  end

  def rewrite_yield(exp)
    case exp.last
    when true
      exp.pop
      args = exp.last
      case args
      when Array
        if args.first == :array
          args[0] = :yield
          return args
        end
      end
    when false
      exp.pop
      exp.pop unless exp.last
    end

    exp
  end

  def rewrite_zarray(exp)
    s(:array)
  end
end
