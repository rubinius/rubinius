require 'sexp_processor'

module Rubinius; end
class Rubinius::LitRewriter < SexpProcessor # TODO: move
  # HACK: until we figger out how PT should be
  def rewrite_dregx(exp)
    exp << 0 unless Integer === exp.last
    exp
  end

  # HACK: until we figger out how PT should be
  def rewrite_dregx_once(exp)
    exp << 0 unless Integer === exp.last
    exp
  end

  def rewrite_lit(exp)
    raise "wtf: #{exp.inspect}" unless exp.size == 2
    exp.shift
    value = exp.shift
    case value
    when Float then
      s(:float, value)
    when Integer then
      s(:fixnum, value) # TODO: rename :int
    when Range then
      l = rewrite_lit(s(:lit, value.begin))
      r = rewrite_lit(s(:lit, value.end))
      t = value.exclude_end? ? :dot3 : :dot2
      s(t, l, r)
    when Regexp then
      s(:regex, value.source, value.options)
    when Symbol then
      s(:lit, value)
    else
      raise "unhandled :lit type #{value.inspect}"
    end
  end
end
