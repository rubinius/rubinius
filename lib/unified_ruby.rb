# Imported from ParseTree 2.2.0

$TESTING ||= false

module UnifiedRuby
  def rewrite_bmethod(exp)
    exp[0] = :scope

    args =
      if exp.masgn and exp.masgn.dasgn_curr then
        arg = exp.masgn(true).dasgn_curr(true).sexp_body
        raise "nope: #{arg.size}" unless arg.size == 1
        s(:args, :"*#{arg.last}")
      else
        args = exp.dasgn_curr(true)
        if args then
          s(:args, *args.sexp_body)
        else
          exp.delete_at 1 # nil
          s(:args)
        end
      end

    exp = s(:scope, s(:block, *exp.sexp_body)) unless exp.block
    exp.block.insert 1, args
    exp.find_and_replace_all(:dvar, :lvar)

    exp
  end

  def rewrite_call(exp)
    args = exp.last
    case args
    when nil
      exp.pop
    when Array
      case args.first
      when :array, :arglist then
        args[0] = :arglist
      when :argscat, :splat then
        # do nothing
      else
        raise "unknown type in call #{args.first.inspect} in #{exp.inspect}"
      end
      return exp
    end

    exp << s(:arglist)

    exp
  end

  ##
  # :defn is one of the most complex of all the ASTs in ruby. We do
  # one of 3 different translations:
  #
  # 1) From:
  #
  #   s(:defn, :name, s(:scope, s(:block, s(:args, ...), ...)))
  #   s(:defn, :name, s(:bmethod, s(:masgn, s(:dasgn_curr, :args)), s(:block, ...)))
  #   s(:defn, :name, s(:fbody, s(:bmethod, s(:masgn, s(:dasgn_curr, :splat)), s(:block, ...))))
  #
  # to:
  #
  #   s(:defn, :name, s(:args, ...), s(:scope, s:(block, ...)))
  #
  # 2) From:
  #
  #   s(:defn, :writer=, s(:attrset, :@name))
  #
  # to:
  #
  #   s(:defn, :writer=, s(:args), s(:attrset, :@name))
  #
  # 3) From:
  #
  #   s(:defn, :reader, s(:ivar, :@name))
  #
  # to:
  #
  #   s(:defn, :reader, s(:args), s(:ivar, :@name))
  #

  def rewrite_defn(exp)
    weirdo = exp.ivar || exp.attrset

    fbody = exp.fbody(true)
    exp.push(fbody.scope) if fbody

    args = exp.scope.block.args(true) unless weirdo
    exp.insert 2, args if args

    # move block_arg up and in
    block_arg = exp.scope.block.block_arg(true) rescue nil
    exp.args << block_arg if block_arg

    # patch up attr_accessor methods
    exp.insert 2, s(:args) if weirdo

    exp
  end

  def rewrite_defs(exp)
    receiver = exp.delete_at 1

    # TODO: I think this would be better as rewrite_scope, but that breaks others
    exp = s(exp.shift, exp.shift,
            s(:scope,
              s(:block, exp.scope.args))) if exp.scope.args

    result = rewrite_defn(exp)
    result.insert 1, receiver

    result
  end

  def rewrite_dmethod(exp)
    exp.shift # type
    exp.shift # dmethod name
    exp.shift # scope / block / body
  end

  def rewrite_fcall(exp)
    exp[0] = :call
    exp.insert 1, nil
    exp.push nil if exp.size <= 3

    rewrite_call(exp)
  end

  def rewrite_resbody(exp) # TODO: clean up and move to unified
    result = s()

    code = result
    while exp and exp.first == :resbody do
      code << exp.shift
      list = exp.shift || s(:array)
      body = exp.empty? ? nil : exp.shift
      exp  = exp.empty? ? nil : exp.shift

      # code may be nil, :lasgn, or :block
      case body.first
      when nil then
        # do nothing
      when :lasgn then
        # TODO: check that it is assigning $!
        list << body
        body = nil
      when :block then
        # TODO: check that it is assigning $!
        list << body.delete_at(1) if body[1].first == :lasgn
      else
        # do nothing (expression form)
      end if body

      code << list << body
      if exp then
        code = s()
        result << code
      end
    end

    if $DEBUG or $TESTING then
      structure = result.structure
      raise "result structure wrong: #{structure[0..1].inspect}" unless
        structure.flatten[0] == :resbody
      raise "result structure wrong: #{structure[0..1].inspect}" unless
        s(:array, :splat, :argscat).include? structure.flatten[1]
      raise "result body wrong: #{structure[2].inspect}" unless
        structure[2].nil? or not structure[2].empty?
    end

    result
  end

  def rewrite_vcall(exp)
    exp.push nil
    rewrite_fcall(exp)
  end
end

##
# Quick and easy SexpProcessor that unified the sexp structure.

class Unifier < SexpProcessor
  include UnifiedRuby
end

