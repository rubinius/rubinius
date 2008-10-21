require 'sexp_processor'
require 'compiler/lit_rewriter'

class Rubinius::SydneyRewriter < SexpProcessor
  def rewrite_begin(exp)
    exp.last
  end

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
        exp.args.insert -2, sym
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

  # Adapted from UnifiedRuby
  def rewrite_fcall(exp)
    exp[0] = :call
    exp.insert 1, nil
    exp.push nil if exp.size <= 3

    rewrite_call(exp)
  end

  # HACK: rewrite
  def rewrite_masgn(exp)
    if exp.size == 4
      splat = exp.delete_at(2)
      splat = s(:splat, splat) unless splat[0] == :splat
      exp[1] << splat
    elsif exp.size == 2
      args = exp.last
      case args
      when Array
        if args.first != :array
          args = s(:splat, args) unless args.first == :splat
          exp[-1] = s(:array, args)
        end
      end
    end
    exp
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

  # Adapted from UnifiedRuby
  def rewrite_resbody(exp)
    result = s()

    code = result
    while exp and exp.first == :resbody
      code << exp.shift

      list = exp.shift || s(:array)
      body = exp.empty? ? nil : exp.shift
      exp  = exp.empty? ? nil : exp.shift

      # code may be nil, :lasgn, or :block
      case body.first
      when :lasgn then
        # TODO: check that it is assigning $!
        list << body
        body = nil
      when :block then
        # TODO: check that it is assigning $!
        list << body.delete_at(1) if body[1].first == :lasgn
      end if body

      body = s(:break) if body == s(:block, s(:break))

      code << list << body
      if exp then
        code = s()
        result << code
      end
    end

    result
  end

  def rewrite_super(exp)
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

  def rewrite_zarray(exp)
    s(:array)
  end
end

