class String
  def self.sydney_parser
    alias_method :to_sexp, :to_sexp_sydney_parser
  end

  def self.ruby_parser
    alias_method :to_sexp, :to_sexp_ruby_parser
  end

  def parse(name, line)
    Ruby.primitive :string_parse
    raise PrimitiveFailure, "String#parse primitive failed"
  end

  def to_sexp_sydney_parser(name="(eval)", line=1, lit_rewriter=true)
    sexp = parse name, line
    if sexp.kind_of? Rubinius::Tuple
      exc = SyntaxError.new sexp.at(0)
      exc.import_position sexp[1], sexp[2], sexp[3]
      exc.file = name
      raise exc
    end

    sexp = [:newline, 0, "<empty: #{name}>", [:nil]] unless sexp
    sexp = Rubinius::SydneyRewriter.new.process sexp, name, line
    sexp
  end

  # TODO - Pass the starting line info into RubyParser
  def to_sexp_ruby_parser(name="(eval)", line = 1, lit_rewriter=true)
    require 'ruby_parser'
    require 'compiler/lit_rewriter'
    sexp = RubyParser.new.process(self, name)
    sexp = Rubinius::LitRewriter.new.process(sexp) if lit_rewriter
    sexp
  end

  def to_sexp(name="(eval)", line=1, rewriter=true)
    to_sexp_sydney_parser name, line, rewriter
  end
end
