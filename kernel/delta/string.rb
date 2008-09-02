
$: << File.expand_path("~/Work/p4/zss/src/ParseTree/dev/lib/")   # HACK
$: << File.expand_path("~/Work/p4/zss/src/ruby_parser/dev/lib/") # HACK
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

class String
  BASE_64_A2B = {}
  (?A..?Z).each {|x| BASE_64_A2B[x] = x - ?A}
  (?a..?z).each {|x| BASE_64_A2B[x] = x - ?a + 26}
  (?0..?9).each {|x| BASE_64_A2B[x] = x - ?0 + 52}
  BASE_64_A2B[?+]  = ?>
  BASE_64_A2B[?\/] = ??
  BASE_64_A2B[?=]  = 0

  def to_sexp(name="(eval)") # TODO: maybe move into lib/compiler and after_load
    require 'ruby_parser'

    sexp = RubyParser.new.process(self, name)
    sexp = Rubinius::LitRewriter.new.process(sexp)
    sexp
  end
end
