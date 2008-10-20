class String
  BASE_64_A2B = {}
  (?A..?Z).each {|x| BASE_64_A2B[x] = x - ?A}
  (?a..?z).each {|x| BASE_64_A2B[x] = x - ?a + 26}
  (?0..?9).each {|x| BASE_64_A2B[x] = x - ?0 + 52}
  BASE_64_A2B[?+]  = ?>
  BASE_64_A2B[?\/] = ??
  BASE_64_A2B[?=]  = 0

  @@parser = :rp

  def self.parser=(parser)
    @@parser = parser
  end

  def self.parser
    @@parser
  end

  def parse(name, line)
    Ruby.primitive :string_parse
    raise PrimitiveFailure, "String#parse primitive failed"
  end

  def to_sexp_pt(name="(eval)", line=1, rewriter=true, unifier=true)
    require 'unified_ruby'
    require 'compiler/lit_rewriter'

    sexp = parse name, line
    if sexp.kind_of? Tuple
      exc = SyntaxError.new sexp.at(0)
      exc.import_position sexp[1], sexp[2], sexp[3]
      exc.file = name
      raise exc
    end

    sexp = [:newline, 0, "<empty: #{name}>", [:nil]] unless sexp
    sexp = Sexp.from_array sexp
    sexp = Unifier.new.process sexp if unifier
    sexp = Rubinius::LitRewriter.new.process sexp if rewriter
    sexp
  end

  # TODO - Pass the starting line info into RubyParser
  def to_sexp_rp(name="(eval)", line = 1, lit_rewriter=true)
    require 'ruby_parser'
    require 'compiler/lit_rewriter'
    sexp = RubyParser.new.process(self, name)
    sexp = Rubinius::LitRewriter.new.process(sexp) if lit_rewriter
    sexp
  end

  def to_sexp(name="(eval)", line=1, rewriter=true)
    if self.class.parser == :pt
      to_sexp_pt name, line, rewriter
    else
      to_sexp_rp name, line, rewriter
    end
  end
end
