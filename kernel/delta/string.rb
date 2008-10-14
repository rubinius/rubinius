class String
  BASE_64_A2B = {}
  (?A..?Z).each {|x| BASE_64_A2B[x] = x - ?A}
  (?a..?z).each {|x| BASE_64_A2B[x] = x - ?a + 26}
  (?0..?9).each {|x| BASE_64_A2B[x] = x - ?0 + 52}
  BASE_64_A2B[?+]  = ?>
  BASE_64_A2B[?\/] = ??
  BASE_64_A2B[?=]  = 0

  def parse(name, line, newlines)
    Ruby.primitive :string_parse
    raise PrimitiveFailure, "String#parse primitive failed"
  end

  def to_sexp(name="(eval)", line=1, newlines=true)
    out = parse(name, line, newlines)
    if out.kind_of? Tuple
      exc = SyntaxError.new out.at(0)
      exc.import_position out.at(1), out.at(2), out.at(3)
      exc.file = name
      raise exc
    end

    out = [:newline, 0, "<empty: #{name}>", [:nil]] unless out
    out
  end

  # TODO - Pass the starting line info into RubyParser
  def to_sexp_rp(name="(eval)", line = 1, lit_rewriter=true)
    require 'ruby_parser'
    require 'compiler/lit_rewriter'
    sexp = RubyParser.new.process(self, name)
    sexp = Rubinius::LitRewriter.new.process(sexp) if lit_rewriter
    sexp
  end
end
